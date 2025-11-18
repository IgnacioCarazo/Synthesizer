# TEC Synth Plugin

## Overview

This folder contains the source code and CMake configuration for the TEC Synth audio plugin. The plugin is implemented using the JUCE framework and supports VST3, AU, AUv3, and Standalone formats.

## Requirements

- **C++17 compatible compiler**
- **CMake 3.24 or higher**
- **JUCE framework** cloned locally: [https://github.com/juce-framework/JUCE](https://github.com/juce-framework/JUCE)

> Make sure to update the JUCE path in `CMakeLists.txt`:
>
> ```cmake
> add_subdirectory(/path/to/JUCE JUCE-build)
> ```

## Building the Plugin

1. Open a terminal and navigate to the plugin folder:

```bash
cd /path/to/TEC-Synthesizer
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```
After building, you should look for the plugin inside the folder:

build/TEC-Synthesizer_artefacts/Debug/Standalone
