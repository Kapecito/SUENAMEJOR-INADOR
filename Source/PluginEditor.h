#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class LitSwitchButton : public juce::Button
{
public:
    LitSwitchButton() : juce::Button ("LitSwitch") { setClickingTogglesState (true); }

    void paintButton (juce::Graphics& g, bool, bool) override
    {
        auto bounds = getLocalBounds().toFloat().reduced (2.0f);
        bool on = getToggleState();

        g.setColour (juce::Colour (0x55000000));
        g.fillRoundedRectangle (bounds.translated (0.0f, 2.5f), 6.0f);

        if (on)
        {
            juce::ColourGradient litGrad (juce::Colour (0xffffa026), bounds.getCentreX(), bounds.getY(),
                                         juce::Colour (0xffd84f00), bounds.getCentreX(), bounds.getBottom(), false);
            g.setGradientFill (litGrad);
            g.fillRoundedRectangle (bounds, 6.0f);

            g.setColour (juce::Colour (0xffffe3a3));
            g.drawRoundedRectangle (bounds, 6.0f, 2.0f);

            g.setColour (juce::Colour (0xff210a01));
            g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
            g.drawText ("CLAVE DE SOL", bounds, juce::Justification::centred);
        }
        else
        {
            juce::ColourGradient offGrad (juce::Colour (0xff484d56), bounds.getCentreX(), bounds.getY(),
                                         juce::Colour (0xff25272c), bounds.getCentreX(), bounds.getBottom(), false);
            g.setGradientFill (offGrad);
            g.fillRoundedRectangle (bounds, 6.0f);

            g.setColour (juce::Colour (0xff606672));
            g.drawRoundedRectangle (bounds, 6.0f, 1.2f);

            g.setColour (juce::Colour (0xff8f96a3));
            g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
            g.drawText ("CLAVE DE SOL", bounds, juce::Justification::centred);
        }
    }
};

class GlowingKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider& /*slider*/) override
    {
        auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (6.0f);
        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto center = bounds.getCentre();

        juce::Colour glowOff (0x00000000);
        juce::Colour glowActive (0xff00d5ff);
        auto glowColor = glowOff.interpolatedWith (glowActive, sliderPos * 0.9f);

        if (sliderPos > 0.02f)
        {
            g.setColour (glowColor.withAlpha (sliderPos * 0.45f));
            g.fillEllipse (bounds.expanded (4.0f * sliderPos));
        }

        g.setColour (juce::Colour (0x44000000));
        g.fillEllipse (bounds.translated (0.0f, 2.5f));

        juce::ColourGradient knobGrad (juce::Colour (0xffedf0f5), center.x - radius, center.y - radius,
                                      juce::Colour (0xff8e94a0), center.x + radius, center.y + radius, false);
        g.setGradientFill (knobGrad);
        g.fillEllipse (bounds);

        g.setColour (sliderPos > 0.05f ? glowActive.withAlpha (sliderPos * 0.85f) : juce::Colour (0xffffffff).withAlpha (0.4f));
        g.drawEllipse (bounds, 1.2f);

        juce::Path p;
        auto pointerLength = radius * 0.72f;
        p.addRectangle (-1.5f, -radius, 3.0f, pointerLength * 0.5f);
        p.applyTransform (juce::AffineTransform::rotation (toAngle).translated (center.x, center.y));
        g.setColour (sliderPos > 0.1f ? juce::Colour (0xff002538) : juce::Colour (0xff151619));
        g.fillPath (p);
    }

    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
                           juce::Slider::SliderStyle /*style*/, juce::Slider& /*slider*/) override
    {
        // Riel metálico hendido (slot hundido)
        auto trackY = (float)y + (float)height * 0.5f - 2.5f;
        auto trackRect = juce::Rectangle<float> ((float)x, trackY, (float)width, 5.0f);

        g.setColour (juce::Colour (0xff1a1c20));
        g.fillRoundedRectangle (trackRect, 2.5f);

        g.setColour (juce::Colour (0xff454a54));
        g.drawRoundedRectangle (trackRect, 2.5f, 1.0f);

        // Handle 3D de fader analógico
        float handleW = 16.0f;
        float handleH = 22.0f;
        float handleX = sliderPos - handleW * 0.5f;
        float handleY = (float)y + (float)height * 0.5f - handleH * 0.5f;
        auto handleRect = juce::Rectangle<float> (handleX, handleY, handleW, handleH);

        // Sombra arrojada del handle
        g.setColour (juce::Colour (0x55000000));
        g.fillRoundedRectangle (handleRect.translated (0.0f, 2.0f), 3.0f);

        // Bloque del fader con relieve satinado
        juce::ColourGradient handleGrad (juce::Colour (0xfff0f2f7), handleX, handleY,
                                         juce::Colour (0xff8f96a3), handleX + handleW, handleY + handleH, false);
        g.setGradientFill (handleGrad);
        g.fillRoundedRectangle (handleRect, 3.0f);

        // Borde biselado
        g.setColour (juce::Colour (0xffffffff).withAlpha (0.8f));
        g.drawRoundedRectangle (handleRect, 3.0f, 1.0f);

        // Ranura central en bajo relieve con indicador rojo
        g.setColour (juce::Colour (0xff202226));
        g.fillRect (handleX + handleW * 0.5f - 1.0f, handleY + 3.0f, 2.0f, handleH - 6.0f);
        g.setColour (juce::Colour (0xffff3b14));
        g.fillRect (handleX + handleW * 0.5f - 0.7f, handleY + 4.0f, 1.4f, handleH - 8.0f);
    }
};

class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::Timer
{
public:
    AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
        : AudioProcessorEditor (&p), audioProcessor (p)
    {
        setSize (700, 530);
        setLookAndFeel (&glowingLook);

        auto setupKnob = [this](juce::Slider& s) {
            s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
            s.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            addAndMakeVisible (s);
        };

        setupKnob (sliderPunche);
        sliderPunche.setRange (1.0, 67.0, 0.1);
        sliderPunche.setValue (*audioProcessor.paramPunche);
        sliderPunche.onValueChange = [this] { *audioProcessor.paramPunche = (float)sliderPunche.getValue(); repaint(); };

        setupKnob (sliderEfecto);
        sliderEfecto.setRange (0.0, 1.0, 0.01);
        sliderEfecto.setValue (*audioProcessor.paramEfecto);
        sliderEfecto.onValueChange = [this] { *audioProcessor.paramEfecto = (float)sliderEfecto.getValue(); repaint(); };

        setupKnob (sliderArmonia);
        sliderArmonia.setRange (0.0, 1.0, 0.01);
        sliderArmonia.setValue (*audioProcessor.paramArmonia);
        sliderArmonia.onValueChange = [this] { *audioProcessor.paramArmonia = (float)sliderArmonia.getValue(); repaint(); };

        setupKnob (sliderMezcla);
        sliderMezcla.setRange (0.0, 1.0, 0.01);
        sliderMezcla.setValue (*audioProcessor.paramMezcla);
        sliderMezcla.onValueChange = [this] { *audioProcessor.paramMezcla = (float)sliderMezcla.getValue(); repaint(); };

        sliderSazon.setSliderStyle (juce::Slider::LinearHorizontal);
        sliderSazon.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        sliderSazon.setRange (1.0, 420.0, 1.0);
        sliderSazon.setValue (*audioProcessor.paramSazon);
        sliderSazon.onValueChange = [this] { *audioProcessor.paramSazon = (float)sliderSazon.getValue(); repaint(); };
        addAndMakeVisible (sliderSazon);

        btnClaveSol.setToggleState (*audioProcessor.paramClaveSol, juce::dontSendNotification);
        btnClaveSol.onClick = [this] { *audioProcessor.paramClaveSol = btnClaveSol.getToggleState(); repaint(); };
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
        phaseCounter += 0.12f;

        float rawPeak = audioProcessor.currentAudioLevel.load();
        
        if (rawPeak > recentMaxPeak)
            recentMaxPeak = rawPeak;
        else
            recentMaxPeak = juce::jmax (0.0001f, recentMaxPeak * 0.985f);

        float normalizedPeak = rawPeak / recentMaxPeak;
        float dynamicAura = (rawPeak > 0.0005f) ? (0.5f + normalizedPeak * 0.5f) : 0.0f;
        
        float mezclaMultiplier = (float)sliderMezcla.getValue();
        float targetAura = dynamicAura * mezclaMultiplier; 
        
        if (targetAura > peakEnvelope)
            peakEnvelope += (targetAura - peakEnvelope) * 0.45f;
        else
            peakEnvelope += (targetAura - peakEnvelope) * 0.08f;

        auraNeedle = juce::jlimit (0.0f, 1.0f, peakEnvelope);

        if (audioProcessor.nextFFTBlockReady)
        {
            audioProcessor.drawNextFrameOfSpectrum();
            audioProcessor.nextFFTBlockReady = false;
        }
        else
        {
            for (int i = 0; i < AudioPluginAudioProcessor::scopeSize; ++i)
                audioProcessor.scopeData[i] *= 0.90f;
        }
        
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        juce::ColourGradient metalBase (juce::Colour (0xffdadfe8), 0, 0,
                                        juce::Colour (0xffb2b9c4), (float)getWidth(), (float)getHeight(), false);
        g.setGradientFill (metalBase);
        g.fillAll();

        g.setColour (juce::Colour (0xff000000));
        g.setFont (juce::FontOptions (26.0f, juce::Font::bold));
        g.drawText ("SUENAMEJOR - INADOR", 36, 22, getWidth() - 72, 32, juce::Justification::centredLeft);

        auto drawTicks = [&g](float cx, float cy, float rad) {
            g.setColour (juce::Colour (0xff000000)); 
            for (int i = 0; i <= 10; ++i)
            {
                float angle = juce::MathConstants<float>::pi * 0.75f + (float)i / 10.0f * (juce::MathConstants<float>::pi * 1.5f);
                float x1 = cx + std::cos (angle) * (rad + 2.5f);
                float y1 = cy + std::sin (angle) * (rad + 2.5f);
                float x2 = cx + std::cos (angle) * (rad + 7.5f);
                float y2 = cy + std::sin (angle) * (rad + 7.5f);
                g.drawLine (x1, y1, x2, y2, 2.0f); 
            }
        };

        g.setColour (juce::Colour (0xff000000)); 
        g.setFont (juce::FontOptions (14.0f, juce::Font::bold));

        // Punche (1 a 67)
        drawTicks (68.0f, 138.0f, 34.0f);
        g.drawText ("PUNCHE", 18, 68, 100, 18, juce::Justification::centred);
        g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
        g.drawText ("1", 22, 182, 18, 16, juce::Justification::centred);
        g.drawText ("67", 96, 182, 26, 16, juce::Justification::centred);

        // Efecto (POP a INDIE)
        drawTicks ((float)getWidth() - 68.0f, 138.0f, 34.0f);
        g.setFont (juce::FontOptions (14.0f, juce::Font::bold));
        g.drawText ("EFECTO", getWidth() - 118, 68, 100, 18, juce::Justification::centred);
        g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
        g.drawText ("POP", getWidth() - 130, 182, 36, 16, juce::Justification::centred);
        g.drawText ("INDIE", getWidth() - 54, 182, 42, 16, juce::Justification::centred);

        // Armonía (0 a MAX)
        drawTicks (68.0f, 274.0f, 34.0f);
        g.setFont (juce::FontOptions (14.0f, juce::Font::bold));
        g.drawText ("ARMONIA", 18, 204, 100, 18, juce::Justification::centred);
        g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
        g.drawText ("0", 22, 318, 18, 16, juce::Justification::centred);
        g.drawText ("MAX", 96, 318, 30, 16, juce::Justification::centred);

        // Mezcla (0 a 100)
        drawTicks ((float)getWidth() - 68.0f, 274.0f, 34.0f);
        g.setFont (juce::FontOptions (14.0f, juce::Font::bold));
        g.drawText ("MEZCLA", getWidth() - 118, 204, 100, 18, juce::Justification::centred);
        g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
        g.drawText ("0", getWidth() - 118, 318, 18, 16, juce::Justification::centred);
        g.drawText ("100", getWidth() - 48, 318, 26, 16, juce::Justification::centred);

        // Sazón (más corto y centrado)
        g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
        g.drawText ("SAZON", getWidth() / 2 - 40, 342, 80, 16, juce::Justification::centred);
        g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
        g.drawText ("1", getWidth() / 2 - 128, 364, 16, 18, juce::Justification::centred);
        g.drawText ("420", getWidth() / 2 + 112, 364, 28, 18, juce::Justification::centred);

        // --- PANTALLA DEL REACTOR ---
        auto screenRect = juce::Rectangle<float> (136.0f, 76.0f, (float)getWidth() - 272.0f, 248.0f);
        g.setColour (juce::Colour (0xff0e1014));
        g.fillRoundedRectangle (screenRect, 9.0f);

        auto innerScreen = screenRect.reduced (3.5f);

        float sazonNorm = (float)(sliderSazon.getValue() - 1.0) / 419.0f;
        juce::Colour bgCold (0xff050b18);
        juce::Colour bgWarm (0xff320a02);
        auto currentBg = bgCold.interpolatedWith (bgWarm, sazonNorm);

        juce::ColourGradient chamberGlow (currentBg.brighter (0.4f * sazonNorm), innerScreen.getCentreX(), innerScreen.getCentreY(),
                                         currentBg.darker (0.5f), innerScreen.getX(), innerScreen.getBottom(), true);
        g.setGradientFill (chamberGlow);
        g.fillRoundedRectangle (innerScreen, 6.0f);

        g.setColour (juce::Colour (0x15ffffff));
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

        float puncheNorm = ((float)sliderPunche.getValue() - 1.0f) / 66.0f;
        float efectoVal = (float)sliderEfecto.getValue();
        float armoniaVal = (float)sliderArmonia.getValue();
        float mezclaVal = (float)sliderMezcla.getValue();

        float spreadFactor = 0.40f + efectoVal * 0.60f;
        float activeWidth = innerScreen.getWidth() * spreadFactor;
        float startX = innerScreen.getCentreX() - (activeWidth / 2.0f);

        juce::Path spectrumPath;
        juce::Path fillPath;
        float bottomY = innerScreen.getBottom();

        fillPath.startNewSubPath (startX, bottomY);

        for (int i = 0; i < AudioPluginAudioProcessor::scopeSize; ++i)
        {
            float rawLevel = audioProcessor.scopeData[i];
            float dynamicLevel = rawLevel * (0.35f + puncheNorm * 1.5f);

            if (armoniaVal > 0.01f)
            {
                float ripple = std::sin ((float)i * 0.35f + phaseCounter) * (0.18f * armoniaVal) * (rawLevel + 0.1f);
                dynamicLevel += ripple;
            }

            dynamicLevel = juce::jlimit (0.01f, 0.98f, dynamicLevel);

            float x = startX + ((float)i / (float)(AudioPluginAudioProcessor::scopeSize - 1)) * activeWidth;
            float y = bottomY - (dynamicLevel * innerScreen.getHeight() * 0.92f);

            if (i == 0) spectrumPath.startNewSubPath (x, y);
            else        spectrumPath.lineTo (x, y);

            fillPath.lineTo (x, y);
        }

        fillPath.lineTo (startX + activeWidth, bottomY);
        fillPath.closeSubPath();

        juce::Colour beamCold (0xff00d5ff);
        juce::Colour beamWarm (0xffff6200);
        auto coreColor = beamCold.interpolatedWith (beamWarm, sazonNorm);

        float fillOpacity = 0.08f + mezclaVal * 0.55f;
        juce::ColourGradient fillGrad (coreColor.withAlpha (fillOpacity), innerScreen.getCentreX(), innerScreen.getY(),
                                      coreColor.withAlpha (0.02f), innerScreen.getCentreX(), bottomY, false);
        g.setGradientFill (fillGrad);
        g.fillPath (fillPath);

        float strokeThickness = 4.5f - (puncheNorm * 3.0f);
        g.setColour (coreColor.brighter (0.4f).withAlpha (0.95f));
        g.strokePath (spectrumPath, juce::PathStrokeType (strokeThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));


        // =========================================================================
        // --- MEDIDOR AURA 3D CON TEXTURA DE PAPEL VINTAGE Y RELIEVE ---
        // =========================================================================
        auto vuRect = juce::Rectangle<float> ((float)getWidth() / 2.0f - 110.0f, 428.0f, 220.0f, 86.0f);
        
        juce::ColourGradient bezelGrad (juce::Colour (0xffffffff), vuRect.getX(), vuRect.getY(),
                                        juce::Colour (0xff666a72), vuRect.getX(), vuRect.getBottom(), false);
        g.setGradientFill (bezelGrad);
        g.fillRoundedRectangle (vuRect, 6.0f);

        auto innerVu = vuRect.reduced (3.5f);
        
        // Base de tono pergamino vintage
        juce::ColourGradient dialGrad (juce::Colour (0xfffaf3e5), innerVu.getX(), innerVu.getY(),
                                       juce::Colour (0xffe5d3ab), innerVu.getX(), innerVu.getBottom(), false);
        g.setGradientFill (dialGrad);
        g.fillRoundedRectangle (innerVu, 4.0f);

        // --- TEXTURA MICROESTRIADA DEL DIAL (PAPEL ENVEJECIDO) ---
        g.setColour (juce::Colour (0x0d000000));
        for (float ly = innerVu.getY(); ly < innerVu.getBottom(); ly += 2.0f)
            g.drawHorizontalLine ((int)ly, innerVu.getX(), innerVu.getRight());

        // Halo de viñeteado en los bordes del medidor
        juce::ColourGradient vignette (juce::Colour (0x00000000), innerVu.getCentreX(), innerVu.getCentreY(),
                                       juce::Colour (0x33442200), innerVu.getX(), innerVu.getY(), true);
        g.setGradientFill (vignette);
        g.fillRoundedRectangle (innerVu, 4.0f);

        // Sombras de profundidad
        g.setColour (juce::Colour (0x66000000));
        g.drawRoundedRectangle (innerVu, 4.0f, 1.5f);
        g.setColour (juce::Colour (0x25000000));
        g.drawRoundedRectangle (innerVu.reduced (1.0f), 4.0f, 0.5f);

        // Rótulo AURA
        g.setColour (juce::Colour (0xff000000));
        g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
        g.drawText ("AURA", innerVu.withTrimmedTop (4.0f), juce::Justification::centredTop);

        // Escala
        g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        g.drawText ("0", innerVu.getX() + 10, innerVu.getBottom() - 20, 20, 14, juce::Justification::left);
        g.drawText ("50", innerVu.getCentreX() - 12, innerVu.getY() + 24, 24, 14, juce::Justification::centred);
        g.drawText ("100", innerVu.getRight() - 32, innerVu.getBottom() - 20, 24, 14, juce::Justification::right);

        float pivotX = innerVu.getCentreX();
        float pivotY = innerVu.getBottom() + 16.0f;
        float radius = 62.0f;
        float startAngle = -0.72f;
        float endAngle = 0.72f;

        // Banda coloreada del arco de fondo (amarillo a rojo clásico de saturación)
        juce::Path arcWarm;
        arcWarm.addCentredArc (pivotX, pivotY, radius + 2.0f, radius + 2.0f, 0.0f, startAngle + 0.75f * (endAngle - startAngle), endAngle, true);
        g.setColour (juce::Colour (0xddcc1100));
        g.strokePath (arcWarm, juce::PathStrokeType (3.0f));

        juce::Path arc;
        arc.addCentredArc (pivotX, pivotY, radius, radius, 0.0f, startAngle, endAngle, true);
        g.setColour (juce::Colour (0xff000000));
        g.strokePath (arc, juce::PathStrokeType (1.2f));

        // Ticks de medición graduados
        for (int i = 0; i <= 20; ++i)
        {
            float frac = (float)i / 20.0f;
            float tickAngle = startAngle + frac * (endAngle - startAngle);
            bool isMajor = (i % 5 == 0);
            
            float tickLen = isMajor ? 7.0f : 4.0f;
            float tickThick = isMajor ? 1.6f : 1.0f;

            if (i >= 16)
                g.setColour (juce::Colour (0xffcc1100));
            else
                g.setColour (juce::Colour (0xff000000));

            float cosA = std::sin (tickAngle);
            float sinA = -std::cos (tickAngle);

            float x1 = pivotX + cosA * radius;
            float y1 = pivotY + sinA * radius;
            float x2 = pivotX + cosA * (radius + tickLen);
            float y2 = pivotY + sinA * (radius + tickLen);

            g.drawLine (x1, y1, x2, y2, tickThick);
        }

        // Aguja analógica
        float needleAngle = startAngle + auraNeedle * (endAngle - startAngle);
        juce::Path needle;
        needle.startNewSubPath (pivotX, pivotY);
        needle.lineTo (pivotX - 1.5f, pivotY);
        needle.lineTo (pivotX, pivotY - radius - 5.0f);
        needle.lineTo (pivotX + 1.5f, pivotY);
        needle.closeSubPath();
        needle.applyTransform (juce::AffineTransform::rotation (needleAngle, pivotX, pivotY));

        // Sombra de aguja
        g.setColour (juce::Colour (0x45000000));
        juce::Path needleShadow = needle;
        needleShadow.applyTransform (juce::AffineTransform::translation (2.0f, 2.0f));
        g.fillPath (needleShadow);

        // Aguja roja
        g.setColour (juce::Colour (0xffd90d00));
        g.fillPath (needle);

        // Pivote
        g.setColour (juce::Colour (0xff22252a));
        g.fillEllipse (pivotX - 4.5f, pivotY - 4.5f, 9.0f, 9.0f);

        // Reflejo curvo del cristal
        juce::Path glassReflect;
        glassReflect.addRoundedRectangle (innerVu.getX(), innerVu.getY(), innerVu.getWidth(), innerVu.getHeight() * 0.46f, 4.0f, 4.0f, false, false, false, false);
        juce::ColourGradient glassGrad (juce::Colour (0x60ffffff), innerVu.getX(), innerVu.getY(),
                                        juce::Colour (0x00ffffff), innerVu.getX(), innerVu.getY() + innerVu.getHeight() * 0.46f, false);
        g.setGradientFill (glassGrad);
        g.fillPath (glassReflect);
    }

    void resized() override
    {
        sliderPunche.setBounds (36, 106, 64, 64);
        sliderEfecto.setBounds (getWidth() - 100, 106, 64, 64);
        sliderArmonia.setBounds (36, 242, 64, 64);
        sliderMezcla.setBounds (getWidth() - 100, 242, 64, 64);

        // Slider de Sazón más corto y centrado (200px de longitud)
        sliderSazon.setBounds (getWidth() / 2 - 100, 362, 200, 24);

        // Interruptor Clave de Sol
        btnClaveSol.setBounds (getWidth() - 165, 452, 130, 36);
    }

private:
    AudioPluginAudioProcessor& audioProcessor;
    GlowingKnobLookAndFeel glowingLook;

    juce::Slider sliderPunche;
    juce::Slider sliderEfecto;
    juce::Slider sliderArmonia;
    juce::Slider sliderMezcla;
    juce::Slider sliderSazon;
    LitSwitchButton btnClaveSol;

    float phaseCounter = 0.0f;
    float auraNeedle = 0.0f;
    float peakEnvelope = 0.0f;
    float recentMaxPeak = 0.001f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};