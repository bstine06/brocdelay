/*
  ==============================================================================

    Parameters.cpp
    Created: 26 Apr 2025 3:11:09pm
    Author:  Brett

  ==============================================================================
*/

#include "Parameters.h"
#include "DSP.h"

// Helper function to cast Parameter values to usable types
template<typename T> static void castParameter(juce::AudioProcessorValueTreeState& apvts,
                                               const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination); // parameter does not exist or wrong type
}

static juce::String stringFromMilliseconds(float value, int)
{
    if (value < 10.0f) {
        return juce::String(value, 2) + "ms";
    } else if (value < 100.f) {
        return juce::String(value, 1) + "ms";
    } else if (value < 1000.0f) {
        return juce::String(int(value)) + "ms";
    } else {
        return juce::String(value * 0.001f, 2) + "s";
    }
}

static float millisecondsFromString(const juce::String& text)
{
    float value = text.getFloatValue();
    if (!text.endsWithIgnoreCase("ms")) {
        if (text.endsWithIgnoreCase("s") || value < Parameters::minDelayTime) {
            return value * 1000.0f;
        }
    }
    return value;
}

static juce::String stringFromDecibels(float value, int) {
    return juce::String(value, 1) + "dB";
}

static juce::String stringFromPercent(float value, int) {
    return juce::String(int(value)) + " %";
}

static juce::String stringFromBool(bool value, int) {
    return value ? juce::String("On") : juce::String("Off");
}

static juce::String stringFromHz(float value, int) {
    if (value < 1000.0f) {
        return juce::String(int(value)) + "Hz";
    } else if (value < 10000.0f) {
        return juce::String(value / 1000.0f, 2) + "kHz";
    } else {
        return juce::String(value / 10000.0f, 1) + "kHz";
    }
}

static float hzFromString(const juce::String& text) {
    float value = text.getFloatValue();
    if (value < 20.0f || text.endsWithIgnoreCase("k") || text.endsWithIgnoreCase("kHz")) {
        return value * 1000.0f;
    }
    return value;
}

// Constructor
Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
    castParameter(apvts, ParamIDs::gain, gainParam);
    castParameter(apvts, ParamIDs::delayTime, delayTimeParam);
    castParameter(apvts, ParamIDs::mix, mixParam);
    castParameter(apvts, ParamIDs::feedback, feedbackParam);
    castParameter(apvts, ParamIDs::flipFlop, flipFlopParam);
    castParameter(apvts, ParamIDs::lowCut, lowCutParam);
    castParameter(apvts, ParamIDs::highCut, highCutParam);
    castParameter(apvts, ParamIDs::accelerateMode, accelerateModeParam);
    castParameter(apvts, ParamIDs::decelerateMode, decelerateModeParam);
    castParameter(apvts, ParamIDs::tempoSync, tempoSyncParam);
    castParameter(apvts, ParamIDs::delayNote, delayNoteParam);
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // Gain parameter
    layout.add(std::make_unique<juce::AudioParameterFloat>(
               //parameter ID
               ParamIDs::gain,
               //parameter display name
               "Output Gain",
               //min, max, step interval, skew
               juce::NormalisableRange<float> { -60.0f, 12.0f, 0.001f, 3.8f },
               //default value
               0.0f,
               //set value string display
               juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromDecibels)));
    
    // Delay Time parameter
    layout.add(std::make_unique<juce::AudioParameterFloat>(
               //Parameter ID
               ParamIDs::delayTime,
               //parameter display name
               "Delay Time",
               //min, max, step interval, skew
               juce::NormalisableRange<float> { minDelayTime, maxDelayTime, 0.001f, 0.35f },
               //default value
               100.0f,
               //set value string display
               juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromMilliseconds).withValueFromStringFunction(millisecondsFromString)));
    
    // Mix parameter
    layout.add(std::make_unique<juce::AudioParameterFloat>(
               //Parameter ID
               ParamIDs::mix,
               //parameter display name
               "Mix",
               //min, max, step interval, skew
               juce::NormalisableRange<float> { 0.0f, 100.0f, 1.0f },
               //default value
               50.0f,
               //set value string display
               juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    
    // feedback parameter
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                //Parameter ID
                ParamIDs::feedback,
                "Feedback",
                juce::NormalisableRange<float> { -100.0f, 100.0f, 1.0f },
                0.0f,
                juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    
    layout.add(std::make_unique<juce::AudioParameterBool>(
        ParamIDs::flipFlop,
        "Flip Flop",
        false, // default value
        juce::AudioParameterBoolAttributes().withStringFromValueFunction(stringFromBool)
    ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                 ParamIDs::lowCut,
                 "Low Cut",
                 juce::NormalisableRange<float> { 20.0f, 20000.0f, 1.0f, 0.3f },
                 20.0f,
                 juce::AudioParameterFloatAttributes()
                     .withStringFromValueFunction(stringFromHz)
                     .withValueFromStringFunction(hzFromString)
               ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                 ParamIDs::highCut,
                 "High Cut",
                 juce::NormalisableRange<float> { 20.0f, 20000.0f, 1.0f, 0.3f },
                 20000.0f,
                 juce::AudioParameterFloatAttributes()
                     .withStringFromValueFunction(stringFromHz)
                     .withValueFromStringFunction(hzFromString)
               ));
    
    const juce::StringArray delayModes = {
        "Repitch",
        "Fade",
        "Jump",
    };
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
                ParamIDs::accelerateMode,
                "Accelerate Mode",
                delayModes,
                0
                ));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
                ParamIDs::decelerateMode,
                "Decelerate Mode",
                delayModes,
                0
                ));
    
    layout.add(std::make_unique<juce::AudioParameterBool>(
                ParamIDs::tempoSync,
                "Tempo Sync",
                false
                ));
    
    const juce::StringArray noteLengths = {
        "1/32",
        "1/16 trip",
        "1/32 dot",
        "1/16",
        "1/8 trip",
        "1/16 dot",
        "1/8",
        "1/4 trip",
        "1/8 dot",
        "1/4",
        "1/2 trip",
        "1/4 dot",
        "1/2",
        "1/1 trip",
        "1/2 dot",
        "1/1",
    };
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
                ParamIDs::delayNote,
                "DelayNote",
                noteLengths,
                9 //"1/4"
                ));
    
    return layout;
}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
    double duration = 0.02;
    gainSmoother.reset(sampleRate, duration);
    mixSmoother.reset(sampleRate, duration);
    
    coeff = 1.0f - std::exp(-1.0f / (0.2f * float(sampleRate)));
    
    feedbackSmoother.reset(sampleRate, duration);
    
    lowCutSmoother.reset(sampleRate, duration);
    highCutSmoother.reset(sampleRate, duration);
    invertStereoSmoother.reset(sampleRate, duration);
}

void Parameters::reset() noexcept
{
    gain = 0.0f;
    gainSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    
    delayTime = 0.0f;
    delayNote = 9;
    
    mix = 50.0f;
    mixSmoother.setCurrentAndTargetValue(mixParam->get() * 0.01f);
    
    feedback = 0.0f;
    feedbackSmoother.setCurrentAndTargetValue(feedbackParam->get() * 0.01f);
    
    flipFlop = false;
    invertStereo = 0.0f;
    invertStereoSmoother.setCurrentAndTargetValue(invertStereo);
    
    lowCut = 0.0f;
    lowCutSmoother.setCurrentAndTargetValue(lowCutParam->get());
    
    highCut = 0.0f;
    highCutSmoother.setCurrentAndTargetValue(highCutParam->get());
    
    accelerateMode = 0;
    decelerateMode = 0;
}

void Parameters::update() noexcept
{
    gainSmoother.setTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    
    targetDelayTime = delayTimeParam->get();
    if (delayTime == 0.0f) {
        delayTime = targetDelayTime;
    }
    
    accelerateMode = accelerateModeParam->getIndex();
    decelerateMode = decelerateModeParam->getIndex();
    
    mixSmoother.setTargetValue(mixParam->get() * 0.01f);
    
    feedbackSmoother.setTargetValue(feedbackParam->get() * 0.01f);
    
    lowCutSmoother.setTargetValue(lowCutParam->get());
    highCutSmoother.setTargetValue(highCutParam->get());
    
    invertStereoSmoother.setTargetValue(flipFlopParam->get() ? 1.0f : 0.0f);
    
    delayNote = delayNoteParam->getIndex();
    tempoSync = tempoSyncParam->get();
}

void Parameters::smoothen() noexcept
{
    gain = gainSmoother.getNextValue();
    
    delayTime += (targetDelayTime - delayTime) * coeff;
    
    mix = mixSmoother.getNextValue();
    
    feedback = feedbackSmoother.getNextValue();
    
    lowCut = lowCutSmoother.getNextValue();
    highCut = highCutSmoother.getNextValue();
    
    invertStereo = invertStereoSmoother.getNextValue();
}
