# Implementation Plan

## Phase A — Foundation

Status: Implemented (baseline)

- Repository scaffold and build entry points
- JUCE + CMake setup
- Standalone app skeleton
- Basic drum synthesis for development/testing
- Automated test scaffold for note mapping and audio generation

## Phase B — Drum engine core

Status: In progress

- Expand drum mappings beyond the initial set
- Add velocity handling and note-to-piece logic
- Preserve a clear distinction between procedural fallback voices and future sample-based content

## Phase C — Production interface

Status: Not started

- Kit view and production UI panels
- Mixer and effect routing
- Presets and kit management
- Sample import flow and library manifest

## Phase D — Creative workflow

Status: Not started

- Groove browser
- Tap2Find
- Song Creator and arrangement
- MIDI export and drag-and-drop support

## Phase E — Integrations

Status: Not started

- VST3 / AU host validation
- Standalone device handling and multi-output setup
- State restoration and plugin serialization

## Phase F — Stabilize and package

Status: Not started

- Final automated validation
- Packaging and release notes
- Unsupported platform documentation

## Feature matrix

| Feature | Status |
|---|---|
| JUCE + CMake foundation | Implemented |
| Standalone app shell | Implemented |
| Drum synthesis fallback | Implemented |
| MIDI note mapping | Implemented |
| Sample library architecture | In progress |
| Mixer/effects | Not started |
| Groove browser | Not started |
| Tap2Find | Not started |
| Song Creator | Not started |
| VST3/AU validation | Blocked (requires platform verification) |
| Packaging | Not started |
