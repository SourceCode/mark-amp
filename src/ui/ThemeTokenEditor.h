#pragma once

#include "core/ThemeEngine.h"

#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/stattext.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// A single editable token entry in the token list.
struct TokenEntry
{
    std::string label;
    core::ThemeColorToken token;
    core::Color* color_ref; // mutable pointer into the theme's layer struct
};

/// Phase 4 — dialog for editing theme tokens with live preview.
///
/// Layout:
///   ┌──────────────────────────────────────────────┐
///   │  Tab: Chrome │ Syntax │ Render               │
///   ├──────────────┬───────────────────────────────┤
///   │  Token list  │  Live preview panel           │
///   │  ┌ swatch ─┐ │  ┌───────┬──────────────────┐ │
///   │  │ bg_app   │ │  │Sidebar│     Editor       │ │
///   │  │ bg_panel │ │  │       ├──────────────────┤ │
///   │  │ accent   │ │  │       │    Preview       │ │
///   │  │ ...      │ │  └───────┴──────────────────┘ │
///   ├──────────────┴───────────────────────────────┤
///   │              [Apply]  [Save]  [Cancel]        │
///   └──────────────────────────────────────────────┘
class ThemeTokenEditor : public wxDialog
{
public:
    ThemeTokenEditor(wxWindow* parent, core::ThemeEngine& engine);

    /// Returns true if the user clicked "Apply" or "Save".
    [[nodiscard]] auto was_applied() const -> bool
    {
        return applied_;
    }

private:
    // --- UI construction ---
    void build_ui();
    void build_token_list(wxPanel* parent, const std::vector<TokenEntry>& entries);
    auto build_preview_panel(wxPanel* parent) -> wxPanel*;

    // --- Population ---
    void populate_chrome_tokens();
    void populate_syntax_tokens();
    void populate_render_tokens();

    // --- Events ---
    void on_tab_changed(wxBookCtrlEvent& event);
    void on_token_clicked(wxListEvent& event);
    void on_apply(wxCommandEvent& event);
    void on_save(wxCommandEvent& event);
    void on_cancel(wxCommandEvent& event);

    // --- Preview ---
    void refresh_preview();

    // State
    core::ThemeEngine& engine_;
    core::Theme working_theme_; // copy being edited
    bool applied_{false};

    // Current tab's token entries
    std::vector<TokenEntry> current_entries_;

    // Widgets
    wxNotebook* tabs_{nullptr};
    wxListCtrl* token_list_{nullptr};
    wxPanel* preview_panel_{nullptr};

    // Preview sub-panels (simple colored rectangles)
    wxPanel* preview_sidebar_{nullptr};
    wxPanel* preview_editor_{nullptr};
    wxPanel* preview_rendered_{nullptr};

    static constexpr int kDialogWidth = 800;
    static constexpr int kDialogHeight = 520;
};

} // namespace markamp::ui
