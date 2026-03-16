/// @file VsCodeThemeAdapter.h
/// @brief V8 Phase 12 (Phase 40) — VSCode Theme Ingestion Engine.
/// Parses VSCode-compatible theme JSON and converts to internal Theme model.

#pragma once

#include "core/Theme.h"

#include <cstdint>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

/// Compatibility level of an imported VSCode theme.
enum class ThemeCompatibility : std::uint8_t
{
    kFull,   ///< All tokens mapped directly
    kMapped, ///< Mapped with fallbacks for some tokens
    kPartial ///< Significant unmapped tokens
};

/// A single token coloring rule from a VSCode theme.
struct VsCodeTokenRule
{
    std::string scope;      ///< TextMate scope selector (e.g. "keyword.control")
    std::string foreground; ///< Hex color (e.g. "#C678DD")
    std::string font_style; ///< "italic", "bold", "underline", or combinations
};

/// A single color entry from a VSCode theme's "colors" section.
struct VsCodeColorEntry
{
    std::string key;   ///< e.g. "editor.background"
    std::string value; ///< e.g. "#282C34"
};

/// Compatibility report for an imported VSCode theme.
struct VsCodeCompatibilityReport
{
    ThemeCompatibility level{ThemeCompatibility::kPartial};
    int supported_tokens{0};
    int total_tokens{0};
    std::vector<std::string> unmapped_scopes; ///< Scopes that couldn't be mapped
    std::vector<std::string> unmapped_colors; ///< Color keys that couldn't be mapped
    std::vector<std::string> warnings;        ///< Non-fatal issues
};

/// Adapter for ingesting VSCode themes without manual conversion.
class VsCodeThemeAdapter
{
public:
    /// Parse a VSCode theme JSON file.
    [[nodiscard]] static auto parse_json(const std::filesystem::path& path)
        -> std::expected<std::pair<std::vector<VsCodeColorEntry>, std::vector<VsCodeTokenRule>>,
                         std::string>;

    /// Parse VSCode theme JSON from a string.
    [[nodiscard]] static auto parse_json_string(const std::string& json_content)
        -> std::expected<std::pair<std::vector<VsCodeColorEntry>, std::vector<VsCodeTokenRule>>,
                         std::string>;

    /// Convert parsed VSCode theme data to internal Theme model.
    [[nodiscard]] static auto convert_to_theme(const std::string& theme_name,
                                               const std::vector<VsCodeColorEntry>& colors,
                                               const std::vector<VsCodeTokenRule>& token_rules)
        -> Theme;

    /// Generate a compatibility report for the conversion.
    [[nodiscard]] static auto compatibility_report(const std::vector<VsCodeColorEntry>& colors,
                                                   const std::vector<VsCodeTokenRule>& token_rules)
        -> VsCodeCompatibilityReport;

    /// Map a VSCode color key to internal ThemeColorToken.
    /// Returns empty string if no mapping exists.
    [[nodiscard]] static auto map_vscode_color(const std::string& vscode_key) -> std::string;

    /// V16 Phase 17: Map a TextMate scope to internal syntax token key.
    /// Returns empty string if no mapping exists.
    [[nodiscard]] static auto map_vscode_scope(const std::string& scope) -> std::string;

    /// (#149) Return the number of mapped VSCode color keys.
    [[nodiscard]] static auto mapped_color_count() -> std::size_t;

    /// (#150) Return the number of mapped VSCode scope keys.
    [[nodiscard]] static auto mapped_scope_count() -> std::size_t;
};

} // namespace markamp::core
