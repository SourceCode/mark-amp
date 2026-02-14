# Phase 18: Math/KaTeX Rendering Foundation

**Priority:** Low
**Estimated Scope:** ~6 files affected
**Dependencies:** None

## Objective

Implement basic math expression rendering in the preview panel, replacing the current CSS-only placeholder that displays math blocks as monospace text without actual typesetting.

## Background/Context

### Current State

PreviewPanel.cpp line 594 contains a comment `/* Item 39: KaTeX / math placeholder */` followed by CSS classes for `.math-inline` and `.math-block`. These classes style math expressions as monospace code blocks with accent colors -- they do NOT perform any mathematical typesetting.

The Markdown parser (md4c) supports LaTeX-style math delimiters when the `MD_FLAG_LATEXMATHSPANS` flag is enabled. When it encounters `$...$` (inline) or `$$...$$` (block), it produces `<x-equation>` tags that the HtmlRenderer can detect and wrap in `.math-inline` or `.math-block` divs.

However, no actual rendering engine converts the LaTeX source into typeset output. The raw LaTeX source is displayed as-is in a styled `<div>`.

### Rendering Options

For a C++ desktop application, there are several approaches to render LaTeX math:

1. **KaTeX (via embedded JavaScript)** -- The most common web approach. Requires a JavaScript engine (QuickJS, V8, or WebView).
2. **MathJax (via embedded JavaScript)** -- Heavier than KaTeX but more complete. Same JS engine requirement.
3. **texmath (Haskell library)** -- Not practical for C++ embedding.
4. **Custom SVG generation** -- Convert a subset of LaTeX math to SVG paths. Lightweight but limited.
5. **wxMathPlot or LaTeX-to-image** -- Shell out to `latex`/`dvipng`. Requires LaTeX installation.

The recommended approach for MarkAmp is **KaTeX via QuickJS**, consistent with the architecture already designed for MermaidRenderer (which also needs a JS engine for full Mermaid.js support).

However, as a first step, this phase implements a **server-side rendering approach** where KaTeX's JavaScript is executed once at startup to pre-render math expressions into HTML/MathML, which the preview panel can display natively.

### Simpler First Step: MathML Output

Modern browsers (and wxHtmlWindow to some extent) support MathML. A simpler approach for the MVP is to:
1. Parse LaTeX math syntax into MathML using a lightweight C++ parser
2. Embed the MathML directly in the HTML output
3. Let wxWebView (or wxHtmlWindow) render it natively

## Detailed Tasks

### Task 1: Enable MD_FLAG_LATEXMATHSPANS in Md4cWrapper

**File:** `/Users/ryanrentfro/code/markamp/src/core/Md4cWrapper.cpp`

Check if `MD_FLAG_LATEXMATHSPANS` is currently enabled. If not, add it to the parser flags:

```cpp
// In the md4c parse call:
unsigned flags = MD_FLAG_TABLES
               | MD_FLAG_STRIKETHROUGH
               | MD_FLAG_TASKLISTS
               | MD_FLAG_LATEXMATHSPANS;  // Enable math
```

### Task 2: Detect math nodes in HtmlRenderer

**File:** `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.cpp`

When md4c produces `MD_SPAN_LATEXMATH` (inline) or `MD_SPAN_LATEXMATH_DISPLAY` (block), ensure HtmlRenderer wraps them correctly:

```cpp
case MD_SPAN_LATEXMATH:
    if (entering)
        output += "<span class=\"math-inline\">";
    else
        output += "</span>";
    break;

case MD_SPAN_LATEXMATH_DISPLAY:
    if (entering)
        output += "<div class=\"math-block\">";
    else
        output += "</div>";
    break;
```

If the AST-based renderer is used instead, handle `MdNodeType::MathInline` and `MdNodeType::MathBlock` similarly.

### Task 3: Create MathRenderer interface

**File:** New file `/Users/ryanrentfro/code/markamp/src/core/IMathRenderer.h`

```cpp
#pragma once

#include <string>

namespace markamp::core
{

/// Interface for rendering LaTeX math expressions to HTML.
class IMathRenderer
{
public:
    virtual ~IMathRenderer() = default;

    /// Render a LaTeX math expression to HTML/MathML.
    /// Returns the rendered HTML string, or the original source wrapped in
    /// an error div if rendering fails.
    [[nodiscard]] virtual auto render(const std::string& latex_source,
                                       bool display_mode) -> std::string = 0;

    /// Returns true if the renderer is available and functional.
    [[nodiscard]] virtual auto is_available() const -> bool = 0;
};

} // namespace markamp::core
```

### Task 4: Create a basic MathRenderer implementation

**File:** New file `/Users/ryanrentfro/code/markamp/src/core/MathRenderer.h`
**File:** New file `/Users/ryanrentfro/code/markamp/src/core/MathRenderer.cpp`

For the MVP, implement a basic renderer that converts common LaTeX math expressions to HTML with Unicode characters. This handles common cases (fractions, superscripts, subscripts, Greek letters, common operators) without requiring a JavaScript engine:

```cpp
#pragma once

#include "IMathRenderer.h"

#include <string>
#include <unordered_map>

namespace markamp::core
{

/// Basic math renderer that converts common LaTeX expressions to HTML
/// using Unicode math symbols and HTML entities. Does not require a
/// JavaScript engine. Handles ~80% of common Markdown math use cases.
class MathRenderer : public IMathRenderer
{
public:
    MathRenderer();

    [[nodiscard]] auto render(const std::string& latex_source,
                               bool display_mode) -> std::string override;

    [[nodiscard]] auto is_available() const -> bool override { return true; }

private:
    /// Map of LaTeX commands to Unicode/HTML replacements
    std::unordered_map<std::string, std::string> symbol_map_;

    /// Convert LaTeX source to HTML with Unicode math symbols
    [[nodiscard]] auto convert_to_html(const std::string& source) -> std::string;

    /// Handle superscripts (^) and subscripts (_)
    [[nodiscard]] auto process_scripts(const std::string& source) -> std::string;

    /// Replace \frac{a}{b} with HTML fraction
    [[nodiscard]] auto process_fractions(const std::string& source) -> std::string;
};

} // namespace markamp::core
```

The symbol map should include at minimum:
- Greek letters: `\alpha` -> "&#945;", `\beta` -> "&#946;", etc.
- Operators: `\sum` -> "&#8721;", `\prod` -> "&#8719;", `\int` -> "&#8747;"
- Relations: `\leq` -> "&#8804;", `\geq` -> "&#8805;", `\neq` -> "&#8800;"
- Arrows: `\rightarrow` -> "&#8594;", `\leftarrow` -> "&#8592;"
- Common: `\infty` -> "&#8734;", `\pm` -> "&#177;", `\times` -> "&#215;"

### Task 5: Integrate MathRenderer into HtmlRenderer

**File:** `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.h` and `HtmlRenderer.cpp`

Add `IMathRenderer*` as an optional dependency (like `IMermaidRenderer`):

```cpp
class HtmlRenderer
{
public:
    void set_math_renderer(IMathRenderer* renderer);

private:
    IMathRenderer* math_renderer_{nullptr};
};
```

When rendering math spans, if a math renderer is available, use it:
```cpp
case MD_SPAN_LATEXMATH:
    if (entering)
    {
        // Buffer the math content
        in_math_span_ = true;
        math_buffer_.clear();
    }
    else
    {
        in_math_span_ = false;
        if (math_renderer_ && math_renderer_->is_available())
        {
            output += math_renderer_->render(math_buffer_, false);
        }
        else
        {
            output += "<span class=\"math-inline\">" + html_escape(math_buffer_) + "</span>";
        }
    }
    break;
```

### Task 6: Create and wire MathRenderer in MarkAmpApp

**File:** `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.h` -- Add `std::unique_ptr<core::MathRenderer> math_renderer_`
**File:** `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

```cpp
// After MermaidRenderer initialization:
math_renderer_ = std::make_unique<core::MathRenderer>();
MARKAMP_LOG_INFO("MathRenderer initialized");
```

Pass to PreviewPanel or HtmlRenderer via constructor injection.

## Acceptance Criteria

1. `$...$` inline math expressions render with proper Unicode symbols instead of raw LaTeX
2. `$$...$$` block math expressions render as centered, styled blocks with rendered symbols
3. Common LaTeX commands (Greek letters, fractions, superscripts, subscripts) are handled
4. Unknown LaTeX commands fall back to displaying the raw source in a styled div
5. Math rendering does not break existing Markdown rendering
6. Performance: math rendering adds < 1ms per expression

## Testing Requirements

- Add test: `\alpha + \beta = \gamma` produces correct Unicode output
- Add test: `\frac{1}{2}` produces an HTML fraction
- Add test: `x^2 + y_1` produces superscript and subscript
- Add test: Unknown command `\unknowncmd` falls back to raw display
- Add test: Empty math expression produces empty output
- Existing rendering tests pass unchanged
