# STICKTONE BY WAVEY BEAT

A minimal JUCE-based foundation for the StickTone drum instrument. This repository now contains a working standalone shell and a deterministic drum synthesis foundation. The goal is to evolve it toward a full commercial drum production workflow without pretending the product is complete.

## Current foundation

- JUCE project via CMake
- Standalone app shell
- Drum voice synthesis for kick, snare, hi-hat, tom, and crash
- MIDI-note mapping helpers
- Initial automated test scaffold
- Architecture and implementation planning documents

## Requirements

- CMake 3.22 or newer
- C++17 compiler
- Internet access is required while CMake fetches JUCE on first configure

## Build

```bash
cmake -B build -S .
cmake --build build
```

## Run the standalone app

```bash
./build/StickToneApp
```

## Run tests

```bash
ctest --test-dir build --output-on-failure
```

## Status

This is the first buildable slice of the product and not a final commercial release. Platform-specific plugin targets and expanded sample-based features remain future work.
