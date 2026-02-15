// ============================================================================
// File: src/ui/DiffPanel.cpp
// Phase 33: Version Diff & Comparison — DiffPanel implementation (stub)
// ============================================================================
#include "DiffPanel.h"

#include <wx/dcclient.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(DiffPanel, wxPanel) EVT_PAINT(DiffPanel::on_paint) wxEND_EVENT_TABLE()

    DiffPanel::DiffPanel(wxWindow* parent, core::EventBus& event_bus)
    : wxPanel(parent, wxID_ANY)
    , event_bus_(event_bus)
{
    SetBackgroundColour(wxColour(250, 250, 250));
}

void DiffPanel::show_diff(const core::BlockDiffResult& diff_result)
{
    total_changes_ = diff_result.stats.changed_blocks();
    current_change_index_ = total_changes_ > 0 ? 0 : -1;
    // Stub: would render HTML via DiffRenderer and display in webview
    Refresh();
}

void DiffPanel::set_view_mode(core::DiffViewMode mode)
{
    view_mode_ = mode;
    Refresh();
}

auto DiffPanel::view_mode() const -> core::DiffViewMode
{
    return view_mode_;
}

void DiffPanel::navigate_next_change()
{
    if (current_change_index_ < total_changes_ - 1)
    {
        ++current_change_index_;
        Refresh();
    }
}

void DiffPanel::navigate_prev_change()
{
    if (current_change_index_ > 0)
    {
        --current_change_index_;
        Refresh();
    }
}

void DiffPanel::on_paint(wxPaintEvent& /*evt*/)
{
    wxPaintDC dc(this);
    dc.SetTextForeground(wxColour(128, 128, 128));

    auto mode_str = (view_mode_ == core::DiffViewMode::Inline) ? "Inline" : "Side-by-Side";
    dc.DrawText(
        wxString::Format(
            "Diff Viewer (%s) — Change %d/%d", mode_str, current_change_index_ + 1, total_changes_),
        10,
        10);
}

} // namespace markamp::ui
