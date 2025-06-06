/*
  ==============================================================================

    HorizontalSlider.cpp
    Created: 25 May 2025 10:27:40pm
    Author:  Brett

  ==============================================================================
*/

#include <JuceHeader.h>
#include "HorizontalSlider.h"
#include "LookAndFeel.h"

//==============================================================================
HorizontalSlider::HorizontalSlider(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID) : attachment(apvts, parameterID.getParamID(), slider)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 70, 16);
    slider.setBounds(0,0,70,36);
    addAndMakeVisible(slider);
    
    label.setText(text, juce::NotificationType::dontSendNotification);
    label.setJustificationType(juce::Justification::horizontallyCentred);
    label.setBorderSize(juce::BorderSize<int>{ 0, 0 , 0, 0 });
    label.attachToComponent(&slider, false);
    addAndMakeVisible(label);
    
    setLookAndFeel(HorizontalSliderLookAndFeel::get());
    
    setSize(70, 60);
    
}

HorizontalSlider::~HorizontalSlider()
{
}

void HorizontalSlider::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    slider.setTopLeftPosition(0, 24);

}
