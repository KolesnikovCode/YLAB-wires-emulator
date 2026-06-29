#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace WireType
{
    enum Index
    {
        Gold = 0,
        Copper,
        GoldOnWood,
        CopperOnWood,
        count
    };

    inline const juce::StringArray& choices()
    {
        static const juce::StringArray wireChoices {
            "Gold Wire",
            "Copper Wire",
            "Gold Wire on Wooden Stand",
            "Copper Wire on Wooden Stand"
        };
        return wireChoices;
    }

    inline const juce::String& description (int index)
    {
        static const juce::StringArray wireDescriptions {
            "Crystalline highs, effortless air, and uncolored transparency.",
            "Rich harmonics, velvety mids, and an inviting musical warmth.",
            "Seasoned hardwood riser reveals hidden micro-detail and spatial depth.",
            "Elevated on kiln-dried birch for a wider, more organic soundstage."
        };
        return wireDescriptions[juce::jlimit (0, count - 1, index)];
    }
}

class WireEmulatorAudioProcessor final : public juce::AudioProcessor
{
public:
    WireEmulatorAudioProcessor();
    ~WireEmulatorAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    int getSelectedWireType() const;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WireEmulatorAudioProcessor)
};
