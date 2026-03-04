#pragma once

/**
 * @file DropTargetHighlighter.h
 * @brief Phase 32 Task 2: Visual drop zone indicator management.
 *
 * Manages drop target highlights, insertion markers, and zone validity
 * styling to help users predict drop outcomes.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Insertion marker position relative to the drop target.
enum class InsertionPosition : uint8_t
{
    kNone,
    kBefore,
    kAfter,
    kInside,
};

/// Visual highlight state for a drop zone.
enum class HighlightStyle : uint8_t
{
    kNone,    ///< No highlight
    kValid,   ///< Green — valid drop target
    kInvalid, ///< Red — invalid drop target
    kNeutral, ///< Gray — unknown/default
    kActive,  ///< Blue — actively hovering
};

/// A single drop target's visual state.
struct DropTargetState
{
    std::string zone_id;
    HighlightStyle style{HighlightStyle::kNone};
    InsertionPosition insertion{InsertionPosition::kNone};
    bool is_hovered{false};

    /// Human-readable style name.
    [[nodiscard]] auto style_name() const -> std::string;

    /// Human-readable insertion position.
    [[nodiscard]] auto insertion_name() const -> std::string;
};

/**
 * @brief Manages visual drop target highlights.
 *
 * Tracks which zones are highlighted and their visual states, supporting
 * insertion markers and hover effects.
 */
class DropTargetHighlighter
{
public:
    DropTargetHighlighter() = default;

    // ── Zone management ────────────────────────────────────────────

    /// Register a drop target zone.
    void add_zone(const std::string& zone_id);

    /// Remove a drop target zone.
    void remove_zone(const std::string& zone_id);

    /// Clear all zones.
    void clear_zones();

    /// Get the number of registered zones.
    [[nodiscard]] auto zone_count() const -> int;

    // ── Highlight state ────────────────────────────────────────────

    /// Set the highlight style for a zone.
    void set_style(const std::string& zone_id, HighlightStyle style);

    /// Set the insertion marker for a zone.
    void set_insertion(const std::string& zone_id, InsertionPosition position);

    /// Mark a zone as hovered.
    void set_hovered(const std::string& zone_id);

    /// Clear hover from all zones.
    void clear_hover();

    /// Get the state of a specific zone.
    [[nodiscard]] auto zone_state(const std::string& zone_id) const -> DropTargetState;

    /// Get all zone states.
    [[nodiscard]] auto all_states() const -> std::vector<DropTargetState>;

    /// Get only highlighted (non-None) zones.
    [[nodiscard]] auto highlighted_zones() const -> std::vector<DropTargetState>;

    /// Reset all highlights and insertions.
    void reset_all();

    // ── Convenience ────────────────────────────────────────────────

    /// Highlight a zone as valid with an insertion position.
    void highlight_valid(const std::string& zone_id,
                         InsertionPosition insertion = InsertionPosition::kInside);

    /// Highlight a zone as invalid.
    void highlight_invalid(const std::string& zone_id);

    /// Check if any zones are highlighted.
    [[nodiscard]] auto has_highlights() const -> bool;

private:
    std::vector<DropTargetState> zones_;

    auto find_zone(const std::string& zone_id) -> DropTargetState*;
    [[nodiscard]] auto find_zone(const std::string& zone_id) const -> const DropTargetState*;
};

} // namespace markamp::ui
