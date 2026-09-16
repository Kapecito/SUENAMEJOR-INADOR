#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

// Botón Switch iluminado completo
class LitSwitchButton : public juce::Button
{
public:
    LitSwitchButton() : juce::Button ("LitSwitch")
    {
        setClickingTogglesState (true);
    }

    void paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat().reduced (2.0f);
        bool on = getToggleState();

        // Sombra exterior
        g.setColour (juce::Colour (0x55000000));
        g.fillRoundedRectangle (bounds.translated (0.0f, 2.0f), 6.0f);

        // Fondo del switch (iluminado vs apagado)
        if (on)
        {
            // Luz encendida: Gradiente naranja incandescente con borde brillante
            juce::ColourGradient litGrad (juce::Colour (0xffff9e24), bounds.getCentreX(), bounds.getY(),
                                         juce::Colour (0xffd85100), bounds.getCentreX(), bounds.getBottom(), false);
            g.setGradientFill (litGrad);
            g.fillRoundedRectangle (bounds, 6.0f);

            // Borde resplandor
            g.setColour (juce::Colour (0xffffe099));
            g.drawRoundedRectangle (bounds, 6.0f, 2.0f);

            // Texto en bajo relieve oscuro cuando está activo
            g.setColour (juce::Colour (0xff200b02));
            g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
            g.drawText ("CLAVE DE SOL", bounds, juce::Justification::centred);
        }
        else
        {
            // Luz apagada: Botón metálico/plástico oscuro en relieve
            juce::ColourGradient offGrad (juce::Colour (0xff4b5059), bounds.getCentreX(), bounds.getY(),
                                         juce::Colour (0xff2b2e34), bounds.getCentreX(), bounds.getBottom(), false);
            g.setGradientFill (offGrad);
            g.fillRoundedRectangle (bounds, 6.0f);

            g.setColour (juce::Colour (0xff686f7c));
            g.drawRoundedRectangle (bounds, 6.0f, 1.2f);

            // Texto apagado
            g.setColour (juce::Colour (0xff8d94a0));
            g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
            g.drawText ("CLAVE DE SOL", bounds, juce::Justification::centred);
        }
    }
};

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

        g.setColour (juce::Colour (0xffffffff).withAlpha (0.7f));
        g.drawEllipse (bounds, 1.2f);

        juce::Path p;
        auto pointerLength = radius * 0.75f;
        p.addRectangle (-1.6f, -radius, 3.2f, pointerLength * 0.5f);
        p.applyTransform (juce::AffineTransform::rotation (toAngle).translated (center.x, center.y));
        g.setColour (juce::Colour (0xff111215));
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
        setSize (680, 500);
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

        btnClaveSol.setToggleState (*audioProcessor.paramClaveSol, juce::dontSendNotification);
        btnClaveSol.onClick = [this] {
            *audioProcessor.paramClaveSol = btnClaveSol.getToggleState();
            repaint();
        };
        addAndMakeVisible (btnClaveSol);

        startTimerHz (30);
    }

    ~AudioPluginAudioProcessorEditor() override
    {
        setLookAndFeel (nullptr);
        stopTimer();
    }

    void timerCallback() override
    {
        if (audioProcessor.nextFFTBlockReady)
        {
            audioProcessor.drawNextFrameOfSpectrum();
            audioProcessor.nextFFTBlockReady = false;
            repaint();
        }
        else
        {
            // Mantiene el decaimiento visual suave continuo
            for (int i = 0; i < AudioPluginAudioProcessor::scopeSize; ++i)
                audioProcessor.scopeData[i] *= 0.93f;
            repaint();
        }
    }

    void paint (juce::Graphics& g) override
    {
        // Placa metálica satinada
        juce::ColourGradient metalBase (juce::Colour (0xffd6dbe3), 0, 0,
                                        juce::Colour (0xffb8bfc9), (float)getWidth(), (float)getHeight(), false);
        g.setGradientFill (metalBase);
        g.fillAll();

        // Título superior en negrita negra
        g.setColour (juce::Colour (0xff111215));
        g.setFont (juce::FontOptions (26.0f, juce::Font::bold));
        g.drawText ("SUENAMEJOR - INADOR", 35, 18, getWidth() - 70, 32, juce::Justification::centredLeft);

        auto drawTicks = [&g](float cx, float cy, float radius) {
            g.setColour (juce::Colour (0x66111215));
            for (int i = 0; i <= 10; ++i)
            {
                float angle = juce::MathConstants<float>::pi * 0.75f + (float)i / 10.0f * (juce::MathConstants<float>::pi * 1.5f);
                float x1 = cx + std::cos (angle) * (radius + 3.0f);
                float y1 = cy + std::sin (angle) * (radius + 3.0f);
                float x2 = cx + std::cos (angle) * (radius + 8.0f);
                float y2 = cy + std::sin (angle) * (radius + 8.0f);
                g.drawLine (x1, y1, x2, y2, 1.4f);
            }
        };

        // --- ETIQUETAS Y MARCAS (Negro puro, fuente gruesa y con espacio libre de los ticks) ---
        g.setColour (juce::Colour (0xff000000));
        g.setFont (juce::FontOptions (13.0f, juce::Font::bold));

        // Punche
        drawTicks (85.0f, 135.0f, 40.0f);
        g.drawText ("PUNCHE", 30, 68, 110, 18, juce::Justification::centred);
        g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
        g.drawText ("1", 34, 182, 22, 16, juce::Justification::centred);
        g.drawText ("67", 114, 182, 26, 16, juce::Justification::centred);

        // Efecto
        drawTicks ((float)getWidth() - 85.0f, 135.0f, 40.0f);
        g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
        g.drawText ("EFECTO", getWidth() - 140, 68, 110, 18, juce::Justification::centred);
        g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
        g.drawText ("POP", getWidth() - 148, 182, 34, 16, juce::Justification::centred);
        g.drawText ("INDIE", getWidth() - 66, 182, 40, 16, juce::Justification::centred);

        // Armonía
        drawTicks (85.0f, 265.0f, 40.0f);
        g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
        g.drawText ("ARMONIA", 30, 198, 110, 18, juce::Justification::centred);

        // Mezcla
        drawTicks ((float)getWidth() - 85.0f, 265.0f, 40.0f);
        g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
        g.drawText ("MEZCLA", getWidth() - 140, 198, 110, 18, juce::Justification::centred);

        // Sazón
        g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
        g.drawText ("SAZON", getWidth() / 2 - 50, 346, 100, 18, juce::Justification::centred);
        g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
        g.drawText ("1", 170, 372, 22, 18, juce::Justification::centred);
        g.drawText ("420", getWidth() - 192, 372, 32, 18, juce::Justification::centred);

        // --- PANTALLA CENTRAL OLED ESTILO FABFILTER PRO-Q 3 ---
        auto screenRect = juce::Rectangle<float> (165.0f, 75.0f, (float)getWidth() - 330.0f, 235.0f);
        
        // Marco de la pantalla
        g.setColour (juce::Colour (0xff121417));
        g.fillRoundedRectangle (screenRect, 8.0f);

        auto innerScreen = screenRect.reduced (3.0f);
        juce::ColourGradient screenBg (juce::Colour (0xff0f121a), innerScreen.getX(), innerScreen.getY(),
                                       juce::Colour (0xff07080c), innerScreen.getX(), innerScreen.getBottom(), false);
        g.setGradientFill (screenBg);
        g.fillRoundedRectangle (innerScreen, 6.0f);

        // Rejilla sutil de decibeles/frecuencias
        g.setColour (juce::Colour (0x1affffff));
        for (int row = 1; row <= 3; ++row)
        {
            float yGrid = innerScreen.getY() + (innerScreen.getHeight() / 4.0f) * (float)row;
            g.drawLine (innerScreen.getX(), yGrid, innerScreen.getRight(), yGrid, 1.0f);
        }
        for (int col = 1; col <= 4; ++col)
        {
            float xGrid = innerScreen.getX() + (innerScreen.getWidth() / 5.0f) * (float)col;
            g.drawLine (xGrid, innerScreen.getY(), xGrid, innerScreen.getBottom(), 1.0f);
        }

        // Construcción de la curva de espectro FFT
        juce::Path spectrumPath;
        juce::Path fillPath;

        float width = innerScreen.getWidth();
        float height = innerScreen.getHeight();
        float bottomY = innerScreen.getBottom();

        fillPath.startNewSubPath (innerScreen.getX(), bottomY);

        for (int i = 0; i < AudioPluginAudioProcessor::scopeSize; ++i)
        {
            float level = audioProcessor.scopeData[i];
            
            // Alteración dinámica con los parámetros del usuario
            float puncheBoost = ((float)sliderPunche.getValue() / 67.0f) * 0.25f;
            level = juce::jlimit (0.0f, 1.0f, level * (1.0f + puncheBoost));

            float x = innerScreen.getX() + ((float)i / (float)(AudioPluginAudioProcessor::scopeSize - 1)) * width;
            float y = bottomY - (level * height * 0.92f);

            if (i == 0)
            {
                spectrumPath.startNewSubPath (x, y);
            }
            else
            {
                spectrumPath.lineTo (x, y);
            }
            fillPath.lineTo (x, y);
        }

        fillPath.lineTo (innerScreen.getRight(), bottomY);
        fillPath.closeSubPath();

        // Relleno degradado translúcido bajo la curva tipo Pro-Q 3
        float sazonNorm = (float)(sliderSazon.getValue() - 1.0) / 419.0f;
        juce::Colour gradTop = juce::Colour (0xff00d0ff).interpolatedWith (juce::Colour (0xffff6a00), sazonNorm);
        juce::Colour gradBottom = juce::Colour (0xff002b4d).interpolatedWith (juce::Colour (0xff470f00), sazonNorm);

        juce::ColourGradient fillGrad (gradTop.withAlpha (0.42f), innerScreen.getX(), innerScreen.getY(),
                                      gradBottom.withAlpha (0.05f), innerScreen.getX(), bottomY, false);
        g.setGradientFill (fillGrad);
        g.fillPath (fillPath);

        // Trazo de línea brillante superior neón
        juce::Colour lineGlow = juce::Colour (0xff4de3ff).interpolatedWith (juce::Colour (0xffffa834), sazonNorm);
        g.setColour (lineGlow.withAlpha (0.95f));
        g.strokePath (spectrumPath, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved));
    }

    void resized() override
    {
        sliderPunche.setBounds (45, 95, 80, 80);
        sliderEfecto.setBounds (getWidth() - 125, 95, 80, 80);
        sliderArmonia.setBounds (45, 225, 80, 80);
        sliderMezcla.setBounds (getWidth() - 125, 225, 80, 80);

        sliderSazon.setBounds (195, 368, getWidth() - 390, 28);
        
        // Interruptor retroiluminado central
        btnClaveSol.setBounds (getWidth() / 2 - 80, 425, 160, 36);
    }

private:
    AudioPluginAudioProcessor& audioProcessor;
    MetalKnobLookAndFeel metalLook;

    juce::Slider sliderPunche;
    juce::Slider sliderEfecto;
    juce::Slider sliderArmonia;
    juce::Slider sliderMezcla;
    juce::Slider sliderSazon;
    LitSwitchButton btnClaveSol;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};