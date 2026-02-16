/// @file VsCodeThemeAdapter.cpp
/// @brief V8 Phase 12 (Phase 40) — VSCode Theme Ingestion Engine implementation.

#include "core/VsCodeThemeAdapter.h"

#include "core/Logger.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

namespace markamp::core
{

// ============================================================================
// VSCode color key -> internal token mapping
// ============================================================================

static const std::unordered_map<std::string, std::string>& vscode_color_map()
{
    static const std::unordered_map<std::string, std::string> mapping = {
        // Editor colors
        {"editor.background", "editor_background"},
        {"editor.foreground", "editor_foreground"},
        {"editor.lineHighlightBackground", "editor_line_highlight"},
        {"editor.selectionBackground", "editor_selection"},
        {"editorCursor.foreground", "editor_cursor"},
        {"editorLineNumber.foreground", "line_number_fg"},
        {"editorLineNumber.activeForeground", "line_number_active_fg"},

        // Sidebar
        {"sideBar.background", "sidebar_bg"},
        {"sideBar.foreground", "sidebar_fg"},
        {"sideBarTitle.foreground", "sidebar_title_fg"},

        // Activity bar
        {"activityBar.background", "activity_bar_bg"},
        {"activityBar.foreground", "activity_bar_fg"},

        // Status bar
        {"statusBar.background", "status_bar_bg"},
        {"statusBar.foreground", "status_bar_fg"},

        // Tab bar
        {"tab.activeBackground", "tab_active_bg"},
        {"tab.activeForeground", "tab_active_fg"},
        {"tab.inactiveBackground", "tab_inactive_bg"},
        {"tab.inactiveForeground", "tab_inactive_fg"},

        // Title bar
        {"titleBar.activeBackground", "titlebar_bg"},
        {"titleBar.activeForeground", "titlebar_fg"},

        // Panel
        {"panel.background", "panel_bg"},
        {"panel.border", "panel_border"},

        // Input
        {"input.background", "input_bg"},
        {"input.foreground", "input_fg"},
        {"input.border", "input_border"},

        // Button
        {"button.background", "button_bg"},
        {"button.foreground", "button_fg"},

        // List
        {"list.activeSelectionBackground", "list_active_bg"},
        {"list.activeSelectionForeground", "list_active_fg"},
        {"list.hoverBackground", "list_hover_bg"},
    };
    return mapping;
}

// ============================================================================
// JSON parsing (minimal — no external JSON dependency for stub)
// ============================================================================

auto VsCodeThemeAdapter::parse_json(const std::filesystem::path& path)
    -> std::expected<std::pair<std::vector<VsCodeColorEntry>, std::vector<VsCodeTokenRule>>,
                     std::string>
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return std::unexpected("Failed to open file: " + path.string());
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return parse_json_string(buffer.str());
}

auto VsCodeThemeAdapter::parse_json_string(const std::string& json_content)
    -> std::expected<std::pair<std::vector<VsCodeColorEntry>, std::vector<VsCodeTokenRule>>,
                     std::string>
{
    // Stub: In production this would use nlohmann::json or similar.
    // For now, return empty results to enable compilation and testing.
    if (json_content.empty())
    {
        return std::unexpected("Empty JSON content");
    }

    std::vector<VsCodeColorEntry> colors;
    std::vector<VsCodeTokenRule> rules;

    // Simple detection: if it contains "colors" or "tokenColors" keys,
    // treat as valid (real parser would extract values)
    if (json_content.find("colors") == std::string::npos &&
        json_content.find("tokenColors") == std::string::npos)
    {
        return std::unexpected("JSON does not appear to be a VSCode theme "
                               "(missing 'colors' or 'tokenColors')");
    }

    MARKAMP_LOG_INFO("VSCode theme JSON parsed ({} bytes)", json_content.size());
    return std::make_pair(colors, rules);
}

// ============================================================================
// Theme conversion
// ============================================================================

auto VsCodeThemeAdapter::convert_to_theme(const std::string& theme_name,
                                          const std::vector<VsCodeColorEntry>& colors,
                                          const std::vector<VsCodeTokenRule>& token_rules) -> Theme
{
    (void)colors;
    (void)token_rules;

    Theme theme;
    theme.name = theme_name;
    theme.author = "VSCode Import";
    theme.description = "Imported from VSCode theme";

    MARKAMP_LOG_INFO("Converted VSCode theme '{}' ({} colors, {} token rules)",
                     theme_name,
                     colors.size(),
                     token_rules.size());
    return theme;
}

// ============================================================================
// Compatibility report
// ============================================================================

auto VsCodeThemeAdapter::compatibility_report(const std::vector<VsCodeColorEntry>& colors,
                                              const std::vector<VsCodeTokenRule>& token_rules)
    -> VsCodeCompatibilityReport
{
    VsCodeCompatibilityReport report;
    const auto& color_map = vscode_color_map();

    report.total_tokens = static_cast<int>(colors.size() + token_rules.size());

    // Check color mappings
    for (const auto& color : colors)
    {
        if (color_map.contains(color.key))
        {
            ++report.supported_tokens;
        }
        else
        {
            report.unmapped_colors.push_back(color.key);
        }
    }

    // Token rules are always at least partially supported
    report.supported_tokens += static_cast<int>(token_rules.size());

    // Determine compatibility level
    if (report.total_tokens == 0)
    {
        report.level = ThemeCompatibility::kPartial;
        report.warnings.emplace_back("Theme contains no color or token definitions");
    }
    else if (report.unmapped_colors.empty())
    {
        report.level = ThemeCompatibility::kFull;
    }
    else if (report.unmapped_colors.size() <= colors.size() / 3)
    {
        report.level = ThemeCompatibility::kMapped;
    }
    else
    {
        report.level = ThemeCompatibility::kPartial;
    }

    return report;
}

// ============================================================================
// Color mapping
// ============================================================================

auto VsCodeThemeAdapter::map_vscode_color(const std::string& vscode_key) -> std::string
{
    const auto& color_map = vscode_color_map();
    auto iter = color_map.find(vscode_key);
    if (iter != color_map.end())
    {
        return iter->second;
    }
    return "";
}

} // namespace markamp::core
