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
    HorizontalSlider(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID, const juce::String& tooltipText);
    ~HorizontalSlider() override;
    
    juce::Slider slider;
    juce::Label label;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;

    void mouseEnter (const juce::MouseEvent &event) override;
    void mouseExit (const juce::MouseEvent &event) override;
    
    void resized() override;
    
    std::function<void(const juce::String&)> onHover;
    
private:
    
    juce::String tooltipString;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HorizontalSlider)
};

