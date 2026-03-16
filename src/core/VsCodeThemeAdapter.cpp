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
        {"editorError.foreground", "editor_gutter_error"},
        {"editorWarning.foreground", "editor_gutter_warn"},
        {"editorInfo.foreground", "editor_gutter_info"},
        {"editor.findMatchBackground", "editor_find_hit"},
        {"editor.wordHighlightBackground", "editor_match_highlight"},

        // Sidebar
        {"sideBar.background", "sidebar_bg"},
        {"sideBar.foreground", "sidebar_fg"},
        {"sideBarTitle.foreground", "sidebar_title_fg"},
        {"sideBarSectionHeader.background", "sidebar_section_bg"},

        // Activity bar
        {"activityBar.background", "activity_bar_bg"},
        {"activityBar.foreground", "activity_bar_fg"},
        {"activityBar.inactiveForeground", "activity_bar_inactive_fg"},
        {"activityBarBadge.background", "activity_bar_badge_bg"},
        {"activityBarBadge.foreground", "activity_bar_badge_fg"},

        // Breadcrumb
        {"breadcrumb.foreground", "breadcrumb_fg"},
        {"breadcrumb.focusForeground", "breadcrumb_focus_fg"},
        {"breadcrumb.activeSelectionForeground", "breadcrumb_active_fg"},

        // Status bar
        {"statusBar.background", "status_bar_bg"},
        {"statusBar.foreground", "status_bar_fg"},
        {"statusBar.debuggingBackground", "status_bar_debug_bg"},
        {"statusBar.noFolderBackground", "status_bar_nofolder_bg"},

        // Tab bar
        {"tab.activeBackground", "tab_active_bg"},
        {"tab.activeForeground", "tab_active_fg"},
        {"tab.inactiveBackground", "tab_inactive_bg"},
        {"tab.inactiveForeground", "tab_inactive_fg"},
        {"tab.border", "tab_border"},
        {"tab.activeBorderTop", "tab_active_border_top"},

        // Title bar
        {"titleBar.activeBackground", "titlebar_bg"},
        {"titleBar.activeForeground", "titlebar_fg"},
        {"titleBar.inactiveBackground", "titlebar_inactive_bg"},

        // Panel
        {"panel.background", "panel_bg"},
        {"panel.border", "panel_border"},
        {"panelTitle.activeForeground", "panel_title_active_fg"},

        // Input
        {"input.background", "input_bg"},
        {"input.foreground", "input_fg"},
        {"input.border", "input_border"},
        {"input.placeholderForeground", "input_placeholder_fg"},

        // Button
        {"button.background", "button_bg"},
        {"button.foreground", "button_fg"},
        {"button.hoverBackground", "button_hover_bg"},

        // List
        {"list.activeSelectionBackground", "list_active_bg"},
        {"list.activeSelectionForeground", "list_active_fg"},
        {"list.hoverBackground", "list_hover_bg"},
        {"list.focusBackground", "list_focus_bg"},

        // Diff
        {"diffEditor.insertedTextBackground", "diff_inserted_bg"},
        {"diffEditor.removedTextBackground", "diff_removed_bg"},

        // Minimap
        {"minimap.background", "minimap_bg"},

        // Peek view
        {"peekView.border", "peek_view_border"},
        {"peekViewEditor.background", "peek_view_editor_bg"},
        {"peekViewResult.background", "peek_view_result_bg"},

        // Notebook
        {"notebook.cellEditorBackground", "notebook_cell_bg"},

        // Terminal
        {"terminal.background", "terminal_bg"},
        {"terminal.foreground", "terminal_fg"},

        // Badge
        {"badge.background", "badge_bg"},
        {"badge.foreground", "badge_fg"},

        // Dropdown
        {"dropdown.background", "dropdown_bg"},
        {"dropdown.foreground", "dropdown_fg"},
        {"dropdown.border", "dropdown_border"},

        // Focus border (global)
        {"focusBorder", "focus_border"},
        {"foreground", "foreground"},
    };
    return mapping;
}

// ============================================================================
// V16 Phase 17: TextMate scope -> internal syntax token mapping
// ============================================================================

static const std::unordered_map<std::string, std::string>& vscode_scope_map()
{
    static const std::unordered_map<std::string, std::string> mapping = {
        // Original 8 syntax scopes
        {"keyword", "syntax_keyword"},
        {"keyword.control", "syntax_keyword"},
        {"keyword.operator", "syntax_operator"},
        {"string", "syntax_string"},
        {"string.quoted", "syntax_string"},
        {"comment", "syntax_comment"},
        {"comment.line", "syntax_comment"},
        {"comment.block", "syntax_comment"},
        {"constant.numeric", "syntax_number"},
        {"entity.name.type", "syntax_type"},
        {"entity.name.function", "syntax_function"},
        {"storage.type", "syntax_type"},
        {"meta.preprocessor", "syntax_preprocessor"},

        // V16 Phase 17: Fine-grained scopes
        {"variable", "syntax_variable"},
        {"variable.other", "syntax_variable"},
        {"variable.parameter", "syntax_parameter"},
        {"variable.other.property", "syntax_property"},
        {"constant", "syntax_constant"},
        {"constant.language", "syntax_constant"},
        {"constant.character.escape", "syntax_escape"},
        {"entity.name.tag", "syntax_tag"},
        {"entity.other.attribute-name", "syntax_attribute"},
        {"entity.name.namespace", "syntax_namespace"},
        {"entity.name.type.enum", "syntax_enum"},
        {"entity.name.type.interface", "syntax_interface"},
        {"entity.name.type.struct", "syntax_struct"},
        {"entity.name.function.macro", "syntax_macro"},
        {"meta.decorator", "syntax_decorator"},
        {"punctuation.definition.decorator", "syntax_decorator"},
        {"string.regexp", "syntax_regex"},
        {"comment.block.documentation", "syntax_doc_comment"},

        // LSP semantic token fallbacks
        {"namespace", "syntax_namespace"},
        {"type", "syntax_type"},
        {"class", "syntax_type"},
        {"enum", "syntax_enum"},
        {"interface", "syntax_interface"},
        {"struct", "syntax_struct"},
        {"typeParameter", "syntax_type"},
        {"parameter", "syntax_parameter"},
        {"function", "syntax_function"},
        {"method", "syntax_function"},
        {"macro", "syntax_macro"},
        {"decorator", "syntax_decorator"},
        {"regexp", "syntax_regex"},
        {"property", "syntax_property"},
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

auto VsCodeThemeAdapter::map_vscode_scope(const std::string& scope) -> std::string
{
    const auto& scope_map = vscode_scope_map();
    auto iter = scope_map.find(scope);
    if (iter != scope_map.end())
    {
        return iter->second;
    }
    return "";
}

// ── Batch 23-25 (#149-150) ──

auto VsCodeThemeAdapter::mapped_color_count() -> std::size_t
{
    return vscode_color_map().size();
}

auto VsCodeThemeAdapter::mapped_scope_count() -> std::size_t
{
    return vscode_scope_map().size();
}

} // namespace markamp::core
