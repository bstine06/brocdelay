/*
  ==============================================================================

    RotaryKnob.h
    Created: 29 Apr 2025 5:14:16pm
    Author:  Brett

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class RotaryKnob  : public juce::Component
{
public:
    RotaryKnob(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID, const juce::String& tooltipText, bool drawFromMiddle = false);
    ~RotaryKnob() override;
    
    juce::Slider slider;
    juce::Label label;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
    
    void mouseEnter (const juce::MouseEvent &event) override;
    void mouseExit (const juce::MouseEvent &event) override;

    void resized() override;
    
    std::function<void(const juce::String&)> onHover;
    
private:
    
    juce::String tooltipString;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RotaryKnob)
};
