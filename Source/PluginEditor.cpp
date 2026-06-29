#include "PluginEditor.h"
#include "BinaryData.h"

namespace
{
    constexpr auto wireTypeParamId = "wireType";
    constexpr auto boostyUrl = "https://boosty.to/ylab";
    constexpr int buttonSize = 100;
    constexpr int buttonGap = 12;
    constexpr int borderThickness = 2;
    constexpr int boostyButtonHeight = 24;
    constexpr float boostyLogoAspectRatio = 715.8f / 317.4f;

    namespace Theme
    {
        constexpr juce::uint32 background    = 0xff0d1117;
        constexpr juce::uint32 textPrimary = 0xffe6edf3;
        constexpr juce::uint32 textMuted   = 0xff8b949e;
        constexpr juce::uint32 borderActive   = 0xff3fb950;
        constexpr juce::uint32 borderInactive = 0xffffffff;
    }
}

BoostyLinkButton::BoostyLinkButton (const void* svgData, const int svgDataSize, juce::URL url)
    : linkUrl (std::move (url))
{
    drawable = juce::Drawable::createFromImageData (svgData, static_cast<size_t> (svgDataSize));
    drawable->replaceColour (juce::Colour (0xff242b2c), juce::Colour (Theme::textPrimary));
    setMouseCursor (juce::MouseCursor::PointingHandCursor);
}

int BoostyLinkButton::widthForHeight (const int height)
{
    return juce::roundToInt (static_cast<float> (height) * boostyLogoAspectRatio);
}

void BoostyLinkButton::paint (juce::Graphics& g)
{
    if (drawable == nullptr)
        return;

    const auto alpha = hovered ? 0.85f : 1.0f;
    g.setOpacity (alpha);
    drawable->drawWithin (g,
                          getLocalBounds().toFloat(),
                          juce::RectanglePlacement::centred,
                          1.0f);
    g.setOpacity (1.0f);
}

void BoostyLinkButton::mouseUp (const juce::MouseEvent& e)
{
    if (e.mouseWasClicked())
        linkUrl.launchInDefaultBrowser();
}

void BoostyLinkButton::mouseEnter (const juce::MouseEvent&)
{
    hovered = true;
    repaint();
}

void BoostyLinkButton::mouseExit (const juce::MouseEvent&)
{
    hovered = false;
    repaint();
}

WireOptionButton::WireOptionButton (const void* imageData, const int imageDataSize, const int index)
    : wireIndex (index)
{
    image = juce::ImageCache::getFromMemory (imageData, imageDataSize);
}

void WireOptionButton::setSelected (const bool shouldBeSelected)
{
    if (selected != shouldBeSelected)
    {
        selected = shouldBeSelected;
        repaint();
    }
}

void WireOptionButton::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    const auto borderColour = selected ? juce::Colour (Theme::borderActive)
                                       : juce::Colour (Theme::borderInactive);

    g.setColour (borderColour);
    g.drawRect (bounds, static_cast<float> (borderThickness));

    const auto imageBounds = getLocalBounds().reduced (borderThickness).toFloat();

    if (image.isValid())
    {
        g.drawImage (image,
                     imageBounds,
                     juce::RectanglePlacement::fillDestination);
    }
    else
    {
        g.setColour (juce::Colour (0xff21262d));
        g.fillRect (imageBounds);
    }
}

void WireOptionButton::mouseUp (const juce::MouseEvent& e)
{
    if (e.mouseWasClicked() && onClick != nullptr)
        onClick (wireIndex);
}

WireEmulatorAudioProcessorEditor::WireEmulatorAudioProcessorEditor (WireEmulatorAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    const std::array<std::pair<const void*, int>, WireType::count> wireImages {{
        { BinaryData::gold_png,             BinaryData::gold_pngSize },
        { BinaryData::copper_png,          BinaryData::copper_pngSize },
        { BinaryData::goldonwood_png,      BinaryData::goldonwood_pngSize },
        { BinaryData::copperonwood_png,    BinaryData::copperonwood_pngSize },
    }};

    titleLabel.setText (JucePlugin_Name, juce::dontSendNotification);
    titleLabel.setFont (juce::FontOptions (22.0f).withStyle ("Bold"));
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, juce::Colour (Theme::textPrimary));
    addAndMakeVisible (titleLabel);

    subtitleLabel.setText ("Premium audiophile cable technology", juce::dontSendNotification);
    subtitleLabel.setFont (juce::FontOptions (13.0f).withStyle ("Italic"));
    subtitleLabel.setJustificationType (juce::Justification::centred);
    subtitleLabel.setColour (juce::Label::textColourId, juce::Colour (Theme::textMuted));
    addAndMakeVisible (subtitleLabel);

    for (int i = 0; i < WireType::count; ++i)
    {
        auto button = std::make_unique<WireOptionButton> (wireImages[static_cast<size_t> (i)].first,
                                                          wireImages[static_cast<size_t> (i)].second,
                                                          i);
        button->onClick = [this] (int wireType) { selectWireType (wireType); };
        addAndMakeVisible (*button);
        wireButtons[static_cast<size_t> (i)] = std::move (button);
    }

    descriptionLabel.setFont (juce::FontOptions (12.0f).withStyle ("Italic"));
    descriptionLabel.setJustificationType (juce::Justification::centred);
    descriptionLabel.setColour (juce::Label::textColourId, juce::Colour (Theme::textMuted));
    addAndMakeVisible (descriptionLabel);

    boostyLinkButton = std::make_unique<BoostyLinkButton> (BinaryData::boosty_logo_svg,
                                                           BinaryData::boosty_logo_svgSize,
                                                           juce::URL (boostyUrl));
    addAndMakeVisible (*boostyLinkButton);

    processorRef.getAPVTS().addParameterListener (wireTypeParamId, this);

    updateButtonSelection (processorRef.getSelectedWireType());
    updateDescription (processorRef.getSelectedWireType());

    const auto contentWidth = 40 + WireType::count * buttonSize + (WireType::count - 1) * buttonGap;
    setSize (contentWidth, 320);
    setResizable (false, false);
}

WireEmulatorAudioProcessorEditor::~WireEmulatorAudioProcessorEditor()
{
    processorRef.getAPVTS().removeParameterListener (wireTypeParamId, this);
}

void WireEmulatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (Theme::background));
}

void WireEmulatorAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (20);

    const auto boostyWidth = BoostyLinkButton::widthForHeight (boostyButtonHeight);
    auto bottomRow = bounds.removeFromBottom (boostyButtonHeight);
    boostyLinkButton->setBounds (bottomRow.removeFromRight (boostyWidth));

    bounds.removeFromBottom (8);

    titleLabel.setBounds (bounds.removeFromTop (28));
    subtitleLabel.setBounds (bounds.removeFromTop (18));
    bounds.removeFromTop (20);

    const auto totalButtonWidth = WireType::count * buttonSize + (WireType::count - 1) * buttonGap;
    auto buttonRow = bounds.removeFromTop (buttonSize).withSizeKeepingCentre (totalButtonWidth, buttonSize);

    for (int i = 0; i < WireType::count; ++i)
    {
        wireButtons[static_cast<size_t> (i)]->setBounds (buttonRow.removeFromLeft (buttonSize));

        if (i < WireType::count - 1)
            buttonRow.removeFromLeft (buttonGap);
    }

    bounds.removeFromTop (16);
    descriptionLabel.setBounds (bounds);
}

void WireEmulatorAudioProcessorEditor::parameterChanged (const juce::String& parameterID, float)
{
    if (parameterID == wireTypeParamId)
    {
        const auto wireType = processorRef.getSelectedWireType();
        updateButtonSelection (wireType);
        updateDescription (wireType);
    }
}

void WireEmulatorAudioProcessorEditor::selectWireType (const int wireType)
{
    if (auto* param = processorRef.getAPVTS().getParameter (wireTypeParamId))
        param->setValueNotifyingHost (param->convertTo0to1 (static_cast<float> (wireType)));
}

void WireEmulatorAudioProcessorEditor::updateButtonSelection (const int wireType)
{
    for (int i = 0; i < WireType::count; ++i)
        wireButtons[static_cast<size_t> (i)]->setSelected (i == wireType);
}

void WireEmulatorAudioProcessorEditor::updateDescription (const int wireType)
{
    descriptionLabel.setText (WireType::description (wireType), juce::dontSendNotification);
}
