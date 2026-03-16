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

    // Improvement 41: Fetch history entries from the service
    auto histories_result = history_service_.get_doc_histories(root_id, 0, 50);
    if (histories_result.has_value())
    {
        timeline_entries_ = histories_result.value();
    }
    else
    {
        timeline_entries_.clear();
    }
    Refresh();
}

void HistoryPanel::show_diff(const std::string& old_entry_id, const std::string& new_entry_id)
{
    // Improvement 42: Request a diff between two history entries
    diff_old_id_ = old_entry_id;
    diff_new_id_ = new_entry_id;

    auto diff_result = history_service_.diff_entries(old_entry_id, new_entry_id);
    if (diff_result.has_value())
    {
        current_diff_ = diff_result.value();
    }
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
        else
        {
            // Improvement 42: Refresh timeline after successful rollback
            populate_timeline(current_root_id_);
        }
    }
}

void HistoryPanel::on_paint(wxPaintEvent& /*evt*/)
{
    wxPaintDC dc(this);

    // Improvement 81: Use themed colors instead of default theme
    const core::Theme default_theme;
    const auto bg_color = default_theme.colors.bg_panel.to_wx_colour();
    const auto text_color = default_theme.colors.text_main.to_wx_colour();
    const auto muted_color = default_theme.colors.text_muted.to_wx_colour();
    const auto accent_color = default_theme.colors.accent_primary.to_wx_colour();

    dc.SetBackground(wxBrush(bg_color));
    dc.Clear();
    dc.SetTextForeground(text_color);

    // Improvement 82: Render themed header
    dc.SetFont(dc.GetFont().Bold().Scaled(1.1f));
    dc.DrawText("\xF0\x9F\x93\x9C Document History", 12, 8);
    dc.SetFont(dc.GetFont().GetBaseFont());

    int y_offset = 36;

    if (!current_root_id_.empty())
    {
        dc.SetTextForeground(muted_color);
        dc.DrawText("Document: " + current_root_id_, 12, y_offset);
        y_offset += 20;
    }

    // Improvement 83: Draw timeline with dots and connecting line
    if (timeline_entries_.empty())
    {
        dc.SetTextForeground(muted_color);
        dc.DrawText("No history entries found.", 32, y_offset + 10);
        dc.DrawText("Make changes to start tracking.", 32, y_offset + 28);
    }
    else
    {
        // Draw vertical timeline line
        dc.SetPen(wxPen(muted_color, 2));
        const int kLineX = 24;
        dc.DrawLine(kLineX, y_offset, kLineX, y_offset + static_cast<int>(timeline_entries_.size()) * 48);

        for (size_t idx = 0; idx < timeline_entries_.size(); ++idx)
        {
            const auto& entry = timeline_entries_[idx];
            const int entry_y = y_offset + static_cast<int>(idx) * 48;

            // Draw timeline dot (accent for selected, muted for others)
            const bool is_selected = (entry.title == diff_old_id_ || entry.title == diff_new_id_);
            if (is_selected)
            {
                dc.SetBrush(wxBrush(accent_color));
                dc.SetPen(wxPen(accent_color, 2));
            }
            else
            {
                dc.SetBrush(wxBrush(muted_color));
                dc.SetPen(wxPen(muted_color, 1));
            }
            dc.DrawCircle(kLineX, entry_y + 10, 5);

            // Draw entry text
            dc.SetTextForeground(is_selected ? accent_color : text_color);
            dc.DrawText(entry.title, 40, entry_y);

            // Improvement 84: Draw metadata line
            dc.SetTextForeground(muted_color);
            dc.SetFont(dc.GetFont().Scaled(0.85f));
            wxString meta_text = wxString::Format("Entry %zu", idx + 1);
            dc.DrawText(meta_text, 40, entry_y + 18);
            dc.SetFont(dc.GetFont().GetBaseFont());
        }
    }
}

} // namespace markamp::ui
