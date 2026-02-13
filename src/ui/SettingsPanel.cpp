#include "SettingsPanel.h"

#include "core/Config.h"
#include "core/Events.h"
#include "core/Logger.h"

#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>

#include <fstream>
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
    ApplyTheme();

    // Subscribe to theme changes
    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });
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

    // Search bar
    search_ctrl_ =
        new wxSearchCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 32));
    search_ctrl_->SetDescriptiveText("Search settings...");
    main_sizer->Add(search_ctrl_, 0, wxEXPAND | wxALL, 12);

    search_ctrl_->Bind(wxEVT_TEXT, &SettingsPanel::OnSearchChanged, this);

    // Horizontal splitter: category list + scroll area
    auto* content_sizer = new wxBoxSizer(wxHORIZONTAL);

    // Category sidebar
    wxArrayString cat_items;
    cat_items.Add("All");
    cat_items.Add("Editor");
    cat_items.Add("Appearance");
    cat_items.Add("Advanced");
    category_list_ =
        new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(120, -1), cat_items, wxLB_SINGLE);
    category_list_->SetSelection(0);
    category_list_->Bind(wxEVT_LISTBOX, &SettingsPanel::OnCategorySelected, this);
    content_sizer->Add(category_list_, 0, wxEXPAND | wxRIGHT, 8);

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
    reset_all_btn->SetBackgroundColour(wxColour(200, 60, 60));
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

    main_sizer->Add(toolbar_sizer, 0, wxALL, 12);

    SetSizer(main_sizer);
}

void SettingsPanel::RegisterSetting(SettingDefinition definition)
{
    definitions_.push_back(std::move(definition));
}

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

    RebuildSettingsList();
}

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
        "Editor", "Appearance", "Keybindings", "Plugins", "Advanced"};
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

        // Count badge – "Editor (12 settings, 3 modified)"
        wxString badge_text = category;
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
            }

            if (widget != nullptr)
            {
                category_sizer->Add(widget, 0, wxEXPAND | wxLEFT | wxRIGHT, kCategoryPadding);
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
    RebuildSettingsList();
}

void SettingsPanel::OnCategorySelected(wxCommandEvent& /*event*/)
{
    if (category_list_ == nullptr)
        return;
    int sel = category_list_->GetSelection();
    if (sel == wxNOT_FOUND || sel == 0)
    {
        active_category_.clear(); // "All"
    }
    else
    {
        active_category_ = category_list_->GetString(static_cast<unsigned int>(sel)).ToStdString();
    }
    RebuildSettingsList();
}

void SettingsPanel::OnSettingChanged(const std::string& setting_id, const std::string& new_value)
{
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

    if (category_list_ != nullptr)
    {
        category_list_->SetBackgroundColour(clr.bg_panel.to_wx_colour());
        category_list_->SetForegroundColour(clr.editor_fg.to_wx_colour());
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

} // namespace markamp::ui
