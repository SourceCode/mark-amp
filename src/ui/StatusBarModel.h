#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Priority group that determines item ordering in the status bar (Phase 13 Task 1).
enum class StatusItemPriority : uint8_t
{
    kHigh,   ///< Leftmost — critical info (e.g., errors, encoding)
    kNormal, ///< Middle region
    kLow,    ///< Rightmost — supplemental info (e.g., zoom, notifications)
};

/// Whether a status bar item is passive (display-only) or actionable (clickable).
enum class StatusItemKind : uint8_t
{
    kPassive,   ///< Info display only
    kClickable, ///< Has click/menu action
    kProgress,  ///< Shows a progress indicator
};

/// A single item in the status bar model.
struct StatusBarItemModel
{
    std::string item_id; ///< Unique identifier
    std::string label;   ///< Display text
    std::string tooltip; ///< Hover tooltip
    StatusItemPriority priority{StatusItemPriority::kNormal};
    StatusItemKind kind{StatusItemKind::kPassive};
    bool is_visible{true};

    // Progress fields (only for kProgress kind)
    float progress{0.0F}; ///< 0.0–1.0
    bool is_cancellable{false};
};

/// Testable model for the Status Bar (Phase 13).
///
/// Encapsulates:
/// - Item priority and stable ordering
/// - Clickable vs. passive item distinction
/// - Progress item management
/// - Overflow/truncation rules
class StatusBarModel
{
public:
    /// Add a status item.
    void add_item(StatusBarItemModel item);

    /// Remove a status item.
    void remove_item(const std::string& item_id);

    /// Get items sorted by priority (High → Normal → Low), stable within priority.
    [[nodiscard]] auto sorted_items() const -> std::vector<StatusBarItemModel>;

    /// Total item count.
    [[nodiscard]] auto item_count() const -> int;

    // ── Item queries ────────────────────────────────────────────────

    /// Get clickable items only.
    [[nodiscard]] auto clickable_items() const -> std::vector<StatusBarItemModel>;

    /// Get progress items only.
    [[nodiscard]] auto progress_items() const -> std::vector<StatusBarItemModel>;

    // ── Updates ─────────────────────────────────────────────────────

    /// Update the label for an item.
    void update_label(const std::string& item_id, const std::string& new_label);

    /// Update progress on a progress item.
    void update_progress(const std::string& item_id, float progress);

    /// Mark a progress item as complete (removes it).
    void complete_progress(const std::string& item_id);

    // ── Overflow ────────────────────────────────────────────────────

    /// Given a viewport width and item widths, return which items should be truncated.
    [[nodiscard]] auto truncated_indices(int viewport_width, int item_width) const
        -> std::vector<int>;

private:
    std::vector<StatusBarItemModel> items_;

    [[nodiscard]] auto find_item(const std::string& item_id) -> StatusBarItemModel*;
};

} // namespace markamp::ui
