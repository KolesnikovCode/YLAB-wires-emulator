cmake_minimum_required(VERSION 3.22)

project(WireEmulator VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.12
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_binary_data(WireEmulatorAssets
    SOURCES
        Source/assets/gold.png
        Source/assets/copper.png
        Source/assets/gold-on-wood.png
        Source/assets/copper-on-wood.png
        Source/assets/boosty_logo.svg
)

juce_add_plugin(WireEmulator
    COMPANY_NAME "YLAB"
    BUNDLE_ID com.ylab.wires-emulator
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Ylab
    PLUGIN_CODE Wemu
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "wires-emulator"
    NEEDS_WEB_BROWSER FALSE
    NEEDS_CURL FALSE
)

target_sources(WireEmulator
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

target_compile_definitions(WireEmulator
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(WireEmulator
    PRIVATE
        WireEmulatorAssets
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
