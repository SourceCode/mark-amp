#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Tab indicator state for modified/conflict detection (Phase 08 Task 4).
enum class TabIndicator : uint8_t
{
    kClean,             ///< No unsaved changes
    kModified,          ///< Local unsaved changes (dot indicator)
    kExternallyChanged, ///< File changed on disk externally
    kConflict,          ///< Both local changes AND external changes
};

/// A tab group identifier for colour-coding.
struct TabGroupInfo
{
    std::string group_id;    ///< Unique group ID (e.g., directory-based)
    std::string group_label; ///< Display name for the group
    int color_index{0};      ///< Index into group color palette (0–5)
};

/// A single tab's model state.
struct TabItemModel
{
    std::string file_path;    ///< Unique identifier
    std::string display_name; ///< Visible tab label
    TabIndicator indicator{TabIndicator::kClean};
    bool is_pinned{false};
    bool is_active{false};
    std::optional<TabGroupInfo> group; ///< Group assignment (optional)
};

/// Testable model for the Tab Strip (Phase 08).
///
/// Encapsulates:
/// - Overflow detection and tab list
/// - Tab group operations (close group, pin group)
/// - Drag reorder validation
/// - Modified/conflict indicator rules
class TabStripModel
{
public:
    /// Add a tab.
    void add_tab(TabItemModel tab);

    /// Remove a tab by path.
    void remove_tab(const std::string& file_path);

    /// Get all tabs.
    [[nodiscard]] auto tabs() const -> const std::vector<TabItemModel>&;

    /// Tab count.
    [[nodiscard]] auto tab_count() const -> int;

    // ── Active tab ──────────────────────────────────────────────────

    /// Set active tab by path.
    void set_active(const std::string& file_path);

    /// Get active tab path.
    [[nodiscard]] auto active_path() const -> std::string;

    // ── Overflow ────────────────────────────────────────────────────

    /// Given a viewport width and min/max tab widths, return indices of overflowed tabs.
    [[nodiscard]] auto overflowed_indices(int viewport_width, int min_tab_width) const
        -> std::vector<int>;

    /// Get a searchable list of all tab display names (for overflow dropdown).
    [[nodiscard]] auto tab_names() const -> std::vector<std::string>;

    // ── Tab groups ──────────────────────────────────────────────────

    /// Close all tabs in a group.
    void close_group(const std::string& group_id);

    /// Pin all tabs in a group.
    void pin_group(const std::string& group_id);

    // ── Indicators ──────────────────────────────────────────────────

    /// Set indicator for a tab.
    void set_indicator(const std::string& file_path, TabIndicator indicator);

    /// Get tooltip text for an indicator state.
    [[nodiscard]] static auto indicator_tooltip(TabIndicator indicator) -> std::string;

    // ── Reorder ─────────────────────────────────────────────────────

    /// Move tab from src to dst index. Pinned tabs can only move among pinned.
    auto reorder(int src, int dst) -> bool;

    // ── Pinning ─────────────────────────────────────────────────────

    /// Pin/unpin a tab.
    void pin(const std::string& file_path);
    void unpin(const std::string& file_path);

private:
    std::vector<TabItemModel> tabs_;

    [[nodiscard]] auto find_tab(const std::string& file_path) -> TabItemModel*;
};

} // namespace markamp::ui
