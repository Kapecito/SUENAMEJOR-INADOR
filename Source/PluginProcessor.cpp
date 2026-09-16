#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    addParameter (paramPunche = new juce::AudioParameterFloat ("punche", "Punche", 1.0f, 67.0f, 33.0f));
    addParameter (paramEfecto = new juce::AudioParameterFloat ("efecto", "Efecto", 0.0f, 1.0f, 0.5f));
    addParameter (paramArmonia = new juce::AudioParameterFloat ("armonia", "Armonia", 0.0f, 1.0f, 0.5f));
    addParameter (paramMezcla = new juce::AudioParameterFloat ("mezcla", "Mezcla", 0.0f, 1.0f, 0.75f));
    addParameter (paramSazon = new juce::AudioParameterFloat ("sazon", "Sazon", 1.0f, 420.0f, 210.0f));
    addParameter (paramClaveSol = new juce::AudioParameterBool ("clavesol", "Clave de sol", false));

    std::fill (std::begin (scopeBuffer), std::end (scopeBuffer), 0.0f);
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

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto* channelData = buffer.getReadPointer (0);
    int numSamples = buffer.getNumSamples();

    for (int i = 0; i < numSamples; ++i)
    {
        scopeBuffer[scopeWritePosition] = channelData[i];
        scopeWritePosition = (scopeWritePosition + 1) % scopeSize;
    }
    scopeUpdated.store (true);
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