// ============================================================================
// File: src/ui/DiffPanel.h
// Phase 33: Version Diff & Comparison — Diff viewer panel (stub)
// ============================================================================
#pragma once

#include "../core/DiffTypes.h"

#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/stc/stc.h>

#include <string>

namespace markamp::core
{
class EventBus;
class BlockDiffEngine;
} // namespace markamp::core

namespace markamp::ui
{

/// Diff viewer panel containing two wxStyledTextCtrl instances
/// for side-by-side comparison with synchronized scrolling.
class DiffPanel : public wxPanel
{
public:
    DiffPanel(wxWindow* parent, core::EventBus& event_bus);

    /// Open diff view
    void open_diff(const std::string& left_path, const std::string& right_path);

    /// Display a diff result (legacy struct support for inline mode)

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
    core::DiffViewMode view_mode_{core::DiffViewMode::SideBySide};
    int current_change_index_{-1};
    int total_changes_{0};

    wxSplitterWindow* splitter_{nullptr};
    wxStyledTextCtrl* left_editor_{nullptr};
    wxStyledTextCtrl* right_editor_{nullptr};

    void SyncScroll(wxStyledTextCtrl* source, wxStyledTextCtrl* target);
    void OnLeftScroll(wxStyledTextEvent& evt);
    void OnRightScroll(wxStyledTextEvent& evt);

    void InitializeEditor(wxStyledTextCtrl* editor, bool read_only);
    void LoadFileSide(wxStyledTextCtrl* editor, const std::string& path);

    bool is_syncing_scroll_{false};

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
