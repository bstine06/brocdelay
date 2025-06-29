/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Parameters.h"
#include "Tempo.h"
#include "DelayLine.h"
#include "Measurement.h"

//==============================================================================
/**
*/
class DelayAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DelayAudioProcessor();
    ~DelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    juce::AudioProcessorValueTreeState apvts {
        *this, nullptr, "Parameters", Parameters::createParameterLayout()
    };
    
    Parameters params;
    
    Measurement levelL, levelR;

private:
    //==============================================================================
    DelayLine delayLineL, delayLineR;
    
    float feedbackL = 0.0f;
    float feedbackR = 0.0f;
    
    juce::dsp::StateVariableTPTFilter<float> lowCutFilter;
    juce::dsp::StateVariableTPTFilter<float> highCutFilter;
    
    float lastLowCut = -1.0f;
    float lastHighCut = -1.0f;
    
    float lastMix = -1.0f;
    float dryGain = 1.0f;
    float wetGain = 0.0f;
    
    Tempo tempo;
    
    float delayInSamples = 0.0f;
    float targetDelay = 0.0f;
    
    float xfade = 0.0f;
    float xfadeInc = 0.0f;
    
    float duckFade = 0.0f;
    float duckFadeTarget = 0.0f;
    float duckCoeff = 0.0f;
    float duckWait = 0.0f;
    float duckWaitInc = 0.0f;
    
    float tempoSyncCoeff = 0.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessor)
};
