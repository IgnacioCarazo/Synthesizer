# TEC Synth Plugin

## Overview

This project contains the source code and CMake build configuration for the **TEC Synth** audio plugin.  
It is built using the **JUCE** framework and can generate the following formats:

- **VST3**
- **AU**
- **AUv3**
- **Standalone Application**

---

## Requirements

- C++17 compatible compiler
- CMake **3.24+**
- JUCE cloned locally: https://github.com/juce-framework/JUCE

> Update the JUCE path inside `CMakeLists.txt`:
>
> ```cmake
> add_subdirectory(/path/to/JUCE JUCE-build)
> ```

---

## Build Instructions

### 1. Configure the build folder

```bash
cd /path/to/TEC-Synthesizer
mkdir build
cd build
cmake ..
```

### 2. Build Debug or Release

#### Debug

```bash
cmake --build . --config Debug
```

#### Release

```bash
cmake --build . --config Release
```

## Plugin Output Locations

After building, the generated binaries will be inside:

```bash
build/TECT-Synthesizer_artefacts/<Config>/
```

Where <Config> is either:

- Debug
- Release

Inside each config folder you'll find:

- VST3: VST3/TECSynth.vst3
- AU: AU/TECSynth.component
- Standalone APP: Standalone/TECSynth.app

Example path:

```bash
build/TEC-Synthesizer_artefacts/Release/VST3/TECSynth.vst3
```

## Using the Plugin in Ableton (macOS)

Copy or symlink the generated .vst3 file into:

```bash
~/Library/Audio/Plug-Ins/VST3/
```

Then restart Ableton and enable:

Preferences → Plug-ins → Use VST3 Plug-ins

## Notes

- When using VSCode, CMake will generate both:

  - Debug/

  - Release/

- Your plugin will not appear in Ableton unless you build Release and copy the resulting .vst3 in the respective folder.
