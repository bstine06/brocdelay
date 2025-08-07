/*
  ==============================================================================

    Switch.h
    Created: 18 May 2025 11:18:52pm
    Author:  Brett

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class Switch  : public juce::Component
{
public:
    
    Switch(const juce::String& text,
           juce::AudioProcessorValueTreeState& apvts,
           const juce::ParameterID& parameterID,
           const juce::String& tooltipText);
    
    ~Switch() override;
    
    juce::ToggleButton toggleButton;
    juce::Label label;
    juce::AudioProcessorValueTreeState::ButtonAttachment attachment;
    
    void mouseEnter (const juce::MouseEvent &event) override;
    void mouseExit (const juce::MouseEvent &event) override;

    void resized() override;
    
    std::function<void(const juce::String&)> onHover;
    
private:
    
    juce::String tooltipString;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Switch)
};

