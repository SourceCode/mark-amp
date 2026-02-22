#include "ShortcutEditor.h"

#include "core/Logger.h"

#include <wx/button.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>

#include <algorithm>

namespace markamp::ui
{

ShortcutEditor::ShortcutEditor(wxWindow* parent,
                               core::ThemeEngine& theme_engine,
                               core::EventBus& event_bus,
                               core::ShortcutManager& shortcut_manager)
    : wxDialog(parent,
               wxID_ANY,
               "Keyboard Shortcuts",
               wxDefaultPosition,
               wxSize(700, 500),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , shortcut_manager_(shortcut_manager)
{
    CreateLayout();
    ApplyTheme();
    ReloadShortcuts();
}

void ShortcutEditor::CreateLayout()
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Title
    auto* title = new wxStaticText(this, wxID_ANY, "Keyboard Shortcuts");
    auto title_font = title->GetFont();
    title_font.SetPointSize(14);
    title_font.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(title_font);
    main_sizer->Add(title, 0, wxALL, 12);

    // Search bar
    search_ctrl_ =
        new wxSearchCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 28));
    search_ctrl_->SetDescriptiveText("Search shortcuts by command or key...");
    search_ctrl_->Bind(wxEVT_TEXT, &ShortcutEditor::OnSearchChanged, this);
    main_sizer->Add(search_ctrl_, 0, wxEXPAND | wxLEFT | wxRIGHT, 12);

    // Shortcut list (report view — table-style)
    shortcut_list_ = new wxListCtrl(
        this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    shortcut_list_->InsertColumn(0, "Command", wxLIST_FORMAT_LEFT, 200);
    shortcut_list_->InsertColumn(1, "Description", wxLIST_FORMAT_LEFT, 200);
    shortcut_list_->InsertColumn(2, "Keybinding", wxLIST_FORMAT_LEFT, 140);
    shortcut_list_->InsertColumn(3, "Context", wxLIST_FORMAT_LEFT, 100);
    shortcut_list_->Bind(wxEVT_LIST_ITEM_ACTIVATED, &ShortcutEditor::OnItemActivated, this);
    main_sizer->Add(shortcut_list_, 1, wxEXPAND | wxALL, 12);

    // Status label
    status_label_ = new wxStaticText(this, wxID_ANY, "Double-click a shortcut to rebind it.");
    main_sizer->Add(status_label_, 0, wxLEFT | wxRIGHT, 12);

    // Button row
    auto* btn_sizer = new wxBoxSizer(wxHORIZONTAL);

    auto* reset_btn =
        new wxButton(this, wxID_ANY, "Reset All to Defaults", wxDefaultPosition, wxSize(170, 32));
    reset_btn->SetToolTip("Restore all shortcuts to their default bindings");
    reset_btn->Bind(wxEVT_BUTTON, &ShortcutEditor::OnResetAll, this);
    btn_sizer->Add(reset_btn, 0, wxRIGHT, 8);

    btn_sizer->AddStretchSpacer(1);

    auto* close_btn = new wxButton(this, wxID_CANCEL, "Close", wxDefaultPosition, wxSize(90, 32));
    btn_sizer->Add(close_btn, 0);

    main_sizer->Add(btn_sizer, 0, wxEXPAND | wxALL, 12);

    SetSizer(main_sizer);
}

void ShortcutEditor::ReloadShortcuts()
{
    all_shortcuts_ = shortcut_manager_.get_all_shortcuts();
    PopulateList();
}

void ShortcutEditor::FilterByQuery(const std::string& query)
{
    PopulateList(query);
}

void ShortcutEditor::PopulateList(const std::string& filter)
{
    if (shortcut_list_ == nullptr)
    {
        return;
    }
    shortcut_list_->DeleteAllItems();

    std::string lower_filter = filter;
    std::transform(lower_filter.begin(), lower_filter.end(), lower_filter.begin(), ::tolower);

    int visible_count = 0;
    for (const auto& shortcut : all_shortcuts_)
    {
        if (!lower_filter.empty())
        {
            std::string lower_id = shortcut.id;
            std::string lower_desc = shortcut.description;
            std::string lower_key =
                core::ShortcutManager::format_shortcut(shortcut.key_code, shortcut.modifiers);

            std::transform(lower_id.begin(), lower_id.end(), lower_id.begin(), ::tolower);
            std::transform(lower_desc.begin(), lower_desc.end(), lower_desc.begin(), ::tolower);
            std::transform(lower_key.begin(), lower_key.end(), lower_key.begin(), ::tolower);

            if (lower_id.find(lower_filter) == std::string::npos &&
                lower_desc.find(lower_filter) == std::string::npos &&
                lower_key.find(lower_filter) == std::string::npos)
            {
                continue;
            }
        }

        const long idx = shortcut_list_->InsertItem(visible_count, shortcut.id);
        shortcut_list_->SetItem(idx, 1, shortcut.description);
        shortcut_list_->SetItem(
            idx, 2, core::ShortcutManager::format_shortcut(shortcut.key_code, shortcut.modifiers));
        shortcut_list_->SetItem(idx, 3, shortcut.context);

        // Store the index into all_shortcuts_ for later retrieval
        shortcut_list_->SetItemData(idx, static_cast<long>(&shortcut - all_shortcuts_.data()));

        ++visible_count;
    }

    if (status_label_ != nullptr)
    {
        status_label_->SetLabel(
            wxString::Format("Showing %d of %zu shortcuts. Double-click to rebind.",
                             visible_count,
                             all_shortcuts_.size()));
    }
}

void ShortcutEditor::OnSearchChanged(wxCommandEvent& /*event*/)
{
    if (search_ctrl_ != nullptr)
    {
        PopulateList(search_ctrl_->GetValue().ToStdString());
    }
}

void ShortcutEditor::OnItemActivated(wxListEvent& event)
{
    RecordNewBinding(event.GetIndex());
}

void ShortcutEditor::OnResetAll(wxCommandEvent& /*event*/)
{
    const int result = wxMessageBox(
        "Are you sure you want to reset all keyboard shortcuts to their default bindings?",
        "Reset Shortcuts",
        wxYES_NO | wxICON_QUESTION,
        this);

    if (result == wxYES)
    {
        shortcut_manager_.reset_to_defaults();
        ReloadShortcuts();
        MARKAMP_LOG_INFO("Reset all keyboard shortcuts to defaults");
    }
}

void ShortcutEditor::RecordNewBinding(long item_index)
{
    if (shortcut_list_ == nullptr || item_index < 0)
    {
        return;
    }

    const auto data_index = static_cast<ptrdiff_t>(shortcut_list_->GetItemData(item_index));
    if (data_index < 0 || static_cast<size_t>(data_index) >= all_shortcuts_.size())
    {
        return;
    }

    const std::string target_id = all_shortcuts_[static_cast<size_t>(data_index)].id;

    // Update status to indicate recording mode
    if (status_label_ != nullptr)
    {
        status_label_->SetLabel("Press key combo for \"" + target_id + "\"… (Esc to cancel)");
    }

    // Temporarily highlight the row to indicate focus/recording
    shortcut_list_->SetItemBackgroundColour(
        item_index, theme_engine_.color(core::ThemeColorToken::AccentPrimary));
    shortcut_list_->SetItemTextColour(item_index, wxColour(255, 255, 255));

    // Use a shared flag so the lambda executes only once
    auto recorded = std::make_shared<bool>(false);

    shortcut_list_->Bind(
        wxEVT_KEY_DOWN,
        [this, target_id, recorded](wxKeyEvent& key_evt)
        {
            if (*recorded)
            {
                key_evt.Skip();
                return;
            }

            const int kc = key_evt.GetKeyCode();

            // Skip pure modifier keys
            if (kc == WXK_SHIFT || kc == WXK_CONTROL || kc == WXK_ALT || kc == WXK_RAW_CONTROL)
            {
                return;
            }

            int mods = 0;
            if (key_evt.CmdDown())
            {
                mods |= wxMOD_CMD;
            }
            if (key_evt.ShiftDown())
            {
                mods |= wxMOD_SHIFT;
            }
            if (key_evt.AltDown())
            {
                mods |= wxMOD_ALT;
            }

            // Escape cancels recording
            if (kc == WXK_ESCAPE && mods == 0)
            {
                *recorded = true;
                if (status_label_ != nullptr)
                {
                    status_label_->SetLabel("Rebinding cancelled.");
                }
                ReloadShortcuts(); // Rebuilds list, removing lambda
                return;
            }

            // Check for conflicts
            if (shortcut_manager_.has_conflict(kc, mods, "global"))
            {
                const auto txt = core::ShortcutManager::format_shortcut(kc, mods);
                if (status_label_ != nullptr)
                {
                    status_label_->SetLabel("Conflict: " + txt + " is already bound.");
                }
                return;
            }

            // Apply the remap
            *recorded = true;
            shortcut_manager_.remap_shortcut(target_id, kc, mods);

            const auto txt = core::ShortcutManager::format_shortcut(kc, mods);
            MARKAMP_LOG_INFO("Remapped '{}' to {}", target_id, txt);

            if (status_label_ != nullptr)
            {
                status_label_->SetLabel("Bound \"" + target_id + "\" to " + txt);
            }
            ReloadShortcuts(); // Rebuilds list, removing lambda
        });
}

void ShortcutEditor::ApplyTheme()
{
    const auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    const auto fg_color = theme_engine_.color(core::ThemeColorToken::TextMain);
    SetBackgroundColour(bg_color);
    SetForegroundColour(fg_color);

    if (shortcut_list_ != nullptr)
    {
        shortcut_list_->SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::BgInput));
        shortcut_list_->SetForegroundColour(fg_color);
    }
}

} // namespace markamp::ui
