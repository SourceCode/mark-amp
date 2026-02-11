#include "ThemeGallery.h"

#include "core/Logger.h"
#include "core/ThemeValidator.h"

#include <wx/button.h>
#include <wx/dcbuffer.h>
#include <wx/filedlg.h>
#include <wx/graphics.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include <algorithm>

namespace markamp::ui
{

namespace
{
constexpr int kMinCardWidth = 250;
constexpr int kErrorTimerMs = 5000;
} // namespace

// ═══════════════════════════════════════════════════════
// Construction
// ═══════════════════════════════════════════════════════

ThemeGallery::ThemeGallery(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           core::ThemeRegistry& registry)
    : wxDialog(parent,
               wxID_ANY,
               "Theme Gallery",
               wxDefaultPosition,
               wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCLIP_CHILDREN)
    , theme_engine_(theme_engine)
    , registry_(registry)
    , error_timer_(this)
{
    // Size: 896px wide, 80% of parent height
    int parent_height = parent != nullptr ? parent->GetSize().GetHeight() : 700;
    int dialog_height = static_cast<int>(static_cast<double>(parent_height) * 0.8);
    SetSize(kDialogWidth, dialog_height);
    SetMinSize(wxSize(500, 400));

    // Apply theme background
    const auto& t = theme_engine_.current_theme();
    SetBackgroundColour(wxColour(t.colors.bg_app.to_rgba_string()));

    // Main vertical sizer
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    CreateHeader();
    main_sizer->Add(header_panel_, 0, wxEXPAND);

    CreateToolbar();
    main_sizer->Add(toolbar_panel_, 0, wxEXPAND);

    CreateGrid();
    main_sizer->Add(grid_panel_, 1, wxEXPAND);

    SetSizer(main_sizer);
    PopulateGrid();

    // Center in parent
    CenterOnParent();

    // Event bindings
    Bind(wxEVT_CLOSE_WINDOW, &ThemeGallery::OnClose, this);
    Bind(wxEVT_CHAR_HOOK, &ThemeGallery::OnKeyDown, this);
    Bind(wxEVT_SIZE, &ThemeGallery::OnSize, this);
    Bind(wxEVT_TIMER, &ThemeGallery::OnErrorTimer, this, error_timer_.GetId());
}

// ═══════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════

void ThemeGallery::ShowGallery()
{
    ShowModal();
}

auto ThemeGallery::GetSelectedThemeId() const -> std::string
{
    return selected_theme_id_;
}

auto ThemeGallery::CalculateColumns(int available_width) -> int
{
    int card_width = ThemePreviewCard::kCardWidth;
    int gap = kCardGap;
    return std::max(1, (available_width + gap) / (card_width + gap));
}

auto ThemeGallery::ExportFilename(const std::string& theme_name) -> std::string
{
    std::string result;
    result.reserve(theme_name.size() + 16);
    bool prev_was_space = false;
    for (char ch : theme_name)
    {
        if (ch == ' ' || ch == '-')
        {
            if (!result.empty() && !prev_was_space)
            {
                result += '_';
            }
            prev_was_space = true;
        }
        else if (std::isalnum(static_cast<unsigned char>(ch)))
        {
            result += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            prev_was_space = false;
        }
    }
    // Remove trailing underscore
    if (!result.empty() && result.back() == '_')
    {
        result.pop_back();
    }
    result += ".theme.json";
    return result;
}

// ═══════════════════════════════════════════════════════
// Header
// ═══════════════════════════════════════════════════════

void ThemeGallery::CreateHeader()
{
    const auto& t = theme_engine_.current_theme();

    header_panel_ = new wxPanel(this, wxID_ANY);
    header_panel_->SetBackgroundColour(wxColour(t.colors.bg_header.to_rgba_string()));
    header_panel_->SetMinSize(wxSize(-1, kHeaderHeight));

    auto* header_sizer = new wxBoxSizer(wxHORIZONTAL);

    // Left section: icon placeholder + text
    auto* left_sizer = new wxBoxSizer(wxVERTICAL);

    auto* title = new wxStaticText(header_panel_, wxID_ANY, "Theme Gallery");
    wxFont title_font(wxFontInfo(16).Family(wxFONTFAMILY_SWISS).Bold());
    title->SetFont(title_font);
    title->SetForegroundColour(wxColour(t.colors.text_main.to_rgba_string()));

    auto* subtitle =
        new wxStaticText(header_panel_, wxID_ANY, "SELECT OR IMPORT APPEARANCE PROFILE");
    wxFont subtitle_font(wxFontInfo(9).Family(wxFONTFAMILY_SWISS));
    subtitle->SetFont(subtitle_font);
    subtitle->SetForegroundColour(wxColour(t.colors.text_muted.to_rgba_string()));

    left_sizer->Add(title, 0, wxLEFT | wxTOP, 16);
    left_sizer->Add(subtitle, 0, wxLEFT | wxBOTTOM, 16);

    header_sizer->Add(left_sizer, 1, wxEXPAND);

    // Close button
    auto* close_btn = new wxButton(
        header_panel_, wxID_CLOSE, wxString::FromUTF8("\u2715"), wxDefaultPosition, wxSize(32, 32));
    close_btn->SetBackgroundColour(wxColour(t.colors.bg_header.to_rgba_string()));
    close_btn->SetForegroundColour(wxColour(t.colors.text_muted.to_rgba_string()));
    close_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { Close(); });
    header_sizer->Add(close_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 16);

    header_panel_->SetSizer(header_sizer);
}

// ═══════════════════════════════════════════════════════
// Toolbar
// ═══════════════════════════════════════════════════════

void ThemeGallery::CreateToolbar()
{
    const auto& t = theme_engine_.current_theme();

    toolbar_panel_ = new wxPanel(this, wxID_ANY);
    toolbar_panel_->SetBackgroundColour(wxColour(t.colors.bg_panel.to_rgba_string()));
    toolbar_panel_->SetMinSize(wxSize(-1, kToolbarHeight));

    auto* toolbar_sizer = new wxBoxSizer(wxHORIZONTAL);

    // Theme count label
    auto theme_count = theme_engine_.available_themes().size();
    wxString count_text = wxString::Format("Viewing %zu installed themes", theme_count);
    theme_count_label_ = new wxStaticText(toolbar_panel_, wxID_ANY, count_text);
    wxFont count_font(wxFontInfo(11).Family(wxFONTFAMILY_SWISS));
    theme_count_label_->SetFont(count_font);
    theme_count_label_->SetForegroundColour(wxColour(t.colors.text_muted.to_rgba_string()));

    toolbar_sizer->Add(theme_count_label_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 16);

    // Error label (red, hidden by default)
    error_label_ = new wxStaticText(toolbar_panel_, wxID_ANY, "");
    wxFont error_font(wxFontInfo(10).Family(wxFONTFAMILY_SWISS));
    error_label_->SetFont(error_font);
    error_label_->SetForegroundColour(wxColour(220, 50, 50));
    error_label_->Hide();

    toolbar_sizer->Add(error_label_, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, 16);

    // Spacer
    toolbar_sizer->AddStretchSpacer(1);

    // Import button
    auto* import_btn = new wxButton(toolbar_panel_, wxID_ANY, "IMPORT THEME");
    import_btn->SetBackgroundColour(wxColour(t.colors.bg_panel.to_rgba_string()));
    import_btn->SetForegroundColour(wxColour(t.colors.accent_primary.to_rgba_string()));
    wxFont import_font(wxFontInfo(10).Family(wxFONTFAMILY_SWISS).Bold());
    import_btn->SetFont(import_font);
    import_btn->Bind(wxEVT_BUTTON, &ThemeGallery::OnImportClicked, this);

    toolbar_sizer->Add(import_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 16);

    toolbar_panel_->SetSizer(toolbar_sizer);
}

// ═══════════════════════════════════════════════════════
// Grid
// ═══════════════════════════════════════════════════════

void ThemeGallery::CreateGrid()
{
    grid_panel_ = new wxScrolledWindow(this, wxID_ANY);
    grid_panel_->SetScrollRate(0, 20);

    const auto& t = theme_engine_.current_theme();
    grid_panel_->SetBackgroundColour(wxColour(t.colors.bg_app.to_rgba_string()));
}

void ThemeGallery::PopulateGrid()
{
    // Clear existing cards
    for (auto* card : preview_cards_)
    {
        card->Destroy();
    }
    preview_cards_.clear();

    // Calculate columns
    int available_width = grid_panel_->GetClientSize().GetWidth() - kGridPadding * 2;
    if (available_width < kMinCardWidth)
    {
        available_width = kDialogWidth - kGridPadding * 2;
    }
    int columns = CalculateColumns(available_width);

    auto* grid_sizer = new wxGridSizer(columns, kCardGap, kCardGap);

    auto themes = theme_engine_.available_themes();
    std::string original_theme_id = theme_engine_.current_theme().id;

    // Collect full Theme data by temporarily applying each theme
    std::vector<core::Theme> all_themes;
    all_themes.reserve(themes.size());

    // Track which themes are built-in
    std::vector<bool> builtin_flags;
    builtin_flags.reserve(themes.size());

    for (const auto& info : themes)
    {
        theme_engine_.apply_theme(info.id);
        all_themes.push_back(theme_engine_.current_theme());
        builtin_flags.push_back(info.is_builtin);
    }

    // Restore original theme
    theme_engine_.apply_theme(original_theme_id);

    // Create cards with full theme data
    for (size_t i = 0; i < all_themes.size(); ++i)
    {
        const auto& theme = all_themes[i];
        bool is_active = (theme.id == original_theme_id);
        bool is_builtin = builtin_flags[i];
        auto* card = new ThemePreviewCard(grid_panel_, theme, is_active, is_builtin);
        card->SetOnClick([this](const std::string& id) { OnThemeCardClicked(id); });
        card->SetOnExport([this](const std::string& id) { OnExportTheme(id); });
        card->SetOnDelete([this](const std::string& id) { OnDeleteTheme(id); });
        preview_cards_.push_back(card);
        grid_sizer->Add(card, 0, wxALL, 0);
    }

    auto* outer_sizer = new wxBoxSizer(wxVERTICAL);
    outer_sizer->Add(grid_sizer, 0, wxALL, kGridPadding);
    grid_panel_->SetSizer(outer_sizer);
    grid_panel_->FitInside();
}

void ThemeGallery::UpdateActiveIndicators()
{
    const std::string& current_id = theme_engine_.current_theme().id;
    for (auto* card : preview_cards_)
    {
        card->SetActive(card->GetThemeId() == current_id);
    }
}

void ThemeGallery::RefreshThemeCount()
{
    if (theme_count_label_ != nullptr)
    {
        auto theme_count = theme_engine_.available_themes().size();
        wxString count_text = wxString::Format("Viewing %zu installed themes", theme_count);
        theme_count_label_->SetLabel(count_text);
    }
}

void ThemeGallery::ShowError(const std::string& message)
{
    if (error_label_ != nullptr)
    {
        error_label_->SetLabel(wxString::FromUTF8(message));
        error_label_->Show();
        toolbar_panel_->Layout();
        error_timer_.StartOnce(kErrorTimerMs);
    }
}

void ThemeGallery::ClearError()
{
    if (error_label_ != nullptr)
    {
        error_label_->SetLabel("");
        error_label_->Hide();
        toolbar_panel_->Layout();
    }
}

// ═══════════════════════════════════════════════════════
// Event Handlers
// ═══════════════════════════════════════════════════════

void ThemeGallery::OnClose(wxCloseEvent& event)
{
    EndModal(wxID_CANCEL);
    event.Skip();
}

void ThemeGallery::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_ESCAPE)
    {
        Close();
        return;
    }
    event.Skip();
}

void ThemeGallery::OnThemeCardClicked(const std::string& theme_id)
{
    selected_theme_id_ = theme_id;
    theme_engine_.apply_theme(theme_id);

    // Update card active states
    UpdateActiveIndicators();

    // Re-theme the gallery itself
    const auto& t = theme_engine_.current_theme();
    SetBackgroundColour(wxColour(t.colors.bg_app.to_rgba_string()));

    if (header_panel_ != nullptr)
    {
        header_panel_->SetBackgroundColour(wxColour(t.colors.bg_header.to_rgba_string()));
    }
    if (toolbar_panel_ != nullptr)
    {
        toolbar_panel_->SetBackgroundColour(wxColour(t.colors.bg_panel.to_rgba_string()));
    }
    if (grid_panel_ != nullptr)
    {
        grid_panel_->SetBackgroundColour(wxColour(t.colors.bg_app.to_rgba_string()));
    }

    Refresh();
    Update();

    MARKAMP_LOG_INFO("Theme gallery: applied theme '{}'", theme_id);
}

void ThemeGallery::OnExportTheme(const std::string& theme_id)
{
    // Find theme name for default filename
    std::string default_name;
    for (const auto& card : preview_cards_)
    {
        if (card->GetThemeId() == theme_id)
        {
            default_name = ExportFilename(card->GetThemeName());
            break;
        }
    }

    if (default_name.empty())
    {
        default_name = theme_id + ".theme.json";
    }

    wxFileDialog save_dialog(this,
                             "Export Theme",
                             wxEmptyString,
                             default_name,
                             "Theme files (*.json)|*.json",
                             wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (save_dialog.ShowModal() == wxID_OK)
    {
        std::filesystem::path export_path(save_dialog.GetPath().ToStdString());
        auto result = registry_.export_theme(theme_id, export_path);
        if (result)
        {
            ClearError();
            MARKAMP_LOG_INFO("Theme exported: {} -> {}", theme_id, export_path.string());
        }
        else
        {
            ShowError(result.error());
            MARKAMP_LOG_WARN("Theme export failed: {}", result.error());
        }
    }
}

void ThemeGallery::OnDeleteTheme(const std::string& theme_id)
{
    // Find theme name for the confirmation message
    std::string theme_name = theme_id;
    for (const auto& card : preview_cards_)
    {
        if (card->GetThemeId() == theme_id)
        {
            theme_name = card->GetThemeName();
            break;
        }
    }

    // Confirmation dialog
    wxString msg = wxString::Format("Delete theme \"%s\"?\n\nThis action cannot be undone.",
                                    wxString::FromUTF8(theme_name));
    int result = wxMessageBox(msg, "Delete Theme", wxYES_NO | wxICON_WARNING, this);
    if (result != wxYES)
    {
        return;
    }

    // If deleting the active theme, switch to the first available theme
    bool was_active = (theme_engine_.current_theme().id == theme_id);

    auto delete_result = registry_.delete_theme(theme_id);
    if (!delete_result)
    {
        ShowError(delete_result.error());
        MARKAMP_LOG_WARN("Theme delete failed: {}", delete_result.error());
        return;
    }

    if (was_active)
    {
        auto themes = theme_engine_.available_themes();
        if (!themes.empty())
        {
            theme_engine_.apply_theme(themes.front().id);
        }
    }

    // Repopulate grid
    PopulateGrid();
    RefreshThemeCount();
    Layout();

    MARKAMP_LOG_INFO("Theme deleted: {}", theme_id);
}

void ThemeGallery::OnImportClicked(wxCommandEvent& /*event*/)
{
    wxFileDialog open_dialog(this,
                             "Import Theme",
                             wxEmptyString,
                             wxEmptyString,
                             "Theme files (*.json)|*.json|All files (*.*)|*.*",
                             wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (open_dialog.ShowModal() == wxID_OK)
    {
        std::filesystem::path import_path(open_dialog.GetPath().ToStdString());
        auto result = registry_.import_theme(import_path);
        if (result)
        {
            ClearError();

            // Repopulate the grid with the new theme included
            PopulateGrid();
            RefreshThemeCount();
            Layout();

            MARKAMP_LOG_INFO("Theme imported from: {}", import_path.string());
        }
        else
        {
            ShowError(result.error());
            MARKAMP_LOG_WARN("Theme import failed: {}", result.error());
        }
    }
}

void ThemeGallery::OnSize(wxSizeEvent& event)
{
    // Recalculate grid columns on resize
    if (grid_panel_ != nullptr && !preview_cards_.empty())
    {
        int available_width = grid_panel_->GetClientSize().GetWidth() - kGridPadding * 2;
        int columns = CalculateColumns(available_width);

        auto* sizer = grid_panel_->GetSizer();
        if (sizer != nullptr)
        {
            // Find the grid sizer (first child of the outer sizer)
            auto* outer = dynamic_cast<wxBoxSizer*>(sizer);
            if (outer != nullptr && outer->GetItemCount() > 0)
            {
                auto* grid =
                    dynamic_cast<wxGridSizer*>(outer->GetItem(static_cast<size_t>(0))->GetSizer());
                if (grid != nullptr)
                {
                    grid->SetCols(columns);
                    grid_panel_->FitInside();
                    grid_panel_->Layout();
                }
            }
        }
    }

    event.Skip();
}

void ThemeGallery::OnErrorTimer(wxTimerEvent& /*event*/)
{
    ClearError();
}

} // namespace markamp::ui
