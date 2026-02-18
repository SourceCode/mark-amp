// ============================================================================
// File: src/canvas/DiagramCoordinator.cpp
// Phase 12: Canvas Advanced Objects — diagram orchestration
// ============================================================================
#include "canvas/DiagramCoordinator.h"

#include "canvas/DiagramShapeObject.h"
#include "canvas/UndoRedoStack.h"

#include <algorithm>
#include <unordered_map>

namespace markamp::canvas
{

DiagramCoordinator::DiagramCoordinator(Board& board, UndoRedoStack& undo_stack)
    : board_(board)
    , undo_stack_(undo_stack)
{
}

// ── Class Diagrams ────────────────────────────────────────────────

auto DiagramCoordinator::create_class_diagram(const std::vector<ClassDiagramEntry>& classes,
                                              const std::vector<ClassRelationship>& relationships,
                                              const Point2D& origin) -> DiagramCreationResult
{
    DiagramCreationResult result;
    std::unordered_map<std::string, ObjectId> name_to_id;

    // Create class shapes
    for (const auto& cls : classes)
    {
        auto shape = std::make_unique<DiagramShapeObject>();
        shape->set_name(cls.class_name);
        shape->set_library(DiagramLibrary::UML);
        shape->set_uml_type(UMLShapeType::Class);
        shape->set_title(cls.class_name);

        // Build member text
        std::string member_text;
        for (const auto& member : cls.members)
        {
            const std::string visibility = member.is_public ? "+" : "-";
            if (member.is_method)
            {
                member_text += visibility + " " + member.name + "(): " + member.type_name + "\n";
            }
            else
            {
                member_text += visibility + " " + member.name + ": " + member.type_name + "\n";
            }
        }
        shape->set_compartments({member_text});

        const auto shape_id = board_.add_object(std::move(shape));
        name_to_id[cls.class_name] = shape_id;
        result.shape_ids.push_back(shape_id);
    }

    // Create relationship connectors
    for (const auto& rel : relationships)
    {
        const auto src_iter = name_to_id.find(rel.source_class);
        const auto tgt_iter = name_to_id.find(rel.target_class);
        if (src_iter != name_to_id.end() && tgt_iter != name_to_id.end())
        {
            const auto conn_id = create_connector(
                src_iter->second, tgt_iter->second, rel.label, rel.arrow, rel.line);
            result.connector_ids.push_back(conn_id);
        }
    }

    // Apply layout
    apply_layout(result.shape_ids, origin, LayoutType::Tree);

    if (!result.shape_ids.empty())
    {
        result.root_id = result.shape_ids.front();
    }
    result.success = true;
    return result;
}

// ── Sequence Diagrams ─────────────────────────────────────────────

auto DiagramCoordinator::create_sequence_diagram(
    const std::vector<std::string>& participants,
    const std::vector<std::pair<std::string, std::string>>& messages,
    const Point2D& origin) -> DiagramCreationResult
{
    DiagramCreationResult result;
    std::unordered_map<std::string, ObjectId> name_to_id;

    constexpr double kParticipantSpacing = 200.0;
    constexpr double kParticipantWidth = 120.0;
    constexpr double kParticipantHeight = 50.0;

    // Create participant boxes along the top
    for (size_t idx = 0; idx < participants.size(); ++idx)
    {
        auto shape = std::make_unique<DiagramShapeObject>();
        shape->set_name(participants[idx]);
        shape->set_library(DiagramLibrary::UML);
        shape->set_uml_type(UMLShapeType::Lifeline);
        shape->set_title(participants[idx]);

        Transform2D part_xform;
        part_xform.tx = origin.x + static_cast<double>(idx) * kParticipantSpacing;
        part_xform.ty = origin.y;
        shape->set_transform(part_xform);

        shape->set_dimensions(kParticipantWidth, kParticipantHeight);

        const auto shape_id = board_.add_object(std::move(shape));
        name_to_id[participants[idx]] = shape_id;
        result.shape_ids.push_back(shape_id);
    }

    // Create message arrows between participants
    for (size_t msg_idx = 0; msg_idx < messages.size(); ++msg_idx)
    {
        const auto& [from_name, to_name] = messages[msg_idx];
        const auto from_iter = name_to_id.find(from_name);
        const auto to_iter = name_to_id.find(to_name);

        if (from_iter != name_to_id.end() && to_iter != name_to_id.end())
        {
            const auto conn_id = create_connector(from_iter->second,
                                                  to_iter->second,
                                                  "",
                                                  ArrowheadStyle::kArrow,
                                                  ConnectorLineStyle::kSolid);
            result.connector_ids.push_back(conn_id);
        }
    }

    if (!result.shape_ids.empty())
    {
        result.root_id = result.shape_ids.front();
    }
    result.success = true;
    return result;
}

// ── Flowcharts ────────────────────────────────────────────────────

auto DiagramCoordinator::create_flowchart(const std::vector<FlowchartStep>& steps,
                                          const Point2D& origin) -> DiagramCreationResult
{
    DiagramCreationResult result;
    std::unordered_map<std::string, ObjectId> label_to_id;

    // Create shapes for each step
    for (const auto& step : steps)
    {
        auto shape = std::make_unique<DiagramShapeObject>();
        shape->set_name(step.label);
        shape->set_title(step.label);

        if (step.shape_type == "diamond")
        {
            shape->set_library(DiagramLibrary::BPMN);
            shape->set_bpmn_type(BPMNShapeType::Gateway);
        }
        else if (step.shape_type == "rounded")
        {
            shape->set_library(DiagramLibrary::BPMN);
            shape->set_bpmn_type(BPMNShapeType::StartEvent);
        }
        else if (step.shape_type == "parallelogram")
        {
            shape->set_library(DiagramLibrary::BPMN);
            shape->set_bpmn_type(BPMNShapeType::DataObject);
        }
        else
        {
            shape->set_library(DiagramLibrary::BPMN);
            shape->set_bpmn_type(BPMNShapeType::Task);
        }

        const auto shape_id = board_.add_object(std::move(shape));
        label_to_id[step.label] = shape_id;
        result.shape_ids.push_back(shape_id);
    }

    // Create connectors between steps
    for (const auto& step : steps)
    {
        const auto src_iter = label_to_id.find(step.label);
        if (src_iter == label_to_id.end())
        {
            continue;
        }

        for (const auto& next_label : step.next_steps)
        {
            const auto tgt_iter = label_to_id.find(next_label);
            if (tgt_iter != label_to_id.end())
            {
                const auto conn_id = create_connector(src_iter->second, tgt_iter->second);
                result.connector_ids.push_back(conn_id);
            }
        }
    }

    // Apply vertical tree layout
    apply_layout(result.shape_ids, origin, LayoutType::Vertical);

    if (!result.shape_ids.empty())
    {
        result.root_id = result.shape_ids.front();
    }
    result.success = true;
    return result;
}

// ── BPMN Process Diagrams ─────────────────────────────────────────

auto DiagramCoordinator::create_bpmn_process(const std::vector<BpmnElement>& elements,
                                             const Point2D& origin) -> DiagramCreationResult
{
    DiagramCreationResult result;
    std::unordered_map<std::string, ObjectId> name_to_id;

    for (const auto& elem : elements)
    {
        auto shape = std::make_unique<DiagramShapeObject>();
        shape->set_name(elem.name);
        shape->set_title(elem.name);
        shape->set_library(DiagramLibrary::BPMN);

        const auto shape_id = board_.add_object(std::move(shape));
        name_to_id[elem.name] = shape_id;
        result.shape_ids.push_back(shape_id);
    }

    // Create sequence flows
    for (const auto& elem : elements)
    {
        const auto src_iter = name_to_id.find(elem.name);
        if (src_iter == name_to_id.end())
        {
            continue;
        }

        for (const auto& outgoing_name : elem.outgoing)
        {
            const auto tgt_iter = name_to_id.find(outgoing_name);
            if (tgt_iter != name_to_id.end())
            {
                const auto conn_id = create_connector(src_iter->second, tgt_iter->second);
                result.connector_ids.push_back(conn_id);
            }
        }
    }

    // Apply horizontal layout for BPMN (left-to-right)
    apply_layout(result.shape_ids, origin, LayoutType::Horizontal);

    if (!result.shape_ids.empty())
    {
        result.root_id = result.shape_ids.front();
    }
    result.success = true;
    return result;
}

// ── Layout ────────────────────────────────────────────────────────

auto DiagramCoordinator::auto_layout(const std::vector<ObjectId>& shape_ids, LayoutType layout_type)
    -> void
{
    apply_layout(shape_ids, {0.0, 0.0}, layout_type);
}

// ── Query ─────────────────────────────────────────────────────────

auto DiagramCoordinator::supported_types() -> std::vector<DiagramType>
{
    return {DiagramType::kClassDiagram,
            DiagramType::kSequenceDiagram,
            DiagramType::kFlowchart,
            DiagramType::kBpmnProcess,
            DiagramType::kErdDiagram,
            DiagramType::kStateMachine};
}

auto DiagramCoordinator::diagram_type_name(DiagramType type) -> std::string
{
    switch (type)
    {
        case DiagramType::kClassDiagram:
            return "Class Diagram";
        case DiagramType::kSequenceDiagram:
            return "Sequence Diagram";
        case DiagramType::kFlowchart:
            return "Flowchart";
        case DiagramType::kBpmnProcess:
            return "BPMN Process";
        case DiagramType::kErdDiagram:
            return "ERD Diagram";
        case DiagramType::kStateMachine:
            return "State Machine";
    }
    return "Unknown";
}

// ── Private Helpers ───────────────────────────────────────────────

auto DiagramCoordinator::create_connector(ObjectId from_id,
                                          ObjectId to_id,
                                          const std::string& label,
                                          ArrowheadStyle arrow,
                                          ConnectorLineStyle line_style) -> ObjectId
{
    auto connector = std::make_unique<ConnectorObject>();
    connector->set_start_object(from_id, AnchorPosition::kAuto);
    connector->set_end_object(to_id, AnchorPosition::kAuto);
    connector->set_end_arrow(arrow);
    connector->set_line_style(line_style);

    if (!label.empty())
    {
        connector->set_label(label);
    }

    return board_.add_object(std::move(connector));
}

auto DiagramCoordinator::apply_layout(const std::vector<ObjectId>& shape_ids,
                                      const Point2D& origin,
                                      LayoutType type) -> void
{
    // Collect current bounds for layout computation
    std::vector<std::pair<ObjectId, AABB>> items;
    items.reserve(shape_ids.size());
    for (const auto shape_id : shape_ids)
    {
        const auto* obj = board_.get_object(shape_id);
        if (obj != nullptr)
        {
            items.emplace_back(shape_id, obj->world_bounds());
        }
    }

    LayoutOptions options;
    options.type = type;
    options.spacing = 60.0;
    options.center = origin;

    const auto layout_results = layout_engine_.compute_layout(items, options);

    // Apply computed positions
    for (const auto& [layout_id, new_pos] : layout_results)
    {
        auto* obj = board_.get_object_mut(layout_id);
        if (obj != nullptr)
        {
            auto layout_xform = obj->transform();
            layout_xform.tx = new_pos.x;
            layout_xform.ty = new_pos.y;
            obj->set_transform(layout_xform);
        }
    }
}

} // namespace markamp::canvas
