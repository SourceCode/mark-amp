# Contributing

> For the full contributing guide, see [CONTRIBUTING.md](../CONTRIBUTING.md) in the project root.

---

## Quick Reference

### Build & Test

```bash
# Debug build
cmake --preset debug
cmake --build build/debug -j$(sysctl -n hw.ncpu)

# Run all tests (451 targets)
cd build/debug && ctest --output-on-failure
```

### Branching

1. Fork the repository
2. Branch from `develop`: `git checkout -b feature/my-feature`
3. Commit with conventional format: `feat: add mermaid error overlay`
4. Push and open a PR against `develop`

### Commit Convention

```
type: short description

Types: feat, fix, docs, style, refactor, perf, test, chore
```

### PR Checklist

- [ ] Tests pass (`ctest --output-on-failure`)
- [ ] Code formatted (`clang-format`)
- [ ] No new compiler warnings
- [ ] No `any` or `unknown` types
- [ ] Documentation updated if applicable

### Code Style

- C++23 standard
- `#pragma once` for header guards
- Trailing return types: `auto foo() -> int;`
- `snake_case` functions, `PascalCase` classes
- Member variables: `snake_case_` (trailing underscore)

→ See [CONTRIBUTING.md](../CONTRIBUTING.md) for complete details.
