#include "SettingsPanel.h"

#include "core/Config.h"
#include "core/Events.h"
#include "core/Logger.h"
#include "core/SettingsCatalog.h"

#include <wx/button.h>
#include <wx/imaglist.h>
#include <wx/sizer.h>
#include <wx/statline.h>

#include <algorithm>
#include <fstream>
#include <map>
#include <sstream>

namespace markamp::ui
{

SettingsPanel::SettingsPanel(wxWindow* parent,
                             core::ThemeEngine& theme_engine,
                             core::EventBus& event_bus,
                             core::Config& config)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , config_(config)
{
    CreateLayout();
    RegisterBuiltinSettings();
    BuildCategoryTree();
    ApplyTheme();

    // Subscribe to theme changes
    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });
}

SettingsPanel::SettingsPanel(wxWindow* parent,
                             core::ThemeEngine& theme_engine,
                             core::EventBus& event_bus,
                             core::Config& config,
                             core::SettingsCatalog& catalog)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , config_(config)
    , catalog_(&catalog)
{
    CreateLayout();
    PopulateFromCatalog();
    BuildCategoryTree();
    ApplyTheme();

    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });
}

void SettingsPanel::PopulateFromCatalog()
{
    if (catalog_ == nullptr)
    {
        return;
    }
    definitions_.clear();
    for (const auto& entry : catalog_->all_settings())
    {
        SettingDefinition def;
        def.setting_id = entry.setting_id;
        def.label = entry.label;
        def.description = entry.description;
        def.category = entry.group;
        def.type = entry.type;
        def.default_value = entry.default_value;
        def.choices = entry.choices;
        def.min_int = entry.min_int;
        def.max_int = entry.max_int;
        def.deprecated = entry.deprecated;
        def.experimental = entry.experimental;
        def.restart_required = entry.restart_required;
        def.order_priority = entry.order_priority;
        definitions_.push_back(std::move(def));
    }
    RebuildSettingsList();
}

void SettingsPanel::CreateLayout()
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Title
    auto* title = new wxStaticText(this, wxID_ANY, "Settings");
    auto title_font = title->GetFont();
    title_font.SetPointSize(18);
    title_font.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(title_font);
    main_sizer->Add(title, 0, wxLEFT | wxTOP, 16);

    // Batch 5A: Scope tabs (User / Workspace / Project)
    scope_tabs_ = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 28));
    auto* user_page = new wxPanel(scope_tabs_);
    auto* workspace_page = new wxPanel(scope_tabs_);
    auto* project_page = new wxPanel(scope_tabs_);
    scope_tabs_->AddPage(user_page, "User", true);
    scope_tabs_->AddPage(workspace_page, "Workspace");
    scope_tabs_->AddPage(project_page, "Project");
    scope_tabs_->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &SettingsPanel::OnScopeChanged, this);
    main_sizer->Add(scope_tabs_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 12);

    // Search bar
    search_ctrl_ =
        new wxSearchCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 32));
    search_ctrl_->SetDescriptiveText("Search settings...");
    main_sizer->Add(search_ctrl_, 0, wxEXPAND | wxALL, 16);

    search_ctrl_->Bind(wxEVT_TEXT, &SettingsPanel::OnSearchChanged, this);

    // Batch 5E Task 19: Bind debounce timer to RebuildSettingsList
    search_debounce_timer_.SetOwner(this);
    Bind(wxEVT_TIMER, [this](wxTimerEvent& /*evt*/) { RebuildSettingsList(); });

    // Horizontal splitter: category tree + scroll area
    auto* content_sizer = new wxBoxSizer(wxHORIZONTAL);

    // Batch 5A: Hierarchical category tree (replaces wxListBox)
    category_tree_ =
        new wxTreeCtrl(this,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxSize(180, -1),
                       wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS | wxTR_SINGLE | wxTR_NO_LINES);
    category_tree_->SetIndent(category_tree_->GetIndent() + 8);
    category_tree_->Bind(wxEVT_TREE_SEL_CHANGED, &SettingsPanel::OnTreeSelectionChanged, this);
    content_sizer->Add(category_tree_, 0, wxEXPAND | wxRIGHT, 8);

    // Scrollable area for settings
    scroll_area_ = new wxScrolledWindow(this, wxID_ANY);
    scroll_area_->SetScrollRate(0, 10);
    settings_sizer_ = new wxBoxSizer(wxVERTICAL);
    scroll_area_->SetSizer(settings_sizer_);
    content_sizer->Add(scroll_area_, 1, wxEXPAND);

    main_sizer->Add(content_sizer, 1, wxEXPAND);

    // Phase 9: Export / Import toolbar
    auto* toolbar_sizer = new wxBoxSizer(wxHORIZONTAL);
    auto* export_btn =
        new wxButton(this, wxID_ANY, "Export Settings", wxDefaultPosition, wxSize(130, 28));
    export_btn->SetToolTip("Export all settings to a JSON file");
    export_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& /*evt*/) { ExportSettings(); });
    toolbar_sizer->Add(export_btn, 0, wxRIGHT, 8);

    auto* import_btn =
        new wxButton(this, wxID_ANY, "Import Settings", wxDefaultPosition, wxSize(130, 28));
    import_btn->SetToolTip("Import settings from a JSON file");
    import_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& /*evt*/) { ImportSettings(); });
    toolbar_sizer->Add(import_btn, 0, wxRIGHT, 8);

    // R18 Fix 23: Reset All Settings button
    auto* reset_all_btn =
        new wxButton(this, wxID_ANY, "Reset All", wxDefaultPosition, wxSize(100, 28));
    reset_all_btn->SetToolTip("Reset all settings to their default values");
    reset_all_btn->SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::ErrorColor));
    reset_all_btn->SetForegroundColour(*wxWHITE);
    reset_all_btn->Bind(wxEVT_BUTTON,
                        [this](wxCommandEvent& /*evt*/)
                        {
                            for (const auto& def : definitions_)
                            {
                                ResetSettingToDefault(def.setting_id, def.default_value);
                            }
                            RefreshValues();
                        });
    toolbar_sizer->Add(reset_all_btn, 0);

    // Batch 5C Task 9-10: "Show Modified Only" filter checkbox
    toolbar_sizer->AddSpacer(16);
    show_modified_only_ = new wxCheckBox(this, wxID_ANY, "Show Modified Only");
    show_modified_only_->SetToolTip("Only display settings that differ from their defaults");
    show_modified_only_->Bind(wxEVT_CHECKBOX,
                              [this](wxCommandEvent& /*evt*/) { RebuildSettingsList(); });

    main_sizer->Add(toolbar_sizer, 0, wxALL, 12);

    SetSizer(main_sizer);
}

// Batch 5A: Build hierarchical category tree from registered settings
void SettingsPanel::BuildCategoryTree()
{
    if (category_tree_ == nullptr)
        return;

    category_tree_->DeleteAllItems();
    auto root = category_tree_->AddRoot("Settings");

    // Collect unique categories and subgroups from definitions
    // Using ordered containers for deterministic display
    std::map<std::string, std::set<std::string>> group_to_subgroups;
    for (const auto& def : definitions_)
    {
        // Parse subgroup from dotted setting IDs, e.g. "editor.minimap.enabled" → subgroup
        // "Minimap"
        auto& subgroups = group_to_subgroups[def.category];
        auto dot_pos = def.setting_id.find('.');
        if (dot_pos != std::string::npos)
        {
            auto second_dot = def.setting_id.find('.', dot_pos + 1);
            if (second_dot != std::string::npos)
            {
                auto sub = def.setting_id.substr(dot_pos + 1, second_dot - dot_pos - 1);
                // Capitalize first letter
                if (!sub.empty())
                {
                    sub[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(sub[0])));
                }
                subgroups.insert(sub);
            }
        }
    }

    // Emoji prefix map
    auto get_icon = [](const std::string& cat) -> wxString
    {
        if (cat == "Editor")
            return wxString::FromUTF8("\xF0\x9F\x8E\xA8 "); // 🎨
        if (cat == "Appearance")
            return wxString::FromUTF8("\xF0\x9F\x96\xA5 "); // 🖥
        if (cat == "Keybindings")
            return wxString::FromUTF8("\xE2\x8C\xA8 "); // ⌨
        if (cat == "Plugins")
            return wxString::FromUTF8("\xF0\x9F\x94\x8C "); // 🔌
        if (cat == "Advanced")
            return wxString::FromUTF8("\xE2\x9A\x99 "); // ⚙
        if (cat == "Features")
            return wxString::FromUTF8("\xF0\x9F\xA7\xA9 "); // 🧩
        if (cat == "Syntax Highlighting")
            return wxString::FromUTF8("\xF0\x9F\x92\xA1 "); // 💡
        return {};
    };

    for (const auto& [group, subgroups] : group_to_subgroups)
    {
        auto group_item = category_tree_->AppendItem(root, get_icon(group) + group);
        for (const auto& sub : subgroups)
        {
            category_tree_->AppendItem(group_item, sub);
        }
    }

    category_tree_->ExpandAll();

    wxTreeItemIdValue cookie;
    auto first_child = category_tree_->GetFirstChild(root, cookie);
    if (first_child.IsOk())
    {
        category_tree_->SelectItem(first_child);
    }
}

void SettingsPanel::RegisterSetting(SettingDefinition definition)
{
    definitions_.push_back(std::move(definition));
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
void SettingsPanel::RegisterBuiltinSettings()
{
    // Editor settings
    RegisterSetting({"editor.fontSize",
                     "Font Size",
                     "Controls the editor font size in pixels",
                     "Editor",
                     core::SettingType::Integer,
                     "13",
                     {},
                     8,
                     72});
    RegisterSetting(
        {"editor.fontFamily",
         "Font Family",
         "Controls the editor font family (e.g. 'Menlo', 'Fira Code', 'JetBrains Mono')",
         "Editor",
         core::SettingType::String,
         "Menlo",
         {}});
    RegisterSetting({"editor.tabSize",
                     "Tab Size",
                     "The number of spaces a tab is equal to",
                     "Editor",
                     core::SettingType::Integer,
                     "4",
                     {},
                     1,
                     8});
    RegisterSetting({"editor.wordWrap",
                     "Word Wrap",
                     "Controls how lines should wrap",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.lineNumbers",
                     "Line Numbers",
                     "Controls the display of line numbers",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.bracketMatching",
                     "Bracket Matching",
                     "Highlight matching brackets",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.autoIndent",
                     "Auto Indent",
                     "Controls auto indentation for new lines",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.minimap",
                     "Show Minimap",
                     "Controls whether the minimap is shown",
                     "Editor",
                     core::SettingType::Boolean,
                     "false",
                     {}});
    RegisterSetting({"editor.indentationGuides",
                     "Indentation Guides",
                     "Show indentation guide lines",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.codeFolding",
                     "Code Folding",
                     "Enable code folding in the editor",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.showWhitespace",
                     "Render Whitespace",
                     "Controls how whitespace characters are rendered",
                     "Editor",
                     core::SettingType::Boolean,
                     "false",
                     {}});
    RegisterSetting({"editor.edgeColumn",
                     "Edge Column",
                     "Column number for the vertical ruler line",
                     "Editor",
                     core::SettingType::Integer,
                     "80",
                     {},
                     40,
                     200});
    RegisterSetting({"editor.fontLigatures",
                     "Font Ligatures",
                     "Enable font ligatures",
                     "Editor",
                     core::SettingType::Boolean,
                     "false",
                     {}});
    RegisterSetting({"editor.highlightCurrentLine",
                     "Highlight Current Line",
                     "Highlight the line the cursor is on",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.insertFinalNewline",
                     "Insert Final Newline",
                     "Insert a final newline at the end of the file when saving",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.autoClosingBrackets",
                     "Auto Closing Brackets",
                     "Auto-close brackets, quotes, and parentheses",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.smartListContinuation",
                     "Smart List Continuation",
                     "Continue markdown lists on Enter",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.trimTrailingWhitespace",
                     "Trim Trailing Whitespace",
                     "Remove trailing whitespace on save",
                     "Editor",
                     core::SettingType::Boolean,
                     "false",
                     {}});

    // Appearance settings
    RegisterSetting({"appearance.theme",
                     "Color Theme",
                     "Select the editor color theme",
                     "Appearance",
                     core::SettingType::Choice,
                     "midnight-neon",
                     {"midnight-neon", "synth-wave", "retro-terminal"}});
    RegisterSetting({"appearance.sidebarVisible",
                     "Show Sidebar",
                     "Controls whether the sidebar is visible",
                     "Appearance",
                     core::SettingType::Boolean,
                     "true",
                     {}});

    // Advanced settings
    RegisterSetting({"advanced.autoSave",
                     "Auto Save",
                     "Enable auto-saving of files",
                     "Advanced",
                     core::SettingType::Boolean,
                     "false",
                     {}});
    RegisterSetting({"advanced.autoSaveDelay",
                     "Auto Save Delay",
                     "Delay in seconds before auto-saving (1-120)",
                     "Advanced",
                     core::SettingType::Integer,
                     "30",
                     {},
                     1,
                     120});

    // Phase 7 new settings
    RegisterSetting({"editor.cursorSurroundingLines",
                     "Cursor Surrounding Lines",
                     "Minimum number of visible lines above and below the cursor",
                     "Editor",
                     core::SettingType::Integer,
                     "5",
                     {},
                     0,
                     20});
    RegisterSetting({"editor.scrollBeyondLastLine",
                     "Scroll Beyond Last Line",
                     "Allow scrolling past the end of the file",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.smoothScrolling",
                     "Smooth Scrolling",
                     "Enable smooth scroll animations",
                     "Editor",
                     core::SettingType::Boolean,
                     "false",
                     {}});
    RegisterSetting({"editor.emptySelectionClipboard",
                     "Copy Line Without Selection",
                     "Ctrl+C with no selection copies the current line",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.cursorStyle",
                     "Cursor Style",
                     "Controls the cursor shape: line, block, or underline",
                     "Editor",
                     core::SettingType::Choice,
                     "line",
                     {"line", "block", "underline"}});

    // Phase 8 new settings
    RegisterSetting({"editor.renderWhitespace",
                     "Render Whitespace",
                     "Controls how whitespace characters are rendered (none/boundary/all)",
                     "Editor",
                     core::SettingType::Choice,
                     "none",
                     {"none", "boundary", "all"}});
    RegisterSetting({"editor.showLineNumbers",
                     "Show Line Numbers",
                     "Toggle line number display in the gutter",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.minimap.enabled",
                     "Minimap",
                     "Show the minimap overview panel",
                     "Editor",
                     core::SettingType::Boolean,
                     "false",
                     {}});

    // ═══════════════════════════════════════════════════════
    // R22: 40 VS Code-Equivalent Editor & Syntax Highlighting Settings
    // ═══════════════════════════════════════════════════════

    // ── Editor Behavior (15 settings) ──

    RegisterSetting({"editor.cursorBlinking",
                     "Cursor Blinking",
                     "Controls the cursor animation style (blink, smooth, phase, expand, solid)",
                     "Editor",
                     core::SettingType::Choice,
                     "blink",
                     {"blink", "smooth", "phase", "expand", "solid"}});
    RegisterSetting({"editor.cursorWidth",
                     "Cursor Width",
                     "Controls the width of the cursor in pixels when cursor style is 'line'",
                     "Editor",
                     core::SettingType::Integer,
                     "2",
                     {},
                     1,
                     10});
    RegisterSetting({"editor.mouseWheelZoom",
                     "Mouse Wheel Zoom",
                     "Zoom the font in the editor when using the mouse wheel with Ctrl/Cmd",
                     "Editor",
                     core::SettingType::Boolean,
                     "false",
                     {}});
    RegisterSetting({"editor.renderControlCharacters",
                     "Render Control Characters",
                     "Display control characters such as line endings in the editor",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.roundedSelection",
                     "Rounded Selection",
                     "Render the editor selection with rounded borders",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.selectOnLineNumbers",
                     "Select on Line Numbers",
                     "Select the corresponding line when clicking on a line number",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.matchBrackets",
                     "Match Brackets",
                     "How bracket matching is triggered (always, near cursor, or never)",
                     "Editor",
                     core::SettingType::Choice,
                     "always",
                     {"always", "near", "never"}});
    RegisterSetting({"editor.renderLineHighlight",
                     "Render Line Highlight",
                     "Controls the rendering of the current line highlight",
                     "Editor",
                     core::SettingType::Choice,
                     "all",
                     {"none", "gutter", "line", "all"}});
    RegisterSetting({"editor.renderLineHighlightOnlyWhenFocus",
                     "Line Highlight Only When Focused",
                     "Only show the current line highlight when the editor has focus",
                     "Editor",
                     core::SettingType::Boolean,
                     "false",
                     {}});
    RegisterSetting({"editor.wordWrapColumn",
                     "Word Wrap Column",
                     "Column number at which editor content wraps when Word Wrap is set to column",
                     "Editor",
                     core::SettingType::Integer,
                     "80",
                     {},
                     40,
                     200});
    RegisterSetting({"editor.wrappingIndent",
                     "Wrapping Indent",
                     "Indentation of wrapped lines (none, same, indent, deepIndent)",
                     "Editor",
                     core::SettingType::Choice,
                     "same",
                     {"none", "same", "indent", "deepIndent"}});
    RegisterSetting({"editor.lineHeight",
                     "Line Height",
                     "Extra line height in pixels (0 = use default from font size)",
                     "Editor",
                     core::SettingType::Integer,
                     "0",
                     {},
                     0,
                     40});
    RegisterSetting({"editor.letterSpacing",
                     "Letter Spacing",
                     "Extra letter spacing in pixels between characters",
                     "Editor",
                     core::SettingType::Integer,
                     "0",
                     {},
                     0,
                     10});
    RegisterSetting(
        {"editor.scrollBeyondLastColumn",
         "Scroll Beyond Last Column",
         "Number of extra columns the editor can scroll horizontally past the last character",
         "Editor",
         core::SettingType::Integer,
         "5",
         {},
         0,
         30});
    RegisterSetting({"editor.autoClosingQuotes",
                     "Auto Closing Quotes",
                     "Auto-close quote characters when typing",
                     "Editor",
                     core::SettingType::Boolean,
                     "true",
                     {}});

    // ── Editor Appearance (15 settings) ──

    RegisterSetting({"editor.showFoldingControls",
                     "Show Folding Controls",
                     "When fold controls in the gutter are displayed (always, mouseover, never)",
                     "Appearance",
                     core::SettingType::Choice,
                     "mouseover",
                     {"always", "mouseover", "never"}});
    RegisterSetting({"editor.foldingHighlight",
                     "Folding Highlight",
                     "Highlight folded regions with a background color",
                     "Appearance",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting(
        {"editor.glyphMargin",
         "Glyph Margin",
         "Show the glyph margin to the left of line numbers for breakpoints and decorations",
         "Appearance",
         core::SettingType::Boolean,
         "true",
         {}});
    RegisterSetting({"editor.overviewRulerBorder",
                     "Overview Ruler Border",
                     "Show a border around the overview ruler",
                     "Appearance",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.lineNumbersMinChars",
                     "Line Numbers Minimum Chars",
                     "Minimum number of characters reserved for line number display",
                     "Appearance",
                     core::SettingType::Integer,
                     "5",
                     {},
                     1,
                     10});
    RegisterSetting({"editor.padding.top",
                     "Padding Top",
                     "Extra padding at the top of the editor in pixels",
                     "Appearance",
                     core::SettingType::Integer,
                     "0",
                     {},
                     0,
                     100});
    RegisterSetting({"editor.padding.bottom",
                     "Padding Bottom",
                     "Extra padding at the bottom of the editor in pixels",
                     "Appearance",
                     core::SettingType::Integer,
                     "0",
                     {},
                     0,
                     100});
    RegisterSetting({"editor.minimap.maxColumn",
                     "Minimap Max Column",
                     "Maximum width of the minimap in character columns",
                     "Appearance",
                     core::SettingType::Integer,
                     "120",
                     {},
                     40,
                     300});
    RegisterSetting({"editor.minimap.scale",
                     "Minimap Scale",
                     "Scale factor for the minimap text (1-3)",
                     "Appearance",
                     core::SettingType::Integer,
                     "1",
                     {},
                     1,
                     3});
    RegisterSetting({"editor.minimap.side",
                     "Minimap Side",
                     "Which side the minimap appears on",
                     "Appearance",
                     core::SettingType::Choice,
                     "right",
                     {"left", "right"}});
    RegisterSetting({"editor.stickyScroll.enabled",
                     "Sticky Scroll",
                     "Show sticky heading at the top of the editor while scrolling",
                     "Appearance",
                     core::SettingType::Boolean,
                     "false",
                     {}});
    RegisterSetting({"editor.guides.bracketPairs",
                     "Bracket Pair Guides",
                     "Show vertical guides connecting matching bracket pairs",
                     "Appearance",
                     core::SettingType::Boolean,
                     "false",
                     {}});
    RegisterSetting({"editor.guides.highlightActiveIndentation",
                     "Highlight Active Indentation",
                     "Highlight the active indentation guide with a distinct color",
                     "Appearance",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"editor.bracketPairColorization",
                     "Bracket Pair Colorization",
                     "Colorize matching bracket pairs with distinct colors",
                     "Appearance",
                     core::SettingType::Boolean,
                     "false",
                     {}});
    RegisterSetting({"editor.colorDecorators",
                     "Color Decorators",
                     "Show inline color decorators for recognized color values",
                     "Appearance",
                     core::SettingType::Boolean,
                     "true",
                     {}});

    // ── Syntax Highlighting (10 settings) ──

    RegisterSetting({"syntax.boldKeywords",
                     "Bold Keywords",
                     "Render language keywords in bold in code block syntax highlighting",
                     "Syntax Highlighting",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"syntax.italicComments",
                     "Italic Comments",
                     "Render comments in italic in code block syntax highlighting",
                     "Syntax Highlighting",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"syntax.highlightStrings",
                     "Highlight Strings",
                     "Apply distinct color to string literals in code blocks",
                     "Syntax Highlighting",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"syntax.highlightNumbers",
                     "Highlight Numbers",
                     "Apply distinct color to numeric literals in code blocks",
                     "Syntax Highlighting",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"syntax.highlightOperators",
                     "Highlight Operators",
                     "Apply distinct color to operators in code blocks",
                     "Syntax Highlighting",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"syntax.highlightTypes",
                     "Highlight Types",
                     "Apply distinct color to type names in code blocks",
                     "Syntax Highlighting",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"syntax.highlightFunctions",
                     "Highlight Functions",
                     "Apply distinct color to function names in code blocks",
                     "Syntax Highlighting",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"syntax.highlightConstants",
                     "Highlight Constants",
                     "Apply distinct color to constants (true, false, null) in code blocks",
                     "Syntax Highlighting",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"syntax.highlightPreprocessor",
                     "Highlight Preprocessor",
                     "Apply distinct color to preprocessor directives in code blocks",
                     "Syntax Highlighting",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"syntax.dimWhitespace",
                     "Dim Whitespace",
                     "Render whitespace tokens with a dimmed/muted color in code blocks",
                     "Syntax Highlighting",
                     core::SettingType::Boolean,
                     "false",
                     {}});

    // ── Feature Toggles (Phase 1 Plugin Architecture) ──

    RegisterSetting({"feature.mermaid.enabled",
                     "Mermaid Diagrams",
                     "Enable Mermaid diagram rendering in markdown preview",
                     "Features",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"feature.table-editor.enabled",
                     "Table Editor",
                     "Enable the interactive table editor for markdown tables",
                     "Features",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"feature.format-bar.enabled",
                     "Format Bar",
                     "Show the floating format bar for quick text formatting",
                     "Features",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"feature.theme-gallery.enabled",
                     "Theme Gallery",
                     "Enable the theme gallery panel for browsing and applying themes",
                     "Features",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"feature.link-preview.enabled",
                     "Link Preview",
                     "Show a tooltip preview when hovering over links",
                     "Features",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"feature.image-preview.enabled",
                     "Image Preview",
                     "Show inline image previews in the editor",
                     "Features",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"feature.breadcrumb-bar.enabled",
                     "Breadcrumb Bar",
                     "Show the breadcrumb navigation bar above the editor",
                     "Features",
                     core::SettingType::Boolean,
                     "true",
                     {}});
    RegisterSetting({"feature.syntax-highlighting.enabled",
                     "Syntax Highlighting",
                     "Enable syntax highlighting in code blocks",
                     "Features",
                     core::SettingType::Boolean,
                     "true",
                     {}});

    RebuildSettingsList();
}
#pragma clang diagnostic pop

void SettingsPanel::RebuildSettingsList()
{
    // Clear existing widgets
    settings_sizer_->Clear(true);
    setting_widgets_.clear();

    std::string filter;
    if (search_ctrl_ != nullptr)
    {
        filter = search_ctrl_->GetValue().ToStdString();
    }

    // Group settings by category
    std::vector<std::string> categories = {
        "Editor", "Appearance", "Features", "Keybindings", "Plugins", "Advanced"};
    for (const auto& category : categories)
    {
        // Phase 9: Count how many settings in this category match filters
        int matching_count = 0;
        int modified_count = 0;
        for (const auto& def : definitions_)
        {
            if (def.category != category)
                continue;
            if (!active_category_.empty() && def.category != active_category_)
                continue;
            if (!filter.empty())
            {
                std::string lower_label = def.label;
                std::string lower_desc = def.description;
                std::string lower_filter = filter;
                std::transform(
                    lower_label.begin(), lower_label.end(), lower_label.begin(), ::tolower);
                std::transform(lower_desc.begin(), lower_desc.end(), lower_desc.begin(), ::tolower);
                std::transform(
                    lower_filter.begin(), lower_filter.end(), lower_filter.begin(), ::tolower);
                if (lower_label.find(lower_filter) == std::string::npos &&
                    lower_desc.find(lower_filter) == std::string::npos)
                    continue;
            }
            ++matching_count;
            if (IsSettingModified(def))
                ++modified_count;
        }

        if (matching_count == 0)
            continue;

        bool is_collapsed = collapsed_categories_.count(category) > 0;

        // Phase 9: Category header with count badge and collapse toggle
        auto* header_sizer = new wxBoxSizer(wxHORIZONTAL);

        wxString collapse_icon = is_collapsed ? wxString("▶") : wxString("▼");
        auto* toggle_btn = new wxButton(scroll_area_,
                                        wxID_ANY,
                                        collapse_icon,
                                        wxDefaultPosition,
                                        wxSize(20, 20),
                                        wxBORDER_NONE);
        toggle_btn->Bind(wxEVT_BUTTON,
                         [this, cat = category](wxCommandEvent& /*evt*/)
                         { OnCollapsibleToggle(cat); });
        header_sizer->Add(toggle_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);

        // R21 Fix 30: Category emoji icon prefix
        wxString icon_prefix;
        if (category == "Editor")
            icon_prefix = wxString::FromUTF8("\xF0\x9F\x8E\xA8 "); // 🎨
        else if (category == "Appearance")
            icon_prefix = wxString::FromUTF8("\xF0\x9F\x96\xA5 "); // 🖥
        else if (category == "Keybindings")
            icon_prefix = wxString::FromUTF8("\xE2\x8C\xA8 "); // ⌨
        else if (category == "Plugins")
            icon_prefix = wxString::FromUTF8("\xF0\x9F\x94\x8C "); // 🔌
        else if (category == "Advanced")
            icon_prefix = wxString::FromUTF8("\xE2\x9A\x99 "); // ⚙
        else if (category == "Features")
            icon_prefix = wxString::FromUTF8("\xF0\x9F\xA7\xA9 "); // 🧩

        // Count badge – "Editor (12 settings, 3 modified)"
        wxString badge_text = icon_prefix + category;
        badge_text += wxString::Format(" (%d settings", matching_count);
        if (modified_count > 0)
            badge_text += wxString::Format(", %d modified", modified_count);
        badge_text += ")";

        auto* header = new wxStaticText(scroll_area_, wxID_ANY, badge_text);
        auto header_font = header->GetFont();
        header_font.SetPointSize(14);
        header_font.SetWeight(wxFONTWEIGHT_BOLD);
        header->SetFont(header_font);
        header_sizer->Add(header, 1, wxALIGN_CENTER_VERTICAL);

        settings_sizer_->Add(header_sizer, 0, wxLEFT | wxTOP, kCategoryPadding);

        auto* separator = new wxStaticLine(scroll_area_, wxID_ANY);
        settings_sizer_->Add(separator, 0, wxEXPAND | wxALL, 4);

        // If collapsed, skip rendering individual widgets
        if (is_collapsed)
        {
            settings_sizer_->AddSpacer(4);
            continue;
        }

        auto* category_sizer = new wxBoxSizer(wxVERTICAL);
        settings_sizer_->Add(category_sizer, 0, wxEXPAND);

        for (const auto& def : definitions_)
        {
            if (def.category != category)
                continue;
            if (!active_category_.empty() && def.category != active_category_)
                continue;
            if (!filter.empty())
            {
                std::string lower_label = def.label;
                std::string lower_desc = def.description;
                std::string lower_filter = filter;
                std::transform(
                    lower_label.begin(), lower_label.end(), lower_label.begin(), ::tolower);
                std::transform(lower_desc.begin(), lower_desc.end(), lower_desc.begin(), ::tolower);
                std::transform(
                    lower_filter.begin(), lower_filter.end(), lower_filter.begin(), ::tolower);
                if (lower_label.find(lower_filter) == std::string::npos &&
                    lower_desc.find(lower_filter) == std::string::npos)
                    continue;
            }
            // Batch 5C Task 10: Modified-only filter
            if (show_modified_only_ != nullptr && show_modified_only_->IsChecked())
            {
                if (!IsSettingModified(def))
                {
                    continue;
                }
            }

            // Create widget based on type
            wxPanel* widget = nullptr;
            switch (def.type)
            {
                case core::SettingType::Boolean:
                    widget = CreateBooleanSetting(scroll_area_, def);
                    break;
                case core::SettingType::Integer:
                    widget = CreateIntegerSetting(scroll_area_, def);
                    break;
                case core::SettingType::String:
                case core::SettingType::Double:
                    widget = CreateStringSetting(scroll_area_, def);
                    break;
                case core::SettingType::Choice:
                    widget = CreateChoiceSetting(scroll_area_, def);
                    break;
                case core::SettingType::KeyBinding:
                    widget = CreateKeyBindingSetting(scroll_area_, def);
                    break;
                case core::SettingType::StringList:
                    widget = CreateStringListSetting(scroll_area_, def);
                    break;
                case core::SettingType::FilePath:
                    widget = CreateStringSetting(scroll_area_, def);
                    break;
                case core::SettingType::Color:
                    widget = CreateColorSetting(scroll_area_, def);
                    break;
            }

            if (widget != nullptr)
            {
                // Batch 5D Task 16: Accessibility labels
                widget->SetName(def.label + " setting");
                widget->SetHelpText(def.description);

                // R21 Fix 29: Modified indicator dot
                if (IsSettingModified(def))
                {
                    auto* dot = new wxStaticText(scroll_area_,
                                                 wxID_ANY,
                                                 wxString::FromUTF8("\xE2\x97\x8F")); // ●
                    auto accent = theme_engine_.color(core::ThemeColorToken::AccentPrimary);
                    dot->SetForegroundColour(accent);
                    dot->SetToolTip("Modified from default");
                    auto* row_with_dot = new wxBoxSizer(wxHORIZONTAL);
                    row_with_dot->Add(dot, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
                    row_with_dot->Add(widget, 1, wxEXPAND);
                    category_sizer->Add(
                        row_with_dot, 0, wxEXPAND | wxLEFT | wxRIGHT, kCategoryPadding);
                }
                else
                {
                    category_sizer->Add(widget, 0, wxEXPAND | wxLEFT | wxRIGHT, kCategoryPadding);
                }

                // 6. Zebra striping
                bool is_even = (setting_widgets_.size() % 2 == 0);
                auto base_bg = theme_engine_.current_theme().colors.editor_bg;
                auto row_bg = is_even ? base_bg : base_bg.lighten(0.03F);
                widget->SetBackgroundColour(row_bg.to_wx_colour());

                setting_widgets_.push_back(widget);
            }
        }

        settings_sizer_->AddSpacer(8);
    }

    scroll_area_->FitInside();
    Layout();
}

auto SettingsPanel::CreateBooleanSetting(wxWindow* parent, const SettingDefinition& def) -> wxPanel*
{
    auto* row = new wxPanel(parent, wxID_ANY);
    auto* row_sizer = new wxBoxSizer(wxHORIZONTAL);

    auto* label_sizer = new wxBoxSizer(wxVERTICAL);
    auto* label = new wxStaticText(row, wxID_ANY, def.label);
    auto label_font = label->GetFont();
    label_font.SetWeight(wxFONTWEIGHT_BOLD);
    label->SetFont(label_font);
    label_sizer->Add(label, 0);

    auto* desc = new wxStaticText(row, wxID_ANY, def.description);
    auto desc_font = desc->GetFont();
    desc_font.SetPointSize(desc_font.GetPointSize() - 1);
    desc->SetFont(desc_font);
    label_sizer->Add(desc, 0, wxTOP, 2);
    row_sizer->Add(label_sizer, 1, wxALIGN_CENTER_VERTICAL);

    bool current_val = config_.get_bool(def.setting_id, def.default_value == "true");
    auto* checkbox = new wxCheckBox(row, wxID_ANY, wxEmptyString);
    checkbox->SetValue(current_val);
    row_sizer->Add(checkbox, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 8);

    auto* reset_btn = CreateResetButton(row, def);
    row_sizer->Add(reset_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 4);

    checkbox->Bind(wxEVT_CHECKBOX,
                   [this, setting_id = def.setting_id](wxCommandEvent& evt)
                   { OnSettingChanged(setting_id, evt.IsChecked() ? "true" : "false"); });

    label->SetToolTip(def.description);
    row->SetSizer(row_sizer);
    return row;
}

auto SettingsPanel::CreateIntegerSetting(wxWindow* parent, const SettingDefinition& def) -> wxPanel*
{
    auto* row = new wxPanel(parent, wxID_ANY);
    auto* row_sizer = new wxBoxSizer(wxHORIZONTAL);

    auto* label_sizer = new wxBoxSizer(wxVERTICAL);
    auto* label = new wxStaticText(row, wxID_ANY, def.label);
    auto label_font = label->GetFont();
    label_font.SetWeight(wxFONTWEIGHT_BOLD);
    label_font.SetPointSize(label_font.GetPointSize() + 1);
    label->SetFont(label_font);
    label_sizer->Add(label, 0);

    auto* desc = new wxStaticText(row, wxID_ANY, def.description);
    auto desc_font = desc->GetFont();
    desc_font.SetPointSize(desc_font.GetPointSize() - 1);
    desc->SetFont(desc_font);
    label_sizer->Add(desc, 0, wxTOP, 2);
    row_sizer->Add(label_sizer, 1, wxALIGN_CENTER_VERTICAL);

    int default_int = 0;
    try
    {
        default_int = std::stoi(def.default_value);
    }
    catch (const std::exception& e)
    {
        // R20 Fix 4: Typed catch — log malformed default values
        MARKAMP_LOG_WARN("Invalid integer default '{}' for setting '{}': {}",
                         def.default_value,
                         def.setting_id,
                         e.what());
    }
    int current_val = config_.get_int(def.setting_id, default_int);
    auto* spin = new wxSpinCtrl(row,
                                wxID_ANY,
                                wxEmptyString,
                                wxDefaultPosition,
                                wxSize(80, -1),
                                wxSP_ARROW_KEYS,
                                def.min_int,
                                def.max_int,
                                current_val);
    row_sizer->Add(spin, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 8);

    auto* reset_btn = CreateResetButton(row, def);
    row_sizer->Add(reset_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 4);

    spin->Bind(wxEVT_SPINCTRL,
               [this, setting_id = def.setting_id](wxSpinEvent& evt)
               { OnSettingChanged(setting_id, std::to_string(evt.GetPosition())); });

    label->SetToolTip(def.description);
    row->SetSizer(row_sizer);
    return row;
}

auto SettingsPanel::CreateStringSetting(wxWindow* parent, const SettingDefinition& def) -> wxPanel*
{
    auto* row = new wxPanel(parent, wxID_ANY);
    auto* row_sizer = new wxBoxSizer(wxHORIZONTAL);

    auto* label_sizer = new wxBoxSizer(wxVERTICAL);
    auto* label = new wxStaticText(row, wxID_ANY, def.label);
    auto label_font = label->GetFont();
    label_font.SetWeight(wxFONTWEIGHT_BOLD);
    label_font.SetPointSize(label_font.GetPointSize() + 1);
    label->SetFont(label_font);
    label_sizer->Add(label, 0);

    auto* desc = new wxStaticText(row, wxID_ANY, def.description);
    auto desc_font = desc->GetFont();
    desc_font.SetPointSize(desc_font.GetPointSize() - 1);
    desc->SetFont(desc_font);
    label_sizer->Add(desc, 0, wxTOP, 2);
    row_sizer->Add(label_sizer, 1, wxALIGN_CENTER_VERTICAL);

    auto current_val = config_.get_string(def.setting_id, def.default_value);
    auto* text = new wxTextCtrl(row, wxID_ANY, current_val, wxDefaultPosition, wxSize(200, -1));
    row_sizer->Add(text, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 8);

    auto* reset_btn = CreateResetButton(row, def);
    row_sizer->Add(reset_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 4);

    text->Bind(wxEVT_TEXT,
               [this, setting_id = def.setting_id](wxCommandEvent& evt)
               { OnSettingChanged(setting_id, evt.GetString().ToStdString()); });

    label->SetToolTip(def.description);
    row->SetSizer(row_sizer);
    return row;
}

auto SettingsPanel::CreateChoiceSetting(wxWindow* parent, const SettingDefinition& def) -> wxPanel*
{
    auto* row = new wxPanel(parent, wxID_ANY);
    auto* row_sizer = new wxBoxSizer(wxHORIZONTAL);

    auto* label_sizer = new wxBoxSizer(wxVERTICAL);
    auto* label = new wxStaticText(row, wxID_ANY, def.label);
    auto label_font = label->GetFont();
    label_font.SetWeight(wxFONTWEIGHT_BOLD);
    label_font.SetPointSize(label_font.GetPointSize() + 1);
    label->SetFont(label_font);
    label_sizer->Add(label, 0);

    auto* desc = new wxStaticText(row, wxID_ANY, def.description);
    auto desc_font = desc->GetFont();
    desc_font.SetPointSize(desc_font.GetPointSize() - 1);
    desc->SetFont(desc_font);
    label_sizer->Add(desc, 0, wxTOP, 2);
    row_sizer->Add(label_sizer, 1, wxALIGN_CENTER_VERTICAL);

    wxArrayString choices_arr;
    for (const auto& choice : def.choices)
    {
        choices_arr.Add(choice);
    }

    auto current_val = config_.get_string(def.setting_id, def.default_value);
    auto* choice = new wxChoice(row, wxID_ANY, wxDefaultPosition, wxSize(200, -1), choices_arr);
    choice->SetStringSelection(current_val);
    row_sizer->Add(choice, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 8);

    auto* reset_btn = CreateResetButton(row, def);
    row_sizer->Add(reset_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 4);

    choice->Bind(wxEVT_CHOICE,
                 [this, setting_id = def.setting_id](wxCommandEvent& evt)
                 { OnSettingChanged(setting_id, evt.GetString().ToStdString()); });

    label->SetToolTip(def.description);
    row->SetSizer(row_sizer);
    return row;
}

void SettingsPanel::OnSearchChanged(wxCommandEvent& /*event*/)
{
    // Batch 5E Task 19: Debounce search — restart 300ms one-shot timer.
    // The timer fires RebuildSettingsList() only after the user stops typing.
    search_debounce_timer_.Stop();
    search_debounce_timer_.StartOnce(kSearchDebounceMs);
}

// Batch 5A: Handle tree selection for hierarchical category navigation
void SettingsPanel::OnTreeSelectionChanged(wxTreeEvent& /*event*/)
{
    if (category_tree_ == nullptr)
        return;
    auto sel = category_tree_->GetSelection();
    if (!sel.IsOk() || sel == category_tree_->GetRootItem())
    {
        active_category_.clear();
        active_subgroup_.clear();
    }
    else
    {
        auto parent_item = category_tree_->GetItemParent(sel);
        if (parent_item == category_tree_->GetRootItem())
        {
            // Top-level group selected
            active_category_ = category_tree_->GetItemText(sel).ToStdString();
            active_subgroup_.clear();
        }
        else
        {
            // Subgroup selected
            active_category_ = category_tree_->GetItemText(parent_item).ToStdString();
            active_subgroup_ = category_tree_->GetItemText(sel).ToStdString();
        }
    }
    RebuildSettingsList();
}

// Batch 5A: Handle scope tab changes (User / Workspace / Project)
void SettingsPanel::OnScopeChanged(wxBookCtrlEvent& /*event*/)
{
    if (scope_tabs_ == nullptr)
        return;
    switch (scope_tabs_->GetSelection())
    {
        case 0:
            active_scope_ = core::ConfigScope::kApplication;
            break;
        case 1:
            active_scope_ = core::ConfigScope::kWorkspace;
            break;
        case 2:
            active_scope_ = core::ConfigScope::kProject;
            break;
        default:
            active_scope_ = core::ConfigScope::kApplication;
            break;
    }
    RebuildSettingsList();
}

void SettingsPanel::OnSettingChanged(const std::string& setting_id, const std::string& new_value)
{
    // Batch 5A: Capture old value for undo before writing
    auto old_value = config_.get_string(setting_id);
    undo_stack_.push_back({setting_id, old_value, new_value});
    redo_stack_.clear(); // Clear redo on new change

    config_.set(setting_id, new_value);
    (void)config_.save(); // best-effort persist

    // Fire event so other components can react
    core::events::SettingChangedEvent evt(setting_id, new_value);
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Setting changed: {} = {}", setting_id, new_value);
}

void SettingsPanel::ApplyTheme()
{
    const auto& theme = theme_engine_.current_theme();
    const auto& clr = theme.colors;
    SetBackgroundColour(clr.editor_bg.to_wx_colour());
    SetForegroundColour(clr.editor_fg.to_wx_colour());

    if (search_ctrl_ != nullptr)
    {
        search_ctrl_->SetBackgroundColour(clr.bg_input.to_wx_colour());
        search_ctrl_->SetForegroundColour(clr.editor_fg.to_wx_colour());
    }

    // Batch 5A: Theme the category tree instead of list
    if (category_tree_ != nullptr)
    {
        category_tree_->SetBackgroundColour(clr.bg_panel.to_wx_colour());
        category_tree_->SetForegroundColour(clr.editor_fg.to_wx_colour());
    }

    if (scope_tabs_ != nullptr)
    {
        scope_tabs_->SetBackgroundColour(clr.bg_panel.to_wx_colour());
        scope_tabs_->SetForegroundColour(clr.editor_fg.to_wx_colour());
    }

    Refresh();
}

void SettingsPanel::RefreshValues()
{
    RebuildSettingsList();
}

void SettingsPanel::ResetSettingToDefault(const std::string& setting_id,
                                          const std::string& default_val)
{
    config_.set(setting_id, default_val);
    (void)config_.save(); // best-effort persist

    core::events::SettingChangedEvent evt(setting_id, default_val);
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Setting reset to default: {} = {}", setting_id, default_val);
    RebuildSettingsList();
}

auto SettingsPanel::CreateResetButton(wxWindow* parent, const SettingDefinition& def) -> wxButton*
{
    auto* btn = new wxButton(parent, wxID_ANY, "↩", wxDefaultPosition, wxSize(28, 28));
    btn->SetToolTip("Reset to default (" + def.default_value + ")");
    btn->Bind(wxEVT_BUTTON,
              [this, setting_id = def.setting_id, default_val = def.default_value](
                  wxCommandEvent& /*evt*/) { ResetSettingToDefault(setting_id, default_val); });
    return btn;
}

// Phase 9: Export settings to a JSON-like file
void SettingsPanel::ExportSettings()
{
    wxFileDialog save_dlg(this,
                          "Export Settings",
                          wxEmptyString,
                          "markamp-settings.json",
                          "JSON files (*.json)|*.json",
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (save_dlg.ShowModal() == wxID_CANCEL)
        return;

    std::ofstream out(save_dlg.GetPath().ToStdString());
    if (!out.is_open())
        return;

    out << "{\n";
    bool first = true;
    for (const auto& def : definitions_)
    {
        auto current_val = config_.get_string(def.setting_id, def.default_value);
        if (!first)
            out << ",\n";
        first = false;
        out << "  \"" << def.setting_id << "\": \"" << current_val << "\"";
    }
    out << "\n}\n";
    // R20 Fix 14: Check write success before closing
    if (!out.good())
    {
        MARKAMP_LOG_WARN("Settings export may have failed (write error): {}",
                         save_dlg.GetPath().ToStdString());
    }
    out.close();

    MARKAMP_LOG_DEBUG("Settings exported to: {}", save_dlg.GetPath().ToStdString());
}

// Phase 9: Import settings from a JSON-like file
void SettingsPanel::ImportSettings()
{
    wxFileDialog open_dlg(this,
                          "Import Settings",
                          wxEmptyString,
                          wxEmptyString,
                          "JSON files (*.json)|*.json",
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (open_dlg.ShowModal() == wxID_CANCEL)
        return;

    std::ifstream in(open_dlg.GetPath().ToStdString());
    if (!in.is_open())
        return;

    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    // Simple JSON parser: look for "key": "value" pairs
    size_t pos = 0;
    while (pos < content.size())
    {
        size_t key_start = content.find('"', pos);
        if (key_start == std::string::npos)
            break;
        size_t key_end = content.find('"', key_start + 1);
        if (key_end == std::string::npos)
            break;

        size_t val_start = content.find('"', key_end + 1);
        if (val_start == std::string::npos)
            break;
        size_t val_end = content.find('"', val_start + 1);
        if (val_end == std::string::npos)
            break;

        std::string key = content.substr(key_start + 1, key_end - key_start - 1);
        std::string value = content.substr(val_start + 1, val_end - val_start - 1);

        // R20 Fix 32: Cap key/value length to prevent malformed files from consuming memory
        constexpr size_t kMaxFieldLength = 1024;
        if (key.size() > kMaxFieldLength || value.size() > kMaxFieldLength)
        {
            pos = val_end + 1;
            continue;
        }

        config_.set(key, value);
        pos = val_end + 1;
    }

    (void)config_.save(); // best-effort persist
    RebuildSettingsList();

    MARKAMP_LOG_DEBUG("Settings imported from: {}", open_dlg.GetPath().ToStdString());
}

// Phase 9: Check if a setting differs from its default
auto SettingsPanel::IsSettingModified(const SettingDefinition& def) const -> bool
{
    auto current = config_.get_string(def.setting_id, def.default_value);
    return current != def.default_value;
}

// Phase 9: Toggle category collapsed/expanded state
void SettingsPanel::OnCollapsibleToggle(const std::string& category)
{
    if (collapsed_categories_.count(category) > 0)
    {
        collapsed_categories_.erase(category);
    }
    else
    {
        collapsed_categories_.insert(category);
    }
    RebuildSettingsList();
}

void SettingsPanel::SetSearchText(const std::string& query)
{
    if (search_ctrl_ != nullptr)
    {
        search_ctrl_->SetValue(query);
        RebuildSettingsList();
    }
}

void SettingsPanel::FocusSearch()
{
    if (search_ctrl_ != nullptr)
    {
        search_ctrl_->SetFocus();
    }
}

void SettingsPanel::RestoreAllDefaults()
{
    for (const auto& def : definitions_)
    {
        ResetSettingToDefault(def.setting_id, def.default_value);
    }
    RebuildSettingsList();
}

// ── Staged-edit API ──

void SettingsPanel::ApplyPendingChanges()
{
    for (const auto& [setting_id, new_value] : pending_changes_)
    {
        config_.set(setting_id, new_value);
    }
    if (!pending_changes_.empty())
    {
        event_bus_.publish(core::events::SettingsBatchChangedEvent{});
    }
    pending_changes_.clear();
    RebuildSettingsList();
}

void SettingsPanel::DiscardPendingChanges()
{
    pending_changes_.clear();
    RebuildSettingsList();
}

auto SettingsPanel::HasPendingChanges() const -> bool
{
    return !pending_changes_.empty();
}

auto SettingsPanel::PendingChangeCount() const -> std::size_t
{
    return pending_changes_.size();
}

// ── Batch 5A: Undo/Redo API ──

void SettingsPanel::UndoLastChange()
{
    if (undo_stack_.empty())
        return;
    auto change = undo_stack_.back();
    undo_stack_.pop_back();

    config_.set(change.setting_id, change.old_value);
    (void)config_.save();

    core::events::SettingChangedEvent evt(change.setting_id, change.old_value);
    event_bus_.publish(evt);

    redo_stack_.push_back(std::move(change));
    RebuildSettingsList();
}

void SettingsPanel::RedoLastChange()
{
    if (redo_stack_.empty())
        return;
    auto change = redo_stack_.back();
    redo_stack_.pop_back();

    config_.set(change.setting_id, change.new_value);
    (void)config_.save();

    core::events::SettingChangedEvent evt(change.setting_id, change.new_value);
    event_bus_.publish(evt);

    undo_stack_.push_back(std::move(change));
    RebuildSettingsList();
}

auto SettingsPanel::CanUndo() const -> bool
{
    return !undo_stack_.empty();
}

auto SettingsPanel::CanRedo() const -> bool
{
    return !redo_stack_.empty();
}

// ── Batch 5A: Fuzzy Search ──

auto SettingsPanel::FuzzyScore(const SettingDefinition& def, const std::string& query) -> double
{
    if (query.empty())
        return 1.0;

    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

    double score = 0.0;

    // Label match (weight 3x)
    std::string lower_label = def.label;
    std::transform(lower_label.begin(), lower_label.end(), lower_label.begin(), ::tolower);
    if (lower_label.find(lower_query) != std::string::npos)
        score += 3.0;
    if (lower_label == lower_query)
        score += 5.0; // Exact match bonus

    // Setting ID match (weight 2.5x)
    std::string lower_id = def.setting_id;
    std::transform(lower_id.begin(), lower_id.end(), lower_id.begin(), ::tolower);
    if (lower_id.find(lower_query) != std::string::npos)
        score += 2.5;

    // Tags/keywords match (weight 2x)
    for (const auto& tag : def.tags)
    {
        std::string lower_tag = tag;
        std::transform(lower_tag.begin(), lower_tag.end(), lower_tag.begin(), ::tolower);
        if (lower_tag.find(lower_query) != std::string::npos)
        {
            score += 2.0;
            break;
        }
    }

    // Description match (weight 1x)
    std::string lower_desc = def.description;
    std::transform(lower_desc.begin(), lower_desc.end(), lower_desc.begin(), ::tolower);
    if (lower_desc.find(lower_query) != std::string::npos)
        score += 1.0;

    // Category match (weight 0.5x)
    std::string lower_cat = def.category;
    std::transform(lower_cat.begin(), lower_cat.end(), lower_cat.begin(), ::tolower);
    if (lower_cat.find(lower_query) != std::string::npos)
        score += 0.5;

    return score;
}

// ── Query API ──

auto SettingsPanel::setting_count() const -> std::size_t
{
    return definitions_.size();
}

auto SettingsPanel::filtered_count() const -> std::size_t
{
    return filtered_count_;
}

auto SettingsPanel::modified_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& def : definitions_)
    {
        if (IsSettingModified(def))
        {
            ++count;
        }
    }
    return count;
}

auto SettingsPanel::GetSettingValue(const std::string& setting_id) const -> std::string
{
    return config_.get_string(setting_id, "");
}

void SettingsPanel::SetSettingValue(const std::string& setting_id, const std::string& value)
{
    OnSettingChanged(setting_id, value);
}

void SettingsPanel::ResetCategoryToDefaults(const std::string& category)
{
    for (const auto& def : definitions_)
    {
        if (def.category == category)
        {
            ResetSettingToDefault(def.setting_id, def.default_value);
        }
    }
    RebuildSettingsList();
}

auto SettingsPanel::CreateKeyBindingSetting(wxWindow* parent, const SettingDefinition& def)
    -> wxPanel*
{
    auto* panel = new wxPanel(parent);
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    auto* label = new wxStaticText(panel, wxID_ANY, def.label);
    auto label_font = label->GetFont();
    label_font.SetWeight(wxFONTWEIGHT_BOLD);
    label_font.SetPointSize(label_font.GetPointSize() + 1);
    label->SetFont(label_font);
    label->SetToolTip(def.description);
    sizer->Add(label, 1, wxALIGN_CENTER_VERTICAL);

    const std::string current_val = config_.get_string(def.setting_id, def.default_value);
    auto* text_ctrl =
        new wxTextCtrl(panel, wxID_ANY, current_val, wxDefaultPosition, wxSize(160, -1));
    text_ctrl->SetEditable(false);

    const std::string setting_id = def.setting_id;

    auto* record_btn =
        new wxButton(panel, wxID_ANY, "Record Shortcut", wxDefaultPosition, wxSize(120, -1));
    record_btn->SetToolTip("Press to capture a new keyboard shortcut");
    record_btn->Bind(wxEVT_BUTTON,
                     [this, text_ctrl, record_btn, setting_id](wxCommandEvent& /*evt*/)
                     {
                         record_btn->SetLabel("Press keys...");
                         record_btn->Bind(
                             wxEVT_KEY_DOWN,
                             [this, text_ctrl, record_btn, setting_id](wxKeyEvent& key_evt)
                             {
                                 wxString combo;
                                 if (key_evt.ControlDown())
                                     combo += "Ctrl+";
                                 if (key_evt.AltDown())
                                     combo += "Alt+";
                                 if (key_evt.ShiftDown())
                                     combo += "Shift+";
                                 if (key_evt.MetaDown())
                                     combo += "Cmd+";

                                 const int kc = key_evt.GetKeyCode();
                                 if (kc >= 'A' && kc <= 'Z')
                                 {
                                     combo += static_cast<char>(kc);
                                 }
                                 else if (kc >= WXK_F1 && kc <= WXK_F12)
                                 {
                                     combo += wxString::Format("F%d", kc - WXK_F1 + 1);
                                 }
                                 else
                                 {
                                     combo += wxString::Format("0x%X", kc);
                                 }

                                 text_ctrl->SetValue(combo);
                                 OnSettingChanged(setting_id, combo.ToStdString());
                                 record_btn->SetLabel("Record Shortcut");
                             });
                     });

    sizer->Add(text_ctrl, 0, wxALIGN_CENTER_VERTICAL);
    sizer->Add(record_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 4);
    sizer->Add(CreateResetButton(panel, def), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 4);
    panel->SetSizer(sizer);
    return panel;
}

auto SettingsPanel::CreateStringListSetting(wxWindow* parent, const SettingDefinition& def)
    -> wxPanel*
{
    auto* panel = new wxPanel(parent);
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    auto* label = new wxStaticText(panel, wxID_ANY, def.label);
    auto label_font = label->GetFont();
    label_font.SetWeight(wxFONTWEIGHT_BOLD);
    label_font.SetPointSize(label_font.GetPointSize() + 1);
    label->SetFont(label_font);
    label->SetToolTip(def.description);
    sizer->Add(label, 1, wxALIGN_TOP | wxTOP, 4);

    // Display comma-separated values as newline-separated in a multi-line text area
    const std::string current_val = config_.get_string(def.setting_id, def.default_value);
    std::string display_val = current_val;
    std::replace(display_val.begin(), display_val.end(), ',', '\n');

    auto* text_ctrl = new wxTextCtrl(
        panel, wxID_ANY, display_val, wxDefaultPosition, wxSize(200, 80), wxTE_MULTILINE);

    const std::string setting_id = def.setting_id;
    text_ctrl->Bind(wxEVT_TEXT,
                    [this, setting_id](wxCommandEvent& evt)
                    {
                        // Convert newlines back to commas for storage
                        std::string val = evt.GetString().ToStdString();
                        std::replace(val.begin(), val.end(), '\n', ',');
                        OnSettingChanged(setting_id, val);
                    });

    // Batch 5B Task 5: Move-up / Move-down buttons for reordering items
    auto* btn_sizer = new wxBoxSizer(wxVERTICAL);

    auto* move_up_btn = new wxButton(
        panel, wxID_ANY, wxString::FromUTF8("\xE2\x96\xB2"), wxDefaultPosition, wxSize(28, 28));
    move_up_btn->SetToolTip("Move selected item up");
    move_up_btn->Bind(wxEVT_BUTTON,
                      [text_ctrl](wxCommandEvent& /*evt*/)
                      {
                          long from = 0;
                          long to_pos = 0;
                          text_ctrl->GetSelection(&from, &to_pos);
                          const wxString content = text_ctrl->GetValue();
                          wxArrayString lines = wxSplit(content, '\n');
                          // Find which line the cursor is on
                          long cursor_line = 0;
                          long accum = 0;
                          for (size_t idx = 0; idx < lines.GetCount(); ++idx)
                          {
                              accum += static_cast<long>(lines[idx].length()) + 1;
                              if (accum > from)
                              {
                                  cursor_line = static_cast<long>(idx);
                                  break;
                              }
                          }
                          if (cursor_line > 0)
                          {
                              wxString tmp = lines[static_cast<size_t>(cursor_line)];
                              lines[static_cast<size_t>(cursor_line)] =
                                  lines[static_cast<size_t>(cursor_line - 1)];
                              lines[static_cast<size_t>(cursor_line - 1)] = tmp;
                              text_ctrl->SetValue(wxJoin(lines, '\n'));
                          }
                      });
    btn_sizer->Add(move_up_btn, 0, wxBOTTOM, 2);

    auto* move_down_btn = new wxButton(
        panel, wxID_ANY, wxString::FromUTF8("\xE2\x96\xBC"), wxDefaultPosition, wxSize(28, 28));
    move_down_btn->SetToolTip("Move selected item down");
    move_down_btn->Bind(wxEVT_BUTTON,
                        [text_ctrl](wxCommandEvent& /*evt*/)
                        {
                            long from = 0;
                            long to_pos = 0;
                            text_ctrl->GetSelection(&from, &to_pos);
                            const wxString content = text_ctrl->GetValue();
                            wxArrayString lines = wxSplit(content, '\n');
                            long cursor_line = 0;
                            long accum = 0;
                            for (size_t idx = 0; idx < lines.GetCount(); ++idx)
                            {
                                accum += static_cast<long>(lines[idx].length()) + 1;
                                if (accum > from)
                                {
                                    cursor_line = static_cast<long>(idx);
                                    break;
                                }
                            }
                            if (cursor_line < static_cast<long>(lines.GetCount()) - 1)
                            {
                                wxString tmp = lines[static_cast<size_t>(cursor_line)];
                                lines[static_cast<size_t>(cursor_line)] =
                                    lines[static_cast<size_t>(cursor_line + 1)];
                                lines[static_cast<size_t>(cursor_line + 1)] = tmp;
                                text_ctrl->SetValue(wxJoin(lines, '\n'));
                            }
                        });
    btn_sizer->Add(move_down_btn, 0);

    sizer->Add(text_ctrl, 0, wxALIGN_TOP);
    sizer->Add(btn_sizer, 0, wxALIGN_TOP | wxLEFT, 2);
    sizer->Add(CreateResetButton(panel, def), 0, wxALIGN_TOP | wxLEFT, 4);
    panel->SetSizer(sizer);
    return panel;
}

// ── Batch 5B Task 5: Color picker setting renderer ──

auto SettingsPanel::CreateColorSetting(wxWindow* parent, const SettingDefinition& def) -> wxPanel*
{
    auto* panel = new wxPanel(parent);
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    auto* label_sizer = new wxBoxSizer(wxVERTICAL);
    auto* label = new wxStaticText(panel, wxID_ANY, def.label);
    auto label_font = label->GetFont();
    label_font.SetWeight(wxFONTWEIGHT_BOLD);
    label_font.SetPointSize(label_font.GetPointSize() + 1);
    label->SetFont(label_font);
    label_sizer->Add(label, 0);

    auto* desc = new wxStaticText(panel, wxID_ANY, def.description);
    auto desc_font = desc->GetFont();
    desc_font.SetPointSize(desc_font.GetPointSize() - 1);
    desc->SetFont(desc_font);
    label_sizer->Add(desc, 0, wxTOP, 2);
    sizer->Add(label_sizer, 1, wxALIGN_CENTER_VERTICAL);

    const std::string current_val = config_.get_string(def.setting_id, def.default_value);

    // Color swatch preview: a small panel that shows the current color
    auto* swatch = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(32, 24));
    wxColour initial_colour(current_val);
    if (!initial_colour.IsOk())
    {
        initial_colour = *wxWHITE;
    }
    swatch->SetBackgroundColour(initial_colour);
    sizer->Add(swatch, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);

    // Hex text display
    auto* hex_text =
        new wxTextCtrl(panel, wxID_ANY, current_val, wxDefaultPosition, wxSize(80, -1));

    const std::string setting_id = def.setting_id;
    hex_text->Bind(wxEVT_TEXT,
                   [this, swatch, setting_id](wxCommandEvent& evt)
                   {
                       const std::string hex_val = evt.GetString().ToStdString();
                       wxColour new_colour(hex_val);
                       if (new_colour.IsOk())
                       {
                           swatch->SetBackgroundColour(new_colour);
                           swatch->Refresh();
                       }
                       OnSettingChanged(setting_id, hex_val);
                   });
    sizer->Add(hex_text, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);

    // "Pick…" button that opens wxColourDialog
    auto* pick_btn = new wxButton(
        panel, wxID_ANY, wxString::FromUTF8("Pick\xE2\x80\xA6"), wxDefaultPosition, wxSize(60, -1));
    pick_btn->SetToolTip("Open color picker dialog");
    pick_btn->Bind(wxEVT_BUTTON,
                   [this, panel, swatch, hex_text, setting_id](wxCommandEvent& /*evt*/)
                   {
                       wxColourData colour_data;
                       colour_data.SetColour(swatch->GetBackgroundColour());
                       colour_data.SetChooseFull(true);

                       wxColourDialog dlg(panel, &colour_data);
                       if (dlg.ShowModal() == wxID_OK)
                       {
                           const wxColour chosen = dlg.GetColourData().GetColour();
                           const wxString hex = chosen.GetAsString(wxC2S_HTML_SYNTAX);
                           swatch->SetBackgroundColour(chosen);
                           swatch->Refresh();
                           hex_text->ChangeValue(hex);
                           OnSettingChanged(setting_id, hex.ToStdString());
                       }
                   });
    sizer->Add(pick_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    sizer->Add(CreateResetButton(panel, def), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 4);

    panel->SetSizer(sizer);
    return panel;
}

} // namespace markamp::ui
