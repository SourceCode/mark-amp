#pragma once

/**
 * @file MultiSelectModel.h
 * @brief Phase 33 Task 1: Platform-aware multi-select model.
 *
 * Handles Ctrl/Cmd toggle, Shift range, single-click, and provides
 * focus vs selection distinction, selection summary, and mode tracking.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Selection interaction mode.
enum class SelectionMode : uint8_t
{
    kSingle,   ///< Click replaces selection
    kToggle,   ///< Ctrl/Cmd+Click toggles single item
    kRange,    ///< Shift+Click extends to range
    kAdditive, ///< Always adds to selection
};

/// Modifier key state for platform-aware selection.
struct SelectionModifiers
{
    bool ctrl_or_cmd{false}; ///< Ctrl (Win/Linux) or Cmd (Mac)
    bool shift{false};
    bool alt{false};

    /// Determine the selection mode from modifier state.
    [[nodiscard]] auto mode() const -> SelectionMode;
};

/// Summary of current selection state.
struct SelectionSummary
{
    int total_count{0};
    int focused_index{-1};
    int anchor_index{-1}; ///< Range selection anchor
    std::string focused_id;
};

/**
 * @brief Platform-aware multi-selection model.
 *
 * Manages item selection with Ctrl/Cmd toggle, Shift range, focus tracking,
 * and selection summary. Items are identified by string IDs.
 */
class MultiSelectModel
{
public:
    MultiSelectModel() = default;

    // ── Item management ────────────────────────────────────────────

    /// Set the available items (ordered list of IDs).
    void set_items(std::vector<std::string> item_ids);

    /// Get the item count.
    [[nodiscard]] auto item_count() const -> int;

    // ── Selection operations ───────────────────────────────────────

    /// Click on an item with modifier state.
    void click(const std::string& item_id, const SelectionModifiers& modifiers = {});

    /// Select a single item (replaces selection).
    void select(const std::string& item_id);

    /// Toggle selection of a single item.
    void toggle(const std::string& item_id);

    /// Select a range from anchor to target.
    void select_range(const std::string& target_id);

    /// Select all items.
    void select_all();

    /// Clear selection.
    void clear_selection();

    // ── Query ──────────────────────────────────────────────────────

    /// Check if an item is selected.
    [[nodiscard]] auto is_selected(const std::string& item_id) const -> bool;

    /// Get all selected item IDs (in order).
    [[nodiscard]] auto selected_ids() const -> std::vector<std::string>;

    /// Get the number of selected items.
    [[nodiscard]] auto selection_count() const -> int;

    /// Get selection summary.
    [[nodiscard]] auto summary() const -> SelectionSummary;

    // ── Focus ──────────────────────────────────────────────────────

    /// Get the focused item ID.
    [[nodiscard]] auto focused_id() const -> const std::string&;

    /// Check if an item has focus.
    [[nodiscard]] auto is_focused(const std::string& item_id) const -> bool;

private:
    std::vector<std::string> items_;
    std::vector<bool> selected_;
    std::string focused_id_;
    int anchor_index_{-1};

    [[nodiscard]] auto index_of(const std::string& item_id) const -> int;
    void set_focus(const std::string& item_id);
};

} // namespace markamp::ui
