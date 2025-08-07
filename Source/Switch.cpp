/*
  ==============================================================================

    Switch.cpp
    Created: 18 May 2025 11:18:52pm
    Author:  Brett

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Switch.h"
#include "LookAndFeel.h"

//==============================================================================
Switch::Switch(const juce::String& text,
               juce::AudioProcessorValueTreeState& apvts,
               const juce::ParameterID& parameterID,
               const juce::String& tooltipText) :
                    attachment(apvts, parameterID.getParamID(), toggleButton),
                    tooltipString(tooltipText)
{
    
    toggleButton.setButtonText(text);
    toggleButton.setBounds(0,0,70,70);
    toggleButton.setClickingTogglesState(true);
    addAndMakeVisible(toggleButton);
    
    label.setText(text, juce::NotificationType::dontSendNotification);
    label.setJustificationType(juce::Justification::horizontallyCentred);
    label.setBorderSize(juce::BorderSize<int>{ 0,0,2,0 });
    label.attachToComponent(&toggleButton, false);
    addAndMakeVisible(label);
    
    setLookAndFeel(SwitchLookAndFeel::get());
    
    addMouseListener(this, true);
    
    setSize(70,110);
    
}

Switch::~Switch()
{
}

void Switch::resized()
{
    toggleButton.setTopLeftPosition(0,24);
}

void Switch::mouseEnter(const juce::MouseEvent &event) {
    DBG(tooltipString);
    if (onHover)
        onHover(tooltipString);
}

void Switch::mouseExit(const juce::MouseEvent &event) {
    //DBG("Mouse Exit Switch!");
    if (onHover)
        onHover("");
}
