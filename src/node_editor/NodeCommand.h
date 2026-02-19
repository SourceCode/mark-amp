#pragma once

// V11 Phase 07: Command Stack & Undo/Redo Transactions
// Command pattern interface and concrete commands for node editor operations.

#include "node_editor/NodeEditorTypes.h"
#include "node_editor/NodeGraph.h"

#include <memory>
#include <string>

namespace markamp::node_editor
{

// ============================================================================
// Command Interface
// ============================================================================

/// Abstract base for undoable node editor commands.
class INodeCommand
{
public:
    INodeCommand() = default;
    virtual ~INodeCommand() = default;
    INodeCommand(const INodeCommand&) = default;
    INodeCommand(INodeCommand&&) = default;
    auto operator=(const INodeCommand&) -> INodeCommand& = default;
    auto operator=(INodeCommand&&) -> INodeCommand& = default;

    virtual void execute(NodeGraph& graph) = 0;
    virtual void undo(NodeGraph& graph) = 0;
    [[nodiscard]] virtual auto description() const -> std::string = 0;
};

// ============================================================================
// Add Node Command
// ============================================================================

class AddNodeCommand : public INodeCommand
{
public:
    AddNodeCommand(std::string type_name, Vec2 position);

    void execute(NodeGraph& graph) override;
    void undo(NodeGraph& graph) override;
    [[nodiscard]] auto description() const -> std::string override;

    [[nodiscard]] auto created_node_id() const -> NodeId
    {
        return created_id_;
    }

private:
    std::string type_name_;
    Vec2 position_;
    NodeId created_id_;
};

// ============================================================================
// Remove Node Command
// ============================================================================

class RemoveNodeCommand : public INodeCommand
{
public:
    explicit RemoveNodeCommand(NodeId node_id);

    void execute(NodeGraph& graph) override;
    void undo(NodeGraph& graph) override;
    [[nodiscard]] auto description() const -> std::string override;

private:
    NodeId node_id_;
    // Stashed state for undo
    GraphNode stashed_node_;
    std::vector<GraphSocket> stashed_sockets_;
    std::vector<GraphLink> stashed_links_;
};

// ============================================================================
// Move Node Command
// ============================================================================

class MoveNodeCommand : public INodeCommand
{
public:
    MoveNodeCommand(NodeId node_id, Vec2 new_position);

    void execute(NodeGraph& graph) override;
    void undo(NodeGraph& graph) override;
    [[nodiscard]] auto description() const -> std::string override;

private:
    NodeId node_id_;
    Vec2 new_position_;
    Vec2 old_position_;
};

// ============================================================================
// Add Link Command
// ============================================================================

class AddLinkCommand : public INodeCommand
{
public:
    AddLinkCommand(SocketId source, SocketId target);

    void execute(NodeGraph& graph) override;
    void undo(NodeGraph& graph) override;
    [[nodiscard]] auto description() const -> std::string override;

    [[nodiscard]] auto created_link_id() const -> LinkId
    {
        return created_id_;
    }

private:
    SocketId source_;
    SocketId target_;
    LinkId created_id_;
};

// ============================================================================
// Remove Link Command
// ============================================================================

class RemoveLinkCommand : public INodeCommand
{
public:
    explicit RemoveLinkCommand(LinkId link_id);

    void execute(NodeGraph& graph) override;
    void undo(NodeGraph& graph) override;
    [[nodiscard]] auto description() const -> std::string override;

private:
    LinkId link_id_;
    // Stashed state for undo
    SocketId source_;
    SocketId target_;
};

} // namespace markamp::node_editor
