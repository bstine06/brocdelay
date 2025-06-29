/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ProtectYourEars.h"
#include "DSP.h"

//==============================================================================
DelayAudioProcessor::DelayAudioProcessor() :
    AudioProcessor(
                    BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ),
    params(apvts)
{
    lowCutFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    highCutFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
}

DelayAudioProcessor::~DelayAudioProcessor()
{
}

//==============================================================================
const juce::String DelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    // (re)initialize settings for parameters on play start
    params.prepareToPlay(sampleRate);
    params.reset();
    
    // prepare delay line
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = juce::uint32(samplesPerBlock);
    spec.numChannels = 2;
    
    double numSamples = Parameters::maxDelayTime / 1000.0 * sampleRate;
    int maxDelayInSamples = int(std::ceil(numSamples));
    delayLineL.setMaximumDelayInSamples(maxDelayInSamples);
    delayLineR.setMaximumDelayInSamples(maxDelayInSamples);
    delayLineL.reset();
    delayLineR.reset();
    
    feedbackL = 0.0f;
    feedbackR = 0.0f;
    
    lowCutFilter.prepare(spec);
    lowCutFilter.reset();
    highCutFilter.prepare(spec);
    highCutFilter.reset();
    
    lastLowCut = -1.0f;
    lastHighCut = -1.0f;
    
    lastMix = -1.0f;
    dryGain = 1.0f;
    wetGain = 0.0f;
    
    delayInSamples = 0.0f;
    targetDelay = 0.0f;
    
    xfade = 0.0f;
    xfadeInc = static_cast<float>(1.0 / (0.05 * sampleRate)); //50ms
    
    duckFade = 1.0f;
    duckFadeTarget = 1.0f;
    duckCoeff = 1.0f - std::exp(-1.0f / (0.05f * float(sampleRate)));
    duckWait = 0.0f;
    duckWaitInc = 1.0f / (0.05f * float(sampleRate)); // 50ms
    
    tempo.reset();
    
    tempoSyncCoeff = 1.0f - std::exp(-1.0f / (0.2f * float(sampleRate)));
    
    levelL.reset();
    levelR.reset();
}

void DelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto mono = juce::AudioChannelSet::mono();
    const auto stereo = juce::AudioChannelSet::stereo();
    const auto mainIn = layouts.getMainInputChannelSet();
    const auto mainOut = layouts.getMainOutputChannelSet();
    
    if (mainIn == mono && mainOut == mono) { return true; }
    if (mainIn == mono && mainOut == stereo ) { return true; }
    if (mainIn == stereo && mainOut == stereo ) { return true; }
    
    return false;
}
#endif

void DelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    params.update();
    tempo.update(getPlayHead());
    
    float syncedTime = float(tempo.getMillisecondsForNoteLength(params.delayNote));
    if (syncedTime > Parameters::maxDelayTime) {
        syncedTime = Parameters::maxDelayTime;
    }
    
    float sampleRate = float(getSampleRate());
    
    auto mainInput = getBusBuffer(buffer, true, 0);
    auto mainInputChannels = mainInput.getNumChannels();
    auto isMainInputStereo = mainInputChannels > 1;
    const float* inputDataL = mainInput.getReadPointer(0);
    const float* inputDataR = mainInput.getReadPointer(isMainInputStereo ? 1 : 0);
    
    auto mainOutput = getBusBuffer(buffer, false, 0);
    auto mainOutputChannels = mainOutput.getNumChannels();
    auto isMainOutputStereo = mainOutputChannels > 1;
    float* outputDataL = mainOutput.getWritePointer(0);
    float* outputDataR = mainOutput.getWritePointer(isMainOutputStereo ? 1 : 0);
    
    float maxL = 0.0f;
    float maxR = 0.0f;
    DBG("shiftMode: " << (params.getShiftMode() == ShiftMode::REPITCH ? "repitch" : "other"));
    
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        
        params.smoothen();
        ShiftMode shiftMode = params.getShiftMode();
        
        if (shiftMode == ShiftMode::FADE) {
            if (xfade == 0.0f) {
                float delayTime = params.tempoSync ? syncedTime : params.delayTime;
                targetDelay = (delayTime / 1000.0f) * sampleRate;
                if (delayInSamples == 0.0f) {
                    delayInSamples = targetDelay;
                } else if (targetDelay != delayInSamples) {
                    xfade = xfadeInc;
                }
            }
        } else if (shiftMode == ShiftMode::DUCK) {
            float delayTime = params.tempoSync ? syncedTime : params.delayTime;
            float newTargetDelay = (delayTime / 1000.0f) * sampleRate;
            if (newTargetDelay != targetDelay) {
                targetDelay = newTargetDelay;
                if (delayInSamples == 0.0f) {
                    delayInSamples = targetDelay; // first time
                }
                else {
                    duckWait = duckWaitInc; // start counter
                    duckFadeTarget = 0.0f; // fade out
                }
            }
        } else {
            float newTargetDelayMs = params.tempoSync ? syncedTime : params.delayTime;
            float newTargetDelay = (newTargetDelayMs / 1000.0f) * sampleRate;

            if (params.tempoSync) {
                
                if (targetDelay != newTargetDelay) {
                    targetDelay = newTargetDelay;
                }

                if (delayInSamples == 0.0f) {
                    delayInSamples = targetDelay; // first-time setup
                } else {
                    // Always smooth toward targetDelay every sample
                    delayInSamples = (1.0f - tempoSyncCoeff) * delayInSamples + tempoSyncCoeff * targetDelay;
                }
            } else {
                delayInSamples = newTargetDelay;
                targetDelay = newTargetDelay; // keep them in sync for next time
            }
        }
        
        if (params.lowCut != lastLowCut) {
            lowCutFilter.setCutoffFrequency(params.lowCut);
            lastLowCut = params.lowCut;
        }
        
        if (params.highCut != lastHighCut) {
            highCutFilter.setCutoffFrequency(params.highCut);
            lastHighCut = params.highCut;
        }
        
        float currentMix = params.mix;
        if (std::abs(currentMix - lastMix) > 0.001f) // small tolerance to avoid floating point jitter
        {
            // blend with sinusoids for equal power mixing
            dryGain = std::cos(currentMix * juce::MathConstants<float>::halfPi);
            wetGain = std::sin(currentMix * juce::MathConstants<float>::halfPi);
            lastMix = currentMix;
        }
        
        highCutFilter.setCutoffFrequency(params.highCut);
        
        float dryL = inputDataL[sample];
        float dryR = inputDataR[sample];
        
        // if flipFlop is on, invertStereo will be 1
        // if flipFlop is off, invertStereo will be 0;
        float fbInL = feedbackL * (1 - params.invertStereo) + feedbackR * params.invertStereo;
        float fbInR = feedbackR * (1 - params.invertStereo) + feedbackL * params.invertStereo;
        
        float dryInL = dryL * (1 - params.invertStereo) + dryR * params.invertStereo;
        float dryInR = dryR * (1 - params.invertStereo) + dryL * params.invertStereo;
        
        //push affected signals into delay line
        //L
        delayLineL.write(dryInL + fbInL);
        //R
        delayLineR.write(dryInR + fbInR);
        
        float wetL = delayLineL.read(delayInSamples);
        float wetR = delayLineR.read(delayInSamples);
        
        if (shiftMode == ShiftMode::FADE) {
            if (xfade > 0.0f) {
                float newL = delayLineL.read(targetDelay);
                float newR = delayLineR.read(targetDelay);
                
                wetL = (1.0f - xfade) * wetL + xfade * newL;
                wetR = (1.0f - xfade) * wetR + xfade * newR;
                
                xfade += xfadeInc;
                
                if (xfade >= 1.0f) {
                    delayInSamples = targetDelay;
                    xfade = 0.0f;
                }
            }
        } else if (shiftMode == ShiftMode::DUCK) {
            
            duckFade += (duckFadeTarget - duckFade) * duckCoeff;
            
            wetL *= duckFade;
            wetR *= duckFade;
            
            if (duckWait > 0.0f) {
                duckWait += duckWaitInc;
                if (duckWait >= 1.0f) {
                    delayInSamples = targetDelay;
                    duckWait = 0.0f;
                    duckFadeTarget = 1.0f;
                }
            }
        }
        
        
        
        wetL = lowCutFilter.processSample(0, wetL);
        wetL = highCutFilter.processSample(0, wetL);
        wetR = lowCutFilter.processSample(1, wetR);
        wetR = highCutFilter.processSample(1, wetR);
        
        feedbackL = wetL * params.feedback;
        feedbackR = wetR * params.feedback;

        float mixL = (dryL * dryGain) + (wetL * wetGain * params.mix);
        float mixR = (dryR * dryGain) + (wetR * wetGain * params.mix);
        
        float outL = mixL * params.gain;
        float outR = mixR * params.gain;
        
        outputDataL[sample] = outL;
        outputDataR[sample] = outR;
        
        maxL = std::max(maxL, std::abs(outL));
        maxR = std::max(maxR, std::abs(outR));
        
    }
    
    levelL.updateIfGreater(maxL);
    levelR.updateIfGreater(maxR);
    
    #if JUCE_DEBUG
    protectYourEars(buffer);
    #endif

}

//==============================================================================
bool DelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
    return new DelayAudioProcessorEditor (*this);
}

//==============================================================================
void DelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
    
}

void DelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
    
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}
