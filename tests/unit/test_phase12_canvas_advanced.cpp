// ============================================================================
// File: tests/unit/test_phase12_canvas_advanced.cpp
// Phase 12: Canvas Advanced Objects & Diagrams — comprehensive unit tests
// ============================================================================
#include <catch2/catch_test_macros.hpp>

// ── Headers under test ──
#include "canvas/CanvasClipboardService.h"
#include "canvas/CanvasExportService.h"
#include "canvas/CanvasObjectAligner.h"
#include "canvas/CanvasObjectFactory.h"
#include "canvas/CanvasObjectLifecycleEvents.h"
#include "canvas/CanvasStylePresetService.h"
#include "canvas/ConnectorRoutingService.h"
#include "canvas/DiagramCoordinator.h"
#include "core/EventBus.h"

using namespace markamp::canvas;

// Helper to create a concrete CanvasObject via the factory.
static auto make_object(CanvasObjectType type) -> std::unique_ptr<CanvasObject>
{
    CanvasObjectFactory factory;
    auto result = factory.create(type);
    return std::move(result.object);
}

// Helper to create a shared EventBus for SelectionManager.
static auto make_event_bus() -> std::shared_ptr<markamp::core::EventBus>
{
    return std::make_shared<markamp::core::EventBus>();
}

// ============================================================================
// CanvasClipboardService Tests
// ============================================================================

TEST_CASE("ClipboardService: default state is empty", "[canvas][clipboard]")
{
    Board board;
    SelectionManager selection(make_event_bus());
    UndoRedoStack undo;
    CanvasClipboardService clipboard(board, selection, undo);

    REQUIRE_FALSE(clipboard.has_data());
    REQUIRE(clipboard.clipboard_count() == 0);
}

TEST_CASE("ClipboardService: copy requires selection", "[canvas][clipboard]")
{
    Board board;
    SelectionManager selection(make_event_bus());
    UndoRedoStack undo;
    CanvasClipboardService clipboard(board, selection, undo);

    REQUIRE_FALSE(clipboard.copy());
    REQUIRE_FALSE(clipboard.has_data());
}

TEST_CASE("ClipboardService: copy stores selected objects", "[canvas][clipboard]")
{
    Board board;
    auto obj = make_object(CanvasObjectType::StickyNote);
    const auto id = board.add_object(std::move(obj));

    SelectionManager selection(make_event_bus());
    selection.add_to_selection(id);
    UndoRedoStack undo;
    CanvasClipboardService clipboard(board, selection, undo);

    REQUIRE(clipboard.copy());
    REQUIRE(clipboard.has_data());
    REQUIRE(clipboard.clipboard_count() == 1);
}

TEST_CASE("ClipboardService: paste returns failure when deserialization unimplemented",
          "[canvas][clipboard]")
{
    Board board;
    auto obj = make_object(CanvasObjectType::StickyNote);
    const auto id = board.add_object(std::move(obj));

    SelectionManager selection(make_event_bus());
    selection.add_to_selection(id);
    UndoRedoStack undo;
    CanvasClipboardService clipboard(board, selection, undo);

    REQUIRE(clipboard.copy());
    auto result = clipboard.paste();
    // Deserialization is now implemented for StickyNote objects
    REQUIRE(result.success);
}

TEST_CASE("ClipboardService: paste without data fails", "[canvas][clipboard]")
{
    Board board;
    SelectionManager selection(make_event_bus());
    UndoRedoStack undo;
    CanvasClipboardService clipboard(board, selection, undo);

    auto result = clipboard.paste();
    REQUIRE_FALSE(result.success);
}

TEST_CASE(
    "ClipboardService: duplicate_selection returns failure when deserialization unimplemented",
    "[canvas][clipboard]")
{
    Board board;
    auto obj = make_object(CanvasObjectType::StickyNote);
    const auto id = board.add_object(std::move(obj));

    SelectionManager selection(make_event_bus());
    selection.add_to_selection(id);
    UndoRedoStack undo;
    CanvasClipboardService clipboard(board, selection, undo);

    auto result = clipboard.duplicate_selection();
    // Deserialization is now implemented for StickyNote objects
    REQUIRE(result.success);
}

TEST_CASE("ClipboardService: clear empties clipboard", "[canvas][clipboard]")
{
    Board board;
    auto obj = make_object(CanvasObjectType::StickyNote);
    const auto id = board.add_object(std::move(obj));

    SelectionManager selection(make_event_bus());
    selection.add_to_selection(id);
    UndoRedoStack undo;
    CanvasClipboardService clipboard(board, selection, undo);

    clipboard.copy();
    REQUIRE(clipboard.has_data());
    clipboard.clear();
    REQUIRE_FALSE(clipboard.has_data());
}

TEST_CASE("ClipboardService: paste offset configuration", "[canvas][clipboard]")
{
    Board board;
    SelectionManager selection(make_event_bus());
    UndoRedoStack undo;
    CanvasClipboardService clipboard(board, selection, undo);

    clipboard.set_paste_offset({30.0, 30.0});
    REQUIRE(clipboard.paste_offset().x == 30.0);
    REQUIRE(clipboard.paste_offset().y == 30.0);
}

// ============================================================================
// CanvasStylePresetService Tests
// ============================================================================

TEST_CASE("StylePresetService: loads builtin presets", "[canvas][styles]")
{
    Board board;
    CanvasStylePresetService styles(board);

    REQUIRE(styles.preset_count() > 0);
    REQUIRE(styles.preset_count() == 11);
}

TEST_CASE("StylePresetService: find preset by name", "[canvas][styles]")
{
    Board board;
    CanvasStylePresetService styles(board);

    const auto* preset = styles.find_preset("Yellow Sticky");
    REQUIRE(preset != nullptr);
    REQUIRE(preset->category == "Sticky Notes");
    REQUIRE(preset->fill_color.has_value());
}

TEST_CASE("StylePresetService: presets_in_category filters correctly", "[canvas][styles]")
{
    Board board;
    CanvasStylePresetService styles(board);

    auto sticky_presets = styles.presets_in_category("Sticky Notes");
    REQUIRE(sticky_presets.size() == 6);

    auto shape_presets = styles.presets_in_category("Shapes");
    REQUIRE(shape_presets.size() == 3);
}

TEST_CASE("StylePresetService: add and remove custom preset", "[canvas][styles]")
{
    Board board;
    CanvasStylePresetService styles(board);
    const auto initial_count = styles.preset_count();

    StylePreset custom;
    custom.name = "Custom Purple";
    custom.category = "Custom";
    custom.fill_color = CanvasColor{128, 0, 128, 255};
    styles.add_preset(custom);
    REQUIRE(styles.preset_count() == initial_count + 1);

    REQUIRE(styles.find_preset("Custom Purple") != nullptr);
    REQUIRE(styles.remove_preset("Custom Purple"));
    REQUIRE(styles.preset_count() == initial_count);
}

TEST_CASE("StylePresetService: apply_preset modifies object", "[canvas][styles]")
{
    Board board;
    auto obj = make_object(CanvasObjectType::StickyNote);
    const auto id = board.add_object(std::move(obj));

    CanvasStylePresetService styles(board);

    StylePreset preset;
    preset.name = "Test";
    preset.opacity = 0.5;

    auto result = styles.apply_preset(id, preset);
    REQUIRE(result.success);
    REQUIRE(result.objects_modified == 1);
}

TEST_CASE("StylePresetService: create_preset_from_object", "[canvas][styles]")
{
    Board board;
    auto obj = make_object(CanvasObjectType::StickyNote);
    obj->set_opacity(0.8);
    const auto id = board.add_object(std::move(obj));

    CanvasStylePresetService styles(board);
    auto preset = styles.create_preset_from_object(id, "Extracted");
    REQUIRE(preset.name == "Extracted");
    REQUIRE(preset.opacity.has_value());
    REQUIRE(preset.opacity.value() == 0.8);
}

TEST_CASE("StylePresetService: category_names returns all categories", "[canvas][styles]")
{
    auto names = CanvasStylePresetService::category_names();
    REQUIRE(names.size() == 5);
}

// ============================================================================
// ConnectorRoutingService Tests
// ============================================================================

TEST_CASE("RoutingService: default config", "[canvas][routing]")
{
    Board board;
    ConnectorRoutingService routing(board);

    REQUIRE(routing.config().min_segment_length == 20.0);
    REQUIRE(routing.config().obstacle_padding == 10.0);
    REQUIRE(routing.config().anchor_snap_distance == 15.0);
}

TEST_CASE("RoutingService: straight route has no waypoints", "[canvas][routing]")
{
    Board board;
    ConnectorRoutingService routing(board);

    auto result = routing.route_between({0, 0}, {100, 100}, ConnectorRouting::kStraight);
    REQUIRE(result.success);
    REQUIRE(result.waypoints.empty());
    REQUIRE(result.total_length > 0.0);
}

TEST_CASE("RoutingService: orthogonal route has waypoints", "[canvas][routing]")
{
    Board board;
    ConnectorRoutingService routing(board);

    auto result = routing.route_between({0, 0}, {200, 200}, ConnectorRouting::kOrthogonal);
    REQUIRE(result.success);
    REQUIRE_FALSE(result.waypoints.empty());
}

TEST_CASE("RoutingService: curved route has control points", "[canvas][routing]")
{
    Board board;
    ConnectorRoutingService routing(board);

    auto result = routing.route_between({0, 0}, {200, 200}, ConnectorRouting::kCurved);
    REQUIRE(result.success);
    REQUIRE(result.waypoints.size() == 3); // Control points for curved route
}

TEST_CASE("RoutingService: custom config", "[canvas][routing]")
{
    Board board;
    RoutingConfig config;
    config.min_segment_length = 50.0;
    config.obstacle_padding = 20.0;
    ConnectorRoutingService routing(board, config);

    REQUIRE(routing.config().min_segment_length == 50.0);
    REQUIRE(routing.config().obstacle_padding == 20.0);
}

TEST_CASE("RoutingService: reroute_all on empty board", "[canvas][routing]")
{
    Board board;
    ConnectorRoutingService routing(board);

    auto results = routing.reroute_all();
    REQUIRE(results.empty());
}

// ============================================================================
// DiagramCoordinator Tests
// ============================================================================

TEST_CASE("DiagramCoordinator: supported diagram types", "[canvas][diagrams]")
{
    auto types = DiagramCoordinator::supported_types();
    REQUIRE(types.size() == 6);
}

TEST_CASE("DiagramCoordinator: diagram type names", "[canvas][diagrams]")
{
    REQUIRE(DiagramCoordinator::diagram_type_name(DiagramType::kClassDiagram) == "Class Diagram");
    REQUIRE(DiagramCoordinator::diagram_type_name(DiagramType::kFlowchart) == "Flowchart");
    REQUIRE(DiagramCoordinator::diagram_type_name(DiagramType::kBpmnProcess) == "BPMN Process");
}

TEST_CASE("DiagramCoordinator: create class diagram", "[canvas][diagrams]")
{
    Board board;
    UndoRedoStack undo;
    DiagramCoordinator coordinator(board, undo);

    std::vector<ClassDiagramEntry> classes = {
        {"Animal", {{"name", "string", false, true}, {"speak", "void", true, true}}, {}},
        {"Dog", {{"breed", "string", false, true}}, {}}};

    std::vector<ClassRelationship> rels = {
        {"Dog", "Animal", "extends", ArrowheadStyle::kFilledArrow, ConnectorLineStyle::kSolid}};

    auto result = coordinator.create_class_diagram(classes, rels);
    REQUIRE(result.success);
    REQUIRE(result.shape_ids.size() == 2);
    REQUIRE(result.connector_ids.size() == 1);
}

TEST_CASE("DiagramCoordinator: create flowchart", "[canvas][diagrams]")
{
    Board board;
    UndoRedoStack undo;
    DiagramCoordinator coordinator(board, undo);

    std::vector<FlowchartStep> steps = {{"Start", "rounded", {"Process"}},
                                        {"Process", "rectangle", {"Decision"}},
                                        {"Decision", "diamond", {"End"}},
                                        {"End", "rounded", {}}};

    auto result = coordinator.create_flowchart(steps);
    REQUIRE(result.success);
    REQUIRE(result.shape_ids.size() == 4);
    REQUIRE(result.connector_ids.size() == 3);
}

TEST_CASE("DiagramCoordinator: create BPMN process", "[canvas][diagrams]")
{
    Board board;
    UndoRedoStack undo;
    DiagramCoordinator coordinator(board, undo);

    std::vector<BpmnElement> elements = {{"Start", "start", {"Task1"}},
                                         {"Task1", "task", {"Gateway"}},
                                         {"Gateway", "gateway", {"End"}},
                                         {"End", "end", {}}};

    auto result = coordinator.create_bpmn_process(elements);
    REQUIRE(result.success);
    REQUIRE(result.shape_ids.size() == 4);
    REQUIRE(result.connector_ids.size() == 3);
}

// ============================================================================
// CanvasObjectLifecycleEvents Tests
// ============================================================================

TEST_CASE("EventEmitter: default has no subscriptions", "[canvas][events]")
{
    CanvasObjectEventEmitter emitter;
    REQUIRE(emitter.subscription_count() == 0);
    REQUIRE(emitter.recent_events().empty());
}

TEST_CASE("EventEmitter: emit records to history", "[canvas][events]")
{
    CanvasObjectEventEmitter emitter;

    emitter.emit_created(1, CanvasObjectType::StickyNote, "Note 1");
    REQUIRE(emitter.recent_events().size() == 1);
    REQUIRE(emitter.recent_events().front().type == CanvasLifecycleEventType::kObjectCreated);
}

TEST_CASE("EventEmitter: subscribe receives events", "[canvas][events]")
{
    CanvasObjectEventEmitter emitter;
    int event_count = 0;

    emitter.subscribe([&](const CanvasLifecycleEvent&) { ++event_count; });

    emitter.emit_created(1, CanvasObjectType::StickyNote, "Note 1");
    emitter.emit_deleted(2, CanvasObjectType::Shape, "Shape 1");
    REQUIRE(event_count == 2);
}

TEST_CASE("EventEmitter: subscribe_to filters by type", "[canvas][events]")
{
    CanvasObjectEventEmitter emitter;
    int move_count = 0;

    emitter.subscribe_to(CanvasLifecycleEventType::kObjectMoved,
                         [&](const CanvasLifecycleEvent&) { ++move_count; });

    emitter.emit_created(1, CanvasObjectType::StickyNote, "Note");
    emitter.emit_moved(1, {0, 0}, {100, 100});
    emitter.emit_deleted(1, CanvasObjectType::StickyNote, "Note");

    REQUIRE(move_count == 1); // Only the move event
}

TEST_CASE("EventEmitter: unsubscribe stops delivery", "[canvas][events]")
{
    CanvasObjectEventEmitter emitter;
    int event_count = 0;

    auto sub_id = emitter.subscribe([&](const CanvasLifecycleEvent&) { ++event_count; });

    emitter.emit_created(1, CanvasObjectType::StickyNote, "Note");
    REQUIRE(event_count == 1);

    emitter.unsubscribe(sub_id);
    emitter.emit_created(2, CanvasObjectType::StickyNote, "Note 2");
    REQUIRE(event_count == 1); // No increment
}

TEST_CASE("EventEmitter: event type names", "[canvas][events]")
{
    REQUIRE(CanvasObjectEventEmitter::event_type_name(CanvasLifecycleEventType::kObjectCreated) ==
            "ObjectCreated");
    REQUIRE(CanvasObjectEventEmitter::event_type_name(CanvasLifecycleEventType::kObjectMoved) ==
            "ObjectMoved");
}

TEST_CASE("EventEmitter: clear_history empties events", "[canvas][events]")
{
    CanvasObjectEventEmitter emitter;
    emitter.emit_created(1, CanvasObjectType::StickyNote, "Note");
    REQUIRE_FALSE(emitter.recent_events().empty());
    emitter.clear_history();
    REQUIRE(emitter.recent_events().empty());
}

TEST_CASE("EventEmitter: batch emission", "[canvas][events]")
{
    CanvasObjectEventEmitter emitter;
    int event_count = 0;
    emitter.subscribe([&](const CanvasLifecycleEvent&) { ++event_count; });

    std::vector<CanvasLifecycleEvent> events;
    CanvasLifecycleEvent ev1;
    ev1.type = CanvasLifecycleEventType::kObjectMoved;
    ev1.object_id = 1;
    CanvasLifecycleEvent ev2;
    ev2.type = CanvasLifecycleEventType::kObjectMoved;
    ev2.object_id = 2;
    events.push_back(ev1);
    events.push_back(ev2);

    emitter.emit_batch(events);
    REQUIRE(event_count == 2);
    REQUIRE(emitter.recent_events().size() == 2);
}

// ============================================================================
// CanvasExportService Tests
// ============================================================================

TEST_CASE("ExportService: empty board export fails", "[canvas][export]")
{
    Board board;
    CanvasExportService exporter(board);

    auto result = exporter.export_svg();
    REQUIRE_FALSE(result.success);
}

TEST_CASE("ExportService: SVG export produces valid output", "[canvas][export]")
{
    Board board;
    auto obj = make_object(CanvasObjectType::StickyNote);
    obj->set_name("Test Note");
    board.add_object(std::move(obj));

    CanvasExportService exporter(board);
    auto result = exporter.export_svg();
    REQUIRE(result.success);
    REQUIRE(result.object_count == 1);
    REQUIRE(result.data.find("<svg") != std::string::npos);
    REQUIRE(result.data.find("</svg>") != std::string::npos);
}

TEST_CASE("ExportService: JSON export", "[canvas][export]")
{
    Board board;
    auto obj = make_object(CanvasObjectType::StickyNote);
    board.add_object(std::move(obj));

    CanvasExportService exporter(board);
    auto result = exporter.export_json();
    REQUIRE(result.success);
    REQUIRE(result.data.find("objects") != std::string::npos);
}

TEST_CASE("ExportService: format utilities", "[canvas][export]")
{
    REQUIRE(CanvasExportService::format_extension(ExportFormat::kSvg) == ".svg");
    REQUIRE(CanvasExportService::format_extension(ExportFormat::kPng) == ".png");
    REQUIRE(CanvasExportService::format_extension(ExportFormat::kJson) == ".json");
    REQUIRE(CanvasExportService::format_name(ExportFormat::kSvg) == "SVG");
    REQUIRE(CanvasExportService::supported_formats().size() == 4);
}

TEST_CASE("ExportService: export options with scale", "[canvas][export]")
{
    Board board;
    auto obj = make_object(CanvasObjectType::StickyNote);
    board.add_object(std::move(obj));

    CanvasExportService exporter(board);
    ExportOptions options;
    options.scale = 2.0;
    auto result = exporter.export_svg(options);
    REQUIRE(result.success);
}

// ============================================================================
// CanvasObjectAligner Tests
// ============================================================================

TEST_CASE("Aligner: align requires at least 2 objects", "[canvas][alignment]")
{
    Board board;
    auto obj = make_object(CanvasObjectType::StickyNote);
    const auto id = board.add_object(std::move(obj));

    CanvasObjectAligner aligner(board);
    auto result = aligner.align({id}, AlignDirection::kLeft);
    REQUIRE_FALSE(result.success);
}

TEST_CASE("Aligner: align left", "[canvas][alignment]")
{
    Board board;

    auto obj1 = make_object(CanvasObjectType::StickyNote);
    obj1->set_position(100.0, 50.0);
    const auto id1 = board.add_object(std::move(obj1));

    auto obj2 = make_object(CanvasObjectType::StickyNote);
    obj2->set_position(200.0, 150.0);
    const auto id2 = board.add_object(std::move(obj2));

    CanvasObjectAligner aligner(board);
    auto result = aligner.align({id1, id2}, AlignDirection::kLeft);
    REQUIRE(result.success);
    REQUIRE(result.objects_moved >= 1);
}

TEST_CASE("Aligner: distribute requires at least 3 objects", "[canvas][alignment]")
{
    Board board;
    auto obj1 = make_object(CanvasObjectType::StickyNote);
    auto obj2 = make_object(CanvasObjectType::StickyNote);
    const auto id1 = board.add_object(std::move(obj1));
    const auto id2 = board.add_object(std::move(obj2));

    CanvasObjectAligner aligner(board);
    auto result = aligner.distribute({id1, id2}, DistributeMode::kHorizontal);
    REQUIRE_FALSE(result.success);
}

TEST_CASE("Aligner: direction and mode names", "[canvas][alignment]")
{
    REQUIRE(CanvasObjectAligner::direction_name(AlignDirection::kLeft) == "Align Left");
    REQUIRE(CanvasObjectAligner::direction_name(AlignDirection::kCenterHorizontal) ==
            "Center Horizontally");
    REQUIRE(CanvasObjectAligner::distribute_mode_name(DistributeMode::kHorizontal) ==
            "Distribute Horizontally");
}

TEST_CASE("Aligner: smart alignment suggestions", "[canvas][alignment]")
{
    Board board;

    // Create three objects with similar left edges
    auto obj1 = make_object(CanvasObjectType::StickyNote);
    obj1->set_position(100.0, 0.0);
    const auto id1 = board.add_object(std::move(obj1));

    auto obj2 = make_object(CanvasObjectType::StickyNote);
    obj2->set_position(102.0, 50.0); // Almost aligned
    const auto id2 = board.add_object(std::move(obj2));

    auto obj3 = make_object(CanvasObjectType::StickyNote);
    obj3->set_position(101.0, 100.0); // Almost aligned
    const auto id3 = board.add_object(std::move(obj3));

    CanvasObjectAligner aligner(board);
    auto suggestions = aligner.suggest_alignments({id1, id2, id3}, 5.0);
    // Should detect at least one alignment opportunity
    REQUIRE_FALSE(suggestions.empty());
}
