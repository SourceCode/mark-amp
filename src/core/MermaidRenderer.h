#pragma once

#include "IMermaidRenderer.h"
#include "Theme.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// MermaidRenderer converts Mermaid diagram source to SVG using the mmdc CLI tool.
/// Implements IMermaidRenderer. Uses temp files for I/O and caches PATH availability.

/// Severity level for Mermaid diagram diagnostics.
enum class DiagnosticSeverity
{
    Error,
    Warning,
    Info
};

/// Diagnostic information for a Mermaid diagram validation error.
struct DiagnosticInfo
{
    int line{0};
    std::string message;
    DiagnosticSeverity severity{DiagnosticSeverity::Error};
};

class MermaidRenderer : public IMermaidRenderer
{
public:
    MermaidRenderer();
    ~MermaidRenderer() override = default;

    // IMermaidRenderer interface
    [[nodiscard]] auto render(std::string_view mermaid_source)
        -> std::expected<std::string, std::string> override;
    [[nodiscard]] auto is_available() const -> bool override;

    // Configuration
    void set_theme(const Theme& theme);
    void set_font_family(const std::string& font);

    /// Generate Mermaid JSON config from current settings.
    [[nodiscard]] auto get_mermaid_config() const -> std::string;

    /// Clear the SVG render cache (e.g. on theme change).
    void clear_cache();

    /// Maximum number of cached SVG renders.
    static constexpr size_t kMaxCacheEntries = 100;

    /// Sanitize SVG output: strip <script>, <foreignObject>, on* attributes.
    [[nodiscard]] static auto sanitize_svg(const std::string& svg) -> std::string;

    // --- Phase 3: Mermaid First-Class Experience ---

    /// Validate Mermaid source and return diagnostics.
    [[nodiscard]] auto validate(std::string_view mermaid_source) -> std::vector<DiagnosticInfo>;

    /// Export rendered SVG as raw string.
    [[nodiscard]] auto export_svg(std::string_view mermaid_source)
        -> std::expected<std::string, std::string>;

    /// Export rendered diagram as PNG bytes.
    [[nodiscard]] auto export_png(std::string_view mermaid_source, int width = 800)
        -> std::expected<std::vector<uint8_t>, std::string>;

    /// Set diagram theme independently of editor theme.
    void set_diagram_theme(const std::string& theme_name);

    /// Get the current diagram theme name.
    [[nodiscard]] auto diagram_theme() const -> const std::string&;

private:
    /// Execute mmdc CLI to render Mermaid source to SVG.
    [[nodiscard]] auto render_via_mmdc(std::string_view source)
        -> std::expected<std::string, std::string>;

    /// Detect whether mmdc is available on PATH.
    [[nodiscard]] static auto detect_mmdc() -> bool;

    /// Compute cache key from source + theme config.
    [[nodiscard]] auto cache_key(std::string_view source) const -> size_t;

    bool mmdc_available_{false};
    std::string mermaid_theme_{"dark"};
    std::string font_family_{"JetBrains Mono"};
    std::string primary_color_{"#6C63FF"};
    std::string primary_text_color_{"#E0E0E0"};
    std::string primary_border_color_{"#333333"};
    std::string line_color_{"#999999"};
    std::string secondary_color_{"#FF6B9D"};
    std::string tertiary_color_{"#1A1A2E"};

    /// Diagram theme override (independent of editor theme)
    std::string diagram_theme_override_;

    /// SVG cache: hash(source + theme) -> rendered SVG
    std::unordered_map<size_t, std::string> svg_cache_;

    /// Insertion order for LRU eviction
    std::vector<size_t> cache_order_;
};

} // namespace markamp::core
