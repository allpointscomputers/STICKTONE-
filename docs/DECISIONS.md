# Decisions

## Decision 1: JUCE + CMake

A JUCE CMake setup is the most maintainable foundation for a cross-platform drum instrument and allows future expansion to standalone and plugin targets.

## Decision 2: Procedural fallback before sample assets

The project uses synthesised drum voices as a clear placeholder while sample import and production assets are implemented later.

## Decision 3: Keep the code buildable in incremental slices

The repo intentionally ships a small but functional slice rather than a large unbuildable prototype. This reduces risk and keeps the project in a working state throughout evolution.

## Decision 4: Do not claim unsupported plugin targets

VST3 and AU support will be documented and verified only after the required platform/toolchain is available.
