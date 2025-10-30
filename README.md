# TEC Synth

## Project Overview

TEC Synth is a polyphonic wavetable synthesizer project that combines **MATLAB-based modeling** with a **C++/JUCE plugin implementation**.

Initially, MATLAB is used to model and analyze waveforms, perform wavetable generation, and prototype signal processing algorithms. The results from MATLAB are then implemented in C++ using the JUCE framework to create a real-time audio plugin supporting multiple oscillator types (Sine, Saw, Square, Triangle) with full MIDI integration.

### Features

- **Oscillator Types**: Sine, Saw, Square, Triangle, implemented as band-limited wavetable oscillators.
- **Wavetable Synthesis**: Uses precomputed waveform banks for alias-free playback across the frequency spectrum.
- **MIDI Integration**: Converts MIDI note numbers to frequency for accurate pitch control.
- **Amplitude Control**: Supports velocity-based amplitude scaling.
- **Waveform Switching**: Unified interface through `OscillatorWrapper` to change waveform type at runtime.
- **JUCE GUI**: Includes a simple interface with waveform selector, velocity slider, and MIDI keyboard component.

### Architecture

- **MATLAB Modeling**: Prototyping waveforms and signal processing routines, testing wavetable generation, harmonic content, and amplitude scaling.
- **OscillatorBase**: Abstract base class defining interface and core phase/frequency handling.
- **WavetableOscillator**: Implements band-limited waveform generation with linear interpolation and bank selection.
- **Concrete Waveforms**: `Sine`, `Saw`, `Square`, `Triangle` extend `WavetableOscillator` and generate corresponding wave tables.
- **OscillatorWrapper**: High-level manager that encapsulates all oscillator types and exposes a single interface for waveform selection, frequency, and amplitude control.
- **SynthVoice**: Represents a single voice of the synthesizer, managing note on/off, rendering audio blocks, and connecting to `OscillatorWrapper`.
- **GUI**: JUCE-based user interface for controlling waveform type, velocity, and playing notes via a MIDI keyboard.

### Purpose

The project serves as an educational and functional digital synthesizer, demonstrating:

- MATLAB-based waveform modeling and prototyping
- Band-limited wavetable synthesis techniques
- Real-time audio processing in C++
- MIDI handling and voice management
- GUI integration with JUCE
