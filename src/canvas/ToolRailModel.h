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
};

} // namespace markamp::canvas
