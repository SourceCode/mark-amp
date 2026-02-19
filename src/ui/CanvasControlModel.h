#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Canvas tool type (Phase 25 Task 1).
enum class CanvasTool : uint8_t
{
    kSelect,
    kPan,
    kText,
    kShape,
    kConnector,
    kPen,
    kEraser,
};

/// Canvas tool strip item.
struct CanvasToolItem
{
    CanvasTool tool;
    std::string label;
    std::string shortcut;
    std::string group; ///< Tool category group
};

/// Inspector property for selected object.
struct InspectorProperty
{
    std::string property_id;
    std::string label;
    std::string value;
    std::string value_type; ///< "color", "number", "text", "choice"
    bool is_editable{true};
};

/// Testable model for Canvas Controls (Phase 25).
///
/// Encapsulates:
/// - Tool strip with active tool and grouping
/// - Inspector properties per selection type
/// - Multi-select action availability
/// - Selection count and grouping actions
class CanvasControlModel
{
public:
    /// Set available tools.
    void set_tools(std::vector<CanvasToolItem> tools);
    [[nodiscard]] auto tools() const -> const std::vector<CanvasToolItem>&;

    /// Get tools by group.
    [[nodiscard]] auto tools_by_group(const std::string& group) const
        -> std::vector<CanvasToolItem>;

    // ── Active tool ─────────────────────────────────────────────────

    void set_active_tool(CanvasTool tool);
    [[nodiscard]] auto active_tool() const -> CanvasTool;

    /// Get label for active tool.
    [[nodiscard]] auto active_tool_label() const -> std::string;

    // ── Inspector ───────────────────────────────────────────────────

    /// Set inspector properties for current selection.
    void set_properties(std::vector<InspectorProperty> props);
    [[nodiscard]] auto properties() const -> const std::vector<InspectorProperty>&;

    // ── Selection ───────────────────────────────────────────────────

    void set_selection_count(int count);
    [[nodiscard]] auto selection_count() const -> int;

    /// Is multi-select (2+ objects)?
    [[nodiscard]] auto is_multi_select() const -> bool;

    /// Available actions for current selection (e.g., "Group", "Align", "Delete").
    [[nodiscard]] auto selection_actions() const -> std::vector<std::string>;

private:
    std::vector<CanvasToolItem> tools_;
    CanvasTool active_tool_{CanvasTool::kSelect};
    std::vector<InspectorProperty> properties_;
    int selection_count_{0};
};

} // namespace markamp::ui
