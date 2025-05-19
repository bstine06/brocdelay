/*
  ==============================================================================

    LookAndFeel.h
    Created: 30 Apr 2025 9:02:30pm
    Author:  Brett

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Colors
{
    const juce::Colour background { 0, 100, 0 };
    const juce::Colour header { 0, 60, 0 };

    namespace Knob
    {
        const juce::Colour trackBackground { 100, 160, 100 };
        const juce::Colour trackActive { 150, 220, 150 };
        const juce::Colour outline { 255, 250, 245 };
        const juce::Colour gradientTop { 240, 250, 240 };
        const juce::Colour gradientBottom { 210, 235, 210 };
        const juce::Colour dial { 0, 150, 0 };
        const juce::Colour dropShadow { 195, 190, 185 };
        const juce::Colour label { 255, 255, 255 };
        const juce::Colour textBoxBackground { 0, 60, 0 };
        const juce::Colour value { 240, 240, 240 };
        const juce::Colour caret { 255, 255, 255 };
    }

    namespace Group
    {
        const juce::Colour label { 150, 220, 150 };
        const juce::Colour outline { 235, 230, 225 };
    }

    namespace Footer
    {
        const juce::Colour compliment { 150, 220, 150 };	
    }
}

class Fonts
{
public:
    static juce::Font getFont(float height = 16.0f);
    
    Fonts() = delete;
    
private:
    static const juce::Typeface::Ptr typeface;
};

class RotaryKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RotaryKnobLookAndFeel();
    
    static RotaryKnobLookAndFeel* get()
    {
        static RotaryKnobLookAndFeel instance;
        return &instance;
    }
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
    
    juce::Font getLabelFont(juce::Label&) override;
    
    juce::Label* createSliderTextBox(juce::Slider&) override;
    
    void drawTextEditorOutline(juce:: Graphics&, int, int, juce::TextEditor&) override {}
    
    void fillTextEditorBackground(juce::Graphics&, int width, int height, juce::TextEditor&) override;
    
private:
    juce::DropShadow dropShadow { Colors::Knob::dropShadow, 6, { 0, 3 } };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnobLookAndFeel)
};

class SwitchLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SwitchLookAndFeel();
    
    static SwitchLookAndFeel* get()
    {
        static SwitchLookAndFeel instance;
        return &instance;
    }
    
    void drawToggleButton(juce::Graphics& g,
                          juce::ToggleButton& toggleButton,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;
    
private:
    juce::DropShadow dropShadow { Colors::Knob::dropShadow, 6, { 0, 3 } };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SwitchLookAndFeel)
};

class FooterLookAndFeel : public juce::LookAndFeel_V4
{
public:
    FooterLookAndFeel();
    
    juce::Font getLabelFont(juce::Label&) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FooterLookAndFeel)
};

class MainLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MainLookAndFeel();
    
    juce::Font getLabelFont(juce::Label&) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLookAndFeel)
};
