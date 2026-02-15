# Phase 18 -- Inline LaTeX Rendering Enhancement

## Objective

Enhance the existing MathRenderer (LaTeX->Unicode) with full block-level LaTeX rendering, equation numbering, LaTeX macro definitions, and improved rendering quality. Add support for LaTeX environments (align, matrix, cases), display-mode equations, and a LaTeX preview panel. Extends the existing IMathRenderer interface.

## Prerequisites

- Existing MathRenderer, IMathRenderer interface
- Existing HtmlRenderer (for block-level rendering)
- Existing PreviewPanel

## Feature References (PRD)

- PRD #44: Inline LaTeX Rendering
- PRD #16: Inline LaTeX Support (notebook feature)
- PRD #48: Inline Mathematical Diagrams

## Data Structures to Implement

### File: `src/core/LatexEngine.h`

```cpp
#pragma once

#include "IMathRenderer.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

struct LatexMacro
{
    std::string name;           // e.g., "\\RR"
    std::string expansion;      // e.g., "\\mathbb{R}"
    int arg_count{0};           // Number of arguments
};

struct LatexRenderResult
{
    std::string output;         // Rendered output (Unicode, HTML, or SVG)
    bool success{false};
    std::string error;
    int equation_number{-1};    // -1 = unnumbered

    enum class Format : uint8_t { Unicode, Html, Svg } format{Format::Unicode};
};

struct LatexEnvironment
{
    std::string name;           // "align", "matrix", "cases", "equation"
    std::string content;
    bool numbered{false};
};

class LatexEngine
{
public:
    LatexEngine(EventBus& event_bus, Config& config);

    /// Render inline math ($...$) to display format.
    [[nodiscard]] auto render_inline(const std::string& latex) const -> LatexRenderResult;

    /// Render display math ($$...$$) to display format.
    [[nodiscard]] auto render_display(const std::string& latex) const -> LatexRenderResult;

    /// Render a LaTeX environment (align, matrix, etc.).
    [[nodiscard]] auto render_environment(const LatexEnvironment& env) const -> LatexRenderResult;

    /// Process all LaTeX in a markdown document. Returns HTML with rendered math.
    [[nodiscard]] auto process_document(const std::string& markdown) const -> std::string;

    /// Register a LaTeX macro for expansion.
    auto register_macro(const LatexMacro& macro) -> void;

    /// Load macros from a preamble string.
    auto load_preamble(const std::string& preamble) -> void;

    /// Get the next equation number.
    [[nodiscard]] auto next_equation_number() -> int;

    /// Reset equation numbering.
    auto reset_numbering() -> void;

private:
    EventBus& event_bus_;
    Config& config_;

    std::vector<LatexMacro> macros_;
    int equation_counter_{0};

    [[nodiscard]] auto expand_macros(const std::string& latex) const -> std::string;
    [[nodiscard]] auto render_to_unicode(const std::string& latex) const -> std::string;
    [[nodiscard]] auto render_to_html(const std::string& latex) const -> std::string;
    [[nodiscard]] auto detect_environments(const std::string& latex) const
        -> std::vector<LatexEnvironment>;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`render_inline(latex)`** -- Strip `$` delimiters. Expand macros. Render using existing MathRenderer pipeline. Return Unicode result for editor display.

2. **`render_display(latex)`** -- Strip `$$` delimiters. Expand macros. Render with display-mode formatting (centered, larger). Assign equation number if numbered environment.

3. **`render_environment(env)`** -- Handle LaTeX environments: `\begin{align}...\end{align}`, `\begin{matrix}`, `\begin{cases}`, etc. Each environment type has specific rendering rules.

4. **`process_document(markdown)`** -- Scan markdown for `$...$` (inline) and `$$...$$` (display) blocks. Replace each with rendered HTML/Unicode. Handle `\begin{...}` environments.

5. **`expand_macros(latex)`** -- For each registered macro, replace occurrences in the latex string. Handle macros with arguments.

6. **`load_preamble(preamble)`** -- Parse `\newcommand`, `\renewcommand`, `\def` from a preamble string. Register extracted macros.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LatexRenderErrorEvent)
std::string latex_source;
std::string error_message;
int line_number{0};
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `latex.render_mode` | string | `"unicode"` | Render mode: unicode, html |
| `latex.preamble_file` | string | `""` | Path to LaTeX preamble with custom macros |
| `latex.equation_numbering` | bool | `false` | Number display equations |
| `latex.auto_render` | bool | `true` | Auto-render LaTeX in preview |

## Test Cases

File: `tests/unit/test_latex_engine.cpp`

1. **Inline math** -- `$x^2$` renders to superscript.
2. **Display math** -- `$$\sum_{i=1}^n i$$` renders centered.
3. **Fraction** -- `$\frac{a}{b}$` renders as fraction.
4. **Greek letters** -- `$\alpha, \beta, \gamma$` renders Unicode Greek.
5. **Matrix environment** -- `\begin{matrix}a & b \\ c & d\end{matrix}` renders grid.
6. **Macro expansion** -- Define `\RR` = `\mathbb{R}`. `$\RR$` renders as double-struck R.
7. **Macro with args** -- Define `\norm{1}`. `$\norm{x}$` expands correctly.
8. **Equation numbering** -- Display math with numbering produces (1), (2), etc.
9. **Error handling** -- Invalid LaTeX `$\invalid{$` returns error without crash.
10. **Document processing** -- Markdown with 3 inline and 2 display math blocks. All rendered.
11. **Preamble loading** -- Preamble with 5 `\newcommand` definitions. All registered.
12. **Nested environments** -- `\begin{cases}` inside `$$...$$`. Renders correctly.

## Acceptance Criteria

- [ ] Inline `$...$` math renders correctly in editor and preview
- [ ] Display `$$...$$` math renders centered with optional numbering
- [ ] LaTeX environments (align, matrix, cases) render correctly
- [ ] Custom macros via `\newcommand` work
- [ ] Preamble file loading registers macros
- [ ] Error in LaTeX source produces clear error message, not crash
- [ ] Document-wide processing handles mixed markdown and LaTeX
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/LatexEngine.h` | LatexEngine, LatexMacro, LatexRenderResult |
| CREATE | `src/core/LatexEngine.cpp` | Full implementation |
| MODIFY | `src/core/MathRenderer.cpp` | Extend with environment support |
| MODIFY | `src/rendering/HtmlRenderer.cpp` | Integrate LatexEngine for display math |
| MODIFY | `src/core/Events.h` | Add LatexRenderErrorEvent |
| MODIFY | `src/CMakeLists.txt` | Add LatexEngine.cpp |
| CREATE | `tests/unit/test_latex_engine.cpp` | 12 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_latex_engine target |

## Architecture Notes

- LatexEngine wraps and extends existing MathRenderer (composition, not inheritance)
- Unicode rendering for editor inline display; HTML rendering for preview panel
- Macro expansion happens before rendering (preprocessing step)
- Constructor injection: LatexEngine(EventBus&, Config&)

## Estimated Complexity

**L** -- LaTeX parsing, environment handling, macro system, integration with two renderers, 12 tests.
