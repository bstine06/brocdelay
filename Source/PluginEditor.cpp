/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayAudioProcessorEditor::DelayAudioProcessorEditor (DelayAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p), meter(p.levelL, p.levelR)
{
    
    delayGroup.setText("Delay");
    delayGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    delayGroup.addAndMakeVisible(delayTimeKnob);
    delayGroup.addChildComponent(delayNoteKnob);
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
    outputGroup.addAndMakeVisible(meter);
    addAndMakeVisible(outputGroup);
    
    loadBrocQuotes();
    footerGroup.addAndMakeVisible(brocHelper);
    footerText.setText(getRandomQuote(), juce::dontSendNotification);
    footerText.setColour(juce::Label::textColourId, Colors::Footer::footerText);
    footerText.setJustificationType(juce::Justification::centredLeft);
    footerText.setMinimumHorizontalScale(1.0f);
    footerGroup.addAndMakeVisible(footerText);
    footerText.setLookAndFeel(&footerLookAndFeel);
    addAndMakeVisible(footerGroup);
    
    setLookAndFeel(&mainLookAndFeel);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (590, 420);
    
    updateDelayKnobs(audioProcessor.params.tempoSyncParam->get());
    audioProcessor.params.tempoSyncParam->addListener(this);
    
    setTooltipCallbackFor(delayTimeKnob, footerText);
    setTooltipCallbackFor(delayNoteKnob, footerText);
    setTooltipCallbackFor(tempoSyncSwitch, footerText);
    setTooltipCallbackFor(accelerateModeKnob, footerText);
    setTooltipCallbackFor(decelerateModeKnob, footerText);
    setTooltipCallbackFor(feedbackKnob, footerText);
    setTooltipCallbackFor(flipFlopSwitch, footerText);
    setTooltipCallbackFor(lowCutKnob, footerText);
    setTooltipCallbackFor(highCutKnob, footerText);
    setTooltipCallbackFor(mixKnob, footerText);
    setTooltipCallbackFor(gainKnob, footerText);
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    footerText.setLookAndFeel(nullptr);
    
    audioProcessor.params.tempoSyncParam->removeListener(this);
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
    int row1Height = 270;
    
    // Position the groups
    delayGroup.setBounds(10, y, 200, row1Height);
    
    outputGroup.setBounds(bounds.getWidth() - 160, y, 150, row1Height);
    
    feedbackGroup.setBounds(delayGroup.getRight() + 10, y, outputGroup.getX() - delayGroup.getRight() - 20, row1Height);
    
    footerGroup.setBounds(shiftModesGroup.getWidth() + 10, delayGroup.getBottom(), bounds.getWidth() - 20, 90);
    
    delayTimeKnob.setTopLeftPosition(20, 20);
    tempoSyncSwitch.setTopLeftPosition(delayTimeKnob.getRight() + 20, delayTimeKnob.getY());
    delayNoteKnob.setTopLeftPosition(delayTimeKnob.getX(), delayTimeKnob.getY());
    shiftModesGroup.setBounds(0, delayTimeKnob.getBottom() + 20, delayGroup.getWidth(), row1Height - delayNoteKnob.getBottom()-20);

    accelerateModeKnob.setTopLeftPosition(20, 30);
    decelerateModeKnob.setTopRightPosition(shiftModesGroup.getWidth() - 20, 30);

    feedbackKnob.setTopLeftPosition(20, 20);
    flipFlopSwitch.setTopLeftPosition(feedbackKnob.getRight() + 20, feedbackKnob.getY());
    lowCutKnob.setTopLeftPosition(feedbackKnob.getX(), feedbackKnob.getBottom() + 10);
    highCutKnob.setTopLeftPosition(lowCutKnob.getRight() + 20, lowCutKnob.getY());
    
    mixKnob.setTopLeftPosition(20, 20);
    gainKnob.setTopLeftPosition(mixKnob.getX(), mixKnob.getBottom() + 10);
    meter.setBounds(outputGroup.getWidth() - 45, 30, 30, gainKnob.getBottom() - 30);
    

    
    auto footerBounds = footerGroup.getLocalBounds().reduced(10);

    auto brocWidth = 60;
    brocHelper.setBounds(footerBounds.removeFromLeft(brocWidth + 20));

    footerText.setBounds(footerBounds); // Remaining space

    
    
}

void DelayAudioProcessorEditor::parameterValueChanged(int, float value)
{
    DBG("parameter changed: " << value);
    if (juce::MessageManager::getInstance()->isThisTheMessageThread()) {
        updateDelayKnobs(value != 0.0f);
    } else {
        juce::MessageManager::callAsync([this, value]
        {
            updateDelayKnobs(value != 0.0f);
        });
    }
}

void DelayAudioProcessorEditor::updateDelayKnobs(bool tempoSyncActive)
{
    delayTimeKnob.setVisible(!tempoSyncActive);
    delayNoteKnob.setVisible(tempoSyncActive);
}
