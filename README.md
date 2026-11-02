# Test plan

Initial manual smoke tests:

1. Build VST3 and Standalone on Windows and macOS.
2. Open the standalone app and verify audio device selection.
3. Click each kit button; confirm audible output.
4. Send MIDI notes 36, 38, 42, 46 and 49 from a DAW or MIDI keyboard.
5. Test repeated hits and overlapping voices.
6. Change kit-piece levels and confirm the result changes.
7. Save and reload plugin state in a host.
8. Verify mono and stereo output configurations.
9. Stress-test at 44.1, 48, 88.2 and 96 kHz with small and large buffer sizes.
10. Run host validation and inspect for audio-thread allocations, glitches and stuck notes.

Automated tests should be added for note mapping, state round-tripping, groove scheduling, sample playback bounds, choke groups and MIDI file import/export.
