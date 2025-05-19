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

// Constructor
Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
    castParameter(apvts, ParamIDs::gain, gainParam);
    castParameter(apvts, ParamIDs::delayTime, delayTimeParam);
    castParameter(apvts, ParamIDs::mix, mixParam);
    castParameter(apvts, ParamIDs::feedback, feedbackParam);
    castParameter(apvts, ParamIDs::inputStereo, inputStereoParam);
    castParameter(apvts, ParamIDs::flipFlop, flipFlopParam);
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
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
               //Parameter ID
               ParamIDs::inputStereo,
               //parameter display name
               "Stereo",
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
    
    return layout;
}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
    double duration = 0.02;
    gainSmoother.reset(sampleRate, duration);
    mixSmoother.reset(sampleRate, duration);
    
    coeff = 1.0f - std::exp(-1.0f / (0.2f * float(sampleRate)));
    
    feedbackSmoother.reset(sampleRate, duration);
    inputStereoSmoother.reset(sampleRate, duration);
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
    
    inputStereo = 100.0f;
    inputStereoSmoother.setCurrentAndTargetValue(inputStereoParam->get() * 0.01f);
    
    flipFlop = 0.0f;
}

void Parameters::update() noexcept
{
    gainSmoother.setTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    
    targetDelayTime = delayTimeParam->get();
    if (delayTime == 0.0f) {
        delayTime = targetDelayTime;
    }
    
    mixSmoother.setTargetValue(mixParam->get() * 0.01f);
    
    feedbackSmoother.setTargetValue(feedbackParam->get() * 0.01f);
    
    inputStereoSmoother.setCurrentAndTargetValue(inputStereoParam->get() * 0.01f);
}

void Parameters::smoothen() noexcept
{
    gain = gainSmoother.getNextValue();
    
    delayTime += (targetDelayTime - delayTime) * coeff;
    
    mix = mixSmoother.getNextValue();
    
    feedback = feedbackSmoother.getNextValue();
    
    inputStereo = inputStereoSmoother.getNextValue();
}
