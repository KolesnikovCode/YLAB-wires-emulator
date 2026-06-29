#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    constexpr auto wireTypeParamId = "wireType";
}

WireEmulatorAudioProcessor::WireEmulatorAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

WireEmulatorAudioProcessor::~WireEmulatorAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout WireEmulatorAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        wireTypeParamId,
        "Wire Type",
        WireType::choices(),
        static_cast<int> (WireType::Gold)));

    return layout;
}

int WireEmulatorAudioProcessor::getSelectedWireType() const
{
    return static_cast<int> (*apvts.getRawParameterValue (wireTypeParamId));
}

const juce::String WireEmulatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WireEmulatorAudioProcessor::acceptsMidi() const  { return false; }
bool WireEmulatorAudioProcessor::producesMidi() const { return false; }
bool WireEmulatorAudioProcessor::isMidiEffect() const { return false; }
double WireEmulatorAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int WireEmulatorAudioProcessor::getNumPrograms() { return 1; }
int WireEmulatorAudioProcessor::getCurrentProgram() { return 0; }
void WireEmulatorAudioProcessor::setCurrentProgram (int index) { juce::ignoreUnused (index); }
const juce::String WireEmulatorAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}
void WireEmulatorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void WireEmulatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void WireEmulatorAudioProcessor::releaseResources() {}

bool WireEmulatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void WireEmulatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                               juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages, apvts);

    juce::ScopedNoDenormals noDenormals;

    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

}

bool WireEmulatorAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* WireEmulatorAudioProcessor::createEditor()
{
    return new WireEmulatorAudioProcessorEditor (*this);
}

void WireEmulatorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    const auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void WireEmulatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));

    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WireEmulatorAudioProcessor();
}
