// ============================================================================
// File: src/ui/DiffPanel.h
// Phase 33: Version Diff & Comparison — Diff viewer panel (stub)
// ============================================================================
#pragma once

#include "../core/DiffTypes.h"

#include <wx/panel.h>

#include <string>

namespace markamp::core
{
class EventBus;
class BlockDiffEngine;
} // namespace markamp::core

namespace markamp::ui
{

/// Stub panel for displaying block-level diffs between
/// two document versions, with inline and side-by-side modes.
class DiffPanel : public wxPanel
{
public:
    DiffPanel(wxWindow* parent, core::EventBus& event_bus);

    /// Display a diff result.
    void show_diff(const core::BlockDiffResult& diff_result);

    /// Switch between inline and side-by-side view.
    void set_view_mode(core::DiffViewMode mode);

    /// Get the current view mode.
    [[nodiscard]] auto view_mode() const -> core::DiffViewMode;

    /// Navigate to the next change.
    void navigate_next_change();

    /// Navigate to the previous change.
    void navigate_prev_change();

private:
    core::EventBus& event_bus_;
    core::DiffViewMode view_mode_{core::DiffViewMode::Inline};
    int current_change_index_{-1};
    int total_changes_{0};

    void on_paint(wxPaintEvent& evt);

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
