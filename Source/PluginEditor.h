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
#include "LevelMeter.h"

//==============================================================================
/**
*/


//A tiny class to draw the Broc character
class BrocHelperComponent : public juce::Component
{
public:
    void paint(juce::Graphics& g) override
    {
        auto broc = juce::ImageCache::getFromMemory(BinaryData::broc_png, BinaryData::broc_pngSize);

        // Scale to fit component bounds
        g.drawImageWithin(broc, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::centred);
    }
};



class DelayAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                   private juce::AudioProcessorParameter::Listener
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
    void parameterValueChanged(int, float) override;
    void parameterGestureChanged(int, bool) override { }
    
    void updateDelayKnobs(bool tempoSyncActive);
    
    template <typename HoverableComponent>
    void setTooltipCallbackFor(HoverableComponent& comp, juce::Label& targetLabel)
    {
        comp.onHover = [&targetLabel, this](const juce::String& tooltipText)
        {
            targetLabel.setText(
                tooltipText.isEmpty() ? getRandomQuote() : tooltipText,
                juce::dontSendNotification
            );
        };
    }
    
    std::vector<juce::String> brocQuotes;

    void loadBrocQuotes()
    {
        auto input = juce::MemoryInputStream(BinaryData::brocQuotes_txt, BinaryData::brocQuotes_txtSize, false);
        juce::StringArray lines;
        lines.addLines(input.readString());

        for (const auto& line : lines)
            if (line.isNotEmpty())
                brocQuotes.push_back(line);
    }
    
    juce::String getRandomQuote() const
    {
        if (brocQuotes.empty()) return "You're doing a great job!";

        auto index = juce::Random::getSystemRandom().nextInt((int)brocQuotes.size());
        return brocQuotes[index];
    }

    
    DelayAudioProcessor& audioProcessor;
    
    BrocHelperComponent brocHelper;
    
    RotaryKnob gainKnob { "Gain", audioProcessor.apvts, ParamIDs::gain, "The gain knob controls the overall volume output.", true };
    RotaryKnob mixKnob { "Mix", audioProcessor.apvts, ParamIDs::mix, "The mix knob controls how much of the delay effect is applied." };
    RotaryKnob delayTimeKnob { "Delay Time", audioProcessor.apvts, ParamIDs::delayTime, "Delay time affects how long the signal takes to repeat in milliseconds or seconds" };
    RotaryKnob delayNoteKnob { "Delay Note", audioProcessor.apvts, ParamIDs::delayNote, "Delay note affects how long the signal takes to repeat with respect to your project's tempo" };
    RotaryKnob feedbackKnob { "Feedback", audioProcessor.apvts, ParamIDs::feedback, "Feedback controls how loud each repetition from the delay is compared to the last. Negative feedback will flip the polarity on each repetition. Be careful with values above 100% or below -100%!", true };
    Switch flipFlopSwitch { "Flip Flop", audioProcessor.apvts, ParamIDs::flipFlop, "Engaging the flip flop switch will reverse the stereo field on each repetition of the delay" };
    RotaryKnob lowCutKnob { "Low Cut", audioProcessor.apvts, ParamIDs::lowCut, "The low cut knob controls how much low-end to remove from the delayed signal" };
    RotaryKnob highCutKnob { "High Cut", audioProcessor.apvts, ParamIDs::highCut, "The high cut knob controls how much high-end to remove from the delayed signal" };
    
    HorizontalSlider accelerateModeKnob { "Accelerate", audioProcessor.apvts, ParamIDs::accelerateMode, "The accelerate selector changes which algorithm is used to make the delay time shorter (speed it up). You'll have to also move the delay time/note to hear this make any difference!" };
    HorizontalSlider decelerateModeKnob { "Decelerate", audioProcessor.apvts, ParamIDs::decelerateMode, "The decelerate selector changes which algorithm is used to make the delay time longer (slow it down). You'll have to also move the delay time/note to hear this make any difference!" };
    
    Switch tempoSyncSwitch { "Tempo Sync", audioProcessor.apvts, ParamIDs::tempoSync, "Toggle the tempo sync to use perfect divisions of your project's tempo as the delay time" };
    
    LevelMeter meter;
    
    juce::Label footerText;
    FooterLookAndFeel footerLookAndFeel;
    
    MainLookAndFeel mainLookAndFeel;
    
    juce::GroupComponent delayGroup, shiftModesGroup, feedbackGroup, outputGroup, footerGroup;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
