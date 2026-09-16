#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class MetalKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider& /*slider*/) override
    {
        auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (8.0f);
        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto center = bounds.getCentre();

        g.setColour (juce::Colour (0x44000000));
        g.fillEllipse (bounds.translated (0.0f, 3.0f));

        juce::ColourGradient knobGrad (juce::Colour (0xffe6e9ef), center.x - radius, center.y - radius,
                                      juce::Colour (0xff9aa0ac), center.x + radius, center.y + radius, false);
        g.setGradientFill (knobGrad);
        g.fillEllipse (bounds);

        g.setColour (juce::Colour (0xffffffff).withAlpha (0.6f));
        g.drawEllipse (bounds, 1.0f);

        juce::Path p;
        auto pointerLength = radius * 0.75f;
        p.addRectangle (-1.5f, -radius, 3.0f, pointerLength * 0.5f);
        p.applyTransform (juce::AffineTransform::rotation (toAngle).translated (center.x, center.y));
        g.setColour (juce::Colour (0xff202226));
        g.fillPath (p);
    }
};

class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::Timer
{
public:
    AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
        : AudioProcessorEditor (&p), audioProcessor (p)
    {
        setSize (640, 480);
        setLookAndFeel (&metalLook);

        auto setupKnob = [this](juce::Slider& s) {
            s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            s.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            addAndMakeVisible (s);
        };

        setupKnob (sliderPunche);
        sliderPunche.setRange (1.0, 67.0, 0.1);
        sliderPunche.setValue (*audioProcessor.paramPunche);
        sliderPunche.onValueChange = [this] { *audioProcessor.paramPunche = (float)sliderPunche.getValue(); };

        setupKnob (sliderEfecto);
        sliderEfecto.setRange (0.0, 1.0, 0.01);
        sliderEfecto.setValue (*audioProcessor.paramEfecto);
        sliderEfecto.onValueChange = [this] { *audioProcessor.paramEfecto = (float)sliderEfecto.getValue(); };

        setupKnob (sliderArmonia);
        sliderArmonia.setRange (0.0, 1.0, 0.01);
        sliderArmonia.setValue (*audioProcessor.paramArmonia);
        sliderArmonia.onValueChange = [this] { *audioProcessor.paramArmonia = (float)sliderArmonia.getValue(); };

        setupKnob (sliderMezcla);
        sliderMezcla.setRange (0.0, 1.0, 0.01);
        sliderMezcla.setValue (*audioProcessor.paramMezcla);
        sliderMezcla.onValueChange = [this] { *audioProcessor.paramMezcla = (float)sliderMezcla.getValue(); };

        sliderSazon.setSliderStyle (juce::Slider::LinearHorizontal);
        sliderSazon.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        sliderSazon.setRange (1.0, 420.0, 1.0);
        sliderSazon.setValue (*audioProcessor.paramSazon);
        sliderSazon.onValueChange = [this] { *audioProcessor.paramSazon = (float)sliderSazon.getValue(); };
        addAndMakeVisible (sliderSazon);

        btnClaveSol.setButtonText ("CLAVE DE SOL");
        btnClaveSol.setClickingTogglesState (true);
        btnClaveSol.setToggleState (*audioProcessor.paramClaveSol, juce::dontSendNotification);
        btnClaveSol.onClick = [this] {
            *audioProcessor.paramClaveSol = btnClaveSol.getToggleState();
            repaint();
        };
        addAndMakeVisible (btnClaveSol);

        std::fill (std::begin (scopeData), std::end (scopeData), 0.0f);
        startTimerHz (30);
    }

    ~AudioPluginAudioProcessorEditor() override
    {
        setLookAndFeel (nullptr);
        stopTimer();
    }

    void timerCallback() override
    {
        if (audioProcessor.scopeUpdated.exchange (false))
        {
            for (int i = 0; i < AudioPluginAudioProcessor::scopeSize; ++i)
                scopeData[i] = audioProcessor.scopeBuffer[i];
            repaint();
        }
    }

    void paint (juce::Graphics& g) override
    {
        juce::ColourGradient metalBase (juce::Colour (0xffd2d7e0), 0, 0,
                                        juce::Colour (0xffbac0cc), (float)getWidth(), (float)getHeight(), false);
        g.setGradientFill (metalBase);
        g.fillAll();

        g.setColour (juce::Colour (0xff32353b));
        g.setFont (juce::FontOptions (24.0f, juce::Font::bold));
        g.drawText ("SUENAMEJOR - INADOR", 30, 20, getWidth() - 60, 30, juce::Justification::centredLeft);

        auto drawTicks = [&g](float cx, float cy, float radius) {
            g.setColour (juce::Colour (0x6632353b));
            for (int i = 0; i <= 10; ++i)
            {
                float angle = juce::MathConstants<float>::pi * 0.75f + (float)i / 10.0f * (juce::MathConstants<float>::pi * 1.5f);
                float x1 = cx + std::cos (angle) * (radius + 2.0f);
                float y1 = cy + std::sin (angle) * (radius + 2.0f);
                float x2 = cx + std::cos (angle) * (radius + 7.0f);
                float y2 = cy + std::sin (angle) * (radius + 7.0f);
                g.drawLine (x1, y1, x2, y2, 1.2f);
            }
        };

        drawTicks (85.0f, 130.0f, 42.0f);
        g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
        g.drawText ("PUNCHE", 35, 70, 100, 20, juce::Justification::centred);
        g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        g.drawText ("1", 38, 170, 20, 20, juce::Justification::centred);
        g.drawText ("67", 112, 170, 25, 20, juce::Justification::centred);

        drawTicks ((float)getWidth() - 85.0f, 130.0f, 42.0f);
        g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
        g.drawText ("EFECTO", getWidth() - 135, 70, 100, 20, juce::Justification::centred);
        g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        g.drawText ("POP", getWidth() - 145, 170, 30, 20, juce::Justification::centred);
        g.drawText ("INDIE", getWidth() - 60, 170, 35, 20, juce::Justification::centred);

        drawTicks (85.0f, 255.0f, 42.0f);
        g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
        g.drawText ("ARMON\xc3\x8da", 35, 195, 100, 20, juce::Justification::centred);

        drawTicks ((float)getWidth() - 85.0f, 255.0f, 42.0f);
        g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
        g.drawText ("MEZCLA", getWidth() - 135, 195, 100, 20, juce::Justification::centred);

        g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
        g.drawText ("SAZ\xc3\x93N", getWidth() / 2 - 50, 345, 100, 20, juce::Justification::centred);
        g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        g.drawText ("1", 175, 370, 20, 20, juce::Justification::centred);
        g.drawText ("420", getWidth() - 195, 370, 30, 20, juce::Justification::centred);

        auto screenRect = juce::Rectangle<float> (160.0f, 75.0f, (float)getWidth() - 320.0f, 230.0f);
        g.setColour (juce::Colour (0xff181a20));
        g.fillRoundedRectangle (screenRect, 8.0f);

        float sazonNorm = (float)(sliderSazon.getValue() - 1.0) / 419.0f;
        juce::Colour colorFrio (0xff3b1654);
        juce::Colour colorCalido (0xffb8440d);
        auto fondoColor = colorFrio.interpolatedWith (colorCalido, sazonNorm);

        juce::ColourGradient screenGrad (fondoColor.withAlpha (0.4f), screenRect.getX(), screenRect.getY(),
                                         fondoColor.withAlpha (0.9f), screenRect.getX(), screenRect.getBottom(), false);
        g.setGradientFill (screenGrad);
        g.fillRoundedRectangle (screenRect.reduced (3.0f), 6.0f);

        float puncheVal = (float)sliderPunche.getValue();
        float efectoVal = (float)sliderEfecto.getValue();
        float armoniaVal = (float)sliderArmonia.getValue();
        float mezclaVal = (float)sliderMezcla.getValue();

        juce::Path wavePath;
        float midY = screenRect.getCentreY();
        float step = screenRect.getWidth() / (float)AudioPluginAudioProcessor::scopeSize;

        for (int i = 0; i < AudioPluginAudioProcessor::scopeSize; ++i)
        {
            float rawSample = scopeData[i];
            float processedSample = std::tanh (rawSample * (1.0f + (puncheVal / 67.0f) * 4.0f));
            processedSample += std::sin ((float)i * 0.4f) * 0.15f * armoniaVal;
            processedSample *= (1.0f + (efectoVal - 0.5f) * 0.5f);

            float finalSample = rawSample + (processedSample - rawSample) * mezclaVal;

            float x = screenRect.getX() + (float)i * step;
            float y = midY - finalSample * (screenRect.getHeight() * 0.42f);

            if (i == 0) wavePath.startNewSubPath (x, y);
            else        wavePath.lineTo (x, y);
        }

        g.setColour (juce::Colour (0xffffffff).withAlpha (0.9f));
        g.strokePath (wavePath, juce::PathStrokeType (1.8f));

        if (btnClaveSol.getToggleState())
        {
            g.setColour (juce::Colour (0xffff8c00));
            g.fillEllipse ((float)getWidth() / 2.0f - 4.0f, 460.0f, 8.0f, 8.0f);
        }
    }

    void resized() override
    {
        sliderPunche.setBounds (45, 90, 80, 80);
        sliderEfecto.setBounds (getWidth() - 125, 90, 80, 80);
        sliderArmonia.setBounds (45, 215, 80, 80);
        sliderMezcla.setBounds (getWidth() - 125, 215, 80, 80);

        sliderSazon.setBounds (200, 365, getWidth() - 400, 30);
        btnClaveSol.setBounds (getWidth() / 2 - 60, 425, 120, 30);
    }

private:
    AudioPluginAudioProcessor& audioProcessor;
    MetalKnobLookAndFeel metalLook;

    juce::Slider sliderPunche;
    juce::Slider sliderEfecto;
    juce::Slider sliderArmonia;
    juce::Slider sliderMezcla;
    juce::Slider sliderSazon;
    juce::TextButton btnClaveSol;

    float scopeData[AudioPluginAudioProcessor::scopeSize];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};