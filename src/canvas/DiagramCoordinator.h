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
};

/// Configuration for creating a class in a class diagram.
struct ClassDiagramEntry
{
    std::string class_name;
    std::vector<ClassMember> members;
    Point2D position{0.0, 0.0}; ///< Optional initial position (overridden by layout)
};

/// Configuration for a relationship between two classes.
struct ClassRelationship
{
    std::string source_class;
    std::string target_class;
    std::string label;
    ArrowheadStyle arrow{ArrowheadStyle::kArrow};
    ConnectorLineStyle line{ConnectorLineStyle::kSolid};
};

/// Configuration for a flowchart step.
struct FlowchartStep
{
    std::string label;
    std::string shape_type;              ///< "rectangle", "diamond", "rounded", "parallelogram"
    std::vector<std::string> next_steps; ///< Labels of connected steps
};

/// Configuration for a BPMN element.
struct BpmnElement
{
    std::string name;
    std::string element_type;          ///< "start", "task", "gateway", "end", "event"
    std::vector<std::string> outgoing; ///< Names of connected elements
};

/// Result of diagram creation.
struct DiagramCreationResult
{
    bool success{false};
    std::vector<ObjectId> shape_ids;     ///< Created shape object IDs
    std::vector<ObjectId> connector_ids; ///< Created connector object IDs
    ObjectId root_id{kInvalidObjectId};  ///< ID of the main/root element
    std::string error_message;
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
