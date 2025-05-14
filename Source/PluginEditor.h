/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Parameters.h"
#include "RotaryKnob.h"
#include "LookAndFeel.h"

//==============================================================================
/**
*/
class DelayAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DelayAudioProcessorEditor (DelayAudioProcessor&);
    ~DelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DelayAudioProcessor& audioProcessor;
    
    RotaryKnob gainKnob { "Gain", audioProcessor.apvts, ParamIDs::gain, true };
    RotaryKnob mixKnob { "Mix", audioProcessor.apvts, ParamIDs::mix };
    RotaryKnob delayTimeKnob { "Delay Time", audioProcessor.apvts, ParamIDs::delayTime };
    RotaryKnob feedbackKnob { "Feedback", audioProcessor.apvts, ParamIDs::feedback };
    
    juce::Label footerCompliment;
    FooterLookAndFeel footerLookAndFeel;
    
    MainLookAndFeel mainLookAndFeel;
    
    juce::GroupComponent delayGroup, feedbackGroup, outputGroup, footerGroup;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
