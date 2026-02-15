# Phase 09: Runtime Profile Baseline & Hot Path Identification

## Metadata

| Field | Value |
|---|---|
| Phase ID | 09 |
| Prerequisites | Phase 06 |
| Estimated Complexity | Medium |
| Estimated File Count | 6 created, 1 modified |
| PRD Sections | 4.1-4.6 Runtime Performance, 7.1 Frame Budget |

---

## Objective

Run benchmarks on core runtime operations, identify the top 10 hot paths, and document findings to guide subsequent optimization phases.

---

## Background

The PRD requires <16ms per frame for UI operations and <8ms soft limit for editor operations. Before optimizing runtime performance, we need detailed measurements of the rendering pipeline, syntax highlighting, markdown parsing, configuration lookups, and theme engine operations. This data drives decisions in Phases 14-28.

---

## Scope

### Tasks

1. **Create runtime benchmarks**:

   **`benchmarks/bench_html_renderer.cpp`**:
   - `BM_HtmlRenderer_SmallDoc`: render 10-line markdown
   - `BM_HtmlRenderer_MediumDoc`: render 500-line markdown
   - `BM_HtmlRenderer_LargeDoc`: render 5000-line markdown
   - `BM_HtmlRenderer_IncrementalUpdate`: re-render after single-line change
   - Measure time per render pass

   **`benchmarks/bench_syntax_highlighter.cpp`**:
   - `BM_SyntaxHighlighter_FullTokenize`: full document tokenization
   - `BM_SyntaxHighlighter_IncrementalTokenize`: single-line change
   - `BM_SyntaxHighlighter_LargeFile`: 10K lines
   - Parameterize by language (Markdown, C++, Python)

   **`benchmarks/bench_markdown_parser.cpp`**:
   - `BM_MarkdownParser_Parse`: full document parse
   - `BM_MarkdownParser_IncrementalParse`: parse after edit
   - `BM_MarkdownParser_ComplexDocument`: headers, lists, code blocks, links
   - Parameterize by document size

   **`benchmarks/bench_config.cpp`**:
   - `BM_Config_GetString`: string value lookup throughput
   - `BM_Config_GetInt`: integer value lookup throughput
   - `BM_Config_GetBool`: boolean value lookup throughput
   - `BM_Config_SetAndNotify`: set value with event publication

   **`benchmarks/bench_theme_engine.cpp`**:
   - `BM_ThemeEngine_ColorLookup`: get color by name
   - `BM_ThemeEngine_BrushCache`: cached wxBrush retrieval
   - `BM_ThemeEngine_PenCache`: cached wxPen retrieval
   - `BM_ThemeEngine_FontLookup`: cached wxFont retrieval
   - `BM_ThemeEngine_ApplyTheme`: full theme switch

2. **Run all benchmarks and record baseline**:
   - Save baseline JSON to `benchmarks/baseline/`

3. **Create `docs/v6_docs/runtime_profile.md`**:
   - Top 10 hot paths ranked by total CPU time
   - Per-operation timings with statistical confidence
   - Cache miss analysis where measurable
   - Memory allocation frequency per operation
   - Specific optimization targets identified for subsequent phases

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `benchmarks/bench_html_renderer.cpp` |
| Create | `benchmarks/bench_syntax_highlighter.cpp` |
| Create | `benchmarks/bench_markdown_parser.cpp` |
| Create | `benchmarks/bench_config.cpp` |
| Create | `benchmarks/bench_theme_engine.cpp` |
| Create | `docs/v6_docs/runtime_profile.md` |
| Modify | `benchmarks/CMakeLists.txt` |

---

## Implementation Notes

- For HtmlRenderer benchmarks, construct a minimal renderer without wxWidgets dependencies where possible. If wxWidgets is required, use mock objects or test in a context that doesn't need a display.
- ThemeEngine benchmarks should test the cached lookup paths (hot) separately from the theme loading paths (cold).
- Config benchmarks should include both the fast path (cached value) and the slow path (file read).
- Use `benchmark::Counter` to report throughput in operations/second in addition to time per operation.
- The runtime_profile.md should explicitly identify which hot paths will be addressed in which future phases.

---

## Acceptance Criteria

- [ ] All 5 new benchmark files build and run successfully
- [ ] Each benchmark produces valid JSON output
- [ ] `runtime_profile.md` lists top 10 hot paths with measured timings
- [ ] Baseline JSON saved to `benchmarks/baseline/`
- [ ] Documentation identifies specific optimization targets with phase references
- [ ] All benchmarks complete in <60 seconds total
- [ ] Benchmark results are reproducible (variance <15%)

---

## Testing Strategy

- Build with benchmarks enabled and run full suite
- Verify JSON output is parseable by `benchmark_compare.py` (Phase 07)
- Run twice and compare for stability
- Review runtime_profile.md for completeness and accuracy
