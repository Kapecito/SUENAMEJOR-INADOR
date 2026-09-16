cmake_minimum_required(VERSION 3.20)
project(SUENAMEJOR_INADOR VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(SUENAMEJOR_INADOR
    COMPANY_NAME "Doofenshmirtz Evil Inc"
    PLUGIN_MANUFACTURER_CODE Doof
    PLUGIN_CODE Smei
    FORMATS VST3 AU
    PRODUCT_NAME "SUENAMEJOR - INADOR"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(SUENAMEJOR_INADOR
    PRIVATE
        Source/PluginProcessor.h
        Source/PluginProcessor.cpp
        Source/PluginEditor.h
        Source/PluginEditor.cpp
)

target_compile_definitions(SUENAMEJOR_INADOR
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(SUENAMEJOR_INADOR
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)