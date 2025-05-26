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

const juce::StringArray delayModeNames = {
    "Repitch",
    "Fade",
    "Jump"
};

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

static int delayModeToIndex(DelayMode mode)
{
    return static_cast<int>(mode);
}

static DelayMode indexToDelayMode(int index)
{
    index = std::clamp(index, 0, static_cast<int>(delayModeNames.size() - 1));
    return static_cast<DelayMode>(index);
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
    castParameter<juce::AudioParameterChoice*>(apvts, ParamIDs::accelerateMode, accelerateModeParam);
    castParameter<juce::AudioParameterChoice*>(apvts, ParamIDs::decelerateMode, decelerateModeParam);
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
               100.0f,
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
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
                ParamIDs::accelerateMode,
                "Accelerate Mode",
                delayModeNames,
                delayModeToIndex(DelayMode::Repitch),
                ""
                ));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
                ParamIDs::decelerateMode,
                "Decelerate Mode",
                delayModeNames,
                delayModeToIndex(DelayMode::Repitch),
                ""
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
    
    mix = 1.0f;
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
    
    accelerateMode = DelayMode::Repitch;
    decelerateMode = DelayMode::Repitch;
}

void Parameters::update() noexcept
{
    gainSmoother.setTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    
    targetDelayTime = delayTimeParam->get();
    if (delayTime == 0.0f) {
        delayTime = targetDelayTime;
    }
    
    accelerateMode = indexToDelayMode(accelerateModeParam->getIndex());
    decelerateMode = indexToDelayMode(decelerateModeParam->getIndex());
    
    mixSmoother.setTargetValue(mixParam->get() * 0.01f);
    
    feedbackSmoother.setTargetValue(feedbackParam->get() * 0.01f);
    
    lowCutSmoother.setTargetValue(lowCutParam->get());
    highCutSmoother.setTargetValue(highCutParam->get());
    
    invertStereoSmoother.setTargetValue(flipFlopParam->get() ? 1.0f : 0.0f);
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
