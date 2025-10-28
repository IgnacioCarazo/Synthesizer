cmake_minimum_required(VERSION 3.24)

# Project name
project(TEC-Synthesizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_XCODE_GENERATE_SCHEME OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Agregar JUCE
add_subdirectory(/Users/ignaciocarazo/Documents/JUCE JUCE-build)


# src files
set(SourceFiles
    src/PluginProcessor.cpp
    src/PluginProcessor.h
    src/PluginEditor.cpp
    src/PluginEditor.h
)

juce_add_plugin(${PROJECT_NAME}
    VERSION 1.0.0   
    BUNDLE_NAME "TECSynth"
    COMPANY_NAME "SynthCompany"
    BUNDLE_IDENTIFIER "com.synthcompany.tecsynth"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE NCSP
    PLUGIN_CODE NC01
    FORMATS VST3 AU AUv3 Standalone
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${SourceFiles})


target_sources(${PROJECT_NAME}
    PRIVATE
        ${SourceFiles}
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Juce libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_analytics
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_core
        juce::juce_data_structures
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_audio_utils
        juce::juce_dsp
)