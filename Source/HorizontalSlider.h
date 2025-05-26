/*
  ==============================================================================

    HorizontalSlider.h
    Created: 25 May 2025 10:27:40pm
    Author:  Brett

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class HorizontalSlider  : public juce::Component
{
public:
    HorizontalSlider(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID);
    ~HorizontalSlider() override;
    
    juce::Slider slider;
    juce::Label label;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;

    void resized() override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HorizontalSlider)
};

