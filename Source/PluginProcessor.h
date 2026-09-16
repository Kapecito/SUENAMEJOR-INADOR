#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class MetalKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider& slider) override
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
    AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

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