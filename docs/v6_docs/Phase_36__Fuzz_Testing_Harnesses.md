# Phase 36: Fuzz Testing Harnesses

## Metadata

| Field | Value |
|---|---|
| Phase ID | 36 |
| Prerequisites | Phase 06 |
| Estimated Complexity | Medium |
| Estimated File Count | 9 created, 1 modified |
| PRD Sections | 6.5 Fuzz Testing |

---

## Objective

Create libFuzzer harnesses for all parser and loader entry points. Fuzz testing discovers crashes, hangs, and undefined behavior from unexpected inputs.

---

## Background

The PRD mandates fuzz testing with libFuzzer or AFL++ for: Markdown parsing, YAML loading, Extension manifest parsing, Theme loading, and Mermaid rendering. Fuzzing is one of the most effective techniques for finding security vulnerabilities and stability bugs in parsers.

---

## Scope

### Tasks

1. **Create `fuzz/` directory structure**:
   - `fuzz/CMakeLists.txt`: build configuration for fuzz targets
   - `fuzz/corpus/`: seed corpus directories for each target
   - Each target is a standalone executable linked with `-fsanitize=fuzzer,address`

2. **Create `fuzz/CMakeLists.txt`**:
   - Gated behind `MARKAMP_BUILD_FUZZ` option (OFF by default)
   - Only available with Clang (libFuzzer is Clang-only)
   - Compile flags: `-fsanitize=fuzzer,address -fno-omit-frame-pointer`
   - Each target links against `markamp_core`

3. **Create fuzz harnesses**:

   **`fuzz/fuzz_markdown_parser.cpp`**:
   - Fuzz `Md4cWrapper` or `MarkdownParser` with arbitrary byte input
   - Entry: `LLVMFuzzerTestOneInput(data, size)`
   - Construct string_view from input, pass to parser
   - Must not crash, must not hang (timeout: 10s)

   **`fuzz/fuzz_yaml_config.cpp`**:
   - Fuzz Config YAML loading path
   - Write input bytes to a temp file, attempt to load as config
   - Must handle malformed YAML gracefully

   **`fuzz/fuzz_extension_manifest.cpp`**:
   - Fuzz `ExtensionManifest` JSON parsing
   - Pass arbitrary bytes as JSON string
   - Must handle invalid JSON, missing fields, wrong types

   **`fuzz/fuzz_theme_loader.cpp`**:
   - Fuzz ThemeLoader with arbitrary markdown-with-YAML-frontmatter input
   - Must handle malformed frontmatter, missing keys, invalid CSS values

   **`fuzz/fuzz_html_sanitizer.cpp`**:
   - Fuzz HtmlSanitizer with arbitrary HTML input
   - Critical for security: must not allow XSS bypass
   - Must handle deeply nested tags, malformed HTML, null bytes

   **`fuzz/fuzz_mermaid_renderer.cpp`**:
   - Fuzz MermaidRenderer with arbitrary diagram text
   - Must handle syntax errors, deeply nested structures, huge inputs

4. **Create seed corpus for each target**:
   - `fuzz/corpus/markdown/`: 10+ valid markdown files of varying complexity
   - `fuzz/corpus/yaml/`: 10+ valid YAML config files
   - `fuzz/corpus/json/`: 10+ valid extension manifest JSON files
   - `fuzz/corpus/themes/`: 10+ valid theme markdown files
   - `fuzz/corpus/html/`: 10+ valid HTML snippets
   - `fuzz/corpus/mermaid/`: 10+ valid Mermaid diagram definitions

5. **Create `scripts/run_fuzz.sh`**:
   - Builds fuzz targets
   - Runs each target for configurable duration (default: 60s)
   - Reports any findings (crashes, timeouts)
   - Stores crash artifacts in `fuzz/artifacts/`

6. **Create `docs/v6_docs/fuzz_testing.md`**:
   - Document fuzzing setup and prerequisites
   - How to build fuzz targets
   - How to run fuzzing locally
   - How to reproduce findings
   - How to add new fuzz targets
   - How to expand the seed corpus

7. **Update root `CMakeLists.txt`**:
   - Add `MARKAMP_BUILD_FUZZ` option
   - `add_subdirectory(fuzz)` when enabled

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `fuzz/CMakeLists.txt` |
| Create | `fuzz/fuzz_markdown_parser.cpp` |
| Create | `fuzz/fuzz_yaml_config.cpp` |
| Create | `fuzz/fuzz_extension_manifest.cpp` |
| Create | `fuzz/fuzz_theme_loader.cpp` |
| Create | `fuzz/fuzz_html_sanitizer.cpp` |
| Create | `fuzz/fuzz_mermaid_renderer.cpp` |
| Create | `scripts/run_fuzz.sh` |
| Create | `docs/v6_docs/fuzz_testing.md` |
| Modify | `CMakeLists.txt` |

---

## Implementation Notes

- Each fuzz harness must define `extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)`. Return 0 for success.
- libFuzzer is built into Clang. Compile with `-fsanitize=fuzzer` and it provides the main function.
- Combine with ASan (`-fsanitize=fuzzer,address`) to catch memory errors triggered by fuzz input.
- Seed corpus helps the fuzzer start from valid inputs and mutate toward crashes. Better seeds = faster bug finding.
- Timeout per input: default 1200ms (libFuzzer `-timeout=1200`). Inputs that take longer are considered hangs.
- The fuzz harness must not call `exit()`, `abort()`, or throw unhandled exceptions (these are treated as crashes by the fuzzer).
- For file-based fuzzing (YAML, themes), use `mkstemp()` to create temporary files, fuzz the file content, then clean up.

---

## Acceptance Criteria

- [ ] All 6 fuzz targets compile with `-fsanitize=fuzzer`
- [ ] Each target runs for 60 seconds without crashes on seed corpus
- [ ] Seed corpus contains at least 10 valid samples per target
- [ ] `run_fuzz.sh` automates harness execution and reports findings
- [ ] `fuzz_testing.md` documents setup, usage, and corpus expansion
- [ ] Each harness handles null bytes, empty input, and huge input gracefully
- [ ] No fuzz target leaks memory (verified by ASan)
- [ ] Crash artifacts are stored in `fuzz/artifacts/` for reproduction

---

## Testing Strategy

- Build all fuzz targets and verify compilation
- Run each target for 60 seconds on seed corpus
- Review any crashes found during initial fuzzing
- Verify run_fuzz.sh produces correct output
- Expand corpus with edge-case inputs and rerun
