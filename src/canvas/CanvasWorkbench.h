// ============================================================================
// File: src/canvas/CanvasWorkbench.h
// Phase 11: Canvas Workbench Shell — workbench mode, board lifecycle, tool switching
// ============================================================================
#pragma once

#include "canvas/Board.h"
#include "canvas/BoardSerializer.h"
#include "canvas/ToolRailModel.h"
#include "canvas/CanvasToolStateMachine.h"
#include "canvas/KeyboardCommandModel.h"
#include "canvas/AutosaveModel.h"
#include "canvas/ContextMenuModel.h"
#include "canvas/InspectorModel.h"
#include "canvas/MinimapModel.h"
#include "canvas/OnboardingModel.h"
#include "canvas/BoardSettingsModel.h"
#include "canvas/CanvasThemeModel.h"
#include "canvas/CanvasAnalyticsModel.h"
#include "canvas/SmartSpacingModel.h"
#include "canvas/PrecisionNudgeModel.h"
#include "canvas/DragAutoscrollModel.h"
#include "canvas/PenEngineModel.h"
#include "canvas/PenPresetModel.h"
#include "ui/CommandPaletteModel.h"
#include "canvas/CursorFeedbackModel.h"
#include "canvas/GridRulerModel.h"
#include "canvas/SnapPreferenceModel.h"
#include "canvas/BoardTemplate.h"
#include "canvas/CanvasTool.h"
#include "canvas/CanvasTypes.h"
#include "canvas/SelectionManager.h"
#include "canvas/UndoRedoStack.h"
#include "canvas/ViewportTransform.h"
#include "core/EventBus.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

// Forward declarations
class SelectTool;
class PanTool;
class DrawTool;

// ── Workbench State ────────────────────────────────────────────────

/// Persistent state of the canvas workbench.
struct CanvasWorkbenchState
{
    std::string active_board_id; ///< Currently open board
    ToolMode active_tool{ToolMode::Select};
    double zoom_level{1.0};
    Point2D viewport_center{0.0, 0.0};
    bool grid_visible{true};
    bool snap_enabled{true};
    bool minimap_visible{true};
    bool inspector_visible{true};
    std::vector<std::string> recent_board_ids; ///< Most-recently-used boards

    /// Maximum number of recent boards to track.
    static constexpr size_t kMaxRecentBoards = 20;
};

/// Summary information for board listings (e.g. open/recent dialogs).
struct BoardSummary
{
    std::string id;
    std::string name;
    std::string description;
    size_t object_count{0};
    std::chrono::system_clock::time_point modified_at;
    bool is_dirty{false};
};

// ── Canvas Workbench ───────────────────────────────────────────────

/// Central controller for the canvas mode.
///
/// Responsibilities:
///   - Register canvas as a SurfaceKind mode
///   - Board lifecycle (open, create, close, recent history)
///   - Tool switching (Select, Pan, Draw, etc.)
///   - Wire to SelectionManager, UndoRedoStack, ViewportTransform
///   - Persist workbench state across sessions
class CanvasWorkbench
{
public:
    explicit CanvasWorkbench(std::shared_ptr<core::EventBus> event_bus);

    // ── Board Lifecycle ────────────────────────────────────────────

    /// Open an existing board from disk.
    auto open_board(const std::string& board_id, const std::filesystem::path& board_path) -> bool;

    /// Create a new blank board.
    auto create_board(const std::string& name) -> std::string;

    /// Create a board from a template.
    auto create_from_template(const std::string& name, const std::string& template_id)
        -> std::string;

    /// Close a board (prompts dirty-save upstream).
    auto close_board(const std::string& board_id) -> bool;

    /// Rename an open board.
    auto rename_board(const std::string& board_id, const std::string& new_name) -> bool;

    /// Duplicate an open board, returning the new board's ID.
    auto duplicate_board(const std::string& board_id) -> std::string;

    /// Save the active board to disk.
    auto save_board(const std::filesystem::path& path) -> bool;

    /// Whether a board with the given ID is currently open.
    [[nodiscard]] auto board_exists(const std::string& board_id) const -> bool;

    /// Switch the active board.
    auto set_active_board(const std::string& board_id) -> bool;

    /// Get the active board (non-const for editing).
    [[nodiscard]] auto active_board() -> Board*;
    [[nodiscard]] auto active_board() const -> const Board*;

    /// Get a board by ID.
    [[nodiscard]] auto get_board(const std::string& board_id) -> Board*;
    [[nodiscard]] auto get_board(const std::string& board_id) const -> const Board*;

    /// List all currently open boards.
    [[nodiscard]] auto open_boards() const -> std::vector<BoardSummary>;

    /// Get recently opened board IDs.
    [[nodiscard]] auto recent_boards() const -> const std::vector<std::string>&;

    // ── Tool Switching ─────────────────────────────────────────────

    /// Set the active tool mode.
    auto set_tool(ToolMode mode) -> void;

    /// Get the active tool mode.
    [[nodiscard]] auto active_tool_mode() const -> ToolMode;

    /// Get the active tool instance.
    [[nodiscard]] auto active_tool() -> ICanvasTool*;
    [[nodiscard]] auto active_tool() const -> const ICanvasTool*;

    /// Register an external tool for a specific mode.
    auto register_tool(ToolMode mode, std::unique_ptr<ICanvasTool> tool) -> void;

    // ── Sub-System Access ──────────────────────────────────────────

    [[nodiscard]] auto selection_manager() -> SelectionManager&;
    [[nodiscard]] auto selection_manager() const -> const SelectionManager&;

    [[nodiscard]] auto undo_stack() -> UndoRedoStack&;
    [[nodiscard]] auto undo_stack() const -> const UndoRedoStack&;

    [[nodiscard]] auto viewport() -> ViewportTransform&;
    [[nodiscard]] auto viewport() const -> const ViewportTransform&;

    [[nodiscard]] auto template_library() -> BoardTemplateLibrary&;
    [[nodiscard]] auto template_library() const -> const BoardTemplateLibrary&;

    // ── W03: Viewport Camera ──────────────────────────────────────

    /// Zoom to fit all objects on the active board.
    auto zoom_to_fit() -> void;

    /// Zoom to fit a specific selection region.
    auto zoom_to_selection(const AABB& selection) -> void;

    /// Reset viewport to default (zoom 100%, pan origin).
    auto reset_viewport() -> void;

    // ── W04: Tool Rail ────────────────────────────────────────────

    /// Select a tool by its string ID (via ToolRailModel).
    auto select_tool_by_id(const std::string& tool_id) -> bool;

    /// Quick-switch to the previous tool.
    auto quick_switch_tool() -> void;

    /// Access the tool rail model.
    [[nodiscard]] auto tool_rail() -> ToolRailModel&;
    [[nodiscard]] auto tool_rail() const -> const ToolRailModel&;

    // ── W06: Input State Machine ──────────────────────────────────

    /// Cancel the active gesture (Escape key).
    auto cancel_active_gesture() -> bool;

    /// Access the tool state machine.
    [[nodiscard]] auto tool_state_machine() -> CanvasToolStateMachine&;
    [[nodiscard]] auto tool_state_machine() const -> const CanvasToolStateMachine&;

    // ── W07: Keyboard Commanding ──────────────────────────────────

    /// Execute a keyboard command by ID.
    auto execute_key_command(const std::string& command_id) -> bool;

    /// Access the keyboard command model.
    [[nodiscard]] auto keyboard_commands() -> KeyboardCommandModel&;
    [[nodiscard]] auto keyboard_commands() const -> const KeyboardCommandModel&;

    // ── W08: Cursor & Mode Feedback ───────────────────────────────

    /// Set the cursor based on the current tool mode.
    auto set_cursor_for_tool(ToolMode mode) -> void;

    /// Access the cursor feedback model.
    [[nodiscard]] auto cursor_feedback() -> CursorFeedbackModel&;
    [[nodiscard]] auto cursor_feedback() const -> const CursorFeedbackModel&;

    // ── W09: Grid & Rulers ────────────────────────────────────────

    /// Toggle grid visibility.
    auto toggle_grid() -> void;

    /// Toggle ruler visibility.
    auto toggle_rulers() -> void;

    /// Access the grid/ruler model.
    [[nodiscard]] auto grid_ruler() -> GridRulerModel&;
    [[nodiscard]] auto grid_ruler() const -> const GridRulerModel&;

    // ── W10: Snap Preferences ─────────────────────────────────────

    /// Toggle a snap mode by name ("grid", "object", "angle").
    auto toggle_snap_mode(const std::string& mode) -> void;

    /// Access the snap preference model.
    [[nodiscard]] auto snap_prefs() -> SnapPreferenceModel&;
    [[nodiscard]] auto snap_prefs() const -> const SnapPreferenceModel&;

    // ── W11: Context Menus ─────────────────────────────────────────

    /// Show the context menu for the given scope and object type.
    auto show_context_menu(ContextScope scope, const std::string& object_type = {}) -> void;

    /// Access the context menu model.
    [[nodiscard]] auto context_menu_model() -> ContextMenuModel&;
    [[nodiscard]] auto context_menu_model() const -> const ContextMenuModel&;

    // ── W12: Undo & Redo Plumbing ─────────────────────────────────

    /// Perform undo and emit UndoExecutedEvent.
    auto perform_undo() -> bool;

    /// Perform redo and emit RedoExecutedEvent.
    auto perform_redo() -> bool;

    // ── W13: Autosave & Recovery ──────────────────────────────────

    /// Trigger an autosave for the active board.
    auto trigger_autosave() -> void;

    /// Access the autosave model.
    [[nodiscard]] auto autosave() -> AutosaveModel&;
    [[nodiscard]] auto autosave() const -> const AutosaveModel&;

    // ── W14: Inspector Shell ─────────────────────────────────────

    /// Toggle inspector panel open/closed.
    auto toggle_inspector() -> void;

    /// Access the inspector model.
    [[nodiscard]] auto inspector() -> InspectorModel&;
    [[nodiscard]] auto inspector() const -> const InspectorModel&;

    // ── W15: Minimap Shell ───────────────────────────────────────

    /// Toggle minimap visibility.
    auto toggle_minimap() -> void;

    /// Access the minimap model.
    [[nodiscard]] auto minimap() -> MinimapModel&;
    [[nodiscard]] auto minimap() const -> const MinimapModel&;

    // ── W16: Onboarding & Empty States ───────────────────────────

    /// Complete an onboarding step and emit event.
    auto complete_onboarding_step(const std::string& step_id) -> void;

    /// Access the onboarding model.
    [[nodiscard]] auto onboarding() -> OnboardingModel&;
    [[nodiscard]] auto onboarding() const -> const OnboardingModel&;

    // ── W17: Board Settings ───────────────────────────────────

    /// Apply a board setting and emit event.
    auto apply_board_setting(const std::string& key, const std::string& value) -> void;

    /// Access the board settings model.
    [[nodiscard]] auto board_settings() -> BoardSettingsModel&;
    [[nodiscard]] auto board_settings() const -> const BoardSettingsModel&;

    // ── W18: Theme Tokens ─────────────────────────────────────

    /// Resolve a theme token and emit event.
    auto resolve_theme_token(const std::string& token_name) -> std::string;

    /// Access the canvas theme model.
    [[nodiscard]] auto canvas_theme() -> CanvasThemeModel&;
    [[nodiscard]] auto canvas_theme() const -> const CanvasThemeModel&;

    // ── W19: Command Palette Integration ───────────────────────

    /// Open the command palette and emit event.
    auto open_command_palette() -> void;

    /// Access the command palette model.
    [[nodiscard]] auto command_palette() -> ui::CommandPaletteModel&;
    [[nodiscard]] auto command_palette() const -> const ui::CommandPaletteModel&;

    // ── W20: Event & Analytics Contracts ────────────────────────

    /// Record an analytics event and emit.
    auto record_analytics_event(const std::string& event_name, const std::string& category) -> void;

    /// Access the canvas analytics model.
    [[nodiscard]] auto canvas_analytics() -> CanvasAnalyticsModel&;
    [[nodiscard]] auto canvas_analytics() const -> const CanvasAnalyticsModel&;

    // ── P02-W01: Single Selection ─────────────────────────────────

    /// Select a single object by ID and emit event.
    auto select_object(ObjectId object_id) -> void;

    /// Clear all selection and emit event.
    auto clear_selection() -> void;

    // ── P02-W02: Multi Selection ──────────────────────────────────

    /// Box-select all objects within an AABB region and emit event.
    auto box_select(const AABB& region) -> void;

    /// Select all objects on the active board and emit event.
    auto select_all_objects() -> void;

    // ── P02-W03: Move Operations ──────────────────────────────────

    /// Begin a move operation on selected objects.
    auto begin_object_move() -> void;

    /// End a move operation with the given delta.
    auto end_object_move(double delta_x, double delta_y) -> void;

    // ── P02-W04: Resize Operations ────────────────────────────

    /// Begin a resize operation on selected objects.
    auto begin_resize(const std::string& handle_name) -> void;

    /// End a resize operation with the given scale.
    auto end_resize(double scale_x, double scale_y) -> void;

    // ── P02-W05: Rotation Operations ──────────────────────────

    /// Begin a rotate operation on selected objects.
    auto begin_rotate() -> void;

    /// End a rotate operation with the given angle.
    auto end_rotate(double angle_degrees) -> void;

    // ── P02-W06: Alignment Guides ─────────────────────────────

    /// Show alignment guides and emit event.
    auto show_alignment_guides(int guide_count, const std::string& axis) -> void;

    /// Hide alignment guides and emit event.
    auto hide_alignment_guides() -> void;

    // ── P02-W07: Object Snapping ──────────────────────────────

    /// Engage snap and emit event.
    auto engage_snap(const std::string& snap_type, const std::string& target_id) -> void;

    /// Disengage snap and emit event.
    auto disengage_snap() -> void;

    // ── P02-W08: Distribute & Align Actions ────────────────────

    /// Align selected objects and emit event.
    auto align_selected(const std::string& action) -> void;

    /// Distribute selected objects and emit event.
    auto distribute_selected(const std::string& axis) -> void;

    // ── P02-W09: Smart Spacing ────────────────────────────────

    /// Activate smart spacing and emit event.
    auto activate_smart_spacing() -> void;

    /// Deactivate smart spacing and emit event.
    auto deactivate_smart_spacing() -> void;

    /// Access the smart spacing model.
    [[nodiscard]] auto smart_spacing() -> SmartSpacingModel&;
    [[nodiscard]] auto smart_spacing() const -> const SmartSpacingModel&;

    // ── P02-W10: Grouping ────────────────────────────────────

    /// Group selected objects and emit event.
    auto group_selected() -> void;

    /// Ungroup selected group and emit event.
    auto ungroup_selected() -> void;

    // ── P02-W11: Frame Membership ───────────────────────────

    /// Add an object to a frame and emit event.
    auto add_to_frame(const std::string& frame_id, const std::string& object_id) -> void;

    /// Remove an object from a frame and emit event.
    auto remove_from_frame(const std::string& frame_id, const std::string& object_id) -> void;

    // ── P02-W12: Layers & Z Order ───────────────────────────

    /// Bring selected objects to front and emit event.
    auto bring_to_front() -> void;

    /// Send selected objects to back and emit event.
    auto send_to_back() -> void;

    // ── P02-W13: Locking & Pinning ──────────────────────────

    /// Lock selected objects and emit event.
    auto lock_selected() -> void;

    /// Unlock selected objects and emit event.
    auto unlock_selected() -> void;

    // ── P02-W14: Precision Nudge ────────────────────────────

    /// Execute a precision nudge and emit event.
    auto precision_nudge(NudgeDirection direction, bool big_step) -> void;

    /// Access the precision nudge model.
    [[nodiscard]] auto nudge_model() -> PrecisionNudgeModel&;
    [[nodiscard]] auto nudge_model() const -> const PrecisionNudgeModel&;

    // ── P02-W15: Duplicate & Paste In Place ───────────────────

    /// Duplicate selected objects in place and emit event.
    auto duplicate_in_place() -> void;

    /// Paste clipboard contents in place and emit event.
    auto paste_in_place() -> void;

    // ── P02-W16: Drag Autoscroll ───────────────────────────

    /// Start autoscrolling and emit event.
    auto start_autoscroll(AutoscrollDirection dir) -> void;

    /// Stop autoscrolling and emit event.
    auto stop_autoscroll() -> void;

    /// Access the autoscroll model.
    [[nodiscard]] auto autoscroll_model() -> DragAutoscrollModel&;
    [[nodiscard]] auto autoscroll_model() const -> const DragAutoscrollModel&;

    // ── P02-W17: Viewport-Aware Transforms ───────────────────

    /// Sync viewport transform and emit event.
    auto sync_viewport_transform() -> void;

    /// Clamp viewport and emit event.
    auto clamp_viewport() -> void;

    // ── P02-W18: Multi User Selection ────────────────────────

    /// Receive a remote selection and emit event.
    auto receive_remote_selection(const std::string& user_id, int object_count) -> void;

    /// Report a remote lock conflict and emit event.
    auto report_lock_conflict(const std::string& user_id, const std::string& object_id) -> void;

    // ── P02-W19: Transform History Quality ───────────────────

    /// Undo last transform action and emit event.
    auto undo_transform() -> void;

    /// Redo last undone transform and emit event.
    auto redo_transform() -> void;

    // ── P02-W20: Selection Action Surfaces ───────────────────

    /// Show an action surface and emit event.
    auto show_action_surface(const std::string& surface_type) -> void;

    /// Hide action surface and emit event.
    auto hide_action_surface() -> void;

    // ── P03-W01: Freehand Pen Engine ───────────────────────

    /// Capture a stroke and emit event.
    auto capture_stroke(int point_count) -> void;

    /// Smooth a stroke and emit event.
    auto smooth_stroke(const std::string& stroke_id) -> void;

    /// Access the pen engine model.
    [[nodiscard]] auto pen_engine() -> PenEngineModel&;
    [[nodiscard]] auto pen_engine() const -> const PenEngineModel&;

    // ── P03-W02: Pen Presets ──────────────────────────────

    /// Apply a pen preset and emit event.
    auto apply_pen_preset(const std::string& name) -> void;

    /// Save current pen state as preset and emit event.
    auto save_pen_preset(const std::string& name) -> void;

    /// Access the pen presets model.
    [[nodiscard]] auto pen_presets() -> PenPresetModel&;
    [[nodiscard]] auto pen_presets() const -> const PenPresetModel&;

    // ── P03-W03: Eraser & Lasso ──────────────────────────

    /// Erase strokes and emit event.
    auto erase_stroke(int count) -> void;

    /// Perform lasso selection and emit event.
    auto lasso_select(int count) -> void;

    // ── P03-W04: Shape Tool ───────────────────────────────

    /// Create a shape and emit event.
    auto create_shape(const std::string& type) -> void;

    /// Resize a shape and emit event.
    auto resize_shape(const std::string& shape_id) -> void;

    // ── P03-W05: Text Box Authoring ───────────────────────

    /// Create a text box and emit event.
    auto create_text_box() -> void;

    /// Edit a text box and emit event.
    auto edit_text_box(const std::string& text_box_id) -> void;

    // ── P03-W06: Sticky Notes ─────────────────────────────────

    /// Create a sticky note and emit event.
    auto create_sticky_note(const std::string& color) -> void;

    /// Change sticky note color and emit event.
    auto change_sticky_note_color(const std::string& note_id, const std::string& color) -> void;

    // ── P03-W07: Color Authoring ──────────────────────────────

    /// Select a palette color and emit event.
    auto select_palette_color(const std::string& color) -> void;

    /// Save a palette color and emit event.
    auto save_palette_color(const std::string& color) -> void;

    // ── P03-W08: Typography Controls ──────────────────────────

    /// Change a typography property and emit event.
    auto change_typography(const std::string& property, const std::string& value) -> void;

    /// Apply a font and emit event.
    auto apply_font(const std::string& font_name) -> void;

    // ── P03-W09: Fill, Stroke & Effects ───────────────────────

    /// Change fill style and emit event.
    auto change_fill_style(const std::string& object_id, const std::string& fill) -> void;

    /// Change stroke style and emit event.
    auto change_stroke_style(const std::string& object_id, const std::string& stroke) -> void;

    // ── P03-W10: Style Presets ────────────────────────────────

    /// Apply a style preset and emit event.
    auto apply_style_preset(const std::string& preset_name) -> void;

    /// Save a style preset and emit event.
    auto save_style_preset(const std::string& preset_name) -> void;

    // ── P03-W11: Inline Rich Text ─────────────────────────────

    /// Enable inline rich text on an object and emit event.
    auto enable_inline_rich_text(const std::string& object_id) -> void;

    /// Format inline rich text and emit event.
    auto format_inline_rich_text(const std::string& object_id, const std::string& format) -> void;

    // ── P03-W12: Shape Library Quick Insert ───────────────────

    /// Insert shape from library and emit event.
    auto insert_from_shape_library(const std::string& shape_name) -> void;

    /// Open shape library and emit event.
    auto open_shape_library() -> void;

    // ── P03-W13: Shape Recognition ────────────────────────────

    /// Recognize a shape from freehand and emit event.
    auto recognize_shape(const std::string& type, double confidence) -> void;

    /// Reject shape recognition and emit event.
    auto reject_shape_recognition(const std::string& stroke_id) -> void;

    // ── P03-W14: Image Annotation ─────────────────────────────

    /// Add image annotation and emit event.
    auto add_image_annotation(const std::string& image_id, const std::string& annotation_type) -> void;

    /// Remove image annotation and emit event.
    auto remove_image_annotation(const std::string& image_id, const std::string& annotation_id) -> void;

    // ── P03-W15: Comments & Callouts ──────────────────────────

    /// Add a comment to an object and emit event.
    auto add_comment(const std::string& object_id, const std::string& text) -> void;

    /// Create a callout and emit event.
    auto create_callout(const std::string& callout_type) -> void;

    // ── P03-W16: Tables For Authoring ─────────────────────────

    /// Create a canvas table and emit event.
    auto create_canvas_table(int rows, int columns) -> void;

    /// Edit a canvas table cell and emit event.
    auto edit_canvas_table_cell(const std::string& table_id, int row, int column) -> void;

    // ── P03-W17: Icons, Stickers & Emoji ──────────────────────

    /// Insert an icon and emit event.
    auto insert_icon(const std::string& icon_name) -> void;

    /// Insert a sticker and emit event.
    auto insert_sticker(const std::string& sticker_id) -> void;

    // ── P03-W18: Creator Shortcuts ────────────────────────────

    /// Invoke a creator shortcut and emit event.
    auto invoke_creator_shortcut(const std::string& shortcut_id) -> void;

    /// Register a creator shortcut and emit event.
    auto register_creator_shortcut(const std::string& shortcut_id, const std::string& action) -> void;

    // ── P03-W19: Bulk Style Editing ───────────────────────────

    /// Apply bulk style and emit event.
    auto apply_bulk_style(int object_count, const std::string& style_property) -> void;

    /// Reset bulk style and emit event.
    auto reset_bulk_style(int object_count) -> void;

    // ── P03-W20: Authoring Motion Feedback ────────────────────

    /// Start motion preview and emit event.
    auto start_motion_preview(const std::string& object_id, const std::string& motion_type) -> void;

    /// End motion preview and emit event.
    auto end_motion_preview(const std::string& object_id) -> void;

    // ── P04-W01: Connector Creation ───────────────────────────

    /// Create a connector between objects and emit event.
    auto create_connector(const std::string& from_id, const std::string& to_id, const std::string& connector_type) -> void;

    /// Delete a connector and emit event.
    auto delete_connector(const std::string& connector_id) -> void;

    // ── P04-W02: Connector Routing ────────────────────────────

    /// Change connector route type and emit event.
    auto change_connector_route(const std::string& connector_id, const std::string& route_type) -> void;

    /// Add a waypoint to a connector and emit event.
    auto add_connector_waypoint(const std::string& connector_id, double waypoint_x, double waypoint_y) -> void;

    // ── P04-W03: Connector Labels & Ports ─────────────────────

    /// Set a connector label and emit event.
    auto set_connector_label(const std::string& connector_id, const std::string& label) -> void;

    /// Assign a port on an object and emit event.
    auto assign_port(const std::string& object_id, const std::string& port_id) -> void;

    // ── P04-W04: Diagram Library ──────────────────────────────

    /// Insert a diagram shape from library and emit event.
    auto insert_diagram_shape(const std::string& shape_type, const std::string& category) -> void;

    /// Browse diagram library and emit event.
    auto browse_diagram_library(const std::string& category) -> void;

    // ── P04-W05: Flowchart Grammar ────────────────────────────

    /// Apply flowchart defaults and emit event.
    auto apply_flowchart_defaults(const std::string& grammar_name) -> void;

    /// Validate flowchart and emit event.
    auto validate_flowchart(int node_count, int connector_count, bool is_valid) -> void;

    // ── P04-W06: Mind Maps ────────────────────────────────────

    /// Add a mind map node and emit event.
    auto add_mind_map_node(const std::string& parent_id, const std::string& label) -> void;

    /// Toggle mind map branch and emit event.
    auto toggle_mind_map_branch(const std::string& node_id, bool collapsed) -> void;

    // ── P04-W07: Kanban Boards ────────────────────────────────

    /// Move a kanban card and emit event.
    auto move_canvas_kanban_card(const std::string& card_id, const std::string& from_lane, const std::string& to_lane) -> void;

    /// Add a kanban lane and emit event.
    auto add_kanban_lane(const std::string& lane_name) -> void;

    // ── P04-W08: Frames & Sections ────────────────────────────

    /// Create a frame and emit event.
    auto create_frame(const std::string& frame_name, double width, double height) -> void;

    /// Add a section to a frame and emit event.
    auto add_section(const std::string& frame_id, const std::string& section_name) -> void;

    // ── P04-W09: Swimlanes & Grids ───────────────────────────

    /// Create swimlane layout and emit event.
    auto create_swimlane(const std::string& orientation, int lane_count) -> void;

    /// Apply grid layout and emit event.
    auto apply_grid_layout(int rows, int columns) -> void;

    // ── P04-W10: Container Auto Layout ────────────────────────

    /// Trigger auto layout and emit event.
    auto trigger_auto_layout(const std::string& container_id, const std::string& layout_type) -> void;

    /// Change auto layout spacing and emit event.
    auto change_auto_layout_spacing(const std::string& container_id, double spacing) -> void;

    // ── P04-W11: Advanced Tables ──────────────────────────────

    /// Add a column to an advanced table and emit event.
    auto add_advanced_table_column(const std::string& table_id, const std::string& column_name) -> void;

    /// Sort an advanced table and emit event.
    auto sort_advanced_table(const std::string& table_id, const std::string& sort_column, bool ascending) -> void;

    // ── P04-W12: Cross Board Links ────────────────────────────

    /// Create a cross board link and emit event.
    auto create_cross_board_link(const std::string& source_board_id, const std::string& target_board_id) -> void;

    /// Navigate a cross board link and emit event.
    auto navigate_cross_board_link(const std::string& link_id) -> void;

    // ── P04-W13: Diagram From Selection ───────────────────────

    /// Generate diagram from selection and emit event.
    auto generate_diagram_from_selection(int selected_count, const std::string& diagram_type) -> void;

    /// Structure selection into diagram and emit event.
    auto structure_selection(const std::string& structure_type) -> void;

    // ── P04-W14: Connected Layout Cleanup ─────────────────────

    /// Trigger layout cleanup and emit event.
    auto trigger_layout_cleanup(int affected_connectors) -> void;

    /// Reflow connector and emit event.
    auto reflow_connector(const std::string& connector_id, const std::string& trigger_action) -> void;

    // ── P04-W15: Relationship Metadata ────────────────────────

    /// Set relationship metadata and emit event.
    auto set_relationship_metadata(const std::string& connector_id, const std::string& key, const std::string& value) -> void;

    /// Assign relationship type and emit event.
    auto assign_relationship_type(const std::string& connector_id, const std::string& relationship_type) -> void;

    // ── P04-W16: Diagram Templates ────────────────────────────

    /// Apply a diagram template and emit event.
    auto apply_diagram_template(const std::string& template_name, const std::string& category) -> void;

    /// Save current diagram as template and emit event.
    auto save_diagram_template(const std::string& template_name, int node_count) -> void;

    // ── P04-W17: Dependency Validation ────────────────────────

    /// Run dependency check and emit event.
    auto run_dependency_check(int total_links, int broken_links) -> void;

    /// Fix broken dependency and emit event.
    auto fix_broken_dependency(const std::string& connector_id, const std::string& fix_action) -> void;

    // ── P04-W18: Diagram Export Readiness ─────────────────────

    /// Preview diagram export and emit event.
    auto preview_diagram_export(const std::string& export_format) -> void;

    /// Complete diagram export and emit event.
    auto complete_diagram_export(const std::string& export_format, const std::string& output_path) -> void;

    // ── P04-W19: Collaborative Diagram Editing ────────────────

    /// Acquire diagram lock and emit event.
    auto acquire_diagram_lock(const std::string& diagram_id, const std::string& user_id) -> void;

    /// Release diagram lock and emit event.
    auto release_diagram_lock(const std::string& diagram_id) -> void;

    // ── P04-W20: Diagram Test Matrix ──────────────────────────

    /// Run diagram test suite and emit event.
    auto run_diagram_test_suite(int tests_passed, int tests_failed) -> void;

    /// Report diagram regression and emit event.
    auto report_diagram_regression(const std::string& test_name, const std::string& description) -> void;

    // ── P05-W01: Image Placement ──────────────────────────────

    /// Place image on canvas and emit event.
    auto place_image(const std::string& image_path, double scale) -> void;

    /// Replace image on canvas object and emit event.
    auto replace_image(const std::string& object_id, const std::string& new_image_path) -> void;

    // ── P05-W02: Video & Web Embeds ───────────────────────────

    /// Add video embed and emit event.
    auto add_video_embed(const std::string& embed_url, const std::string& embed_type) -> void;

    /// Resize web embed and emit event.
    auto resize_web_embed(const std::string& object_id, double width, double height) -> void;

    // ── P05-W03: PDF Pages ────────────────────────────────────

    /// Add PDF page to canvas and emit event.
    auto add_pdf_page(const std::string& pdf_path, int page_number) -> void;

    /// Navigate PDF page and emit event.
    auto navigate_canvas_pdf_page(const std::string& object_id, int target_page) -> void;

    // ── P05-W04: Bookmark Cards ───────────────────────────────

    /// Create bookmark card and emit event.
    auto create_bookmark_card(const std::string& url, const std::string& title) -> void;

    /// Refresh bookmark card and emit event.
    auto refresh_bookmark_card(const std::string& object_id) -> void;

    // ── P05-W05: App Widgets ──────────────────────────────────

    /// Add app widget and emit event.
    auto add_app_widget(const std::string& widget_type, const std::string& widget_config) -> void;

    /// Configure app widget and emit event.
    auto configure_app_widget(const std::string& object_id, const std::string& config_key, const std::string& config_value) -> void;

    // ── P05-W06: Icon Insertion ───────────────────────────────

    /// Insert icon on canvas and emit event.
    auto insert_canvas_icon(const std::string& icon_name, const std::string& icon_set) -> void;

    /// Change icon color and emit event.
    auto change_icon_color(const std::string& object_id, const std::string& new_color) -> void;

    // ── P05-W07: Drag & Drop Intake ──────────────────────────

    /// Handle content drop and emit event.
    auto handle_content_drop(const std::string& content_type, const std::string& source_path) -> void;

    /// Complete drop intake and emit event.
    auto complete_drop_intake(int items_ingested) -> void;

    // ── P05-W08: Clipboard Ingest ────────────────────────────

    /// Paste clipboard content and emit event.
    auto paste_clipboard(const std::string& paste_format) -> void;

    /// Convert rich paste and emit event.
    auto convert_rich_paste(const std::string& source_format, const std::string& target_format) -> void;

    // ── P05-W09: Media Optimization ──────────────────────────

    /// Run media optimization and emit event.
    auto run_media_optimization(int assets_optimized, double size_reduction_pct) -> void;

    /// Set media quality level and emit event.
    auto set_media_quality(const std::string& quality_level) -> void;

    // ── P05-W10: Cropping & Masking ──────────────────────────

    /// Crop image and emit event.
    auto crop_image(const std::string& object_id, double crop_x, double crop_y, double crop_width, double crop_height) -> void;

    /// Apply image mask and emit event.
    auto apply_image_mask(const std::string& object_id, const std::string& mask_type) -> void;

    // ── P05-W11: Captions & Metadata ─────────────────────────

    /// Set caption on asset and emit event.
    auto set_asset_caption(const std::string& object_id, const std::string& caption_text) -> void;

    /// Set alt text on asset and emit event.
    auto set_asset_alt_text(const std::string& object_id, const std::string& alt_text) -> void;

    // ── P05-W12: Asset Library Panel ─────────────────────────

    /// Add asset to library and emit event.
    auto add_to_asset_library(const std::string& asset_name, const std::string& asset_category) -> void;

    /// Remove asset from library and emit event.
    auto remove_from_asset_library(const std::string& asset_id) -> void;

    // ── P05-W13: Asset Styling ───────────────────────────────

    /// Apply frame style to asset and emit event.
    auto apply_asset_frame(const std::string& object_id, const std::string& frame_style) -> void;

    /// Change asset shadow and emit event.
    auto change_asset_shadow(const std::string& object_id, double shadow_offset, double shadow_blur) -> void;

    // ── P05-W14: Mixed Media Boards ─────────────────────────

    /// Create media composition and emit event.
    auto create_media_composition(int object_count, const std::string& layout_mode) -> void;

    /// Reorder media layer and emit event.
    auto reorder_media_layer(const std::string& object_id, int new_z_index) -> void;

    // ── P05-W15: Import & Convert ───────────────────────────

    /// Import external file and emit event.
    auto import_external_file(const std::string& source_path, const std::string& converted_format) -> void;

    /// Complete import conversion and emit event.
    auto complete_import_conversion(int files_converted, bool all_succeeded) -> void;

    // ── P05-W16: Media Review Flows ─────────────────────────

    /// Start media review and emit event.
    auto start_media_review(const std::string& object_id, const std::string& reviewer) -> void;

    /// Complete media review and emit event.
    auto complete_media_review(const std::string& object_id, bool approved) -> void;

    // ── P05-W17: Licensing & Security ───────────────────────

    /// Set content license and emit event.
    auto set_content_license(const std::string& object_id, const std::string& license_type) -> void;

    /// Run security scan and emit event.
    auto run_security_scan(int assets_scanned, int threats_found) -> void;

    // ── P05-W18: Asset Serialization & Sync ─────────────────

    /// Serialize asset metadata and emit event.
    auto serialize_asset_metadata(int assets_serialized, const std::string& format) -> void;

    /// Complete asset sync and emit event.
    auto complete_asset_sync(int assets_synced, int conflicts_detected) -> void;

    // ── P05-W19: Rich Content Export ────────────────────────

    /// Start rich content export and emit event.
    auto start_rich_content_export(const std::string& export_format, int objects_to_export) -> void;

    /// Complete rich content export and emit event.
    auto complete_rich_content_export(const std::string& output_path, bool success) -> void;

    // ── P05-W20: Asset Test Coverage ────────────────────────

    /// Run asset test suite and emit event.
    auto run_asset_test_suite(int tests_run, int tests_passed) -> void;

    /// Report asset regression and emit event.
    auto report_asset_regression(const std::string& test_name, const std::string& description) -> void;

    // ── P06-W01: Live Presence ──────────────────────────────

    /// Register collaborator joining and emit event.
    auto collaborator_joined(const std::string& user_id, const std::string& display_name) -> void;

    /// Register collaborator leaving and emit event.
    auto collaborator_left(const std::string& user_id, const std::string& reason) -> void;

    // ── P06-W02: Remote Selections & Follow ─────────────────

    /// Update remote selection and emit event.
    auto update_remote_selection(const std::string& user_id, int selected_object_count) -> void;

    /// Toggle follow mode and emit event.
    auto toggle_follow_mode(const std::string& target_user_id, bool following) -> void;

    // ── P06-W03: Conflict Handling ──────────────────────────

    /// Report edit conflict and emit event.
    auto report_edit_conflict(const std::string& object_id, const std::string& conflicting_user) -> void;

    /// Resolve conflict and emit event.
    auto resolve_conflict(const std::string& object_id, const std::string& resolution_strategy) -> void;

    // ── P06-W04: Comment Threads ────────────────────────────

    /// Create comment thread and emit event.
    auto create_comment_thread(const std::string& thread_id, const std::string& anchor_object_id) -> void;

    /// Add comment reply and emit event.
    auto add_comment_reply(const std::string& thread_id, const std::string& author) -> void;

    // ── P06-W05: Voting & Reactions ─────────────────────────

    /// Cast vote and emit event.
    auto cast_vote(const std::string& object_id, const std::string& voter) -> void;

    /// Add reaction and emit event.
    auto add_reaction(const std::string& object_id, const std::string& reaction_type) -> void;

    // ── P06-W06: Timer & Facilitation ───────────────────────

    /// Start facilitation timer and emit event.
    auto start_facilitation_timer(int duration_seconds, const std::string& facilitator) -> void;

    /// Handle timer expiration and emit event.
    auto expire_facilitation_timer(const std::string& session_id, bool auto_extended) -> void;

    // ── P06-W07: Private Reveal & Presentation ─────────────

    /// Initiate private reveal and emit event.
    auto initiate_private_reveal(const std::string& presenter, int hidden_object_count) -> void;

    /// Reveal content and emit event.
    auto reveal_content(int revealed_count, const std::string& reveal_mode) -> void;

    // ── P06-W08: Permissions & Shared Locks ─────────────────

    /// Change object permission and emit event.
    auto change_object_permission(const std::string& object_id, const std::string& permission_level) -> void;

    /// Acquire shared lock and emit event.
    auto acquire_shared_lock(const std::string& object_id, const std::string& locked_by) -> void;

    // ── P06-W09: Offline Queue ──────────────────────────────

    /// Queue offline operation and emit event.
    auto queue_offline_operation(int queue_depth, const std::string& operation_type) -> void;

    /// Flush offline queue and emit event.
    auto flush_offline_queue(int operations_applied, int operations_conflicted) -> void;

    // ── P06-W10: Activity Feed ──────────────────────────────

    /// Add activity feed entry and emit event.
    auto add_activity_feed_entry(const std::string& actor, const std::string& action_description) -> void;

    /// Clear activity feed and emit event.
    auto clear_activity_feed(int entries_cleared, const std::string& cleared_by) -> void;

    // ── P06-W11: Change Highlights ──────────────────────────

    auto detect_change_highlights(int changes_since_last_visit, const std::string& last_visitor) -> void;
    auto dismiss_change_highlights(const std::string& user_id, int dismissed_count) -> void;

    // ── P06-W12: Invites & Session Entry ────────────────────

    auto send_session_invite(const std::string& invitee_email, const std::string& session_id) -> void;
    auto join_session_via_invite(const std::string& user_id, const std::string& invite_code) -> void;

    // ── P06-W13: Collaborator Panels ────────────────────────

    auto open_collaborator_panel(const std::string& panel_type, int active_users) -> void;
    auto take_collaborator_panel_action(const std::string& action_type, const std::string& target_user) -> void;

    // ── P06-W14: Co-Editing Text ────────────────────────────

    auto start_co_editing(const std::string& object_id, int editors_count) -> void;
    auto move_co_editing_cursor(const std::string& user_id, int cursor_position) -> void;

    // ── P06-W15: Workshop Templates ─────────────────────────

    auto apply_workshop_template(const std::string& template_name, int objects_created) -> void;
    auto save_workshop_template(const std::string& template_name, const std::string& author) -> void;

    // ── P06-W16: Async Review ───────────────────────────────

    auto request_async_review(const std::string& reviewer_id, const std::string& board_id) -> void;
    auto complete_async_review(const std::string& reviewer_id, bool approved) -> void;

    // ── P06-W17: Moderation & Recovery ──────────────────────

    auto moderate_user(const std::string& target_user, const std::string& moderation_action) -> void;
    auto recover_board_state(const std::string& recovery_point, int objects_restored) -> void;

    // ── P06-W18: Multiplayer Performance ────────────────────

    auto throttle_presence(int active_connections, int throttle_interval_ms) -> void;
    auto send_batch_update(int operations_batched, int batch_size_bytes) -> void;

    // ── P06-W19: Collaboration Analytics ────────────────────

    auto snapshot_collab_metrics(int total_edits, int unique_contributors) -> void;
    auto calculate_engagement_score(double engagement_score, const std::string& session_id) -> void;

    // ── P06-W20: Collaboration Coverage ─────────────────────

    auto run_collab_test_suite(int tests_run, int tests_passed) -> void;
    auto report_collab_regression(const std::string& test_name, const std::string& failure_detail) -> void;

    // ── P07-W01: Minimap Navigation ─────────────────────────
    auto change_minimap_viewport(double viewport_x, double viewport_y) -> void;
    auto toggle_minimap_visibility(bool visible, const std::string& toggle_source) -> void;

    // ── P07-W02: Outline Navigation ─────────────────────────
    auto select_outline_node(const std::string& node_id, const std::string& node_type) -> void;
    auto refresh_outline_tree(int total_nodes, int depth_levels) -> void;

    // ── P07-W03: Search & Find ──────────────────────────────
    auto execute_board_search(const std::string& query, int results_found) -> void;
    auto navigate_search_result(const std::string& object_id, int result_index) -> void;

    // ── P07-W04: Tagging & Filters ──────────────────────────
    auto apply_tag_filter(const std::string& tag_name, int matching_objects) -> void;
    auto clear_tag_filter(int filters_removed, int total_objects_visible) -> void;

    // ── P07-W05: Sections & Landmarks ───────────────────────
    auto create_section_landmark(const std::string& section_name, const std::string& section_id) -> void;
    auto navigate_to_landmark(const std::string& landmark_id, double target_zoom) -> void;

    // ── P07-W06: Large Board Streaming ──────────────────────
    auto load_tile(int tile_x, int tile_y) -> void;
    auto change_streaming_lod(int lod_level, int visible_objects) -> void;

    // ── P07-W07: Breadcrumbs & History ──────────────────────
    auto push_breadcrumb(const std::string& location_label, int stack_depth) -> void;
    auto navigate_breadcrumb_back(int steps_back, const std::string& destination_label) -> void;

    // ── P07-W08: Zoom Presets ───────────────────────────────
    auto apply_zoom_preset(const std::string& preset_name, double zoom_level) -> void;
    auto save_zoom_preset(const std::string& preset_name, double zoom_level) -> void;

    // ── P07-W09: Board Bookmarks ────────────────────────────
    auto create_board_bookmark(const std::string& bookmark_name, const std::string& bookmark_id) -> void;
    auto navigate_board_bookmark(const std::string& bookmark_id, double viewport_zoom) -> void;

    // ── P07-W10: Metadata Driven Navigation ─────────────────
    auto apply_metadata_filter(const std::string& filter_key, const std::string& filter_value) -> void;
    auto jump_to_metadata_nav(const std::string& target_object_id, const std::string& metadata_key) -> void;

    // ── P07-W11: Cross Board Traversal ──────────────────────
    auto follow_cross_board_link(const std::string& source_board_id, const std::string& target_board_id) -> void;
    auto navigate_cross_board_back(const std::string& returning_to_board_id, int boards_traversed) -> void;

    // ── P07-W12: Selection Sync ─────────────────────────────
    auto sync_selection_to_panel(const std::string& panel_id, int synced_objects) -> void;
    auto toggle_selection_sync(bool sync_enabled, const std::string& panel_id) -> void;

    // ── P07-W13: Saved Views ────────────────────────────────
    auto save_named_view(const std::string& view_name, const std::string& view_id) -> void;
    auto restore_named_view(const std::string& view_id, double restored_zoom) -> void;

    // ── P07-W14: Presentation Navigation ────────────────────
    auto advance_presentation_slide(int slide_index, int total_slides) -> void;
    auto toggle_presentation_mode(bool presentation_active, const std::string& presenter_id) -> void;

    // ── P07-W15: Semantic Navigation ────────────────────────
    auto navigate_semantic_cluster(const std::string& cluster_label, int objects_in_cluster) -> void;
    auto recalculate_semantic_grouping(int clusters_found, int ungrouped_objects) -> void;

    // ── P07-W16: Quick Action Navigation ────────────────────
    auto invoke_quick_action(const std::string& action_name, const std::string& action_source) -> void;
    auto select_quick_action_result(const std::string& result_id, int result_rank) -> void;

    // ── P07-W17: Discoverability ────────────────────────────
    auto show_feature_hint(const std::string& hint_id, const std::string& feature_area) -> void;
    auto dismiss_feature_hint(const std::string& hint_id, bool dont_show_again) -> void;

    // ── P07-W18: Responsive Layouts ─────────────────────────
    auto change_layout_breakpoint(const std::string& breakpoint_name, int window_width) -> void;
    auto reposition_nav_panel(const std::string& panel_position, const std::string& trigger) -> void;

    // ── P07-W19: Wayfinding Telemetry ───────────────────────
    auto detect_nav_confusion(int rapid_pans, int zoom_reversals) -> void;
    auto record_wayfinding_metric(const std::string& metric_name, double metric_value) -> void;

    // ── P07-W20: Navigation Coverage ────────────────────────
    auto run_nav_test_suite(int tests_run, int tests_passed) -> void;
    auto report_nav_regression(const std::string& test_name, const std::string& failure_detail) -> void;

    // ── P08-W01: Board Templates ────────────────────────────
    auto apply_board_template(const std::string& template_name, const std::string& template_category) -> void;
    auto save_board_template(const std::string& template_name, int objects_in_template) -> void;

    // ── P08-W02: Object Templates ───────────────────────────
    auto insert_object_template(const std::string& template_id, int objects_inserted) -> void;
    auto register_object_template(const std::string& template_name, const std::string& template_type) -> void;

    // ── P08-W03: Advanced Style Presets ─────────────────────
    auto apply_style_preset(const std::string& preset_name, int objects_affected) -> void;
    auto create_style_preset(const std::string& preset_name, const std::string& preset_scope) -> void;

    // ── P08-W04: Automation Rules ───────────────────────────
    auto trigger_automation_rule(const std::string& rule_name, int actions_executed) -> void;
    auto create_automation_rule(const std::string& rule_name, const std::string& trigger_type) -> void;

    // ── P08-W05: AI Board Generation ────────────────────────
    auto request_ai_board_generation(const std::string& prompt_text, const std::string& board_type) -> void;
    auto complete_ai_board_generation(int objects_generated, double generation_time_ms) -> void;

    // ── P08-W06: AI Cleanup & Refinement ────────────────────
    auto request_ai_cleanup(const std::string& cleanup_type, int objects_in_scope) -> void;
    auto complete_ai_cleanup(int objects_modified, int objects_removed) -> void;

    // ── P08-W07: AI Summaries & Explanations ────────────────
    auto request_ai_summary(const std::string& scope, int objects_summarized) -> void;
    auto generate_ai_summary(int word_count, const std::string& output_format) -> void;

    // ── P08-W08: AI Tags & Links ────────────────────────────
    auto generate_ai_tag_suggestions(int tags_suggested, int objects_analyzed) -> void;
    auto generate_ai_link_suggestions(int links_suggested, int cross_board_links) -> void;

    // ── P08-W09: Batch Operations ───────────────────────────
    auto execute_batch_operation(const std::string& operation_type, int objects_affected) -> void;
    auto undo_batch_operation(const std::string& operation_type, int objects_restored) -> void;

    // ── P08-W10: Markdown To Canvas ─────────────────────────
    auto start_markdown_to_canvas(const std::string& source_file, int lines_to_parse) -> void;
    auto complete_markdown_to_canvas(int objects_created, int connectors_created) -> void;

    // ── P08-W11: CSV & Database Imports ─────────────────────
    auto start_csv_import(const std::string& source_file, int rows_to_import) -> void;
    auto complete_csv_import(int objects_created, int columns_mapped) -> void;

    // ── P08-W12: Export Workflows ───────────────────────────
    auto start_board_export(const std::string& export_format, const std::string& export_scope) -> void;
    auto complete_board_export(const std::string& output_path, int objects_exported) -> void;

    // ── P08-W13: Reusable Components ────────────────────────
    auto save_reusable_component(const std::string& component_name, int child_objects) -> void;
    auto instantiate_reusable_component(const std::string& component_id, const std::string& instance_id) -> void;

    // ── P08-W14: Plugin Hooks ───────────────────────────────
    auto register_plugin_hook(const std::string& hook_name, const std::string& plugin_id) -> void;
    auto invoke_plugin_hook(const std::string& hook_name, int listeners_notified) -> void;

    // ── P08-W15: Command Macros ─────────────────────────────
    auto record_command_macro(const std::string& macro_name, int steps_recorded) -> void;
    auto replay_command_macro(const std::string& macro_name, int objects_affected) -> void;

    // ── P08-W16: Quick Insert ───────────────────────────────
    auto open_quick_insert_menu(const std::string& trigger_source, int items_available) -> void;
    auto select_quick_insert_item(const std::string& item_type, int search_rank) -> void;

    // ── P08-W17: Smart Defaults ─────────────────────────────
    auto apply_smart_default(const std::string& default_type, const std::string& context) -> void;
    auto show_smart_default_suggestion(int suggestions_count, bool suggestion_accepted) -> void;

    // ── P08-W18: Profile & Preference Sync ──────────────────
    auto sync_profile(const std::string& profile_id, int preferences_synced) -> void;
    auto resolve_preference_conflict(const std::string& preference_key, const std::string& resolution) -> void;

    // ── P08-W19: Onboarding Programs ────────────────────────
    auto complete_onboarding_step(const std::string& step_id, int steps_remaining) -> void;
    auto finish_onboarding_program(const std::string& program_id, int total_steps_completed) -> void;

    // ── P08-W20: Agent Ready Scaffolds ──────────────────────
    auto run_creation_test_suite(int tests_run, int tests_passed) -> void;
    auto report_creation_regression(const std::string& test_name, const std::string& failure_detail) -> void;

    // ── P09-W01: Keyboard Only Canvas ───────────────────────
    auto perform_keyboard_nav_action(const std::string& action, const std::string& direction) -> void;
    auto detect_keyboard_shortcut_conflict(const std::string& shortcut_key, const std::string& conflicting_action) -> void;

    // ── P09-W02: Screen Reader Semantics ────────────────────
    auto queue_screen_reader_announcement(const std::string& text, const std::string& priority) -> void;
    auto update_accessible_label(const std::string& object_id, const std::string& new_label) -> void;

    // ── P09-W03: Contrast & Color Blindness ─────────────────
    auto toggle_high_contrast_mode(bool enabled, const std::string& profile) -> void;
    auto run_color_accessibility_check(int elements_checked, int issues_found) -> void;

    // ── P09-W04: Focus & Announcements ──────────────────────
    auto transfer_focus(const std::string& from_id, const std::string& to_id) -> void;
    auto update_live_region(const std::string& region_id, const std::string& text) -> void;

    // ── P09-W05: Touch & Pen Parity ─────────────────────────
    auto recognize_touch_gesture(const std::string& gesture_type, int touch_points) -> void;
    auto apply_pen_pressure(double pressure_level, const std::string& pen_tool) -> void;

    // ── P09-W06: Performance Budgets ────────────────────────
    auto report_perf_budget_exceeded(const std::string& operation, double elapsed_ms) -> void;
    auto record_frame_rate_metric(double fps, int objects_rendered) -> void;

    // ── P09-W07: Virtualization ─────────────────────────────
    auto update_viewport_culling(int visible_objects, int total_objects) -> void;
    auto record_tile_cache_metric(int cache_hits, int cache_misses) -> void;

    // ── P09-W08: Crash Recovery ─────────────────────────────
    auto save_crash_recovery_checkpoint(const std::string& checkpoint_id, int objects_saved) -> void;
    auto recover_crashed_board_state(const std::string& recovery_source, int objects_recovered) -> void;

    // ── P09-W09: Data Integrity ─────────────────────────────
    auto run_board_integrity_check(int objects_validated, int corruption_found) -> void;
    auto apply_data_repair(const std::string& repair_type, int items_repaired) -> void;

    // ── P09-W10: Accessibility Tooling ──────────────────────
    auto run_accessibility_audit(int elements_audited, int violations_found) -> void;
    auto detect_accessibility_regression(const std::string& test_name, const std::string& violation_type) -> void;

    // ── P09-W11: Safe Degradation ───────────────────────────
    auto activate_safe_degradation(const std::string& feature_name, const std::string& fallback_reason) -> void;
    auto exit_degraded_mode(const std::string& feature_name, double degraded_duration_ms) -> void;

    // ── P09-W12: Observability ──────────────────────────────
    auto record_telemetry_span(const std::string& span_name, double duration_ms) -> void;
    auto emit_error_telemetry(const std::string& error_category, const std::string& error_message) -> void;

    // ── P09-W13: Feature Flags ──────────────────────────────
    auto evaluate_feature_flag(const std::string& flag_name, bool flag_value) -> void;
    auto apply_feature_flag_override(const std::string& flag_name, const std::string& override_source) -> void;

    // ── P09-W14: Snapshot & Harnesses ───────────────────────
    auto capture_visual_snapshot(const std::string& snapshot_id, int pixels_diffed) -> void;
    auto compare_snapshot(const std::string& baseline_id, double diff_percentage) -> void;

    // ── P09-W15: Unit & Integration Gaps ────────────────────
    auto identify_test_coverage_gap(const std::string& module_name, int uncovered_lines) -> void;
    auto add_integration_test(const std::string& test_name, const std::string& covered_module) -> void;

    // ── P09-W16: End To End Determinism ─────────────────────
    auto detect_e2e_flakiness(const std::string& test_name, int flaky_runs) -> void;
    auto stabilize_e2e_timing(const std::string& test_name, double variance_ms) -> void;

    // ── P09-W17: Security & Privacy ─────────────────────────
    auto report_security_audit_finding(const std::string& finding_type, const std::string& severity) -> void;
    auto scrub_privacy_data(int fields_scrubbed, const std::string& scrub_scope) -> void;

    // ── P09-W18: Localization & IME ─────────────────────────
    auto switch_locale(const std::string& from_locale, const std::string& to_locale) -> void;
    auto handle_ime_composition(const std::string& ime_state, int composition_length) -> void;

    // ── P09-W19: Compliance Checklists ──────────────────────
    auto run_compliance_check(int checks_passed, int checks_failed) -> void;
    auto flag_compliance_violation(const std::string& rule_id, const std::string& violation_detail) -> void;

    // ── P09-W20: Operational Dashboards ─────────────────────
    auto publish_dashboard_metric(const std::string& metric_name, double metric_value) -> void;
    auto complete_health_check(int healthy_systems, int degraded_systems) -> void;

    // ── P10-W01: Parity Audit ───────────────────────────────
    auto check_parity_audit_item(const std::string& competitor_feature, const std::string& parity_status) -> void;
    auto identify_parity_gap(const std::string& feature_name, const std::string& gap_severity) -> void;

    // ── P10-W02: Control Polish ─────────────────────────────
    auto adjust_control_density(const std::string& control_group, const std::string& density_level) -> void;
    auto apply_affordance_refinement(const std::string& element_id, const std::string& refinement_type) -> void;

    // ── P10-W03: Motion & Microinteraction ──────────────────
    auto trigger_microinteraction(const std::string& interaction_name, double duration_ms) -> void;
    auto apply_motion_preference(const std::string& preference, bool system_prefers_reduced) -> void;

    // ── P10-W04: Platform Conventions ───────────────────────
    auto apply_platform_convention(const std::string& platform, const std::string& convention_type) -> void;
    auto run_platform_parity_check(const std::string& platform, int conventions_matched) -> void;

    // ── P10-W05: Settings Migration ─────────────────────────
    auto start_settings_migration(const std::string& from_version, const std::string& to_version) -> void;
    auto complete_settings_migration(int settings_migrated, int settings_defaulted) -> void;

    // ── P10-W06: Documentation ──────────────────────────────
    auto generate_doc_page(const std::string& page_id, int sections_written) -> void;
    auto check_doc_coverage(int features_documented, int features_undocumented) -> void;

    // ── P10-W07: Benchmarks & Baselines ─────────────────────
    auto complete_benchmark_run(const std::string& benchmark_name, double result_ms) -> void;
    auto detect_benchmark_regression(const std::string& benchmark_name, double regression_pct) -> void;

    // ── P10-W08: Release Gates ──────────────────────────────
    auto evaluate_release_gate(const std::string& gate_name, bool gate_passed) -> void;
    auto summarize_release_readiness(int gates_passed, int gates_failed) -> void;

    // ── P10-W09: Beta Feedback Loops ────────────────────────
    auto submit_beta_feedback(const std::string& feedback_category, const std::string& board_state_id) -> void;
    auto triage_beta_feedback(const std::string& feedback_id, const std::string& triage_priority) -> void;

    // ── P10-W10: Extension Ecosystem ────────────────────────
    auto check_extension_compatibility(const std::string& extension_id, bool is_compatible) -> void;
    auto generate_extension_ecosystem_report(int total_extensions, int compatible_extensions) -> void;

    // ── P10-W11: Advanced Drawing Polish ────────────────────
    auto refine_drawing_stroke(const std::string& tool_name, double pressure_sensitivity) -> void;
    auto calibrate_drawing_tool(const std::string& tool_name, int calibration_points) -> void;

    // ── P10-W12: Advanced Layout Polish ─────────────────────
    auto refine_layout_transform(const std::string& transform_type, double precision_delta) -> void;
    auto resolve_alignment_edge_case(const std::string& alignment_type, int objects_affected) -> void;

    // ── P10-W13: Advanced Collaboration Polish ──────────────
    auto resolve_collab_friction(const std::string& friction_type, double latency_improvement_ms) -> void;
    auto stabilize_multi_user_session(int concurrent_users, double session_uptime_hours) -> void;

    // ── P10-W14: Advanced Navigation Polish ─────────────────
    auto optimize_board_travel(const std::string& navigation_mode, double travel_time_ms) -> void;
    auto set_navigation_waypoint(const std::string& waypoint_id, double x) -> void;

    // ── P10-W15: Advanced Export Polish ──────────────────────
    auto finalize_export_artifact(const std::string& format, int pages_exported) -> void;
    auto validate_export_fidelity(const std::string& format, double fidelity_score) -> void;

    // ── P10-W16: Enterprise & Admin ─────────────────────────
    auto apply_admin_policy(const std::string& policy_name, int users_affected) -> void;
    auto log_governance_audit(const std::string& action_type, const std::string& actor_id) -> void;

    // ── P10-W17: Quality Backlog Triage ─────────────────────
    auto triage_defect(const std::string& defect_id, const std::string& severity) -> void;
    auto reduce_quality_backlog(int defects_resolved, int defects_remaining) -> void;

    // ── P10-W18: Adoption Metrics ───────────────────────────
    auto track_adoption_metric(const std::string& metric_name, double metric_value) -> void;
    auto analyze_usage_trend(const std::string& feature_name, double trend_direction) -> void;

    // ── P10-W19: Support Playbooks ──────────────────────────
    auto activate_support_playbook(const std::string& playbook_id, const std::string& issue_category) -> void;
    auto log_support_resolution(const std::string& ticket_id, double resolution_time_hours) -> void;

    // ── P10-W20: Architecture Follow Ups ────────────────────
    auto address_tech_debt(const std::string& debt_item, const std::string& resolution_type) -> void;
    auto complete_platform_investment(const std::string& investment_area, int files_affected) -> void;

    // ── State Persistence ──────────────────────────────────────────

    /// Snapshot state for session save.
    [[nodiscard]] auto state() const -> const CanvasWorkbenchState&;

    /// Restore state from a previous session.
    auto restore_state(const CanvasWorkbenchState& saved_state) -> void;

    // ── Canvas Registration ────────────────────────────────────────

    /// Surface kind identifier for the canvas mode.
    static constexpr int kSurfaceKindCanvas = 3;

    /// Human-readable name for the canvas surface.
    [[nodiscard]] static auto surface_name() -> std::string;

    // ── Batch 10 (#99-100) ────────────────────────────────────────

    /// (#99) Number of boards currently open.
    [[nodiscard]] auto board_count() const noexcept -> size_t
    {
        return boards_.size();
    }

    /// (#100) Whether there is an active board.
    [[nodiscard]] auto has_active_board() const noexcept -> bool
    {
        return !active_board_id_.empty();
    }

private:
    std::shared_ptr<core::EventBus> event_bus_;

    // Open boards keyed by id.
    std::unordered_map<std::string, std::unique_ptr<Board>> boards_;
    std::string active_board_id_;

    // Tools
    std::unordered_map<uint8_t, std::unique_ptr<ICanvasTool>> tools_;
    ToolMode active_tool_mode_{ToolMode::Select};

    // Sub-systems
    SelectionManager selection_;
    UndoRedoStack undo_stack_;
    ViewportTransform viewport_;
    BoardTemplateLibrary templates_;
    BoardSerializer serializer_;
    ToolRailModel tool_rail_;
    CanvasToolStateMachine tool_state_machine_;
    KeyboardCommandModel keyboard_commands_;
    CursorFeedbackModel cursor_feedback_;
    GridRulerModel grid_ruler_;
    SnapPreferenceModel snap_prefs_;
    ContextMenuModel context_menu_model_;
    AutosaveModel autosave_;
    InspectorModel inspector_;
    MinimapModel minimap_;
    OnboardingModel onboarding_;
    bool inspector_visible_{false};
    bool minimap_visible_{false};
    BoardSettingsModel board_settings_;
    CanvasThemeModel canvas_theme_;
    ui::CommandPaletteModel command_palette_;
    CanvasAnalyticsModel canvas_analytics_;
    SmartSpacingModel smart_spacing_;
    PrecisionNudgeModel nudge_model_;
    DragAutoscrollModel autoscroll_model_;
    PenEngineModel pen_engine_;
    PenPresetModel pen_presets_;
    bool palette_visible_{false};

    // Persistent state
    CanvasWorkbenchState state_;

    /// Per-board viewport state for switching.
    struct ViewportState
    {
        double zoom{1.0};
        Point2D pan{0.0, 0.0};
    };
    std::unordered_map<std::string, ViewportState> board_viewports_;

    // Helpers
    auto add_to_recent(const std::string& board_id) -> void;
    auto emit_board_opened(const std::string& board_id) -> void;
    auto emit_board_closed(const std::string& board_id) -> void;
    auto emit_tool_changed(ToolMode old_mode, ToolMode new_mode) -> void;
};

} // namespace markamp::canvas
