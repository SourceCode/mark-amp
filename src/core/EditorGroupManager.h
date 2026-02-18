/// @file EditorGroupManager.h
/// @brief V9 Phase 46 — VS Code-style editor group and tab management.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Direction for splitting editor groups.
enum class SplitDirection : uint8_t
{
    kHorizontal = 0,
    kVertical = 1,
};

/// A single editor tab within a group.
struct EditorTab
{
    std::string tab_id;
    std::string file_path;
    std::string title;
    bool is_dirty{false};
    bool is_pinned{false};
    bool is_preview{false};
};

/// A group of editor tabs.
struct EditorGroup
{
    std::string group_id;
    std::vector<EditorTab> tabs;
    int active_tab_index{-1};
    bool is_active{false};
};

/// Manages VS Code-style editor groups with tabs, splitting, pinning.
class EditorGroupManager
{
public:
    EditorGroupManager() = default;

    // ── Group lifecycle ───────────────────────────────────────────────
    auto create_group() -> std::string;
    auto close_group(const std::string& group_id) -> bool;
    auto split_group(const std::string& group_id, SplitDirection direction) -> std::string;

    // ── Tab management ────────────────────────────────────────────────
    auto open_tab(const std::string& group_id,
                  const std::string& file_path,
                  const std::string& title = "") -> std::string;
    auto close_tab(const std::string& group_id, const std::string& tab_id) -> bool;
    auto pin_tab(const std::string& group_id, const std::string& tab_id) -> bool;
    auto unpin_tab(const std::string& group_id, const std::string& tab_id) -> bool;
    auto move_tab(const std::string& src_group,
                  const std::string& tab_id,
                  const std::string& dst_group) -> bool;

    // ── Focus ─────────────────────────────────────────────────────────
    auto set_active_group(const std::string& group_id) -> bool;
    [[nodiscard]] auto active_group() const -> const EditorGroup*;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto get_group(const std::string& group_id) const -> const EditorGroup*;
    [[nodiscard]] auto all_groups() const -> std::vector<const EditorGroup*>;
    [[nodiscard]] auto group_count() const -> int;
    [[nodiscard]] auto total_tab_count() const -> int;
    void clear_all();

private:
    std::vector<EditorGroup> groups_;
    int next_group_id_{1};
    int next_tab_id_{1};

    auto find_group_mut(const std::string& group_id) -> EditorGroup*;
    auto find_tab_mut(EditorGroup& group, const std::string& tab_id) -> EditorTab*;
};

} // namespace markamp::core
