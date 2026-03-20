/// @file EditorViewModeManager.h
/// @brief P04-T02: Source of truth for editor view mode (Editor/Preview/Split).
///
/// Centralizes view mode management so that all consumers (Toolbar, PreviewPanel,
/// SplitView, StatusBarPanel) react to a single authoritative state change.
#pragma once

#include <string>

namespace markamp::core
{
class EventBus;

/// Editor view modes.
enum class EditorViewMode
{
    kEditor,
    kPreview,
    kSplit,
    kLivePreview,
};

/// Manages editor view mode and broadcasts state changes.
class EditorViewModeManager
{
public:
    explicit EditorViewModeManager(EventBus& bus);

    /// Set the current view mode and publish ViewModeChangedEvent.
    void set_mode(EditorViewMode mode);

    /// Get the current mode.
    [[nodiscard]] auto mode() const -> EditorViewMode { return current_mode_; }

    /// Cycle through modes: Editor → Split → Preview → Editor.
    void cycle_mode();

    /// Toggle between Editor and Preview.
    void toggle_preview();

    /// Check if preview pane should be visible.
    [[nodiscard]] auto is_preview_visible() const -> bool
    {
        return current_mode_ == EditorViewMode::kPreview ||
               current_mode_ == EditorViewMode::kSplit ||
               current_mode_ == EditorViewMode::kLivePreview;
    }

    /// Check if editor pane should be visible.
    [[nodiscard]] auto is_editor_visible() const -> bool
    {
        return current_mode_ == EditorViewMode::kEditor ||
               current_mode_ == EditorViewMode::kSplit ||
               current_mode_ == EditorViewMode::kLivePreview;
    }

private:
    EventBus& event_bus_;
    EditorViewMode current_mode_{EditorViewMode::kEditor};
};

} // namespace markamp::core
