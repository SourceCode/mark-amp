// ============================================================================
// File: src/canvas/DiagramCoordinator.h
// Phase 12: Canvas Advanced Objects — diagram orchestration
// ============================================================================
#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasTypes.h"
#include "canvas/ConnectorData.h"
#include "canvas/LayoutEngine.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

class UndoRedoStack;

/// Type of diagram to create.
enum class DiagramType : uint8_t
{
    kClassDiagram,
    kSequenceDiagram,
    kFlowchart,
    kBpmnProcess,
    kErdDiagram,
    kStateMachine
};

/// Configuration for a class diagram member (field or method).
struct ClassMember
{
    std::string name;
    std::string type_name; ///< Return type for methods, field type for fields
    bool is_method{false};
    bool is_public{true};

    /// Whether this member is a field (not a method).
    [[nodiscard]] auto is_field() const noexcept -> bool
    {
        return !is_method;
    }

    /// Whether this member is private.
    [[nodiscard]] auto is_private() const noexcept -> bool
    {
        return !is_public;
    }

    // ── Round 3 Batch 3 (#28) ───────────────────────────────────

    /// (#28) Whether a type name is specified.
    [[nodiscard]] auto has_type() const noexcept -> bool
    {
        return !type_name.empty();
    }
};

/// Configuration for creating a class in a class diagram.
struct ClassDiagramEntry
{
    std::string class_name;
    std::vector<ClassMember> members;
    Point2D position{0.0, 0.0}; ///< Optional initial position (overridden by layout)

    /// Number of members in this class.
    [[nodiscard]] auto member_count() const noexcept -> std::size_t
    {
        return members.size();
    }

    /// Whether this class has any members.
    [[nodiscard]] auto has_members() const noexcept -> bool
    {
        return !members.empty();
    }
};

/// Configuration for a relationship between two classes.
struct ClassRelationship
{
    std::string source_class;
    std::string target_class;
    std::string label;
    ArrowheadStyle arrow{ArrowheadStyle::kArrow};
    ConnectorLineStyle line{ConnectorLineStyle::kSolid};

    // ── Round 3 Batch 3 (#29) ───────────────────────────────────

    /// (#29) Whether a relationship label is set.
    [[nodiscard]] auto has_label() const noexcept -> bool
    {
        return !label.empty();
    }
};

/// Configuration for a flowchart step.
struct FlowchartStep
{
    std::string label;
    std::string shape_type;              ///< "rectangle", "diamond", "rounded", "parallelogram"
    std::vector<std::string> next_steps; ///< Labels of connected steps

    /// Whether this step has no outgoing connections (terminal).
    [[nodiscard]] auto is_terminal() const noexcept -> bool
    {
        return next_steps.empty();
    }

    /// Whether this step branches to multiple next steps.
    [[nodiscard]] auto has_branches() const noexcept -> bool
    {
        return next_steps.size() > 1;
    }

    // ── Round 3 Batch 3-4 (#30-31) ──────────────────────────────

    /// (#30) Whether this is a decision step (diamond shape).
    [[nodiscard]] auto is_decision() const noexcept -> bool
    {
        return shape_type == "diamond";
    }

    /// (#31) Whether this step has outgoing connections.
    [[nodiscard]] auto has_connections() const noexcept -> bool
    {
        return !next_steps.empty();
    }
};

/// Configuration for a BPMN element.
struct BpmnElement
{
    std::string name;
    std::string element_type;          ///< "start", "task", "gateway", "end", "event"
    std::vector<std::string> outgoing; ///< Names of connected elements

    /// Whether this is a gateway element (decision point).
    [[nodiscard]] auto is_gateway() const noexcept -> bool
    {
        return element_type == "gateway";
    }

    // ── Round 3 Batch 4 (#32-35) ────────────────────────────────

    /// (#32) Whether this is a start element.
    [[nodiscard]] auto is_start() const noexcept -> bool
    {
        return element_type == "start";
    }

    /// (#33) Whether this is an end element.
    [[nodiscard]] auto is_end() const noexcept -> bool
    {
        return element_type == "end";
    }

    /// (#34) Whether this is a task element.
    [[nodiscard]] auto is_task() const noexcept -> bool
    {
        return element_type == "task";
    }

    /// (#35) Whether this element has outgoing connections.
    [[nodiscard]] auto has_outgoing() const noexcept -> bool
    {
        return !outgoing.empty();
    }
};

/// Result of diagram creation.
struct DiagramCreationResult
{
    bool success{false};
    std::vector<ObjectId> shape_ids;     ///< Created shape object IDs
    std::vector<ObjectId> connector_ids; ///< Created connector object IDs
    ObjectId root_id{kInvalidObjectId};  ///< ID of the main/root element
    std::string error_message;

    /// Total number of objects created (shapes + connectors).
    [[nodiscard]] auto total_count() const noexcept -> std::size_t
    {
        return shape_ids.size() + connector_ids.size();
    }

    /// Whether the diagram creation failed.
    [[nodiscard]] auto failed() const noexcept -> bool
    {
        return !success;
    }

    // ── Round 3 Batch 4 (#36-38) ────────────────────────────────

    /// (#36) Whether shapes were created.
    [[nodiscard]] auto has_shapes() const noexcept -> bool
    {
        return !shape_ids.empty();
    }

    /// (#37) Whether connectors were created.
    [[nodiscard]] auto has_connectors() const noexcept -> bool
    {
        return !connector_ids.empty();
    }

    /// (#38) Whether there's an error message.
    [[nodiscard]] auto has_error() const noexcept -> bool
    {
        return !error_message.empty();
    }
};

/// Orchestrates creation of complete diagrams using DiagramShapeObject,
/// ConnectorObject, and LayoutEngine.
///
/// Provides:
///   - Class diagram creation from specifications
///   - Sequence diagram generation
///   - Flowchart creation with decision branches
///   - BPMN process diagram generation
///   - Automatic layout and connector routing
class DiagramCoordinator
{
public:
    DiagramCoordinator(Board& board, UndoRedoStack& undo_stack);

    // ── Class Diagrams ────────────────────────────────────────────

    /// Create a class diagram from class specifications and relationships.
    [[nodiscard]] auto create_class_diagram(const std::vector<ClassDiagramEntry>& classes,
                                            const std::vector<ClassRelationship>& relationships,
                                            const Point2D& origin = {100.0, 100.0})
        -> DiagramCreationResult;

    // ── Sequence Diagrams ─────────────────────────────────────────

    /// Create a sequence diagram from participant names and message pairs.
    [[nodiscard]] auto
    create_sequence_diagram(const std::vector<std::string>& participants,
                            const std::vector<std::pair<std::string, std::string>>& messages,
                            const Point2D& origin = {100.0, 100.0}) -> DiagramCreationResult;

    // ── Flowcharts ────────────────────────────────────────────────

    /// Create a flowchart from step definitions.
    [[nodiscard]] auto create_flowchart(const std::vector<FlowchartStep>& steps,
                                        const Point2D& origin = {100.0, 100.0})
        -> DiagramCreationResult;

    // ── BPMN Process Diagrams ─────────────────────────────────────

    /// Create a BPMN process diagram from element definitions.
    [[nodiscard]] auto create_bpmn_process(const std::vector<BpmnElement>& elements,
                                           const Point2D& origin = {100.0, 100.0})
        -> DiagramCreationResult;

    // ── Layout ────────────────────────────────────────────────────

    /// Apply automatic layout to existing diagram shapes.
    auto auto_layout(const std::vector<ObjectId>& shape_ids,
                     LayoutType layout_type = LayoutType::Tree) -> void;

    // ── Query ─────────────────────────────────────────────────────

    /// Get all supported diagram types.
    [[nodiscard]] static auto supported_types() -> std::vector<DiagramType>;

    /// Get a human-readable name for a diagram type.
    [[nodiscard]] static auto diagram_type_name(DiagramType type) -> std::string;

private:
    Board& board_;
    [[maybe_unused]] UndoRedoStack& undo_stack_;
    LayoutEngine layout_engine_;

    /// Create a connector between two shapes.
    auto create_connector(ObjectId from_id,
                          ObjectId to_id,
                          const std::string& label = "",
                          ArrowheadStyle arrow = ArrowheadStyle::kArrow,
                          ConnectorLineStyle line_style = ConnectorLineStyle::kSolid) -> ObjectId;

    /// Apply tree layout to the given shape IDs starting from origin.
    auto apply_layout(const std::vector<ObjectId>& shape_ids,
                      const Point2D& origin,
                      LayoutType type = LayoutType::Tree) -> void;
};

} // namespace markamp::canvas
