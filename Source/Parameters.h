/*
  ==============================================================================

    Parameters.h
    Created: 26 Apr 2025 3:11:09pm
    Author:  Brett

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace ParamIDs
{
    static const juce::ParameterID gain { "gain", 1 };
    static const juce::ParameterID delayTime { "delayTime", 1 };
    static const juce::ParameterID mix { "mix", 1 };
    static const juce::ParameterID feedback { "feedback", 1 };
    // add more Parameter IDs here as needed
}

class Parameters
{
public:
        
    Parameters(juce::AudioProcessorValueTreeState& apvts);
    
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    static constexpr float minDelayTime = 5.0f;
    static constexpr float maxDelayTime = 5000.f;
    
    void prepareToPlay(double sampleRate) noexcept;
    void reset() noexcept;
    void update() noexcept;
    void smoothen() noexcept;
    
    float gain = 0.0f;
    float delayTime = 1.0f;
    float mix = 1.0f;
    float feedback = 0.0f;
    
private:
    
    juce::AudioParameterFloat* gainParam;
    juce::LinearSmoothedValue<float> gainSmoother;
    
    juce::AudioParameterFloat* delayTimeParam;
    float targetDelayTime = 0.0f;
    float coeff = 0.0f;
    
    juce::AudioParameterFloat* mixParam;
    juce::LinearSmoothedValue<float> mixSmoother;
    
    juce::AudioParameterFloat* feedbackParam;
    juce::LinearSmoothedValue<float> feedbackSmoother;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Parameters)
};
