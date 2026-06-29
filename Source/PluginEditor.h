#pragma once

#include "PluginProcessor.h"

class BoostyLinkButton final : public juce::Component
{
public:
    BoostyLinkButton (const void* svgData, int svgDataSize, juce::URL url);

    static int widthForHeight (int height);

    void paint (juce::Graphics& g) override;
    void mouseUp (const juce::MouseEvent& e) override;
    void mouseEnter (const juce::MouseEvent& e) override;
    void mouseExit (const juce::MouseEvent& e) override;

private:
    std::unique_ptr<juce::Drawable> drawable;
    juce::URL linkUrl;
    bool hovered = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BoostyLinkButton)
};

class WireOptionButton final : public juce::Component
{
public:
    WireOptionButton (const void* imageData, int imageDataSize, int wireIndex);

    void setSelected (bool shouldBeSelected);
    std::function<void (int)> onClick;

    void paint (juce::Graphics& g) override;
    void mouseUp (const juce::MouseEvent& e) override;

private:
    juce::Image image;
    int wireIndex;
    bool selected = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WireOptionButton)
};

class WireEmulatorAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                               private juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit WireEmulatorAudioProcessorEditor (WireEmulatorAudioProcessor&);
    ~WireEmulatorAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    void selectWireType (int wireType);
    void updateButtonSelection (int wireType);
    void updateDescription (int wireType);

    WireEmulatorAudioProcessor& processorRef;

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::Label descriptionLabel;

    std::array<std::unique_ptr<WireOptionButton>, WireType::count> wireButtons;
    std::unique_ptr<BoostyLinkButton> boostyLinkButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WireEmulatorAudioProcessorEditor)
};
