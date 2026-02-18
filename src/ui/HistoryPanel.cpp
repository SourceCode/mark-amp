// ============================================================================
// File: src/ui/HistoryPanel.cpp
// Phase 32: Document History — History panel implementation (stub)
// ============================================================================
#include "HistoryPanel.h"

#include "core/Theme.h"

#include <wx/dcclient.h>
#include <wx/msgdlg.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(HistoryPanel, wxPanel) EVT_PAINT(HistoryPanel::on_paint) wxEND_EVENT_TABLE()

    HistoryPanel::HistoryPanel(wxWindow* parent,
                               core::EventBus& event_bus,
                               core::HistoryService& history_service)
    : wxPanel(parent, wxID_ANY)
    , event_bus_(event_bus)
    , history_service_(history_service)
{
    const core::Theme default_theme;
    SetBackgroundColour(default_theme.colors.bg_panel.to_wx_colour());
}

void HistoryPanel::populate_timeline(const std::string& root_id)
{
    current_root_id_ = root_id;
    // Stub: would fetch entries via history_service_.get_doc_histories()
    // and display them as a vertical timeline
    Refresh();
}

void HistoryPanel::show_diff(const std::string& /*old_entry_id*/,
                             const std::string& /*new_entry_id*/)
{
    // Stub: would call history_service_.diff_entries() and render result
    Refresh();
}

void HistoryPanel::on_rollback_clicked(const std::string& entry_id)
{
    auto result = wxMessageBox(
        "Roll back to this version? Current changes will be saved as a snapshot first.",
        "Confirm Rollback",
        wxYES_NO | wxICON_QUESTION,
        this);

    if (result == wxYES)
    {
        auto rollback_result = history_service_.rollback_doc(current_root_id_, entry_id);
        if (!rollback_result.has_value())
        {
            wxMessageBox(
                "Rollback failed: " + rollback_result.error(), "Error", wxOK | wxICON_ERROR, this);
        }
    }
}

void HistoryPanel::on_paint(wxPaintEvent& /*evt*/)
{
    wxPaintDC dc(this);
    const core::Theme default_theme;
    dc.SetTextForeground(default_theme.colors.text_muted.to_wx_colour());
    dc.DrawText("Document History: " + current_root_id_, 10, 10);
}

} // namespace markamp::ui
