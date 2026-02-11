# Agent Context for MarkAmp

## Project Overview

MarkAmp is a cross-platform Markdown editor built with C++23 and wxWidgets.

## Key Workflows

- Build: `cmake -B build -S . && cmake --build build`
- Test: `ctest --test-dir build`
- Release: Run `cpp-git-commit-version-push` workflow.

## Codebase Structure

- `src/`: Source code
- `include/`: Header files (if separate, but here headers are in src)
- `tests/`: Unit and integration tests
- `scripts/`: Helper scripts for build and packaging

## Standards

- C++ Standard: C++23
- Documentation: Markdown in `docs/`
- Versioning: Semantic Versioning (managed in `CMakeLists.txt`)
