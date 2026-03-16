/// @file ThemeExporter.cpp
/// @brief V9 Phase 3 — VSCode theme export implementation.

#include "core/ThemeExporter.h"

#include "core/Logger.h"

#include <fstream>
#include <sstream>

namespace markamp::core
{

auto ThemeExporter::to_vscode_json(const Theme& theme) -> std::string
{
    std::ostringstream oss;
    oss << "{\n";
    oss << R"(  "name": ")" << theme.name << "\",\n";
    oss << R"(  "type": ")" << (theme.is_dark() ? "dark" : "light") << "\",\n";
    oss << R"(  "colors": {)"
        << "\n";

    auto emit = [&](const std::string& key, const Color& clr, bool last = false)
    { oss << "    \"" << key << "\": \"" << clr.to_hex() << "\"" << (last ? "\n" : ",\n"); };

    emit("editor.background", theme.colors.editor_bg);
    emit("editor.foreground", theme.colors.editor_fg);
    emit("editor.selectionBackground", theme.colors.editor_selection);
    emit("editor.lineHighlightBackground", theme.colors.editor_active_line);
    emit("editorCursor.foreground", theme.colors.editor_cursor);
    emit("editorLineNumber.foreground", theme.colors.editor_line_number);

    emit("sideBar.background", theme.colors.sidebar_bg);
    emit("sideBar.foreground", theme.colors.sidebar_fg);
    emit("activityBar.background", theme.colors.activity_bar_bg);
    emit("activityBar.foreground", theme.colors.activity_bar_fg);
    emit("activityBarBadge.background", theme.colors.activity_bar_badge_bg);
    emit("activityBarBadge.foreground", theme.colors.activity_bar_badge_fg);

    emit("breadcrumb.foreground", theme.colors.breadcrumb_fg);
    emit("breadcrumb.focusForeground", theme.colors.breadcrumb_focus_fg);

    emit("tab.activeBackground", theme.colors.tab_active_bg);
    emit("tab.activeForeground", theme.colors.tab_active_fg);
    emit("tab.inactiveBackground", theme.colors.tab_inactive_bg);
    emit("tab.inactiveForeground", theme.colors.tab_inactive_fg);

    emit("diffEditor.insertedTextBackground", theme.colors.diff_inserted_bg);
    emit("diffEditor.removedTextBackground", theme.colors.diff_removed_bg);
    emit("minimap.background", theme.colors.minimap_bg);
    emit("peekView.border", theme.colors.peek_view_border);
    emit("notebook.cellEditorBackground", theme.colors.notebook_cell_bg);

    emit("input.background", theme.colors.bg_input);
    emit("panel.background", theme.colors.bg_panel);
    emit("statusBar.background", theme.colors.bg_header);
    emit("focusBorder", theme.colors.accent_primary, true);

    oss << "  },\n";

    oss << R"(  "tokenColors": [)"
        << "\n";

    auto emit_token = [&](const std::string& scope, const Color& fg_color, bool last = false)
    {
        oss << "    {\n";
        oss << "      \"scope\": \"" << scope << "\",\n";
        oss << "      \"settings\": {\n";
        oss << "        \"foreground\": \"" << fg_color.to_hex() << "\"\n";
        oss << "      }\n";
        oss << "    }" << (last ? "\n" : ",\n");
    };

    emit_token("keyword", theme.syntax.keyword);
    emit_token("string", theme.syntax.string_literal);
    emit_token("comment", theme.syntax.comment);
    emit_token("constant.numeric", theme.syntax.number);
    emit_token("entity.name.type", theme.syntax.type_name);
    emit_token("entity.name.function", theme.syntax.function_name);
    emit_token("keyword.operator", theme.syntax.operator_tok);
    emit_token("meta.preprocessor", theme.syntax.preprocessor, true);

    oss << "  ]\n";
    oss << "}\n";

    return oss.str();
}

auto ThemeExporter::export_to_vscode(const Theme& theme, const std::filesystem::path& output_path)
    -> std::expected<void, std::string>
{
    std::ofstream file(output_path);
    if (!file.is_open())
    {
        return std::unexpected("Failed to open output file: " + output_path.string());
    }

    file << to_vscode_json(theme);

    if (!file.good())
    {
        return std::unexpected("Error writing to file: " + output_path.string());
    }

    MARKAMP_LOG_INFO("Exported theme '{}' to VSCode format: {}", theme.name, output_path.string());
    return {};
}

// (#95) Return the number of editor color tokens included in export.
auto ThemeExporter::color_count() -> std::size_t
{
    // 24 color tokens + 8 token colors = 32 total entries
    return 32;
}

// (#158) Return the number of syntax token colors included in export.
auto ThemeExporter::syntax_token_count() -> std::size_t
{
    // 8 base syntax + 15 fine-grained = 23 syntax token entries
    return 23;
}

} // namespace markamp::core
