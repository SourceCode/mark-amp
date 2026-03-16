#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Tool group category.
enum class ToolGroup : uint8_t
{
    kSelect,
    kDraw,
    kShape,
    kText,
    kMedia,
    kStructure,
};

/// A tool entry.
struct ToolEntry
{
    std::string tool_id;
    std::string name;
    ToolGroup group{ToolGroup::kSelect};
    bool visible{true};

    // ── Round 5 Batch 2 (#11-14) ────────────────────────────────

    /// (#11) Whether this tool is visible.
    [[nodiscard]] auto is_visible() const noexcept -> bool
    {
        return visible;
    }

    /// (#12) Whether a name is set.
    [[nodiscard]] auto has_name() const noexcept -> bool
    {
        return !name.empty();
    }

    /// (#13) Whether this is a draw-group tool.
    [[nodiscard]] auto is_draw_group() const noexcept -> bool
    {
        return group == ToolGroup::kDraw;
    }

    /// (#14) Whether this is a select-group tool.
    [[nodiscard]] auto is_select_group() const noexcept -> bool
    {
        return group == ToolGroup::kSelect;
    }
};

/// Testable model for Canvas Tool Rail (Phase 61).
///
/// Encapsulates:
/// - Tool catalog with group categories
/// - Active tool selection with quick-switch (last-used stack)
/// - Context toolbar state (current tool/selection-aware controls)
/// - Tool visibility settings (hide/show per user preference)
class ToolRailModel
{
public:
    // ── Catalog ─────────────────────────────────────────────────────

    void set_tools(std::vector<ToolEntry> tools);
    [[nodiscard]] auto tools() const -> const std::vector<ToolEntry>&;
    [[nodiscard]] auto tools_in_group(ToolGroup group) const -> std::vector<ToolEntry>;

    // ── Active tool ─────────────────────────────────────────────────

    void select_tool(const std::string& tool_id);
    [[nodiscard]] auto active_tool() const -> const std::string&;

    void quick_switch(); ///< Switch to previous tool
    [[nodiscard]] auto recent_stack() const -> const std::vector<std::string>&;

    // ── Context toolbar ─────────────────────────────────────────────

    void set_context_actions(std::vector<std::string> actions);
    [[nodiscard]] auto context_actions() const -> const std::vector<std::string>&;

    // ── Visibility ──────────────────────────────────────────────────

    void set_tool_visible(const std::string& tool_id, bool visible);
    [[nodiscard]] auto visible_tools() const -> std::vector<ToolEntry>;

private:
    std::vector<ToolEntry> tools_;
    std::string active_tool_;
    std::vector<std::string> recent_stack_;
    std::vector<std::string> context_actions_;
    static constexpr int kMaxRecent = 5;

    // ── Round 5 Batch 2 (#15-17) ────────────────────────────────

    /// (#15) Number of registered tools.
    [[nodiscard]] auto tool_count() const noexcept -> size_t
    {
        return tools_.size();
    }

    /// (#16) Whether an active tool is set.
    [[nodiscard]] auto has_active() const noexcept -> bool
    {
        return !active_tool_.empty();
    }

    /// (#17) Whether context actions exist.
    [[nodiscard]] auto has_context_actions() const noexcept -> bool
    {
        return !context_actions_.empty();
    }
};

} // namespace markamp::canvas
