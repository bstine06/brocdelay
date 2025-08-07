/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 30 Apr 2025 9:02:30pm
    Author:  Brett

  ==============================================================================
*/

#include "LookAndFeel.h"

const juce::Typeface::Ptr Fonts::typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::LatoMedium_ttf, BinaryData::LatoMedium_ttfSize);

juce::Font Fonts::getFont(float height)
{
    return juce::FontOptions(typeface).withMetricsKind(juce::TypefaceMetricsKind::legacy)
        .withHeight(height);
}

//===========================================================
//Switch

SwitchLookAndFeel::SwitchLookAndFeel()
{
    setColour(juce::Label::textColourId, Colors::Knob::label);
}

void SwitchLookAndFeel::drawToggleButton(juce::Graphics& g,
                                         juce::ToggleButton& toggleButton,
                                         bool shouldDrawButtonAsHighlighted,
                                         bool shouldDrawButtonAsDown)
{
    
    auto bounds = toggleButton.getLocalBounds().toFloat();
    
    float boundReduction = 7.0f;
    auto buttonBounds = bounds.reduced(boundReduction, boundReduction);
    
    // draw a drop shadow
    // Expand bounds to give room for the shadow blur
    if (toggleButton.getToggleState()) {
        juce::Path shadowPath;
        shadowPath.addRoundedRectangle(bounds.reduced(5.0f, 5.0f), 15.0f);
        dropShadow.drawForPath(g, shadowPath);
    }
    


    // Background fill based on toggle state
    if (toggleButton.getToggleState()) {
        auto gradient = juce::ColourGradient(
                                             Colors::Switch::activeGradientTop, 0.0f, buttonBounds.getY() - 5.0f,
                                             Colors::Switch::activeGradientBottom, 0.0f, buttonBounds.getBottom() + 5.0f, false);
        g.setGradientFill(gradient); // ON state
    } else {
        auto gradient = juce::ColourGradient(
                                             Colors::Switch::inactiveGradientTop, 0.0f, buttonBounds.getY() + 5.0f,
                                             Colors::Switch::inactiveGradientBottom, 0.0f, buttonBounds.getBottom() - 5.0f, false);
        g.setGradientFill(gradient); // OFF state
    }

    g.fillRoundedRectangle(buttonBounds, 12.0f);  // Rounded corners

//    // border
    g.setColour(juce::Colours::black);
//    g.drawRoundedRectangle(bounds, 25.0f, 1.5f);
    
    g.setColour(Colors::background);
    
    g.drawFittedText(toggleButton.getToggleState() ? "ON" : "OFF",
                     bounds.toNearestInt(), juce::Justification::centred, 2);

    // Text
    g.setColour(juce::Colours::white);
    g.setFont(Fonts::getFont(16));
    
}

//===========================================================
//Rotary Knob

RotaryKnobLookAndFeel::RotaryKnobLookAndFeel()
{
    setColour(juce::Label::textColourId, Colors::Knob::label);
    setColour(juce::Slider::textBoxTextColourId, Colors::Knob::label);
    setColour(juce::Slider::rotarySliderFillColourId, Colors::Knob::trackActive);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::CaretComponent::caretColourId, Colors::Knob::caret);
}

void RotaryKnobLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, [[maybe_unused]] int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    // draw the circular knob within the supplied bounds
    auto bounds = juce::Rectangle<int>(x, y, width, width).toFloat();
    auto knobRect = bounds.reduced(10.0f, 10.0f);
    
    // draw a drop shadow
    auto path = juce::Path();
    path.addEllipse(knobRect);
    dropShadow.drawForPath(g, path);
    
    g.setColour(Colors::Knob::outline);
    g.fillEllipse(knobRect);
    
    // draw a subtle gradient on the knob to give it some shape
    auto innerRect = knobRect.reduced(2.0f, 2.0f);
    auto gradient = juce::ColourGradient(
                                         Colors::Knob::gradientTop, 0.0f, innerRect.getY(),
                                         Colors::Knob::gradientBottom, 0.0f, innerRect.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillEllipse(innerRect);
    
    // initialize values for track around the dial
    auto center = bounds.getCentre();
    auto radius = bounds.getWidth() / 2.0f;
    auto lineWidth = 3.0f;
    auto arcRadius = radius - lineWidth/2.0f;
    
    // draw the path for the arc track around the dial
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(center.x, center.y, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    
    auto strokeType = juce::PathStrokeType(lineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    g.setColour(Colors::Knob::trackBackground);
    g.strokePath(backgroundArc, strokeType);
    
    // initialize values for the dial line that points at current value
    auto dialRadius = innerRect.getHeight() / 2.0f - lineWidth;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    
    juce::Point<float> dialStart(center.x + 15.0f * std::sin(toAngle),
                                 center.y - 15.0f * std::cos(toAngle));
    juce::Point<float> dialEnd(center.x + dialRadius * std::sin(toAngle),
                               center.y - dialRadius * std::cos(toAngle));
    
    //draw the dial line that points at the current value
    juce::Path dialPath;
    dialPath.startNewSubPath(dialStart);
    dialPath.lineTo(dialEnd);
    g.setColour(Colors::Knob::dial);
    g.strokePath(dialPath, strokeType);
    
    //draw the arc fill that also shows the current value / how much of the effect is applied
    if (slider.isEnabled()) {
        float fromAngle = rotaryStartAngle;
        if (slider.getProperties()["drawFromMiddle"]) {
            fromAngle += (rotaryEndAngle - rotaryStartAngle) / 2.0f;
        }
        juce::Path valueArc;
        valueArc.addCentredArc(center.x, center.y, arcRadius, arcRadius, 0.0f, fromAngle, toAngle, true);
        g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
        g.strokePath(valueArc, strokeType);
    }
}

juce::Font RotaryKnobLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label)
{
    return Fonts::getFont();
}

void RotaryKnobLookAndFeel::fillTextEditorBackground(juce::Graphics& g, [[maybe_unused]] int width, [[maybe_unused]] int height, juce::TextEditor& textEditor)
{
    g.setColour(Colors::Knob::textBoxBackground);
    g.fillRoundedRectangle(textEditor.getLocalBounds().reduced(4, 0).toFloat(), 4.0f);
}

class RotaryKnobLabel : public juce::Label
{
public:
    RotaryKnobLabel() : juce::Label() {}
    
    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&) override {}
    
    std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override
    {
        return createIgnoredAccessibilityHandler(*this);
    }
    
    juce::TextEditor* createEditorComponent() override
    {
        auto* ed = new juce::TextEditor(getName());
        ed->applyFontToAllText(getLookAndFeel().getLabelFont(*this));
        copyAllExplicitColoursTo(*ed);
        
        ed->setBorder(juce::BorderSize<int>());
        ed->setIndents(2, 1);
        ed->setJustification(juce::Justification::centredTop);
        
        ed->setPopupMenuEnabled(false);
        ed->setInputRestrictions(8);
        
        return ed;
    }
};

juce::Label* RotaryKnobLookAndFeel::createSliderTextBox(juce::Slider& slider)
{
    auto l = new RotaryKnobLabel();
    l->setJustificationType(juce::Justification::centred);
    l->setKeyboardType(juce::TextInputTarget::decimalKeyboard);
    l->setColour(juce::Label::textColourId, slider.findColour(juce::Slider::textBoxTextColourId));
    l->setColour(juce::TextEditor::textColourId, Colors::Knob::value);
    l->setColour(juce::TextEditor::highlightedTextColourId, Colors::Knob::value);
    l->setColour(juce::TextEditor::highlightColourId, Colors::Knob::trackBackground);
    l->setColour(juce::TextEditor::backgroundColourId, Colors::Knob::textBoxBackground);
    return l;
}

//===========================================================
//Horizontal Slider

HorizontalSliderLookAndFeel::HorizontalSliderLookAndFeel()
{
    setColour(juce::Label::textColourId, Colors::Knob::label);
}

void HorizontalSliderLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle sliderStyle, juce::Slider& slider)
{
    // Draw knob
    float knobRadius = 10.0f;
    
    // Draw track
    auto trackHeight = knobRadius*2.0f;
    auto trackY = y + height / 2.0f - trackHeight / 2.0f;
    g.setColour(Colors::Knob::trackBackground);
    g.fillRoundedRectangle((float)x - knobRadius, trackY, (float)width + knobRadius*2.0f, trackHeight, 4.0f);
    
    float knobX = sliderPos - knobRadius;
    float knobY = y + height / 2.0f - knobRadius;
    g.setColour(juce::Colours::white);
    g.fillRoundedRectangle(knobX, knobY, knobRadius * 2.0f, knobRadius * 2.0f, 4.0f);

       
}

void HorizontalSliderLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.setColour(label.findColour(juce::Label::textColourId));
    g.setFont(getLabelFont(label));
    g.drawFittedText(label.getText(), label.getLocalBounds(), label.getJustificationType(), 1);
}

juce::Font HorizontalSliderLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label)
{
    return Fonts::getFont();
}

class HorizontalSliderLabel : public juce::Label
{
public:
    HorizontalSliderLabel() : juce::Label() {}
    
    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&) override {}
    
    std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override
    {
        return createIgnoredAccessibilityHandler(*this);
    }
};

//===========================================================
//Main

MainLookAndFeel::MainLookAndFeel()
{
    setColour(juce::GroupComponent::textColourId, Colors::Group::label);
    setColour(juce::GroupComponent::outlineColourId, Colors::Group::outline);
}

juce::Font MainLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label)
{
    return Fonts::getFont();
}

//===========================================================
//Footer

FooterLookAndFeel::FooterLookAndFeel()
{
    setColour(juce::Label::textColourId, Colors::Footer::footerText);
}

juce::Font FooterLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label)
{
    return Fonts::getFont(18.0f);
}
