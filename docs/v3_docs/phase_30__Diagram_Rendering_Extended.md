# Phase 30 -- Extended Diagram Rendering (PlantUML, Graphviz, Flowchart, Mindmap, ECharts)

## Objective

Extend MarkAmp's diagram rendering capabilities beyond the existing Mermaid support (implemented in MermaidRenderer/MermaidBlockRenderer) to handle five additional diagram languages: PlantUML, Graphviz/DOT, Flowchart.js, Mindmap, and ECharts. SiYuan supports all of these as fenced code blocks with their respective language identifiers (e.g., ` ```plantuml `, ` ```graphviz `, ` ```flowchart `, ` ```mindmap `, ` ```echarts `). Each diagram type takes a textual description as input and produces SVG (or PNG) output for display in the preview panel.

The architecture follows a registry pattern: a `DiagramRendererRegistry` holds registered `IDiagramRenderer` implementations, one per diagram language. When the HtmlRenderer encounters a fenced code block with a recognized diagram language, it dispatches to the registry, which selects the appropriate renderer and invokes it. This design mirrors the existing MermaidBlockRenderer integration but generalizes it to support any number of diagram types. Each renderer produces a `DiagramResult` containing the SVG output, optional PNG bytes, dimensions, and any diagnostic messages (parse errors, warnings).

PlantUML and Graphviz are rendered via external CLI tools (`plantuml.jar` via Java and `dot` respectively), making them available on any system where these tools are installed. The CLI-based renderers run the external process, capture stdout, and cache the resulting SVG keyed by the source hash. Mindmap rendering is implemented natively in C++ using a simple tree layout algorithm (indented text to tree structure to SVG). Flowchart.js and ECharts require JavaScript execution environments and are implemented as optional features that delegate to an embedded browser or external rendering service. All renderers implement validation methods that check syntax before rendering, enabling real-time error feedback in the editor.

## Prerequisites

- No strict phase dependencies. Builds on the existing MermaidRenderer/MermaidBlockRenderer pattern in MarkAmp.

## SiYuan Source Reference

- SiYuan protyle supports code blocks with language identifiers: `plantuml`, `graphviz`, `flowchart`, `mindmap`, `echarts`, plus existing `mermaid`
- `app/src/protyle/render/chartRender.ts` -- ECharts rendering via the echarts library
- `app/src/protyle/render/graphvizRender.ts` -- Graphviz rendering via viz.js (WASM port of Graphviz)
- `app/src/protyle/render/plantumlRender.ts` -- PlantUML rendering via PlantUML server API
- `app/src/protyle/render/mindmapRender.ts` -- Mindmap rendering
- `app/src/protyle/render/flowchartRender.ts` -- Flowchart.js rendering

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|------|-----------|---------|
| `src/core/IDiagramRenderer.h` | `markamp::core` | Generic diagram renderer interface |
| `src/core/DiagramRendererRegistry.h` | `markamp::core` | Registry for diagram type dispatching |
| `src/core/DiagramRendererRegistry.cpp` | `markamp::core` | Registry implementation |
| `src/core/PlantUMLRenderer.h` | `markamp::core` | PlantUML renderer (CLI/server) |
| `src/core/PlantUMLRenderer.cpp` | `markamp::core` | PlantUML implementation |
| `src/core/GraphvizRenderer.h` | `markamp::core` | Graphviz/DOT renderer (dot CLI) |
| `src/core/GraphvizRenderer.cpp` | `markamp::core` | Graphviz implementation |
| `src/core/MindmapRenderer.h` | `markamp::core` | Native C++ mindmap renderer |
| `src/core/MindmapRenderer.cpp` | `markamp::core` | Mindmap implementation |
| `src/core/FlowchartRenderer.h` | `markamp::core` | Flowchart.js renderer (stub/external) |
| `src/core/FlowchartRenderer.cpp` | `markamp::core` | Flowchart implementation |
| `src/core/EChartsRenderer.h` | `markamp::core` | ECharts renderer (stub/external) |
| `src/core/EChartsRenderer.cpp` | `markamp::core` | ECharts implementation |
| `tests/unit/test_diagram_renderers.cpp` | (test) | Catch2 test suite for diagram renderers |

### Existing Files to Modify

| File | Change |
|------|--------|
| `src/core/Events.h` | Add diagram rendering events |
| `src/core/PluginContext.h` | Add `DiagramRendererRegistry*` pointer |
| `src/rendering/HtmlRenderer.h` | Integrate DiagramRendererRegistry for code block dispatch |
| `src/rendering/HtmlRenderer.cpp` | Check registry before rendering code blocks |
| `src/CMakeLists.txt` | Add new source files |
| `tests/CMakeLists.txt` | Add test_diagram_renderers target |
| `resources/config_defaults.json` | Add diagram config defaults |

## Data Structures to Implement

```cpp
// ============================================================================
// File: src/core/IDiagramRenderer.h
// ============================================================================
#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Severity for diagram diagnostic messages.
enum class DiagramDiagnosticSeverity : uint8_t
{
    Error,
    Warning,
    Info
};

/// A diagnostic message from diagram validation or rendering.
struct DiagramDiagnostic
{
    int line{0};
    int column{0};
    std::string message;
    DiagramDiagnosticSeverity severity{DiagramDiagnosticSeverity::Error};
};

/// Result of rendering a diagram.
struct DiagramResult
{
    std::string svg_content;             // Rendered SVG (primary output)
    std::vector<uint8_t> png_bytes;      // Optional PNG output
    int width{0};                        // SVG width in pixels
    int height{0};                       // SVG height in pixels
    std::vector<DiagramDiagnostic> diagnostics; // Errors/warnings from rendering

    [[nodiscard]] auto has_errors() const -> bool
    {
        for (const auto& d : diagnostics)
        {
            if (d.severity == DiagramDiagnosticSeverity::Error)
            {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] auto is_valid() const -> bool
    {
        return !svg_content.empty() && !has_errors();
    }
};

/// Abstract interface for diagram renderers.
/// Each concrete implementation handles one diagram language.
class IDiagramRenderer
{
public:
    virtual ~IDiagramRenderer() = default;

    /// Render diagram source to SVG (and optionally PNG).
    [[nodiscard]] virtual auto render(std::string_view source)
        -> std::expected<DiagramResult, std::string> = 0;

    /// Check if this renderer supports the given diagram type.
    [[nodiscard]] virtual auto supports(std::string_view type) const -> bool = 0;

    /// Validate diagram source without rendering. Returns diagnostics.
    [[nodiscard]] virtual auto validate(std::string_view source)
        -> std::vector<DiagramDiagnostic> = 0;

    /// Whether this renderer is available (external tools installed, etc.).
    [[nodiscard]] virtual auto is_available() const -> bool = 0;

    /// Human-readable name for this renderer.
    [[nodiscard]] virtual auto name() const -> std::string_view = 0;

    /// The diagram type this renderer handles (e.g., "plantuml", "graphviz").
    [[nodiscard]] virtual auto diagram_type() const -> std::string_view = 0;
};

} // namespace markamp::core
```

```cpp
// ============================================================================
// File: src/core/DiagramRendererRegistry.h
// ============================================================================
#pragma once

#include "IDiagramRenderer.h"

#include <expected>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Registry that maps diagram type identifiers to renderer implementations.
/// Used by HtmlRenderer to dispatch code block rendering.
class DiagramRendererRegistry
{
public:
    DiagramRendererRegistry() = default;

    /// Register a renderer for a diagram type. Overwrites any existing
    /// registration for that type.
    void register_renderer(std::string_view type,
                           std::shared_ptr<IDiagramRenderer> renderer);

    /// Get the renderer for a diagram type.
    [[nodiscard]] auto get_renderer(std::string_view type) const
        -> std::shared_ptr<IDiagramRenderer>;

    /// Check if a renderer is registered and available for a diagram type.
    [[nodiscard]] auto has_renderer(std::string_view type) const -> bool;

    /// Render a diagram using the appropriate registered renderer.
    [[nodiscard]] auto render(std::string_view source, std::string_view type)
        -> std::expected<DiagramResult, std::string>;

    /// Validate diagram source using the appropriate renderer.
    [[nodiscard]] auto validate(std::string_view source, std::string_view type)
        -> std::vector<DiagramDiagnostic>;

    /// Get all registered diagram type names.
    [[nodiscard]] auto registered_types() const -> std::vector<std::string>;

    /// Get all available (installed) diagram type names.
    [[nodiscard]] auto available_types() const -> std::vector<std::string>;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<IDiagramRenderer>> renderers_;
};

} // namespace markamp::core
```

```cpp
// ============================================================================
// File: src/core/PlantUMLRenderer.h
// ============================================================================
#pragma once

#include "IDiagramRenderer.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// PlantUML renderer that uses either the plantuml.jar CLI or a PlantUML
/// server HTTP API. Falls back to server if Java is not available.
class PlantUMLRenderer : public IDiagramRenderer
{
public:
    PlantUMLRenderer();

    [[nodiscard]] auto render(std::string_view source)
        -> std::expected<DiagramResult, std::string> override;

    [[nodiscard]] auto supports(std::string_view type) const -> bool override;

    [[nodiscard]] auto validate(std::string_view source)
        -> std::vector<DiagramDiagnostic> override;

    [[nodiscard]] auto is_available() const -> bool override;

    [[nodiscard]] auto name() const -> std::string_view override
    {
        return "PlantUML";
    }

    [[nodiscard]] auto diagram_type() const -> std::string_view override
    {
        return "plantuml";
    }

    /// Set the path to plantuml.jar.
    void set_jar_path(const std::filesystem::path& path);

    /// Set the PlantUML server URL (e.g., "http://www.plantuml.com/plantuml").
    void set_server_url(const std::string& url);

    /// Clear the render cache.
    void clear_cache();

private:
    [[nodiscard]] auto render_via_jar(std::string_view source)
        -> std::expected<DiagramResult, std::string>;

    [[nodiscard]] auto render_via_server(std::string_view source)
        -> std::expected<DiagramResult, std::string>;

    [[nodiscard]] static auto detect_java() -> bool;
    [[nodiscard]] auto detect_jar() const -> bool;
    [[nodiscard]] auto cache_key(std::string_view source) const -> size_t;

    std::filesystem::path jar_path_;
    std::string server_url_;
    bool java_available_{false};
    bool jar_available_{false};

    std::unordered_map<size_t, DiagramResult> cache_;
    static constexpr size_t kMaxCacheEntries = 50;
};

} // namespace markamp::core
```

```cpp
// ============================================================================
// File: src/core/GraphvizRenderer.h
// ============================================================================
#pragma once

#include "IDiagramRenderer.h"

#include <string>
#include <string_view>
#include <unordered_map>

namespace markamp::core
{

/// Graphviz renderer that uses the `dot` CLI tool to render DOT language
/// graphs to SVG. Supports digraph, graph, and subgraph.
class GraphvizRenderer : public IDiagramRenderer
{
public:
    GraphvizRenderer();

    [[nodiscard]] auto render(std::string_view source)
        -> std::expected<DiagramResult, std::string> override;

    [[nodiscard]] auto supports(std::string_view type) const -> bool override;

    [[nodiscard]] auto validate(std::string_view source)
        -> std::vector<DiagramDiagnostic> override;

    [[nodiscard]] auto is_available() const -> bool override;

    [[nodiscard]] auto name() const -> std::string_view override
    {
        return "Graphviz";
    }

    [[nodiscard]] auto diagram_type() const -> std::string_view override
    {
        return "graphviz";
    }

    /// Set the path to the dot executable.
    void set_dot_path(const std::string& path);

    /// Set the layout engine (dot, neato, fdp, sfdp, circo, twopi).
    void set_layout_engine(const std::string& engine);

    /// Clear the render cache.
    void clear_cache();

private:
    [[nodiscard]] auto render_via_dot(std::string_view source)
        -> std::expected<DiagramResult, std::string>;

    [[nodiscard]] static auto detect_dot() -> bool;
    [[nodiscard]] auto cache_key(std::string_view source) const -> size_t;

    std::string dot_path_{"dot"};
    std::string layout_engine_{"dot"};
    bool dot_available_{false};

    std::unordered_map<size_t, DiagramResult> cache_;
    static constexpr size_t kMaxCacheEntries = 50;
};

} // namespace markamp::core
```

```cpp
// ============================================================================
// File: src/core/MindmapRenderer.h
// ============================================================================
#pragma once

#include "IDiagramRenderer.h"

#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// A node in the mindmap tree.
struct MindmapNode
{
    std::string text;
    int level{0}; // Indentation level (0 = root)
    std::vector<MindmapNode> children;

    // Layout computed positions
    double x{0.0};
    double y{0.0};
    double width{0.0};
    double height{0.0};

    [[nodiscard]] auto is_leaf() const -> bool
    {
        return children.empty();
    }

    [[nodiscard]] auto total_descendants() const -> int
    {
        int count = 0;
        for (const auto& child : children)
        {
            count += 1 + child.total_descendants();
        }
        return count;
    }
};

/// Native C++ mindmap renderer. Parses indented text into a tree structure
/// and lays out as SVG using a simple tree layout algorithm.
/// No external dependencies required.
class MindmapRenderer : public IDiagramRenderer
{
public:
    MindmapRenderer() = default;

    [[nodiscard]] auto render(std::string_view source)
        -> std::expected<DiagramResult, std::string> override;

    [[nodiscard]] auto supports(std::string_view type) const -> bool override;

    [[nodiscard]] auto validate(std::string_view source)
        -> std::vector<DiagramDiagnostic> override;

    [[nodiscard]] auto is_available() const -> bool override
    {
        return true; // Always available (native implementation)
    }

    [[nodiscard]] auto name() const -> std::string_view override
    {
        return "Mindmap";
    }

    [[nodiscard]] auto diagram_type() const -> std::string_view override
    {
        return "mindmap";
    }

private:
    /// Parse indented text into a tree of MindmapNodes.
    [[nodiscard]] auto parse_tree(std::string_view source) const
        -> std::expected<MindmapNode, std::string>;

    /// Compute tree layout positions (Reingold-Tilford-like).
    void layout_tree(MindmapNode& root, double x, double y,
                     double h_spacing, double v_spacing) const;

    /// Render the tree to SVG.
    [[nodiscard]] auto render_svg(const MindmapNode& root) const -> std::string;

    /// Render a single node as SVG (rounded rect + text).
    [[nodiscard]] auto render_node_svg(const MindmapNode& node) const -> std::string;

    /// Render a connection line between parent and child.
    [[nodiscard]] auto render_link_svg(const MindmapNode& parent,
                                       const MindmapNode& child) const -> std::string;

    static constexpr double kNodePadding = 10.0;
    static constexpr double kHorizontalSpacing = 40.0;
    static constexpr double kVerticalSpacing = 20.0;
    static constexpr double kFontSize = 14.0;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`DiagramRendererRegistry::render(source, type)`** -- Look up the renderer for the given type. If not found, return error. If found but not available, return error with a message indicating the required tool is not installed. Otherwise, call the renderer's `render()` method and return the result.

2. **`PlantUMLRenderer::render_via_jar(source)`** -- Write the source to a temp file. Execute `java -jar plantuml.jar -tsvg -pipe < input` and capture stdout. Parse the SVG output. If the process exits with an error, parse stderr for diagnostic messages. Cache the result keyed by source hash.

3. **`PlantUMLRenderer::render_via_server(source)`** -- Encode the source using PlantUML's text encoding (deflate + base64). Send an HTTP GET to `{server_url}/svg/{encoded}`. Parse the SVG response. Return an error if the server is unreachable.

4. **`GraphvizRenderer::render_via_dot(source)`** -- Write the source to a temp file. Execute `dot -Tsvg input.dot` and capture stdout. Parse the SVG output, extract width/height from the SVG root element. If the process exits with an error, parse stderr for line-specific error messages and produce DiagramDiagnostics.

5. **`MindmapRenderer::parse_tree(source)`** -- Split input into lines. Determine indentation level for each line (spaces or tabs). Build a tree: root is the first line, subsequent lines become children at their relative indentation level. Return error if indentation is inconsistent or if there is no root line.

6. **`MindmapRenderer::layout_tree(root, x, y, h_spacing, v_spacing)`** -- Recursive tree layout: place root at (x, y). Distribute children vertically below/beside root with `v_spacing` between them. Each child's x position is `root.x + h_spacing`. Compute subtree height to center the parent vertically among its children.

7. **`MindmapRenderer::render_svg(root)`** -- Compute the total bounding box. Generate SVG header with viewBox. Recursively render all nodes as rounded rectangles with text labels. Render connection lines as bezier curves between parent right-edge and child left-edge.

8. **`GraphvizRenderer::validate(source)`** -- Execute `dot -Tsvg` with the source and check for errors without actually needing the output. Parse any error messages from stderr into DiagramDiagnostic entries with line numbers.

9. **`HtmlRenderer code block integration`** -- In the existing code block rendering path, before falling through to syntax-highlighted code display, check `diagram_registry_->has_renderer(language)`. If a renderer exists and is available, call `diagram_registry_->render(source, language)`. If the result is valid, emit the SVG content. If it has errors, show the errors above the source code.

10. **`DiagramRendererRegistry::available_types()`** -- Iterate all registered renderers. Return the type names of those where `is_available()` returns true. Used by the UI to show which diagram languages are supported.

11. **`PlantUMLRenderer::detect_java()`** -- Execute `java -version` and check the exit code. Cache the result. Return true if Java is available on the system PATH.

12. **`GraphvizRenderer::detect_dot()`** -- Execute `dot -V` and check the exit code. Cache the result. Return true if Graphviz is installed.

## Events to Add

Add the following to `src/core/Events.h`:

```cpp
// ============================================================================
// Diagram rendering events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagramRenderedEvent)
std::string block_id;
std::string diagram_type; // "plantuml", "graphviz", "mindmap", etc.
int elapsed_ms{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagramRenderFailedEvent)
std::string block_id;
std::string diagram_type;
std::string error_message;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagramToolAvailabilityChangedEvent)
std::string diagram_type;
bool available{false};
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `knowledgebase.diagrams.plantuml_jar_path` | string | "" | Path to plantuml.jar (auto-detect if empty) |
| `knowledgebase.diagrams.plantuml_server_url` | string | "" | PlantUML server URL (fallback if jar unavailable) |
| `knowledgebase.diagrams.graphviz_dot_path` | string | "dot" | Path to the dot executable |
| `knowledgebase.diagrams.graphviz_engine` | string | "dot" | Graphviz layout engine |
| `knowledgebase.diagrams.cache_enabled` | bool | true | Enable render result caching |
| `knowledgebase.diagrams.cache_max_entries` | int | 100 | Maximum cached diagram renders |
| `knowledgebase.diagrams.render_timeout_ms` | int | 10000 | Timeout for external renderer processes |
| `knowledgebase.diagrams.mindmap_h_spacing` | double | 40.0 | Horizontal spacing for mindmap nodes |
| `knowledgebase.diagrams.mindmap_v_spacing` | double | 20.0 | Vertical spacing for mindmap nodes |

## Test Cases

All tests in `tests/unit/test_diagram_renderers.cpp` using Catch2.

1. **"DiagramRendererRegistry dispatches to correct renderer"** -- Register a mock renderer for type "test-diagram". Call `render(source, "test-diagram")`. Verify the mock's `render()` was called with the source.

2. **"DiagramRendererRegistry returns error for unregistered type"** -- Call `render(source, "unknown-type")`. Verify it returns an error with a descriptive message.

3. **"MindmapRenderer parses simple indented tree"** -- Input: `"Root\n  Child1\n  Child2\n    Grandchild"`. Verify parse produces a tree with Root having 2 children, and Child2 having 1 grandchild.

4. **"MindmapRenderer renders valid SVG output"** -- Render a simple 3-node tree. Verify the result contains valid SVG with `<svg`, `<rect`, and `<text` elements. Verify `is_valid()` returns true.

5. **"MindmapRenderer reports error for empty input"** -- Call `render("")`. Verify the result is an error (no root node).

6. **"GraphvizRenderer supports type 'graphviz' and 'dot'"** -- Verify `supports("graphviz")` and `supports("dot")` both return true. Verify `supports("mermaid")` returns false.

7. **"PlantUMLRenderer supports type 'plantuml'"** -- Verify `supports("plantuml")` returns true. Verify `supports("graphviz")` returns false.

8. **"DiagramResult reports errors correctly"** -- Create a DiagramResult with one Error diagnostic. Verify `has_errors()` returns true. Create one with only Info diagnostics. Verify `has_errors()` returns false.

9. **"MindmapRenderer layout produces non-overlapping positions"** -- Render a tree with 5 children. Verify no two nodes have identical (x, y) positions. Verify all children are to the right of the root (x > root.x).

10. **"DiagramRendererRegistry lists available and registered types"** -- Register 3 renderers: one available, one not available, one available. Verify `registered_types()` returns 3 entries. Verify `available_types()` returns 2 entries.

## Acceptance Criteria

- [ ] IDiagramRenderer interface provides render(), validate(), supports(), is_available()
- [ ] DiagramRendererRegistry correctly dispatches by diagram type
- [ ] MindmapRenderer parses indented text and produces valid SVG natively (no external deps)
- [ ] PlantUMLRenderer renders via jar or server with fallback behavior
- [ ] GraphvizRenderer renders via dot CLI with error parsing
- [ ] All CLI-based renderers cache results keyed by source hash
- [ ] HtmlRenderer integrates with DiagramRendererRegistry for code block dispatch
- [ ] Unavailable renderers return descriptive error messages (e.g., "Graphviz not installed")
- [ ] Render results include diagnostics for syntax errors
- [ ] External process execution respects configurable timeout

## Files to Create/Modify

```
CREATE  src/core/IDiagramRenderer.h
CREATE  src/core/DiagramRendererRegistry.h
CREATE  src/core/DiagramRendererRegistry.cpp
CREATE  src/core/PlantUMLRenderer.h
CREATE  src/core/PlantUMLRenderer.cpp
CREATE  src/core/GraphvizRenderer.h
CREATE  src/core/GraphvizRenderer.cpp
CREATE  src/core/MindmapRenderer.h
CREATE  src/core/MindmapRenderer.cpp
CREATE  src/core/FlowchartRenderer.h
CREATE  src/core/FlowchartRenderer.cpp
CREATE  src/core/EChartsRenderer.h
CREATE  src/core/EChartsRenderer.cpp
CREATE  tests/unit/test_diagram_renderers.cpp
MODIFY  src/core/Events.h              -- add diagram rendering events
MODIFY  src/core/PluginContext.h        -- add DiagramRendererRegistry* pointer
MODIFY  src/rendering/HtmlRenderer.h   -- add DiagramRendererRegistry* member
MODIFY  src/rendering/HtmlRenderer.cpp -- integrate registry in code block rendering
MODIFY  src/CMakeLists.txt              -- add new source files
MODIFY  tests/CMakeLists.txt            -- add test_diagram_renderers target
MODIFY  resources/config_defaults.json  -- add knowledgebase.diagrams.* defaults
```
