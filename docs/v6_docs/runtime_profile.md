# Runtime Profile — Phase 09 Baseline

## Benchmark Results (2026-02-15, Apple Silicon arm64, Debug)

### MarkdownParser

| Scenario                   | 10 para           | 100 para           | 500 para           |
| -------------------------- | ----------------- | ------------------ | ------------------ |
| Parse (md → AST)           | 144µs (12.9 MB/s) | 1.34ms (13.2 MB/s) | 6.64ms (13.3 MB/s) |
| Parse + Render (md → HTML) | 285µs (6.5 MB/s)  | 1.94ms (9.1 MB/s)  | 9.4ms (9.5 MB/s)   |

### SyntaxHighlighter

| Scenario       | 50 lines         | 200 lines        | 1000 lines        |
| -------------- | ---------------- | ---------------- | ----------------- |
| Tokenize (AoS) | 700µs (3.7 MB/s) | 2.8ms (3.5 MB/s) | 13.5ms (3.6 MB/s) |
| Tokenize (SoA) | 784µs (3.3 MB/s) | 3.0ms (3.2 MB/s) | 15.2ms (3.2 MB/s) |
| Render HTML    | 846µs (3.1 MB/s) | 3.2ms (3.0 MB/s) | 16.0ms (3.0 MB/s) |

### Config

| Operation                   | CPU Time   |
| --------------------------- | ---------- |
| get_string (YAML lookup)    | **294ns**  |
| get_bool                    | 266ns      |
| get_int                     | 262ns      |
| set + get (cache rebuild)   | **19.3µs** |
| Cached struct access (O(1)) | **6.4ns**  |

### ThemeRegistry

| Operation                | CPU Time |
| ------------------------ | -------- |
| get_theme (lookup by ID) | 209ns    |
| has_theme                | 77ns     |
| list_themes (126 themes) | **39µs** |
| theme_count              | 4.3ns    |

### HtmlRenderer

| Scenario                  | 10 para           | 100 para         | 500 para          |
| ------------------------- | ----------------- | ---------------- | ----------------- |
| Render (AST → HTML)       | 58µs (26 MB/s)    | 528µs (27 MB/s)  | 3.5ms (21 MB/s)   |
| Full Pipeline (md → HTML) | 292µs (5.1 MB/s)  | 2.7ms (5.4 MB/s) | 12.9ms (5.6 MB/s) |
| escape_html (static)      | 935ns (74.5 MB/s) | —                | —                 |
| slugify (static)          | 1.2µs             | —                | —                 |

## Top Hot Paths (by absolute time at 500 paragraphs)

1. **SyntaxHighlighter::tokenize** — 13.5ms/1000 lines (per-char scanning with hash lookups)
2. **HtmlRenderer full pipeline** — 12.9ms/500 paragraphs (parse + render)
3. **MarkdownParser::parse + render** — 9.4ms/500 paragraphs
4. **MarkdownParser::parse** — 6.6ms/500 paragraphs (md4c AST construction)
5. **ThemeRegistry::list_themes** — 39µs (vector copy of 126 ThemeInfo structs)
6. **Config::set** — 19.3µs per call (includes cache rebuild)
7. **SyntaxHighlighter::render_html** — 16ms/1000 lines
8. **Config::get_string** — 294ns (YAML node traversal)
9. **ThemeRegistry::get_theme** — 209ns (linear scan)
10. **Config::get_int/bool** — ~265ns

## Optimization Opportunities

1. **SyntaxHighlighter tokenize** — cache keyword/type hash sets per language definition
2. **HtmlRenderer** — pre-size output string, reduce allocations in render_node
3. **Config::set** — debounce cache rebuild if multiple set calls happen in sequence
4. **ThemeRegistry::get_theme** — switch from vector to unordered_map for O(1) lookup
5. **Config getters** — bypass YAML node in hot path; already solved by `cached()` accessor at 6.4ns
