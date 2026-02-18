// ============================================================================
// File: src/canvas/RemoteSelectionOverlay.h
// Phase 13: Canvas Collaboration — remote selection highlighting overlay
// ============================================================================
#pragma once

#include "canvas/CanvasTypes.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::canvas
{

/// A single remote selection entry tracked by the overlay.
struct SelectionEntry
{
    std::string participant_id;
    std::string display_name;
    CanvasColor color{0, 120, 215, 255}; ///< Participant highlight color
    std::vector<ObjectId> selected_ids;  ///< Objects selected by this participant
    double border_width{2.0};            ///< Selection border thickness
    bool show_badge{true};               ///< Show participant name badge
};

/// Conflict info when multiple participants select the same object.
struct SelectionConflict
{
    ObjectId object_id{kInvalidObjectId};
    std::vector<std::string> participant_ids; ///< All participants selecting this object
};

/// Configuration for the selection overlay renderer.
struct SelectionOverlayConfig
{
    double border_width{2.0};   ///< Default border width
    double badge_offset_x{0.0}; ///< Badge position offset from object
    double badge_offset_y{-20.0};
    double conflict_border_width{3.0};  ///< Thicker border for conflicts
    bool show_conflict_indicator{true}; ///< Show indicator for multi-select conflicts
    bool show_badges{true};             ///< Show name badges on selections
};

/// Overlay that highlights objects selected by remote participants.
///
/// Features:
///   - Color-coded selection borders per participant
///   - Participant name badge on selected objects
///   - Conflict detection when multiple users select the same object
///   - Configurable border styles and badge positioning
class RemoteSelectionOverlay
{
public:
    RemoteSelectionOverlay();
    explicit RemoteSelectionOverlay(SelectionOverlayConfig config);

    // ── Selection Updates ─────────────────────────────────────────

    /// Update the selection set for a remote participant.
    auto update_selection(const std::string& participant_id,
                          const std::string& display_name,
                          const std::vector<ObjectId>& selected_ids,
                          const CanvasColor& color) -> void;

    /// Clear selection for a specific participant.
    auto clear_selection(const std::string& participant_id) -> void;

    /// Remove a participant entirely (they left the session).
    auto remove_participant(const std::string& participant_id) -> void;

    /// Clear all remote selections.
    auto clear_all() -> void;

    // ── Query ─────────────────────────────────────────────────────

    /// Get all selection entries for rendering.
    [[nodiscard]] auto selections() const -> const std::unordered_map<std::string, SelectionEntry>&;

    /// Get the selection entry for a specific participant.
    [[nodiscard]] auto find_selection(const std::string& participant_id) const
        -> const SelectionEntry*;

    /// Check if a specific object is selected by any remote participant.
    [[nodiscard]] auto is_remotely_selected(ObjectId obj_id) const -> bool;

    /// Get all participants who have selected a specific object.
    [[nodiscard]] auto participants_selecting(ObjectId obj_id) const -> std::vector<std::string>;

    /// Detect all selection conflicts (objects selected by multiple participants).
    [[nodiscard]] auto conflicts() const -> std::vector<SelectionConflict>;

    /// Number of participants with active selections.
    [[nodiscard]] auto active_selection_count() const -> size_t;

    /// Total number of uniquely selected objects across all participants.
    [[nodiscard]] auto total_selected_objects() const -> size_t;

    // ── Configuration ─────────────────────────────────────────────

    [[nodiscard]] auto config() const -> const SelectionOverlayConfig&;
    auto set_config(const SelectionOverlayConfig& config) -> void;

    /// Toggle badge visibility globally.
    auto set_badges_visible(bool visible) -> void;
    [[nodiscard]] auto badges_visible() const -> bool;

private:
    SelectionOverlayConfig config_;
    std::unordered_map<std::string, SelectionEntry> selections_;

    /// Build a reverse index: object_id → set of participant IDs.
    [[nodiscard]] auto build_object_participant_map() const
        -> std::unordered_map<ObjectId, std::vector<std::string>>;
};

} // namespace markamp::canvas
