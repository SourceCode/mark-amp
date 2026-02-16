#include "SettingsCatalog.h"

#include <algorithm>

namespace markamp::core
{

void SettingsCatalog::register_setting(CatalogEntry entry)
{
    const auto setting_id = entry.setting_id;
    if (id_index_.contains(setting_id))
    {
        // Overwrite existing entry
        entries_[id_index_[setting_id]] = std::move(entry);
        return;
    }
    id_index_[setting_id] = entries_.size();
    entries_.push_back(std::move(entry));
}

void SettingsCatalog::register_settings(std::vector<CatalogEntry> entries)
{
    for (auto& entry : entries)
    {
        register_setting(std::move(entry));
    }
}

void SettingsCatalog::ingest_plugin_settings(const std::vector<SettingContribution>& contributions)
{
    for (const auto& contrib : contributions)
    {
        CatalogEntry entry;
        entry.setting_id = contrib.id;
        entry.label = contrib.label;
        entry.description = contrib.description;
        entry.group = contrib.category.empty() ? "Plugins" : contrib.category;
        entry.type = contrib.type;
        entry.default_value = contrib.default_value;
        entry.choices = contrib.choices;
        entry.source = "plugin";
        register_setting(std::move(entry));
    }
}

auto SettingsCatalog::all_settings() const -> const std::vector<CatalogEntry>&
{
    return entries_;
}

auto SettingsCatalog::settings_for_group(std::string_view group) const
    -> std::vector<const CatalogEntry*>
{
    std::vector<const CatalogEntry*> result;
    for (const auto& entry : entries_)
    {
        if (entry.group == group)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto SettingsCatalog::find_setting(std::string_view setting_id) const -> const CatalogEntry*
{
    auto iter = id_index_.find(std::string(setting_id));
    if (iter == id_index_.end())
    {
        return nullptr;
    }
    return &entries_[iter->second];
}

auto SettingsCatalog::groups() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& entry : entries_)
    {
        if (std::find(result.begin(), result.end(), entry.group) == result.end())
        {
            result.push_back(entry.group);
        }
    }
    return result;
}

auto SettingsCatalog::search(std::string_view query) const -> std::vector<const CatalogEntry*>
{
    if (query.empty())
    {
        std::vector<const CatalogEntry*> all;
        all.reserve(entries_.size());
        for (const auto& entry : entries_)
        {
            all.push_back(&entry);
        }
        return all;
    }

    std::string lower_query(query);
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

    std::vector<const CatalogEntry*> result;
    for (const auto& entry : entries_)
    {
        // Check label
        std::string lower_label = entry.label;
        std::transform(lower_label.begin(), lower_label.end(), lower_label.begin(), ::tolower);
        if (lower_label.find(lower_query) != std::string::npos)
        {
            result.push_back(&entry);
            continue;
        }

        // Check description
        std::string lower_desc = entry.description;
        std::transform(lower_desc.begin(), lower_desc.end(), lower_desc.begin(), ::tolower);
        if (lower_desc.find(lower_query) != std::string::npos)
        {
            result.push_back(&entry);
            continue;
        }

        // Check setting_id
        std::string lower_id = entry.setting_id;
        std::transform(lower_id.begin(), lower_id.end(), lower_id.begin(), ::tolower);
        if (lower_id.find(lower_query) != std::string::npos)
        {
            result.push_back(&entry);
            continue;
        }

        // Check keywords
        for (const auto& keyword : entry.keywords)
        {
            std::string lower_kw = keyword;
            std::transform(lower_kw.begin(), lower_kw.end(), lower_kw.begin(), ::tolower);
            if (lower_kw.find(lower_query) != std::string::npos)
            {
                result.push_back(&entry);
                break;
            }
        }
    }
    return result;
}

auto SettingsCatalog::size() const -> std::size_t
{
    return entries_.size();
}

// ── Batch 8: Additional catalog methods ──

auto SettingsCatalog::settings_requiring_restart() const -> std::vector<const CatalogEntry*>
{
    std::vector<const CatalogEntry*> result;
    for (const auto& entry : entries_)
    {
        if (entry.restart_required)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto SettingsCatalog::settings_from_source(std::string_view source) const
    -> std::vector<const CatalogEntry*>
{
    std::vector<const CatalogEntry*> result;
    for (const auto& entry : entries_)
    {
        if (entry.source == source)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto SettingsCatalog::subgroups_for_group(std::string_view group) const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& entry : entries_)
    {
        if (entry.group == group && !entry.subgroup.empty())
        {
            if (std::find(result.begin(), result.end(), entry.subgroup) == result.end())
            {
                result.push_back(entry.subgroup);
            }
        }
    }
    return result;
}

auto SettingsCatalog::has_setting(std::string_view setting_id) const -> bool
{
    return id_index_.contains(std::string(setting_id));
}

void SettingsCatalog::remove_setting(std::string_view setting_id)
{
    auto iter = id_index_.find(std::string(setting_id));
    if (iter == id_index_.end())
    {
        return;
    }
    const auto index = iter->second;
    id_index_.erase(iter);

    // Remove the entry and rebuild the index
    entries_.erase(entries_.begin() + static_cast<std::ptrdiff_t>(index));
    id_index_.clear();
    for (std::size_t idx = 0; idx < entries_.size(); ++idx)
    {
        id_index_[entries_[idx].setting_id] = idx;
    }
}

void SettingsCatalog::clear()
{
    entries_.clear();
    id_index_.clear();
}

// ── New Batch 8: Additional catalog query implementations (#49-54) ──

auto SettingsCatalog::deprecated_settings() const -> std::vector<const CatalogEntry*>
{
    std::vector<const CatalogEntry*> result;
    for (const auto& entry : entries_)
    {
        if (entry.deprecated)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto SettingsCatalog::experimental_settings() const -> std::vector<const CatalogEntry*>
{
    std::vector<const CatalogEntry*> result;
    for (const auto& entry : entries_)
    {
        if (entry.experimental)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto SettingsCatalog::settings_sorted_by_priority() const -> std::vector<const CatalogEntry*>
{
    std::vector<const CatalogEntry*> result;
    result.reserve(entries_.size());
    for (const auto& entry : entries_)
    {
        result.push_back(&entry);
    }
    std::sort(result.begin(),
              result.end(),
              [](const CatalogEntry* lhs, const CatalogEntry* rhs)
              {
                  if (lhs->group != rhs->group)
                  {
                      return lhs->group < rhs->group;
                  }
                  return lhs->order_priority < rhs->order_priority;
              });
    return result;
}

auto SettingsCatalog::settings_by_type(SettingType type) const -> std::vector<const CatalogEntry*>
{
    std::vector<const CatalogEntry*> result;
    for (const auto& entry : entries_)
    {
        if (entry.type == type)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto SettingsCatalog::unique_groups() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& entry : entries_)
    {
        if (std::find(result.begin(), result.end(), entry.group) == result.end())
        {
            result.push_back(entry.group);
        }
    }
    return result;
}

void SettingsCatalog::merge_catalog(const SettingsCatalog& other)
{
    for (const auto& entry : other.entries_)
    {
        if (!id_index_.contains(entry.setting_id))
        {
            id_index_[entry.setting_id] = entries_.size();
            entries_.push_back(entry);
        }
    }
}

// ──────────────────────────────────────────────────────────────────────────
// Built-in settings — migrated from SettingsPanel::RegisterBuiltinSettings
// ──────────────────────────────────────────────────────────────────────────

void SettingsCatalog::register_builtins()
{
    // ── Editor settings ──

    register_setting({"editor.fontSize",
                      "Font Size",
                      "Controls the editor font size in pixels",
                      "Editor",
                      "Text",
                      ConfigScope::kApplication,
                      SettingType::Integer,
                      "13",
                      {},
                      {"font", "size", "text"},
                      false,
                      false,
                      false,
                      8,
                      72,
                      100,
                      "builtin"});

    register_setting(
        {"editor.fontFamily",
         "Font Family",
         "Controls the editor font family (e.g. 'Menlo', 'Fira Code', 'JetBrains Mono')",
         "Editor",
         "Text",
         ConfigScope::kApplication,
         SettingType::String,
         "Menlo",
         {},
         {"font", "typeface"},
         false,
         false,
         false,
         0,
         0,
         100,
         "builtin"});

    register_setting({"editor.tabSize",
                      "Tab Size",
                      "The number of spaces a tab is equal to",
                      "Editor",
                      "Formatting",
                      ConfigScope::kWorkspace,
                      SettingType::Integer,
                      "4",
                      {},
                      {"tab", "indent", "spaces"},
                      false,
                      false,
                      false,
                      1,
                      8,
                      100,
                      "builtin"});

    register_setting({"editor.wordWrap",
                      "Word Wrap",
                      "Controls how lines should wrap",
                      "Editor",
                      "Formatting",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"wrap", "line"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.lineNumbers",
                      "Line Numbers",
                      "Controls the display of line numbers",
                      "Editor",
                      "Gutter",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"gutter", "line number"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.bracketMatching",
                      "Bracket Matching",
                      "Highlight matching brackets",
                      "Editor",
                      "Formatting",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"bracket", "matching", "parentheses"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.autoIndent",
                      "Auto Indent",
                      "Controls auto indentation for new lines",
                      "Editor",
                      "Formatting",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"indent", "auto"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.minimap",
                      "Show Minimap",
                      "Controls whether the minimap is shown",
                      "Editor",
                      "Minimap",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "false",
                      {},
                      {"minimap", "overview"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.indentationGuides",
                      "Indentation Guides",
                      "Show indentation guide lines",
                      "Editor",
                      "Formatting",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"indent", "guide"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.codeFolding",
                      "Code Folding",
                      "Enable code folding in the editor",
                      "Editor",
                      "Formatting",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"fold", "collapse"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.showWhitespace",
                      "Render Whitespace",
                      "Controls how whitespace characters are rendered",
                      "Editor",
                      "Formatting",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "false",
                      {},
                      {"whitespace", "spaces", "tabs"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.edgeColumn",
                      "Edge Column",
                      "Column number for the vertical ruler line",
                      "Editor",
                      "Formatting",
                      ConfigScope::kApplication,
                      SettingType::Integer,
                      "80",
                      {},
                      {"ruler", "column", "edge"},
                      false,
                      false,
                      false,
                      40,
                      200,
                      100,
                      "builtin"});

    register_setting({"editor.fontLigatures",
                      "Font Ligatures",
                      "Enable font ligatures",
                      "Editor",
                      "Text",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "false",
                      {},
                      {"ligature", "font"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.highlightCurrentLine",
                      "Highlight Current Line",
                      "Highlight the line the cursor is on",
                      "Editor",
                      "Cursor",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"highlight", "current", "line"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.insertFinalNewline",
                      "Insert Final Newline",
                      "Insert a final newline at the end of the file when saving",
                      "Editor",
                      "Files",
                      ConfigScope::kWorkspace,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"newline", "save"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.autoClosingBrackets",
                      "Auto Closing Brackets",
                      "Auto-close brackets, quotes, and parentheses",
                      "Editor",
                      "Formatting",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"bracket", "auto", "close"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.smartListContinuation",
                      "Smart List Continuation",
                      "Continue markdown lists on Enter",
                      "Editor",
                      "Formatting",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"list", "markdown", "enter"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.trimTrailingWhitespace",
                      "Trim Trailing Whitespace",
                      "Remove trailing whitespace on save",
                      "Editor",
                      "Files",
                      ConfigScope::kWorkspace,
                      SettingType::Boolean,
                      "false",
                      {},
                      {"whitespace", "trim", "save"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    // ── Appearance settings ──

    register_setting({"appearance.theme",
                      "Color Theme",
                      "Select the editor color theme",
                      "Appearance",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Choice,
                      "midnight-neon",
                      {"midnight-neon", "synth-wave", "retro-terminal"},
                      {"theme", "color", "dark", "light"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"appearance.sidebarVisible",
                      "Show Sidebar",
                      "Controls whether the sidebar is visible",
                      "Appearance",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"sidebar", "panel"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    // ── Advanced settings ──

    register_setting({"advanced.autoSave",
                      "Auto Save",
                      "Enable auto-saving of files",
                      "Advanced",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "false",
                      {},
                      {"auto", "save"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"advanced.autoSaveDelay",
                      "Auto Save Delay",
                      "Delay in seconds before auto-saving (1-120)",
                      "Advanced",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Integer,
                      "30",
                      {},
                      {"auto", "save", "delay"},
                      false,
                      false,
                      false,
                      1,
                      120,
                      100,
                      "builtin"});

    // ── Phase 7 settings ──

    register_setting({"editor.cursorSurroundingLines",
                      "Cursor Surrounding Lines",
                      "Minimum number of visible lines above and below the cursor",
                      "Editor",
                      "Cursor",
                      ConfigScope::kApplication,
                      SettingType::Integer,
                      "5",
                      {},
                      {"cursor", "scroll", "surrounding"},
                      false,
                      false,
                      false,
                      0,
                      20,
                      100,
                      "builtin"});

    register_setting({"editor.scrollBeyondLastLine",
                      "Scroll Beyond Last Line",
                      "Allow scrolling past the end of the file",
                      "Editor",
                      "Scrolling",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"scroll", "end"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.smoothScrolling",
                      "Smooth Scrolling",
                      "Enable smooth scroll animations",
                      "Editor",
                      "Scrolling",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "false",
                      {},
                      {"scroll", "smooth", "animation"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.emptySelectionClipboard",
                      "Copy Line Without Selection",
                      "Ctrl+C with no selection copies the current line",
                      "Editor",
                      "Clipboard",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"copy", "clipboard", "line"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.cursorStyle",
                      "Cursor Style",
                      "Controls the cursor shape: line, block, or underline",
                      "Editor",
                      "Cursor",
                      ConfigScope::kApplication,
                      SettingType::Choice,
                      "line",
                      {"line", "block", "underline"},
                      {"cursor", "shape", "caret"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    // ── Phase 8 settings ──

    register_setting({"editor.renderWhitespace",
                      "Render Whitespace",
                      "Controls how whitespace characters are rendered (none/boundary/all)",
                      "Editor",
                      "Formatting",
                      ConfigScope::kApplication,
                      SettingType::Choice,
                      "none",
                      {"none", "boundary", "all"},
                      {"whitespace", "render"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.showLineNumbers",
                      "Show Line Numbers",
                      "Toggle line number display in the gutter",
                      "Editor",
                      "Gutter",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"line", "number", "gutter"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.minimap.enabled",
                      "Minimap",
                      "Show the minimap overview panel",
                      "Editor",
                      "Minimap",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "false",
                      {},
                      {"minimap"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    // ── R22: VS Code-Equivalent Editor Behavior (15 settings) ──

    register_setting({"editor.cursorBlinking",
                      "Cursor Blinking",
                      "Controls the cursor animation style (blink, smooth, phase, expand, solid)",
                      "Editor",
                      "Cursor",
                      ConfigScope::kApplication,
                      SettingType::Choice,
                      "blink",
                      {"blink", "smooth", "phase", "expand", "solid"},
                      {"cursor", "blink", "animation"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.cursorWidth",
                      "Cursor Width",
                      "Controls the width of the cursor in pixels when cursor style is 'line'",
                      "Editor",
                      "Cursor",
                      ConfigScope::kApplication,
                      SettingType::Integer,
                      "2",
                      {},
                      {"cursor", "width"},
                      false,
                      false,
                      false,
                      1,
                      10,
                      100,
                      "builtin"});

    register_setting({"editor.mouseWheelZoom",
                      "Mouse Wheel Zoom",
                      "Zoom the font in the editor when using the mouse wheel with Ctrl/Cmd",
                      "Editor",
                      "Scrolling",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "false",
                      {},
                      {"zoom", "mouse", "wheel"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.renderControlCharacters",
                      "Render Control Characters",
                      "Display control characters such as line endings in the editor",
                      "Editor",
                      "Formatting",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"control", "characters"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.roundedSelection",
                      "Rounded Selection",
                      "Render the editor selection with rounded borders",
                      "Editor",
                      "Selection",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"selection", "rounded"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.selectOnLineNumbers",
                      "Select on Line Numbers",
                      "Select the corresponding line when clicking on a line number",
                      "Editor",
                      "Gutter",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"select", "line", "gutter"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.matchBrackets",
                      "Match Brackets",
                      "How bracket matching is triggered (always, near cursor, or never)",
                      "Editor",
                      "Formatting",
                      ConfigScope::kApplication,
                      SettingType::Choice,
                      "always",
                      {"always", "near", "never"},
                      {"bracket", "matching"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.renderLineHighlight",
                      "Render Line Highlight",
                      "Controls the rendering of the current line highlight",
                      "Editor",
                      "Cursor",
                      ConfigScope::kApplication,
                      SettingType::Choice,
                      "all",
                      {"none", "gutter", "line", "all"},
                      {"line", "highlight", "current"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.renderLineHighlightOnlyWhenFocus",
                      "Line Highlight Only When Focused",
                      "Only show the current line highlight when the editor has focus",
                      "Editor",
                      "Cursor",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "false",
                      {},
                      {"line", "highlight", "focus"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.wordWrapColumn",
                      "Word Wrap Column",
                      "Column number at which editor content wraps when Word Wrap is set to column",
                      "Editor",
                      "Formatting",
                      ConfigScope::kWorkspace,
                      SettingType::Integer,
                      "80",
                      {},
                      {"wrap", "column"},
                      false,
                      false,
                      false,
                      40,
                      200,
                      100,
                      "builtin"});

    register_setting({"editor.wrappingIndent",
                      "Wrapping Indent",
                      "Indentation of wrapped lines (none, same, indent, deepIndent)",
                      "Editor",
                      "Formatting",
                      ConfigScope::kApplication,
                      SettingType::Choice,
                      "same",
                      {"none", "same", "indent", "deepIndent"},
                      {"wrap", "indent"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.lineHeight",
                      "Line Height",
                      "Extra line height in pixels (0 = use default from font size)",
                      "Editor",
                      "Text",
                      ConfigScope::kApplication,
                      SettingType::Integer,
                      "0",
                      {},
                      {"line", "height", "spacing"},
                      false,
                      false,
                      false,
                      0,
                      40,
                      100,
                      "builtin"});

    register_setting({"editor.letterSpacing",
                      "Letter Spacing",
                      "Extra letter spacing in pixels between characters",
                      "Editor",
                      "Text",
                      ConfigScope::kApplication,
                      SettingType::Integer,
                      "0",
                      {},
                      {"letter", "spacing", "kerning"},
                      false,
                      false,
                      false,
                      0,
                      10,
                      100,
                      "builtin"});

    register_setting(
        {"editor.scrollBeyondLastColumn",
         "Scroll Beyond Last Column",
         "Number of extra columns the editor can scroll horizontally past the last character",
         "Editor",
         "Scrolling",
         ConfigScope::kApplication,
         SettingType::Integer,
         "5",
         {},
         {"scroll", "column", "horizontal"},
         false,
         false,
         false,
         0,
         30,
         100,
         "builtin"});

    register_setting({"editor.autoClosingQuotes",
                      "Auto Closing Quotes",
                      "Auto-close quote characters when typing",
                      "Editor",
                      "Formatting",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"quotes", "auto", "close"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    // ── Editor Appearance (15 settings) ──

    register_setting({"editor.showFoldingControls",
                      "Show Folding Controls",
                      "When fold controls in the gutter are displayed (always, mouseover, never)",
                      "Appearance",
                      "Gutter",
                      ConfigScope::kApplication,
                      SettingType::Choice,
                      "mouseover",
                      {"always", "mouseover", "never"},
                      {"fold", "gutter", "controls"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.foldingHighlight",
                      "Folding Highlight",
                      "Highlight folded regions with a background color",
                      "Appearance",
                      "Gutter",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"fold", "highlight"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting(
        {"editor.glyphMargin",
         "Glyph Margin",
         "Show the glyph margin to the left of line numbers for breakpoints and decorations",
         "Appearance",
         "Gutter",
         ConfigScope::kApplication,
         SettingType::Boolean,
         "true",
         {},
         {"glyph", "margin", "gutter"},
         false,
         false,
         false,
         0,
         0,
         100,
         "builtin"});

    register_setting({"editor.overviewRulerBorder",
                      "Overview Ruler Border",
                      "Show a border around the overview ruler",
                      "Appearance",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"ruler", "border"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.lineNumbersMinChars",
                      "Line Numbers Minimum Chars",
                      "Minimum number of characters reserved for line number display",
                      "Appearance",
                      "Gutter",
                      ConfigScope::kApplication,
                      SettingType::Integer,
                      "5",
                      {},
                      {"line", "number", "width"},
                      false,
                      false,
                      false,
                      1,
                      10,
                      100,
                      "builtin"});

    register_setting({"editor.padding.top",
                      "Padding Top",
                      "Extra padding at the top of the editor in pixels",
                      "Appearance",
                      "Layout",
                      ConfigScope::kApplication,
                      SettingType::Integer,
                      "0",
                      {},
                      {"padding", "top"},
                      false,
                      false,
                      false,
                      0,
                      100,
                      100,
                      "builtin"});

    register_setting({"editor.padding.bottom",
                      "Padding Bottom",
                      "Extra padding at the bottom of the editor in pixels",
                      "Appearance",
                      "Layout",
                      ConfigScope::kApplication,
                      SettingType::Integer,
                      "0",
                      {},
                      {"padding", "bottom"},
                      false,
                      false,
                      false,
                      0,
                      100,
                      100,
                      "builtin"});

    register_setting({"editor.minimap.maxColumn",
                      "Minimap Max Column",
                      "Maximum width of the minimap in character columns",
                      "Appearance",
                      "Minimap",
                      ConfigScope::kApplication,
                      SettingType::Integer,
                      "120",
                      {},
                      {"minimap", "width"},
                      false,
                      false,
                      false,
                      40,
                      300,
                      100,
                      "builtin"});

    register_setting({"editor.minimap.scale",
                      "Minimap Scale",
                      "Scale factor for the minimap text (1-3)",
                      "Appearance",
                      "Minimap",
                      ConfigScope::kApplication,
                      SettingType::Integer,
                      "1",
                      {},
                      {"minimap", "scale"},
                      false,
                      false,
                      false,
                      1,
                      3,
                      100,
                      "builtin"});

    register_setting({"editor.minimap.side",
                      "Minimap Side",
                      "Which side the minimap appears on",
                      "Appearance",
                      "Minimap",
                      ConfigScope::kApplication,
                      SettingType::Choice,
                      "right",
                      {"left", "right"},
                      {"minimap", "side"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.stickyScroll.enabled",
                      "Sticky Scroll",
                      "Show sticky heading at the top of the editor while scrolling",
                      "Appearance",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "false",
                      {},
                      {"sticky", "scroll", "heading"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.guides.bracketPairs",
                      "Bracket Pair Guides",
                      "Show vertical guides connecting matching bracket pairs",
                      "Appearance",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "false",
                      {},
                      {"bracket", "guide", "pair"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.guides.highlightActiveIndentation",
                      "Highlight Active Indentation",
                      "Highlight the active indentation guide with a distinct color",
                      "Appearance",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"indent", "guide", "highlight"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.bracketPairColorization",
                      "Bracket Pair Colorization",
                      "Colorize matching bracket pairs with distinct colors",
                      "Appearance",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "false",
                      {},
                      {"bracket", "colorization", "rainbow"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"editor.colorDecorators",
                      "Color Decorators",
                      "Show inline color decorators for recognized color values",
                      "Appearance",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"color", "decorator", "inline"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    // ── Syntax Highlighting (10 settings) ──

    register_setting({"syntax.boldKeywords",
                      "Bold Keywords",
                      "Render language keywords in bold in code block syntax highlighting",
                      "Syntax Highlighting",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"syntax", "bold", "keyword"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"syntax.italicComments",
                      "Italic Comments",
                      "Render comments in italic in code block syntax highlighting",
                      "Syntax Highlighting",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"syntax", "italic", "comment"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"syntax.highlightStrings",
                      "Highlight Strings",
                      "Apply distinct color to string literals in code blocks",
                      "Syntax Highlighting",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"syntax", "string"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"syntax.highlightNumbers",
                      "Highlight Numbers",
                      "Apply distinct color to numeric literals in code blocks",
                      "Syntax Highlighting",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"syntax", "number"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"syntax.highlightOperators",
                      "Highlight Operators",
                      "Apply distinct color to operators in code blocks",
                      "Syntax Highlighting",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"syntax", "operator"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"syntax.highlightTypes",
                      "Highlight Types",
                      "Apply distinct color to type names in code blocks",
                      "Syntax Highlighting",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"syntax", "type"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"syntax.highlightFunctions",
                      "Highlight Functions",
                      "Apply distinct color to function names in code blocks",
                      "Syntax Highlighting",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"syntax", "function"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"syntax.highlightConstants",
                      "Highlight Constants",
                      "Apply distinct color to constants (true, false, null) in code blocks",
                      "Syntax Highlighting",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"syntax", "constant"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"syntax.highlightPreprocessor",
                      "Highlight Preprocessor",
                      "Apply distinct color to preprocessor directives in code blocks",
                      "Syntax Highlighting",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"syntax", "preprocessor"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"syntax.dimWhitespace",
                      "Dim Whitespace",
                      "Render whitespace tokens with a dimmed/muted color in code blocks",
                      "Syntax Highlighting",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "false",
                      {},
                      {"syntax", "whitespace", "dim"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    // ── Feature Toggles ──

    register_setting({"feature.mermaid.enabled",
                      "Mermaid Diagrams",
                      "Enable Mermaid diagram rendering in markdown preview",
                      "Features",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"mermaid", "diagram"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"feature.table-editor.enabled",
                      "Table Editor",
                      "Enable the interactive table editor for markdown tables",
                      "Features",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"table", "editor"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"feature.format-bar.enabled",
                      "Format Bar",
                      "Show the floating format bar for quick text formatting",
                      "Features",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"format", "bar", "floating"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"feature.theme-gallery.enabled",
                      "Theme Gallery",
                      "Enable the theme gallery panel for browsing and applying themes",
                      "Features",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"theme", "gallery"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"feature.link-preview.enabled",
                      "Link Preview",
                      "Show a tooltip preview when hovering over links",
                      "Features",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"link", "preview", "tooltip"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"feature.image-preview.enabled",
                      "Image Preview",
                      "Show inline image previews in the editor",
                      "Features",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"image", "preview", "inline"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"feature.breadcrumb-bar.enabled",
                      "Breadcrumb Bar",
                      "Show the breadcrumb navigation bar above the editor",
                      "Features",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"breadcrumb", "navigation"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});

    register_setting({"feature.syntax-highlighting.enabled",
                      "Syntax Highlighting",
                      "Enable syntax highlighting in code blocks",
                      "Features",
                      "",
                      ConfigScope::kApplication,
                      SettingType::Boolean,
                      "true",
                      {},
                      {"syntax", "highlighting", "code"},
                      false,
                      false,
                      false,
                      0,
                      0,
                      100,
                      "builtin"});
}

} // namespace markamp::core
