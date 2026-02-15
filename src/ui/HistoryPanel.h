// ============================================================================
// File: src/ui/HistoryPanel.h
// Phase 32: Document History — History browser panel (stub)
// ============================================================================
#pragma once

#include <wx/panel.h>

#include <string>

namespace markamp::core
{
class EventBus;
class HistoryService;
} // namespace markamp::core

namespace markamp::ui
{

/// Stub panel for browsing document history, viewing diffs,
/// and rolling back to previous versions.
class HistoryPanel : public wxPanel
{
public:
    HistoryPanel(wxWindow* parent,
                 core::EventBus& event_bus,
                 core::HistoryService& history_service);

    /// Populate the timeline for a specific document.
    void populate_timeline(const std::string& root_id);

    /// Show a diff between two history entries.
    void show_diff(const std::string& old_entry_id, const std::string& new_entry_id);

    /// Handle rollback request.
    void on_rollback_clicked(const std::string& entry_id);

private:
    core::EventBus& event_bus_;
    core::HistoryService& history_service_;
    std::string current_root_id_;

    void on_paint(wxPaintEvent& evt);

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
