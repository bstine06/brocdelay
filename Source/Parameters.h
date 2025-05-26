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
    static const juce::ParameterID accelerateMode { "accelerateMode", 1 };
    static const juce::ParameterID decelerateMode { "decelerateMode", 1 };
    static const juce::ParameterID mix { "mix", 1 };
    static const juce::ParameterID feedback { "feedback", 1 };
    static const juce::ParameterID flipFlop { "flipFlop", 1 };
    static const juce::ParameterID lowCut { "lowCut", 1 };
    static const juce::ParameterID highCut { "highCut", 1 };
    // add more Parameter IDs here as needed
}

enum class DelayMode
{
    Repitch,
    Fade,
    Jump
};

extern const juce::StringArray delayModeNames;

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
    
    bool flipFlop = false;
    float invertStereo = 0.0f;
    
    float lowCut = 20.0f;
    float highCut = 20000.0f;
    
    DelayMode accelerateMode = DelayMode::Repitch;
    DelayMode decelerateMode = DelayMode::Repitch;
    
private:
    
    juce::AudioParameterFloat* gainParam;
    juce::LinearSmoothedValue<float> gainSmoother;
    
    juce::AudioParameterFloat* delayTimeParam;
    float targetDelayTime = 0.0f;
    float coeff = 0.0f;
    
    juce::AudioParameterChoice* accelerateModeParam;
    juce::AudioParameterChoice* decelerateModeParam;
    
    juce::AudioParameterFloat* mixParam;
    juce::LinearSmoothedValue<float> mixSmoother;
    
    juce::AudioParameterFloat* feedbackParam;
    juce::LinearSmoothedValue<float> feedbackSmoother;
    
    juce::AudioParameterBool* flipFlopParam;
    juce::LinearSmoothedValue<float> invertStereoSmoother;
    
    juce::AudioParameterFloat* lowCutParam;
    juce::LinearSmoothedValue<float> lowCutSmoother;
    
    juce::AudioParameterFloat* highCutParam;
    juce::LinearSmoothedValue<float> highCutSmoother;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Parameters)
};
