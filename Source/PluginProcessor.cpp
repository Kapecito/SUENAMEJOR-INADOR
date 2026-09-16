#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       forwardFFT (fftOrder),
       window (fftSize, juce::dsp::WindowingFunction<float>::hann)
{
    addParameter (paramPunche  = new juce::AudioParameterFloat ("punche",  "Punche",  1.0f, 67.0f, 1.0f));
    addParameter (paramEfecto  = new juce::AudioParameterFloat ("efecto",  "Efecto",  0.0f, 1.0f, 0.0f));
    addParameter (paramArmonia = new juce::AudioParameterFloat ("armonia", "Armonia", 0.0f, 1.0f, 0.0f));
    addParameter (paramMezcla  = new juce::AudioParameterFloat ("mezcla",  "Mezcla",  0.0f, 1.0f, 0.0f));
    addParameter (paramSazon   = new juce::AudioParameterFloat ("sazon",   "Sazon",   1.0f, 420.0f, 1.0f));
    addParameter (paramClaveSol = new juce::AudioParameterBool ("clavesol", "Clave de sol", false));

    std::fill (std::begin (fifo), std::end (fifo), 0.0f);
    std::fill (std::begin (fftData), std::end (fftData), 0.0f);
    std::fill (std::begin (scopeData), std::end (scopeData), 0.0f);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {}

const juce::String AudioPluginAudioProcessor::getName() const { return "SUENAMEJOR - INADOR"; }
bool AudioPluginAudioProcessor::acceptsMidi() const { return false; }
bool AudioPluginAudioProcessor::producesMidi() const { return false; }
bool AudioPluginAudioProcessor::isMidiEffect() const { return false; }
double AudioPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int AudioPluginAudioProcessor::getNumPrograms() { return 1; }
int AudioPluginAudioProcessor::getCurrentProgram() { return 0; }
void AudioPluginAudioProcessor::setCurrentProgram (int) {}
const juce::String AudioPluginAudioProcessor::getProgramName (int) { return {}; }
void AudioPluginAudioProcessor::changeProgramName (int, const juce::String&) {}
void AudioPluginAudioProcessor::prepareToPlay (double, int) {}
void AudioPluginAudioProcessor::releaseResources() {}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return (layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet());
}

void AudioPluginAudioProcessor::pushNextSampleIntoFifo (float sample) noexcept
{
    if (fifoIndex == fftSize)
    {
        if (!nextFFTBlockReady)
        {
            std::fill (std::begin (fftData), std::end (fftData), 0.0f);
            std::copy (fifo, fifo + fftSize, fftData);
            nextFFTBlockReady = true;
        }
        fifoIndex = 0;
    }
    fifo[fifoIndex++] = sample;
}

void AudioPluginAudioProcessor::drawNextFrameOfSpectrum()
{
    window.multiplyWithWindowingTable (fftData, fftSize);
    forwardFFT.performFrequencyOnlyForwardTransform (fftData);

    auto mindB = -75.0f;
    auto maxdB = 0.0f;

    for (int i = 0; i < scopeSize; ++i)
    {
        auto skewedProportionX = 1.0f - std::exp (std::log (1.0f - (float)i / (float)scopeSize) * 0.22f);
        auto fftDataIndex = juce::jlimit (0, fftSize / 2, (int)(skewedProportionX * (float)(fftSize / 2)));
        auto level = juce::jmap (juce::jlimit (mindB, maxdB, juce::Decibels::gainToDecibels (fftData[fftDataIndex]) - juce::Decibels::gainToDecibels ((float)fftSize)),
                                 mindB, maxdB, 0.0f, 1.0f);

        scopeData[i] = juce::jmax (level, scopeData[i] * 0.78f);
    }
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto* leftChannel = buffer.getReadPointer (0);
    auto* rightChannel = buffer.getNumChannels() > 1 ? buffer.getReadPointer (1) : leftChannel;
    int numSamples = buffer.getNumSamples();

    float peak = 0.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        float monoSample = 0.5f * (leftChannel[i] + rightChannel[i]);
        peak = juce::jmax (peak, std::abs (monoSample));
        pushNextSampleIntoFifo (monoSample);
    }
    currentAudioLevel.store (peak);
}

bool AudioPluginAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock&) {}
void AudioPluginAudioProcessor::setStateInformation (const void*, int) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}