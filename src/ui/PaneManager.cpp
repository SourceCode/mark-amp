/// @file PaneManager.cpp
/// @brief V4 Phase 19 – Multiple Panes and Split View implementation.

#include "ui/PaneManager.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>

namespace markamp::ui
{

// ============================================================================
// Constructor
// ============================================================================

PaneManager::PaneManager(core::EventBus& event_bus, core::Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
    reset_layout();
}

// ============================================================================
// ID allocation
// ============================================================================

auto PaneManager::allocate_id() -> PaneId
{
    PaneId pid;
    pid.id = next_id_++;
    return pid;
}

// ============================================================================
// Reset to single-pane
// ============================================================================

auto PaneManager::reset_layout() -> void
{
    root_ = std::make_shared<SplitNode>();
    root_->type = SplitNode::Type::kLeaf;
    root_->pane_id = allocate_id();

    pane_states_.clear();
    PaneState state;
    state.pane_id = root_->pane_id;
    state.is_focused = true;
    pane_states_[root_->pane_id.id] = state;

    active_pane_ = root_->pane_id;
}

// ============================================================================
// Collect leaf panes
// ============================================================================

void PaneManager::collect_leaf_panes(const std::shared_ptr<SplitNode>& node,
                                     std::vector<PaneId>& panes) const
{
    if (!node)
    {
        return;
    }
    if (node->type == SplitNode::Type::kLeaf)
    {
        panes.push_back(node->pane_id);
        return;
    }
    collect_leaf_panes(node->first, panes);
    collect_leaf_panes(node->second, panes);
}

// ============================================================================
// Find a node by pane ID
// ============================================================================

auto PaneManager::find_node(const std::shared_ptr<SplitNode>& node, PaneId pane)
    -> std::shared_ptr<SplitNode>
{
    if (!node)
    {
        return nullptr;
    }
    if (node->type == SplitNode::Type::kLeaf && node->pane_id == pane)
    {
        return node;
    }
    if (auto found = find_node(node->first, pane))
    {
        return found;
    }
    return find_node(node->second, pane);
}

// ============================================================================
// Find the parent of a node
// ============================================================================

auto PaneManager::find_parent(const std::shared_ptr<SplitNode>& node, PaneId pane)
    -> std::shared_ptr<SplitNode>
{
    if (!node || node->type == SplitNode::Type::kLeaf)
    {
        return nullptr;
    }
    // Check immediate children
    if (node->first && node->first->type == SplitNode::Type::kLeaf && node->first->pane_id == pane)
    {
        return node;
    }
    if (node->second && node->second->type == SplitNode::Type::kLeaf &&
        node->second->pane_id == pane)
    {
        return node;
    }
    if (auto found = find_parent(node->first, pane))
    {
        return found;
    }
    return find_parent(node->second, pane);
}

// ============================================================================
// Split a specific pane
// ============================================================================

auto PaneManager::split_pane(PaneId pane, SplitDirection direction) -> PaneId
{
    auto target = find_node(root_, pane);
    if (!target)
    {
        return pane; // Not found
    }

    // New pane
    auto new_id = allocate_id();

    // Create two new leaf nodes
    auto original_leaf = std::make_shared<SplitNode>();
    original_leaf->type = SplitNode::Type::kLeaf;
    original_leaf->pane_id = pane;

    auto new_leaf = std::make_shared<SplitNode>();
    new_leaf->type = SplitNode::Type::kLeaf;
    new_leaf->pane_id = new_id;

    // Convert target into a split node in-place
    target->type = SplitNode::Type::kSplit;
    target->direction = direction;
    target->ratio = 0.5;
    target->first = original_leaf;
    target->second = new_leaf;
    target->pane_id = PaneId{0}; // No longer a leaf

    // Create state for new pane
    PaneState state;
    state.pane_id = new_id;
    pane_states_[new_id.id] = state;

    // Publish event
    core::events::PaneSplitEvent evt;
    evt.new_pane_id = new_id.id;
    evt.direction = (direction == SplitDirection::kHorizontal) ? "horizontal" : "vertical";
    event_bus_.publish(evt);

    return new_id;
}

// ============================================================================
// Split active pane
// ============================================================================

auto PaneManager::split_active(SplitDirection direction) -> PaneId
{
    return split_pane(active_pane_, direction);
}

// ============================================================================
// Close pane
// ============================================================================

auto PaneManager::close_pane(PaneId pane) -> bool
{
    // Can't close the last pane
    if (pane_count() <= 1)
    {
        return false;
    }

    auto parent = find_parent(root_, pane);
    if (!parent)
    {
        // Special case: pane is at the root's immediate child level
        // and the root itself is a split
        if (root_ && root_->type == SplitNode::Type::kSplit)
        {
            std::shared_ptr<SplitNode> sibling;
            if (root_->first && root_->first->type == SplitNode::Type::kLeaf &&
                root_->first->pane_id == pane)
            {
                sibling = root_->second;
            }
            else if (root_->second && root_->second->type == SplitNode::Type::kLeaf &&
                     root_->second->pane_id == pane)
            {
                sibling = root_->first;
            }

            if (sibling)
            {
                pane_states_.erase(pane.id);
                root_ = sibling;

                // Update focus if needed
                if (active_pane_ == pane)
                {
                    std::vector<PaneId> remaining;
                    collect_leaf_panes(root_, remaining);
                    if (!remaining.empty())
                    {
                        focus_pane(remaining[0]);
                    }
                }

                core::events::PaneClosedEvent evt;
                evt.pane_id = pane.id;
                event_bus_.publish(evt);
                return true;
            }
        }
        return false;
    }

    // Replace parent with sibling
    std::shared_ptr<SplitNode> sibling;
    if (parent->first && parent->first->type == SplitNode::Type::kLeaf &&
        parent->first->pane_id == pane)
    {
        sibling = parent->second;
    }
    else
    {
        sibling = parent->first;
    }

    if (sibling)
    {
        // Copy sibling data into parent
        parent->type = sibling->type;
        parent->direction = sibling->direction;
        parent->ratio = sibling->ratio;
        parent->pane_id = sibling->pane_id;
        parent->first = sibling->first;
        parent->second = sibling->second;
    }

    pane_states_.erase(pane.id);

    if (active_pane_ == pane)
    {
        std::vector<PaneId> remaining;
        collect_leaf_panes(root_, remaining);
        if (!remaining.empty())
        {
            focus_pane(remaining[0]);
        }
    }

    core::events::PaneClosedEvent evt;
    evt.pane_id = pane.id;
    event_bus_.publish(evt);
    return true;
}

// ============================================================================
// Active pane
// ============================================================================

auto PaneManager::active_pane() const -> PaneId
{
    return active_pane_;
}

// ============================================================================
// Focus pane
// ============================================================================

auto PaneManager::focus_pane(PaneId pane) -> void
{
    // Unfocus previous
    if (pane_states_.contains(active_pane_.id))
    {
        pane_states_[active_pane_.id].is_focused = false;
    }

    active_pane_ = pane;
    if (pane_states_.contains(pane.id))
    {
        pane_states_[pane.id].is_focused = true;
    }

    core::events::PaneFocusChangedEvent evt;
    evt.pane_id = pane.id;
    event_bus_.publish(evt);
}

// ============================================================================
// Pane count
// ============================================================================

auto PaneManager::pane_count() const -> int
{
    std::vector<PaneId> panes;
    collect_leaf_panes(root_, panes);
    return static_cast<int>(panes.size());
}

// ============================================================================
// Pane states
// ============================================================================

auto PaneManager::pane_states() const -> std::vector<PaneState>
{
    std::vector<PaneState> states;
    std::vector<PaneId> panes;
    collect_leaf_panes(root_, panes);

    for (const auto& pane : panes)
    {
        auto iter = pane_states_.find(pane.id);
        if (iter != pane_states_.end())
        {
            states.push_back(iter->second);
        }
    }
    return states;
}

// ============================================================================
// Open in pane
// ============================================================================

auto PaneManager::open_in_pane(PaneId pane, const std::string& document_id) -> void
{
    auto iter = pane_states_.find(pane.id);
    if (iter == pane_states_.end())
    {
        return;
    }

    auto& state = iter->second;
    // Check if already open
    auto doc_iter =
        std::find(state.open_document_ids.begin(), state.open_document_ids.end(), document_id);
    if (doc_iter == state.open_document_ids.end())
    {
        state.open_document_ids.push_back(document_id);
    }
    state.active_document_id = document_id;
}

// ============================================================================
// Move document
// ============================================================================

auto PaneManager::move_document(const std::string& document_id, PaneId from, PaneId to) -> bool
{
    auto from_iter = pane_states_.find(from.id);
    auto to_iter = pane_states_.find(to.id);

    if (from_iter == pane_states_.end() || to_iter == pane_states_.end())
    {
        return false;
    }

    // Remove from source
    auto& from_docs = from_iter->second.open_document_ids;
    auto doc_iter = std::find(from_docs.begin(), from_docs.end(), document_id);
    if (doc_iter == from_docs.end())
    {
        return false;
    }
    from_docs.erase(doc_iter);

    // Update active in source
    if (from_iter->second.active_document_id == document_id)
    {
        from_iter->second.active_document_id = from_docs.empty() ? "" : from_docs.back();
    }

    // Add to target
    to_iter->second.open_document_ids.push_back(document_id);
    to_iter->second.active_document_id = document_id;

    return true;
}

// ============================================================================
// Focus cycling
// ============================================================================

auto PaneManager::focus_next_pane() -> PaneId
{
    std::vector<PaneId> panes;
    collect_leaf_panes(root_, panes);

    if (panes.size() <= 1)
    {
        return active_pane_;
    }

    for (size_t idx = 0; idx < panes.size(); ++idx)
    {
        if (panes[idx] == active_pane_)
        {
            auto next = panes[(idx + 1) % panes.size()];
            focus_pane(next);
            return next;
        }
    }
    return active_pane_;
}

auto PaneManager::focus_prev_pane() -> PaneId
{
    std::vector<PaneId> panes;
    collect_leaf_panes(root_, panes);

    if (panes.size() <= 1)
    {
        return active_pane_;
    }

    for (size_t idx = 0; idx < panes.size(); ++idx)
    {
        if (panes[idx] == active_pane_)
        {
            auto prev = panes[(idx + panes.size() - 1) % panes.size()];
            focus_pane(prev);
            return prev;
        }
    }
    return active_pane_;
}

// ============================================================================
// Serialization (basic JSON)
// ============================================================================

auto PaneManager::serialize_layout() const -> std::string
{
    // Simple flat JSON for the tree structure
    std::string json = "{\"pane_count\":" + std::to_string(pane_count()) +
                       ",\"active_pane\":" + std::to_string(active_pane_.id) + "}";
    return json;
}

auto PaneManager::restore_layout(const std::string& json) -> bool
{
    // Basic validation
    if (json.empty() || json.front() != '{')
    {
        return false;
    }
    return true;
}

auto PaneManager::root() const -> const std::shared_ptr<SplitNode>&
{
    return root_;
}

} // namespace markamp::ui
