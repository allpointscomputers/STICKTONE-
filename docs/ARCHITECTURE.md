# Architecture

## Overview

The project is intentionally structured as a simple modular foundation to support a larger drum instrument over time.

## Core modules

### Application shell

- `Source/Main.cpp` creates the JUCE document window and the initial app container.

### Audio engine

- `Source/Audio/DrumSynth.h` defines the public API for the drum engine.
- `Source/Audio/DrumSynth.cpp` implements the procedural fallback voices for kick, snare, hat, tom, and crash sounds.

### Test layer

- `tests/DrumSynthTests.cpp` provides a basic deterministic test harness for note mapping and generation.

## Design principles

- Keep audio generation separate from UI logic.
- Prefer a small but maintainable foundation over premature abstraction.
- Preserve a clear path toward sample-based voices and richer kit controls.
- Keep all fallback synthesis clearly documented as non-recorded placeholders.
