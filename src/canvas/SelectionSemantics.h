#pragma once

/**
 * @file SelectionSemantics.h
 * @brief Phase 48 Task 1-2: Selection modes, multi-select, bounding box.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Selection mode.
enum class SelectionMode : uint8_t
{
    kClick,
    kShiftClick,
    kMarquee,
    kLasso,
};

/// A selected item.
struct SelectedItem
{
    std::string object_id;
    double pos_x{0.0};
    double pos_y{0.0};
    double width{0.0};
    double height{0.0};
};

/// Multi-select bounding box.
struct SelectionBounds
{
    double min_x{0.0};
    double min_y{0.0};
    double max_x{0.0};
    double max_y{0.0};

    [[nodiscard]] auto width() const -> double
    {
        return max_x - min_x;
    }
    [[nodiscard]] auto height() const -> double
    {
        return max_y - min_y;
    }
    [[nodiscard]] auto center_x() const -> double
    {
        return (min_x + max_x) / 2.0;
    }
    [[nodiscard]] auto center_y() const -> double
    {
        return (min_y + max_y) / 2.0;
    }
};

/**
 * @brief Manages selection semantics for canvas objects.
 */
class SelectionSemantics
{
public:
    SelectionSemantics() = default;

    /// Get mode as string.
    [[nodiscard]] static auto mode_name(SelectionMode mode) -> std::string;

    // ── Selection operations ───────────────────────────────────────

    /// Select an item (click).
    void select(const SelectedItem& item);

    /// Add to selection (shift-click).
    void add_to_selection(const SelectedItem& item);

    /// Toggle selection of an item.
    void toggle_selection(const std::string& object_id);

    /// Clear all selection.
    void clear();

    /// Remove from selection.
    void deselect(const std::string& object_id);

    // ── Queries ────────────────────────────────────────────────────

    /// Get selected count.
    [[nodiscard]] auto count() const -> int;

    /// Check if an item is selected.
    [[nodiscard]] auto is_selected(const std::string& object_id) const -> bool;

    /// Get all selected items.
    [[nodiscard]] auto items() const -> const std::vector<SelectedItem>&;

    /// Compute bounding box of selection.
    [[nodiscard]] auto bounds() const -> SelectionBounds;

private:
    std::vector<SelectedItem> selected_;
};

} // namespace markamp::canvas
