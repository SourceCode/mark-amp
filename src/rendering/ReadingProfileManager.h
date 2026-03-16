// ============================================================================
// File: src/rendering/ReadingProfileManager.h
// Phase 8: Markdown Rendering Completeness — Reading Profile → CSS mapping
// ============================================================================
#pragma once

#include "MarkdownRenderingFeatures.h"

#include <cstdint>
#include <string>

namespace markamp::rendering
{

/// Reading profile presets (aligned with EditorPanel::ReadingProfile enum).
enum class ReadingProfilePreset : uint8_t
{
    kDefault = 0,
    kFocused = 1,
    kComfortable = 2,
    kCompact = 3,
    kAccessible = 4
};

/// Maps ReadingProfile presets to CSS styling parameters.
/// Header-only for testability — no wxWidgets dependency.
class ReadingProfileManager
{
public:
    ReadingProfileManager() = default;

    /// Get the CSS parameters for a given profile preset.
    [[nodiscard]] static auto profile_css(ReadingProfilePreset preset) -> ReadingProfileCss
    {
        ReadingProfileCss css;
        switch (preset)
        {
            case ReadingProfilePreset::kDefault:
                css.font_size_px = 16.0;
                css.line_height = 1.6;
                css.max_width_px = 720.0;
                css.use_serif = false;
                css.font_family = "Inter, system-ui, sans-serif";
                css.letter_spacing_px = 0.0;
                css.paragraph_spacing_px = 16.0;
                break;

            case ReadingProfilePreset::kFocused:
                css.font_size_px = 18.0;
                css.line_height = 1.8;
                css.max_width_px = 600.0;
                css.use_serif = true;
                css.font_family = "Georgia, serif";
                css.letter_spacing_px = 0.2;
                css.paragraph_spacing_px = 24.0;
                break;

            case ReadingProfilePreset::kComfortable:
                css.font_size_px = 17.0;
                css.line_height = 1.7;
                css.max_width_px = 680.0;
                css.use_serif = false;
                css.font_family = "Inter, system-ui, sans-serif";
                css.letter_spacing_px = 0.1;
                css.paragraph_spacing_px = 20.0;
                break;

            case ReadingProfilePreset::kCompact:
                css.font_size_px = 14.0;
                css.line_height = 1.4;
                css.max_width_px = 900.0;
                css.use_serif = false;
                css.font_family = "Inter, system-ui, sans-serif";
                css.letter_spacing_px = 0.0;
                css.paragraph_spacing_px = 10.0;
                break;

            case ReadingProfilePreset::kAccessible:
                css.font_size_px = 20.0;
                css.line_height = 2.0;
                css.max_width_px = 640.0;
                css.use_serif = false;
                css.font_family = "Atkinson Hyperlegible, Inter, system-ui, sans-serif";
                css.letter_spacing_px = 0.5;
                css.paragraph_spacing_px = 28.0;
                break;
        }
        return css;
    }

    /// Apply a profile and return its CSS as a string.
    [[nodiscard]] static auto apply_profile(ReadingProfilePreset preset) -> std::string
    {
        return profile_css(preset).to_css();
    }

    /// Get the display name for a profile preset.
    [[nodiscard]] static auto profile_name(ReadingProfilePreset preset) -> std::string_view
    {
        switch (preset)
        {
            case ReadingProfilePreset::kDefault:
                return "Default";
            case ReadingProfilePreset::kFocused:
                return "Focused";
            case ReadingProfilePreset::kComfortable:
                return "Comfortable";
            case ReadingProfilePreset::kCompact:
                return "Compact";
            case ReadingProfilePreset::kAccessible:
                return "Accessible";
        }
        return "Default";
    }

    /// Generate print CSS with the given configuration.
    [[nodiscard]] static auto generate_print_css(const PrintCssConfig& config) -> std::string
    {
        std::string css;
        css += "@media print {\n";
        css += "  @page {\n";
        css += "    margin-top: " + std::to_string(config.margin_top_cm) + "cm;\n";
        css += "    margin-bottom: " + std::to_string(config.margin_bottom_cm) + "cm;\n";
        css += "    margin-left: " + std::to_string(config.margin_left_cm) + "cm;\n";
        css += "    margin-right: " + std::to_string(config.margin_right_cm) + "cm;\n";
        css += "  }\n";

        if (config.hide_nav_elements)
        {
            css += "  .heading-nav-overlay,\n";
            css += "  .scroll-to-top-btn,\n";
            css += "  .copy-btn,\n";
            css += "  .code-collapse-toggle { display: none !important; }\n";
        }

        if (config.page_break_before_h1)
        {
            css += "  h1 { page-break-before: always; }\n";
            css += "  h1:first-child { page-break-before: auto; }\n";
        }
        if (config.page_break_before_h2)
        {
            css += "  h2 { page-break-before: always; }\n";
        }

        // Avoid orphans/widows
        css += "  p, li, blockquote { orphans: 3; widows: 3; }\n";
        css += "  pre { page-break-inside: avoid; }\n";
        css += "  table { page-break-inside: avoid; }\n";
        css += "  img { page-break-inside: avoid; max-width: 100%; }\n";

        if (config.monochrome_code)
        {
            css += "  pre, code { background: #f5f5f5 !important; ";
            css += "color: #333 !important; }\n";
        }

        css += "  body { font-size: 12pt; line-height: 1.5; }\n";
        css += "  a { text-decoration: underline; color: inherit; }\n";
        css += "  a[href]:after { content: \" (\" attr(href) \")\"; font-size: 0.8em; }\n";
        css += "}\n";
        return css;
    }

    /// Total number of reading profile presets.
    [[nodiscard]] static constexpr auto total_presets() noexcept -> int
    {
        return 5;
    }
};

} // namespace markamp::rendering
