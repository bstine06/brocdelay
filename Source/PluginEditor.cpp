/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayAudioProcessorEditor::DelayAudioProcessorEditor (DelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    delayGroup.setText("Delay");
    delayGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    delayGroup.addAndMakeVisible(delayTimeKnob);
    delayGroup.addAndMakeVisible(delayNoteKnob);
    delayGroup.addAndMakeVisible(tempoSyncSwitch);
    shiftModesGroup.setText("Shift Modes");
    shiftModesGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    shiftModesGroup.addAndMakeVisible(accelerateModeKnob);
    shiftModesGroup.addAndMakeVisible(decelerateModeKnob);
    delayGroup.addAndMakeVisible(shiftModesGroup);
    addAndMakeVisible(delayGroup);
    
    feedbackGroup.setText("Feedback");
    feedbackGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    feedbackGroup.addAndMakeVisible(feedbackKnob);
    feedbackGroup.addAndMakeVisible(flipFlopSwitch);
    feedbackGroup.addAndMakeVisible(lowCutKnob);
    feedbackGroup.addAndMakeVisible(highCutKnob);
    addAndMakeVisible(feedbackGroup);
    
    
    
    outputGroup.setText("Output");
    outputGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    outputGroup.addAndMakeVisible(gainKnob);
    outputGroup.addAndMakeVisible(mixKnob);
    addAndMakeVisible(outputGroup);
    
    footerCompliment.setText("you're doing a good job!!", juce::dontSendNotification);
    footerCompliment.setColour(juce::Label::textColourId, Colors::Footer::compliment);
    footerCompliment.setJustificationType(juce::Justification::centred);
    footerGroup.addAndMakeVisible(footerCompliment);
    footerCompliment.setLookAndFeel(&footerLookAndFeel);
    addAndMakeVisible(footerGroup);
    
    setLookAndFeel(&mainLookAndFeel);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (610, 430);
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    footerCompliment.setLookAndFeel(nullptr);
}

//==============================================================================
void DelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colors::background);
    
    auto rect = getLocalBounds().withHeight(40);
    g.setColour(Colors::header);
    g.fillRect(rect);
    
    auto image = juce::ImageCache::getFromMemory(BinaryData::Logo_png, BinaryData::Logo_pngSize);
    
    int destWidth = image.getWidth() / 2;
    int destHeight = image.getHeight() / 2;
    g.drawImage(image, getWidth() / 2 - destWidth / 2, 0, destWidth, destHeight, 0, 0, image.getWidth(), image.getHeight());
}

void DelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto bounds = getLocalBounds();
    
    int y = 50;
    int height = bounds.getHeight();
    int row1Height = 280;
    
    // Position the groups
    delayGroup.setBounds(10, y, 210, row1Height);
    
    outputGroup.setBounds(bounds.getWidth() - 160, y, 150, row1Height);
    
    feedbackGroup.setBounds(delayGroup.getRight() + 10, y, outputGroup.getX() - delayGroup.getRight() - 20, row1Height);
    
    footerGroup.setBounds(shiftModesGroup.getWidth() + 10, delayGroup.getBottom(), bounds.getWidth() - 20, 90);
    
    delayTimeKnob.setTopLeftPosition(20, 20);
    tempoSyncSwitch.setTopLeftPosition(delayTimeKnob.getRight() + 30, delayTimeKnob.getY());
    delayNoteKnob.setTopLeftPosition(delayTimeKnob.getX(), delayTimeKnob.getBottom() + 20);
    shiftModesGroup.setBounds(tempoSyncSwitch.getX()-20, tempoSyncSwitch.getBottom()-15, delayGroup.getWidth() - delayTimeKnob.getWidth() - 30, row1Height - tempoSyncSwitch.getHeight()-5);

    accelerateModeKnob.setTopLeftPosition(20, 20);
    decelerateModeKnob.setTopLeftPosition(accelerateModeKnob.getX(), accelerateModeKnob.getBottom()+5);

    feedbackKnob.setTopLeftPosition(20, 20);
    flipFlopSwitch.setTopLeftPosition(feedbackKnob.getRight() + 30, feedbackKnob.getY());
    lowCutKnob.setTopLeftPosition(feedbackKnob.getX(), feedbackKnob.getBottom() + 20);
    highCutKnob.setTopLeftPosition(lowCutKnob.getRight() + 30, lowCutKnob.getY());
    
    mixKnob.setTopLeftPosition(20, 20);
    gainKnob.setTopLeftPosition(mixKnob.getX(), mixKnob.getBottom() + 20);
    
    
    footerCompliment.setBounds(footerGroup.getLocalBounds().reduced(10));

    
    
}
