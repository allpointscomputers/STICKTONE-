# Build and Test

## Environment

The project is intended to build on any machine with a modern C++ compiler and CMake installed.

## Commands

```bash
cmake -B build -S .
cmake --build build
ctest --test-dir build --output-on-failure
```

## Verification status

- File structure created successfully.
- Configuration/build verification depends on the local system toolchain.
- This repo has not yet been fully compiled in this environment until the commands are run here.
