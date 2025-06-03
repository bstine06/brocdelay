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
#include "HorizontalSlider.h"
#include "Switch.h"
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
    RotaryKnob delayNoteKnob { "Delay Note", audioProcessor.apvts, ParamIDs::delayNote };
    RotaryKnob feedbackKnob { "Feedback", audioProcessor.apvts, ParamIDs::feedback, true };
    Switch flipFlopSwitch { "Flip Flop", audioProcessor.apvts, ParamIDs::flipFlop, true };
    RotaryKnob lowCutKnob { "Low Cut", audioProcessor.apvts, ParamIDs::lowCut };
    RotaryKnob highCutKnob { "High Cut", audioProcessor.apvts, ParamIDs::highCut };
    
    HorizontalSlider accelerateModeKnob { "Accelerate", audioProcessor.apvts, ParamIDs::accelerateMode };
    HorizontalSlider decelerateModeKnob { "Decelerate", audioProcessor.apvts, ParamIDs::decelerateMode };
    
    Switch tempoSyncSwitch { "Tempo Sync", audioProcessor.apvts, ParamIDs::tempoSync, true };
    
    juce::Label footerCompliment;
    FooterLookAndFeel footerLookAndFeel;
    
    MainLookAndFeel mainLookAndFeel;
    
    juce::GroupComponent delayGroup, shiftModesGroup, feedbackGroup, outputGroup, footerGroup;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
