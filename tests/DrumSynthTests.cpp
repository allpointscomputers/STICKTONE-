# STICKTONE BY WAVEY BEAT

A JUCE-based drum production foundation. This repository is intentionally a staged implementation rather than a finished commercial plugin. It now includes a working standalone app shell, drum synthesis, kit state, sample-library scaffolding, groove metadata, and mixer/preset support.

## Current status

- Standalone JUCE app foundation: implemented
- Drum synthesis: implemented
- Kit state / persistence: implemented
- Sample library foundation: implemented
- Groove metadata foundation: implemented
- Mixer and preset foundation: implemented
- Full plugin validation: pending on a real host/toolchain

## Build

```bash
cmake -B build -S .
cmake --build build
```

## Tests

```bash
ctest --test-dir build --output-on-failure
```

## Notes

This project is still in active product development. The repo is intentionally structured to evolve toward a larger drum-production workflow without claiming the final product is complete.

