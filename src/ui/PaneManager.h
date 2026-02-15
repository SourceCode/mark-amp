/// @file PaneManager.h
/// @brief V4 Phase 19 – Multiple Panes and Split View (logic only).

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{
class EventBus;
class Config;
} // namespace markamp::core

namespace markamp::ui
{

/// Unique pane identifier.
struct PaneId
{
    int id{0};
    auto operator==(const PaneId& other) const -> bool
    {
        return id == other.id;
    }
    auto operator!=(const PaneId& other) const -> bool
    {
        return id != other.id;
    }
};

/// State of a single pane.
struct PaneState
{
    PaneId pane_id;
    std::vector<std::string> open_document_ids;
    std::string active_document_id;
    int scroll_position{0};
    bool is_focused{false};
};

/// Direction of a split.
enum class SplitDirection : uint8_t
{
    kHorizontal,
    kVertical
};

/// Node in the binary split tree.
struct SplitNode
{
    enum class Type : uint8_t
    {
        kLeaf,
        kSplit
    };

    Type type{Type::kLeaf};
    SplitDirection direction{SplitDirection::kHorizontal};
    double ratio{0.5}; ///< Split ratio (0.0 – 1.0)
    PaneId pane_id;    ///< Valid for leaf nodes

    std::shared_ptr<SplitNode> first;  ///< Left/top child
    std::shared_ptr<SplitNode> second; ///< Right/bottom child
};

/// Manages the multi-pane workspace tree.
class PaneManager
{
public:
    PaneManager(core::EventBus& event_bus, core::Config& config);

    /// Split the active pane.
    auto split_active(SplitDirection direction) -> PaneId;

    /// Split a specific pane by ID.
    auto split_pane(PaneId pane, SplitDirection direction) -> PaneId;

    /// Close a pane (merge sibling up).
    auto close_pane(PaneId pane) -> bool;

    /// Get the currently focused pane.
    [[nodiscard]] auto active_pane() const -> PaneId;

    /// Focus a specific pane.
    auto focus_pane(PaneId pane) -> void;

    /// Number of leaf panes.
    [[nodiscard]] auto pane_count() const -> int;

    /// Get all pane states.
    [[nodiscard]] auto pane_states() const -> std::vector<PaneState>;

    /// Reset to a single-pane layout.
    auto reset_layout() -> void;

    /// Open a document in a specific pane.
    auto open_in_pane(PaneId pane, const std::string& document_id) -> void;

    /// Move a document between panes.
    auto move_document(const std::string& document_id, PaneId from, PaneId to) -> bool;

    /// Focus next/previous pane in traversal order.
    auto focus_next_pane() -> PaneId;
    auto focus_prev_pane() -> PaneId;

    /// Serialize layout to JSON string.
    [[nodiscard]] auto serialize_layout() const -> std::string;

    /// Restore layout from JSON string.
    auto restore_layout(const std::string& json) -> bool;

    /// Get the split tree root (for testing).
    [[nodiscard]] auto root() const -> const std::shared_ptr<SplitNode>&;

private:
    core::EventBus& event_bus_;
    [[maybe_unused]] core::Config& config_;

    std::shared_ptr<SplitNode> root_;
    PaneId active_pane_;
    int next_id_{1};
    std::unordered_map<int, PaneState> pane_states_;

    auto allocate_id() -> PaneId;
    void collect_leaf_panes(const std::shared_ptr<SplitNode>& node,
                            std::vector<PaneId>& panes) const;
    auto find_node(const std::shared_ptr<SplitNode>& node, PaneId pane)
        -> std::shared_ptr<SplitNode>;
    auto find_parent(const std::shared_ptr<SplitNode>& node, PaneId pane)
        -> std::shared_ptr<SplitNode>;
};

} // namespace markamp::ui
