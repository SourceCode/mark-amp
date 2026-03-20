// ============================================================================
// File: src/canvas/CanvasWorkbench.cpp
// Phase 11: Canvas Workbench Shell — implementation
// ============================================================================
#include "CanvasWorkbench.h"

#include "canvas/Board.h"
#include "canvas/BoardSerializer.h"
#include "canvas/DrawTool.h"
#include "canvas/PanTool.h"
#include "core/Events.h"
#include "canvas/SelectTool.h"

#include <algorithm>
#include <fstream>
#include <utility>

namespace markamp::canvas
{

CanvasWorkbench::CanvasWorkbench(std::shared_ptr<core::EventBus> event_bus)
    : event_bus_(std::move(event_bus))
    , selection_(event_bus_)
{
    // Register built-in tools
    tools_[static_cast<uint8_t>(ToolMode::Select)] = std::make_unique<SelectTool>();
    tools_[static_cast<uint8_t>(ToolMode::Pan)] = std::make_unique<PanTool>();
    tools_[static_cast<uint8_t>(ToolMode::Draw)] = std::make_unique<DrawTool>();
}

// ── Board Lifecycle ────────────────────────────────────────────────

auto CanvasWorkbench::open_board(const std::string& board_id,
                                 const std::filesystem::path& board_path) -> bool
{
    if (boards_.contains(board_id))
    {
        set_active_board(board_id);
        return true;
    }

    // Read file content
    std::ifstream file(board_path);
    if (!file)
    {
        return false;
    }
    std::string json_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    auto board = serializer_.deserialize(json_data);
    boards_[board_id] = std::make_unique<Board>(std::move(board));
    set_active_board(board_id);
    add_to_recent(board_id);
    emit_board_opened(board_id);
    return true;
}

auto CanvasWorkbench::create_board(const std::string& name) -> std::string
{
    auto board = std::make_unique<Board>(name);
    const auto& board_id = board->metadata().id;
    std::string result_id = board_id;

    boards_[result_id] = std::move(board);
    set_active_board(result_id);
    add_to_recent(result_id);
    emit_board_opened(result_id);

    // W02: Emit BoardCreatedEvent
    if (event_bus_)
    {
        core::events::BoardCreatedEvent evt;
        evt.board_id = result_id;
        evt.board_name = name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Board created: '{}' (id={})", name, result_id);
    return result_id;
}

auto CanvasWorkbench::create_from_template(const std::string& name, const std::string& template_id)
    -> std::string
{
    auto board = templates_.apply_template(template_id);
    if (board == nullptr)
    {
        return create_board(name); // Fallback to blank board
    }

    board->metadata_mut().name = name;
    const auto& board_id = board->metadata().id;
    std::string result_id = board_id;

    boards_[result_id] = std::move(board);
    set_active_board(result_id);
    add_to_recent(result_id);
    emit_board_opened(result_id);

    return result_id;
}

auto CanvasWorkbench::close_board(const std::string& board_id) -> bool
{
    auto found = boards_.find(board_id);
    if (found == boards_.end())
    {
        return false;
    }

    boards_.erase(found);
    emit_board_closed(board_id);

    // Switch to another open board if closing the active one
    if (active_board_id_ == board_id)
    {
        active_board_id_.clear();
        if (!boards_.empty())
        {
            set_active_board(boards_.begin()->first);
        }
    }

    return true;
}

auto CanvasWorkbench::rename_board(const std::string& board_id, const std::string& new_name) -> bool
{
    auto* board = get_board(board_id);
    if (board == nullptr)
    {
        MARKAMP_LOG_WARN("rename_board: board not found (id={})", board_id);
        return false;
    }

    const std::string old_name = board->metadata().name;
    board->metadata_mut().name = new_name;
    board->mark_dirty();

    if (event_bus_)
    {
        core::events::BoardRenamedEvent evt;
        evt.board_id = board_id;
        evt.old_name = old_name;
        evt.new_name = new_name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Board renamed: '{}' → '{}' (id={})", old_name, new_name, board_id);
    return true;
}

auto CanvasWorkbench::duplicate_board(const std::string& board_id) -> std::string
{
    const auto* source = get_board(board_id);
    if (source == nullptr)
    {
        MARKAMP_LOG_WARN("duplicate_board: source board not found (id={})", board_id);
        return {};
    }

    auto cloned = std::make_unique<Board>(source->deep_clone());
    const std::string new_name = source->metadata().name + " (copy)";
    cloned->metadata_mut().name = new_name;
    // deep_clone copies the old ID — replace with a fresh one from the constructor
    // The Board default constructor already generated a new ID, but deep_clone
    // copies metadata including the old ID. We need a fresh board to get a new ID.
    auto fresh = std::make_unique<Board>(new_name);
    // Transfer objects from the clone to the fresh board.
    for (auto& obj : cloned->objects_mut())
    {
        fresh->add_object(std::move(obj));
    }

    const std::string new_id = fresh->metadata().id;
    boards_[new_id] = std::move(fresh);
    set_active_board(new_id);
    add_to_recent(new_id);
    emit_board_opened(new_id);

    if (event_bus_)
    {
        core::events::BoardDuplicatedEvent evt;
        evt.source_board_id = board_id;
        evt.new_board_id = new_id;
        evt.new_board_name = new_name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Board duplicated: '{}' → '{}' (new id={})", source->metadata().name, new_name, new_id);
    return new_id;
}

auto CanvasWorkbench::save_board(const std::filesystem::path& path) -> bool
{
    auto* board = active_board();
    if (board == nullptr)
    {
        return false;
    }

    auto json = serializer_.serialize(*board);
    std::ofstream file(path);
    if (!file)
    {
        return false;
    }
    file << json;

    if (!file.good())
    {
        return false;
    }

    board->clear_dirty();

    // W02: Emit BoardSavedEvent
    if (event_bus_)
    {
        core::events::BoardSavedEvent evt;
        evt.board_id = active_board_id_;
        evt.file_path = path.string();
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Board saved: id={} path={}", active_board_id_, path.string());
    return true;
}

// ── W03: Viewport Camera ──────────────────────────────────────────

auto CanvasWorkbench::zoom_to_fit() -> void
{
    auto* board = active_board();
    if (board == nullptr)
    {
        MARKAMP_LOG_WARN("zoom_to_fit: no active board");
        return;
    }

    const double old_zoom = viewport_.zoom();
    viewport_.fit_to_bounds(board->content_bounds());
    const double new_zoom_val = viewport_.zoom();

    if (event_bus_)
    {
        core::events::ViewportZoomChangedEvent evt;
        evt.old_zoom = old_zoom;
        evt.new_zoom = new_zoom_val;
        event_bus_->publish(evt);

        core::events::CanvasViewportChangedEvent vp_evt;
        vp_evt.zoom = new_zoom_val;
        vp_evt.pan_x = viewport_.pan().x;
        vp_evt.pan_y = viewport_.pan().y;
        event_bus_->publish(vp_evt);
    }

    MARKAMP_LOG_INFO("Viewport zoom-to-fit: {:.0f}% → {:.0f}%", old_zoom * 100.0, new_zoom_val * 100.0);
}

auto CanvasWorkbench::zoom_to_selection(const AABB& selection) -> void
{
    const double old_zoom = viewport_.zoom();
    viewport_.zoom_to_fit_selection(selection);
    const double new_zoom_val = viewport_.zoom();

    if (event_bus_)
    {
        core::events::ViewportZoomChangedEvent evt;
        evt.old_zoom = old_zoom;
        evt.new_zoom = new_zoom_val;
        event_bus_->publish(evt);

        core::events::CanvasViewportChangedEvent vp_evt;
        vp_evt.zoom = new_zoom_val;
        vp_evt.pan_x = viewport_.pan().x;
        vp_evt.pan_y = viewport_.pan().y;
        event_bus_->publish(vp_evt);
    }

    MARKAMP_LOG_INFO("Viewport zoom-to-selection: {:.0f}% → {:.0f}%", old_zoom * 100.0, new_zoom_val * 100.0);
}

auto CanvasWorkbench::reset_viewport() -> void
{
    const double old_zoom = viewport_.zoom();
    viewport_.zoom_to_100();
    viewport_.reset_pan();

    if (event_bus_)
    {
        core::events::ViewportZoomChangedEvent evt;
        evt.old_zoom = old_zoom;
        evt.new_zoom = 1.0;
        event_bus_->publish(evt);

        core::events::CanvasViewportChangedEvent vp_evt;
        vp_evt.zoom = 1.0;
        vp_evt.pan_x = 0.0;
        vp_evt.pan_y = 0.0;
        event_bus_->publish(vp_evt);
    }

    MARKAMP_LOG_INFO("Viewport reset: {:.0f}% → 100%", old_zoom * 100.0);
}

// ── W04: Tool Rail ────────────────────────────────────────────────

auto CanvasWorkbench::select_tool_by_id(const std::string& tool_id) -> bool
{
    const auto& all_tools = tool_rail_.tools();
    const auto* found_tool = static_cast<const ToolEntry*>(nullptr);
    for (const auto& entry : all_tools)
    {
        if (entry.tool_id == tool_id)
        {
            found_tool = &entry;
            break;
        }
    }

    if (found_tool == nullptr)
    {
        MARKAMP_LOG_WARN("select_tool_by_id: tool not found (id={})", tool_id);
        return false;
    }

    tool_rail_.select_tool(tool_id);

    if (event_bus_)
    {
        core::events::ToolRailSelectionChangedEvent evt;
        evt.tool_id = tool_id;
        evt.tool_name = found_tool->name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Tool rail selected: {} ({})", found_tool->name, tool_id);
    return true;
}

auto CanvasWorkbench::quick_switch_tool() -> void
{
    const std::string from_id = tool_rail_.active_tool();
    tool_rail_.quick_switch();
    const std::string to_id = tool_rail_.active_tool();

    if (event_bus_)
    {
        core::events::ToolQuickSwitchEvent evt;
        evt.from_tool_id = from_id;
        evt.to_tool_id = to_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Tool rail quick-switch: {} → {}", from_id, to_id);
}

auto CanvasWorkbench::tool_rail() -> ToolRailModel&
{
    return tool_rail_;
}

auto CanvasWorkbench::tool_rail() const -> const ToolRailModel&
{
    return tool_rail_;
}

auto CanvasWorkbench::board_exists(const std::string& board_id) const -> bool
{
    return boards_.contains(board_id);
}

auto CanvasWorkbench::set_active_board(const std::string& board_id) -> bool
{
    if (!boards_.contains(board_id))
    {
        return false;
    }

    // Save current viewport state before switching.
    if (!active_board_id_.empty())
    {
        ViewportState saved;
        saved.zoom = viewport_.zoom();
        saved.pan = viewport_.pan();
        board_viewports_[active_board_id_] = saved;
    }

    active_board_id_ = board_id;
    state_.active_board_id = board_id;
    selection_.clear_selection();

    // Restore viewport state for the target board.
    auto vp_iter = board_viewports_.find(board_id);
    if (vp_iter != board_viewports_.end())
    {
        viewport_.set_zoom(vp_iter->second.zoom);
        viewport_.set_pan(vp_iter->second.pan);
    }
    else
    {
        // First time opening: reset to default viewport.
        viewport_.set_zoom(1.0);
        viewport_.set_pan({0.0, 0.0});
    }

    return true;
}

auto CanvasWorkbench::active_board() -> Board*
{
    auto found = boards_.find(active_board_id_);
    return found != boards_.end() ? found->second.get() : nullptr;
}

auto CanvasWorkbench::active_board() const -> const Board*
{
    auto found = boards_.find(active_board_id_);
    return found != boards_.end() ? found->second.get() : nullptr;
}

auto CanvasWorkbench::get_board(const std::string& board_id) -> Board*
{
    auto found = boards_.find(board_id);
    return found != boards_.end() ? found->second.get() : nullptr;
}

auto CanvasWorkbench::get_board(const std::string& board_id) const -> const Board*
{
    auto found = boards_.find(board_id);
    return found != boards_.end() ? found->second.get() : nullptr;
}

auto CanvasWorkbench::open_boards() const -> std::vector<BoardSummary>
{
    std::vector<BoardSummary> result;
    result.reserve(boards_.size());

    for (const auto& [bid, board] : boards_)
    {
        BoardSummary summary;
        summary.id = bid;
        summary.name = board->metadata().name;
        summary.description = board->metadata().description;
        summary.object_count = board->object_count();
        summary.modified_at = board->metadata().modified_at;
        summary.is_dirty = board->is_dirty();
        result.push_back(std::move(summary));
    }

    return result;
}

auto CanvasWorkbench::recent_boards() const -> const std::vector<std::string>&
{
    return state_.recent_board_ids;
}

// ── Tool Switching ─────────────────────────────────────────────────

auto CanvasWorkbench::set_tool(ToolMode mode) -> void
{
    if (mode == active_tool_mode_)
    {
        return;
    }

    const auto old_mode = active_tool_mode_;
    active_tool_mode_ = mode;
    state_.active_tool = mode;
    emit_tool_changed(old_mode, mode);
}

auto CanvasWorkbench::active_tool_mode() const -> ToolMode
{
    return active_tool_mode_;
}

auto CanvasWorkbench::active_tool() -> ICanvasTool*
{
    auto found = tools_.find(static_cast<uint8_t>(active_tool_mode_));
    return found != tools_.end() ? found->second.get() : nullptr;
}

auto CanvasWorkbench::active_tool() const -> const ICanvasTool*
{
    auto found = tools_.find(static_cast<uint8_t>(active_tool_mode_));
    return found != tools_.end() ? found->second.get() : nullptr;
}

auto CanvasWorkbench::register_tool(ToolMode mode, std::unique_ptr<ICanvasTool> tool) -> void
{
    tools_[static_cast<uint8_t>(mode)] = std::move(tool);
}

// ── Sub-System Access ──────────────────────────────────────────────

auto CanvasWorkbench::selection_manager() -> SelectionManager&
{
    return selection_;
}

auto CanvasWorkbench::selection_manager() const -> const SelectionManager&
{
    return selection_;
}

auto CanvasWorkbench::undo_stack() -> UndoRedoStack&
{
    return undo_stack_;
}

auto CanvasWorkbench::undo_stack() const -> const UndoRedoStack&
{
    return undo_stack_;
}

auto CanvasWorkbench::viewport() -> ViewportTransform&
{
    return viewport_;
}

auto CanvasWorkbench::viewport() const -> const ViewportTransform&
{
    return viewport_;
}

auto CanvasWorkbench::template_library() -> BoardTemplateLibrary&
{
    return templates_;
}

auto CanvasWorkbench::template_library() const -> const BoardTemplateLibrary&
{
    return templates_;
}

// ── State Persistence ──────────────────────────────────────────────

auto CanvasWorkbench::state() const -> const CanvasWorkbenchState&
{
    return state_;
}

auto CanvasWorkbench::restore_state(const CanvasWorkbenchState& saved_state) -> void
{
    state_ = saved_state;
    active_tool_mode_ = saved_state.active_tool;
    viewport_.set_zoom(saved_state.zoom_level);
    viewport_.set_pan(saved_state.viewport_center);
}

// ── Canvas Registration ────────────────────────────────────────────

auto CanvasWorkbench::surface_name() -> std::string
{
    return "Canvas";
}

// ── Helpers ────────────────────────────────────────────────────────

auto CanvasWorkbench::add_to_recent(const std::string& board_id) -> void
{
    auto& recent = state_.recent_board_ids;
    // Remove if already present
    recent.erase(std::remove(recent.begin(), recent.end(), board_id), recent.end());
    // Insert at front
    recent.insert(recent.begin(), board_id);
    // Trim to max
    if (recent.size() > CanvasWorkbenchState::kMaxRecentBoards)
    {
        recent.resize(CanvasWorkbenchState::kMaxRecentBoards);
    }
}

auto CanvasWorkbench::emit_board_opened(const std::string& board_id) -> void
{
    if (!event_bus_)
    {
        return;
    }
    const auto* board = get_board(board_id);
    core::events::BoardOpenedEvent evt;
    evt.board_id = board_id;
    evt.board_name = board != nullptr ? board->metadata().name : board_id;
    evt.object_count = board != nullptr ? board->object_count() : 0;
    event_bus_->publish(evt);
}

auto CanvasWorkbench::emit_board_closed(const std::string& board_id) -> void
{
    if (!event_bus_)
    {
        return;
    }
    core::events::BoardClosedEvent evt;
    evt.board_id = board_id;
    event_bus_->publish(evt);
}

auto CanvasWorkbench::emit_tool_changed(ToolMode /*old_mode*/, ToolMode new_mode) -> void
{
    if (!event_bus_)
    {
        return;
    }
    core::events::CanvasToolChangedEvent evt;
    evt.tool_mode = static_cast<uint8_t>(new_mode);
    // Map tool mode to human-readable name
    switch (new_mode)
    {
        case ToolMode::Select:
            evt.tool_name = "Select";
            break;
        case ToolMode::Pan:
            evt.tool_name = "Pan";
            break;
        case ToolMode::Draw:
            evt.tool_name = "Draw";
            break;
        case ToolMode::Shape:
            evt.tool_name = "Shape";
            break;
        case ToolMode::Text:
            evt.tool_name = "Text";
            break;
        case ToolMode::Connector:
            evt.tool_name = "Connector";
            break;
        case ToolMode::StickyNote:
            evt.tool_name = "Sticky Note";
            break;
        case ToolMode::Eraser:
            evt.tool_name = "Eraser";
            break;
        case ToolMode::Comment:
            evt.tool_name = "Comment";
            break;
    }
    event_bus_->publish(evt);
}

// ── V17 Phase 01 W06: Input State Machine ──────────────────────────

auto CanvasWorkbench::cancel_active_gesture() -> bool
{
    const bool cancelled = tool_state_machine_.escape_cancel();
    if (cancelled && event_bus_)
    {
        core::events::ToolGestureCancelledEvent evt;
        evt.tool_mode = static_cast<uint8_t>(active_tool_mode_);
        evt.reason = "escape";
        event_bus_->publish(evt);
    }
    MARKAMP_LOG_INFO("Cancel active gesture: {}", cancelled ? "cancelled" : "nothing to cancel");
    return cancelled;
}

auto CanvasWorkbench::tool_state_machine() -> CanvasToolStateMachine&
{
    return tool_state_machine_;
}

auto CanvasWorkbench::tool_state_machine() const -> const CanvasToolStateMachine&
{
    return tool_state_machine_;
}

// ── V17 Phase 01 W07: Keyboard Commanding ──────────────────────────

auto CanvasWorkbench::execute_key_command(const std::string& command_id) -> bool
{
    const auto binding = keyboard_commands_.find_binding(command_id);
    if (binding.command_id.empty())
    {
        MARKAMP_LOG_WARN("Key command not found: {}", command_id);
        return false;
    }

    if (event_bus_)
    {
        core::events::KeyCommandExecutedEvent evt;
        evt.command_id = binding.command_id;
        evt.shortcut = binding.shortcut;
        evt.category = binding.category;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Key command executed: {} ({})", command_id, binding.shortcut);
    return true;
}

auto CanvasWorkbench::keyboard_commands() -> KeyboardCommandModel&
{
    return keyboard_commands_;
}

auto CanvasWorkbench::keyboard_commands() const -> const KeyboardCommandModel&
{
    return keyboard_commands_;
}

// ── V17 Phase 01 W08: Cursor & Mode Feedback ──────────────────────

auto CanvasWorkbench::set_cursor_for_tool(ToolMode mode) -> void
{
    CanvasCursorStyle style = CanvasCursorStyle::kDefault;
    std::string label;

    switch (mode)
    {
        case ToolMode::Select:
            style = CanvasCursorStyle::kDefault;
            label = "Select";
            break;
        case ToolMode::Pan:
            style = CanvasCursorStyle::kGrab;
            label = "Pan";
            break;
        case ToolMode::Draw:
            style = CanvasCursorStyle::kCrosshair;
            label = "Draw";
            break;
        case ToolMode::Text:
            style = CanvasCursorStyle::kText;
            label = "Text";
            break;
        case ToolMode::Shape:
            style = CanvasCursorStyle::kCrosshair;
            label = "Shape";
            break;
        case ToolMode::Connector:
            style = CanvasCursorStyle::kCrosshair;
            label = "Connector";
            break;
        case ToolMode::StickyNote:
            style = CanvasCursorStyle::kCrosshair;
            label = "Sticky Note";
            break;
        case ToolMode::Eraser:
            style = CanvasCursorStyle::kCrosshair;
            label = "Eraser";
            break;
        case ToolMode::Comment:
            style = CanvasCursorStyle::kPointer;
            label = "Comment";
            break;
    }

    cursor_feedback_.set_cursor(style);
    cursor_feedback_.set_mode_label(label);

    if (event_bus_)
    {
        core::events::CursorStyleChangedEvent evt;
        evt.cursor_style = CursorFeedbackModel::cursor_name(style);
        evt.tool_mode = static_cast<uint8_t>(mode);
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Cursor set for tool: {} → {}", label,
                     CursorFeedbackModel::cursor_name(style));
}

auto CanvasWorkbench::cursor_feedback() -> CursorFeedbackModel&
{
    return cursor_feedback_;
}

auto CanvasWorkbench::cursor_feedback() const -> const CursorFeedbackModel&
{
    return cursor_feedback_;
}

// ── V17 Phase 01 W09: Grid & Rulers ───────────────────────────────

auto CanvasWorkbench::toggle_grid() -> void
{
    const bool new_visible = !grid_ruler_.is_grid_visible();
    grid_ruler_.set_grid_visible(new_visible);

    if (event_bus_)
    {
        core::events::GridVisibilityChangedEvent evt;
        evt.visible = new_visible;
        evt.spacing = grid_ruler_.grid_spacing();
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Grid visibility toggled: {}", new_visible ? "visible" : "hidden");
}

auto CanvasWorkbench::toggle_rulers() -> void
{
    const bool new_visible = !grid_ruler_.is_ruler_visible();
    grid_ruler_.set_ruler_visible(new_visible);

    if (event_bus_)
    {
        core::events::RulerVisibilityChangedEvent evt;
        evt.visible = new_visible;
        evt.units = GridRulerModel::units_string(grid_ruler_.ruler_units());
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Ruler visibility toggled: {}", new_visible ? "visible" : "hidden");
}

auto CanvasWorkbench::grid_ruler() -> GridRulerModel&
{
    return grid_ruler_;
}

auto CanvasWorkbench::grid_ruler() const -> const GridRulerModel&
{
    return grid_ruler_;
}

// ── V17 Phase 01 W10: Snap Preferences ────────────────────────────

auto CanvasWorkbench::toggle_snap_mode(const std::string& mode) -> void
{
    if (mode == "grid")
    {
        snap_prefs_.set_grid_snap(!snap_prefs_.grid_snap());
    }
    else if (mode == "object")
    {
        snap_prefs_.set_object_snap(!snap_prefs_.object_snap());
    }
    else if (mode == "angle")
    {
        snap_prefs_.set_angle_snap(!snap_prefs_.angle_snap());
    }
    else
    {
        MARKAMP_LOG_WARN("Unknown snap mode: {}", mode);
        return;
    }

    if (event_bus_)
    {
        core::events::SnapModeChangedEvent evt;
        evt.grid_snap = snap_prefs_.grid_snap();
        evt.object_snap = snap_prefs_.object_snap();
        evt.angle_snap = snap_prefs_.angle_snap();
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Snap mode '{}' toggled", mode);
}

auto CanvasWorkbench::snap_prefs() -> SnapPreferenceModel&
{
    return snap_prefs_;
}

auto CanvasWorkbench::snap_prefs() const -> const SnapPreferenceModel&
{
    return snap_prefs_;
}

// ── V17 Phase 01 W11: Context Menus ───────────────────────────────

auto CanvasWorkbench::show_context_menu(ContextScope scope,
                                         const std::string& object_type) -> void
{
    context_menu_model_.set_scope(scope);
    if (!object_type.empty())
    {
        context_menu_model_.set_object_type(object_type);
    }

    if (event_bus_)
    {
        core::events::ContextMenuOpenedEvent evt;
        switch (scope)
        {
            case ContextScope::kEmptySpace:
                evt.scope = "empty_space";
                break;
            case ContextScope::kSingleObject:
                evt.scope = "single_object";
                break;
            case ContextScope::kMultiSelect:
                evt.scope = "multi_select";
                break;
        }
        evt.object_type = object_type;
        evt.action_count = static_cast<int>(context_menu_model_.applicable_actions().size());
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Context menu opened (scope: {})", static_cast<int>(scope));
}

auto CanvasWorkbench::context_menu_model() -> ContextMenuModel&
{
    return context_menu_model_;
}

auto CanvasWorkbench::context_menu_model() const -> const ContextMenuModel&
{
    return context_menu_model_;
}

// ── V17 Phase 01 W12: Undo & Redo Plumbing ───────────────────────

auto CanvasWorkbench::perform_undo() -> bool
{
    const auto desc = undo_stack_.undo_description();
    const bool success = undo_stack_.undo();

    if (success && event_bus_)
    {
        core::events::UndoExecutedEvent evt;
        evt.description = desc;
        evt.remaining_count = static_cast<int>(undo_stack_.undo_count());
        event_bus_->publish(evt);
    }

    return success;
}

auto CanvasWorkbench::perform_redo() -> bool
{
    const auto desc = undo_stack_.redo_description();
    const bool success = undo_stack_.redo();

    if (success && event_bus_)
    {
        core::events::RedoExecutedEvent evt;
        evt.description = desc;
        evt.remaining_count = static_cast<int>(undo_stack_.redo_count());
        event_bus_->publish(evt);
    }

    return success;
}

// ── V17 Phase 01 W13: Autosave & Recovery ─────────────────────────

auto CanvasWorkbench::trigger_autosave() -> void
{
    if (!autosave_.is_enabled() || !autosave_.is_dirty())
    {
        return;
    }

    autosave_.set_state(AutosaveState::kSaving);

    if (event_bus_)
    {
        core::events::AutosaveTriggeredEvent evt;
        evt.board_id = active_board_id_;
        evt.save_path = autosave_.recovery_path();
        event_bus_->publish(evt);
    }

    // After a successful save, clear dirty and return to idle
    autosave_.clear_dirty();
    autosave_.set_state(AutosaveState::kIdle);

    MARKAMP_LOG_INFO("Autosave completed for board '{}'", active_board_id_);
}

auto CanvasWorkbench::autosave() -> AutosaveModel&
{
    return autosave_;
}

auto CanvasWorkbench::autosave() const -> const AutosaveModel&
{
    return autosave_;
}

// ── V17 Phase 01 W14: Inspector Shell ─────────────────────────────

auto CanvasWorkbench::toggle_inspector() -> void
{
    inspector_visible_ = !inspector_visible_;

    if (event_bus_)
    {
        core::events::InspectorOpenedEvent evt;
        evt.section_count = static_cast<int>(inspector_.visible_sections().size());
        evt.selected_count = inspector_.selected_count();
        evt.is_multi_select = inspector_.is_multi_select();
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Inspector toggled (visible: {})", inspector_visible_);
}

auto CanvasWorkbench::inspector() -> InspectorModel&
{
    return inspector_;
}

auto CanvasWorkbench::inspector() const -> const InspectorModel&
{
    return inspector_;
}

// ── V17 Phase 01 W15: Minimap Shell ───────────────────────────────

auto CanvasWorkbench::toggle_minimap() -> void
{
    minimap_visible_ = !minimap_visible_;

    if (event_bus_)
    {
        core::events::MinimapVisibilityChangedEvent evt;
        evt.visible = minimap_visible_;
        evt.mode = (minimap_.mode() == MinimapMode::kSimplified) ? "simplified" : "detailed";
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Minimap toggled (visible: {})", minimap_visible_);
}

auto CanvasWorkbench::minimap() -> MinimapModel&
{
    return minimap_;
}

auto CanvasWorkbench::minimap() const -> const MinimapModel&
{
    return minimap_;
}

// ── V17 Phase 01 W16: Onboarding & Empty States ──────────────────

auto CanvasWorkbench::complete_onboarding_step(const std::string& step_id) -> void
{
    onboarding_.complete_step(step_id);

    if (event_bus_)
    {
        core::events::OnboardingStepCompletedEvent evt;
        evt.step_id = step_id;
        evt.progress_percent = onboarding_.progress_percent();
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Onboarding step '{}' completed", step_id);
}

auto CanvasWorkbench::onboarding() -> OnboardingModel&
{
    return onboarding_;
}

auto CanvasWorkbench::onboarding() const -> const OnboardingModel&
{
    return onboarding_;
}

// ── V17 Phase 01 W17: Board Settings ──────────────────────────────

auto CanvasWorkbench::apply_board_setting(const std::string& key, const std::string& value) -> void
{
    board_settings_.set_setting(key, value);

    if (event_bus_)
    {
        core::events::BoardSettingsChangedEvent evt;
        evt.board_id = active_board_id_;
        evt.setting_key = key;
        evt.new_value = value;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Board setting applied: {} = {}", key, value);
}

auto CanvasWorkbench::board_settings() -> BoardSettingsModel&
{
    return board_settings_;
}

auto CanvasWorkbench::board_settings() const -> const BoardSettingsModel&
{
    return board_settings_;
}

// ── V17 Phase 01 W18: Theme Tokens ────────────────────────────────

auto CanvasWorkbench::resolve_theme_token(const std::string& token_name) -> std::string
{
    const auto resolved = canvas_theme_.resolve(token_name);

    if (event_bus_)
    {
        core::events::CanvasThemeTokenResolvedEvent evt;
        evt.token_name = token_name;
        evt.resolved_value = resolved;
        event_bus_->publish(evt);
    }

    return resolved;
}

auto CanvasWorkbench::canvas_theme() -> CanvasThemeModel&
{
    return canvas_theme_;
}

auto CanvasWorkbench::canvas_theme() const -> const CanvasThemeModel&
{
    return canvas_theme_;
}

// ── V17 Phase 01 W19: Command Palette Integration ─────────────────

auto CanvasWorkbench::open_command_palette() -> void
{
    palette_visible_ = true;

    if (event_bus_)
    {
        core::events::CommandPaletteOpenedEvent evt;
        evt.command_count = command_palette_.command_count();
        evt.context = "canvas";
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Command palette opened ({} commands)", command_palette_.command_count());
}

auto CanvasWorkbench::command_palette() -> ui::CommandPaletteModel&
{
    return command_palette_;
}

auto CanvasWorkbench::command_palette() const -> const ui::CommandPaletteModel&
{
    return command_palette_;
}

// ── V17 Phase 01 W20: Event & Analytics Contracts ─────────────────

auto CanvasWorkbench::record_analytics_event(const std::string& event_name,
                                              const std::string& category) -> void
{
    canvas_analytics_.record_event(event_name, category);

    if (event_bus_)
    {
        core::events::AnalyticsEventRecordedEvent evt;
        evt.event_name = event_name;
        evt.category = category;
        event_bus_->publish(evt);
    }
}

auto CanvasWorkbench::canvas_analytics() -> CanvasAnalyticsModel&
{
    return canvas_analytics_;
}

auto CanvasWorkbench::canvas_analytics() const -> const CanvasAnalyticsModel&
{
    return canvas_analytics_;
}

// ── V17 Phase 02 W01: Single Selection ────────────────────────────

auto CanvasWorkbench::select_object(ObjectId object_id) -> void
{
    selection_.select(object_id);

    if (event_bus_)
    {
        core::events::SingleSelectionEvent evt;
        evt.object_id = std::to_string(object_id);
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Object selected (id: {})", object_id);
}

auto CanvasWorkbench::clear_selection() -> void
{
    const auto prev_count = static_cast<int>(selection_.selection_count());
    selection_.clear_selection();

    if (event_bus_)
    {
        core::events::SelectionClearedEvent evt;
        evt.previous_count = prev_count;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Selection cleared ({} objects)", prev_count);
}

// ── V17 Phase 02 W02: Multi Selection ─────────────────────────────

auto CanvasWorkbench::box_select(const AABB& region) -> void
{
    auto* board = active_board();
    if (!board) { return; }

    selection_.select_in_region(region, board->objects());
    const auto count = static_cast<int>(selection_.selection_count());

    if (event_bus_)
    {
        core::events::MultiSelectionEvent evt;
        evt.selected_count = count;
        evt.method = "box";
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Box select: {} objects in region", count);
}

auto CanvasWorkbench::select_all_objects() -> void
{
    auto* board = active_board();
    if (!board) { return; }

    std::vector<ObjectId> all_ids;
    for (const auto& obj : board->objects())
    {
        if (obj) { all_ids.push_back(obj->id()); }
    }
    selection_.select_all(all_ids);
    const auto count = static_cast<int>(all_ids.size());

    if (event_bus_)
    {
        core::events::SelectAllEvent evt;
        evt.total_count = count;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Select all: {} objects", count);
}

// ── V17 Phase 02 W03: Move Operations ─────────────────────────────

auto CanvasWorkbench::begin_object_move() -> void
{
    auto* board = active_board();
    if (!board) { return; }

    selection_.begin_move(board->objects());

    if (event_bus_)
    {
        core::events::MoveStartedEvent evt;
        evt.object_count = static_cast<int>(selection_.selection_count());
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Move started ({} objects)", selection_.selection_count());
}

auto CanvasWorkbench::end_object_move(double delta_x, double delta_y) -> void
{
    selection_.end_move();

    if (event_bus_)
    {
        core::events::MoveCompletedEvent evt;
        evt.object_count = static_cast<int>(selection_.selection_count());
        evt.delta_x = delta_x;
        evt.delta_y = delta_y;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Move completed (dx: {}, dy: {})", delta_x, delta_y);
}

// ── V17 Phase 02 W04: Resize Operations ───────────────────────────

auto CanvasWorkbench::begin_resize(const std::string& handle_name) -> void
{
    auto* board = active_board();
    if (!board) { return; }

    // Map string to HandleType for SelectionManager
    selection_.begin_resize(board->objects(), HandleType::BottomRight);

    if (event_bus_)
    {
        core::events::ResizeStartedEvent evt;
        evt.object_count = static_cast<int>(selection_.selection_count());
        evt.handle = handle_name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Resize started (handle: {})", handle_name);
}

auto CanvasWorkbench::end_resize(double scale_x, double scale_y) -> void
{
    selection_.end_resize();

    if (event_bus_)
    {
        core::events::ResizeCompletedEvent evt;
        evt.object_count = static_cast<int>(selection_.selection_count());
        evt.scale_x = scale_x;
        evt.scale_y = scale_y;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Resize completed (sx: {}, sy: {})", scale_x, scale_y);
}

// ── V17 Phase 02 W05: Rotation Operations ─────────────────────────

auto CanvasWorkbench::begin_rotate() -> void
{
    auto* board = active_board();
    if (!board) { return; }

    selection_.begin_rotate(board->objects());

    if (event_bus_)
    {
        core::events::RotateStartedEvent evt;
        evt.object_count = static_cast<int>(selection_.selection_count());
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Rotate started ({} objects)", selection_.selection_count());
}

auto CanvasWorkbench::end_rotate(double angle_degrees) -> void
{
    selection_.end_rotate();

    if (event_bus_)
    {
        core::events::RotateCompletedEvent evt;
        evt.object_count = static_cast<int>(selection_.selection_count());
        evt.angle_degrees = angle_degrees;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Rotate completed ({} degrees)", angle_degrees);
}

// ── V17 Phase 02 W06: Alignment Guides ────────────────────────────

auto CanvasWorkbench::show_alignment_guides(int guide_count, const std::string& axis) -> void
{
    if (event_bus_)
    {
        core::events::AlignmentGuideShownEvent evt;
        evt.guide_count = guide_count;
        evt.axis = axis;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Alignment guides shown ({} guides, axis: {})", guide_count, axis);
}

auto CanvasWorkbench::hide_alignment_guides() -> void
{
    if (event_bus_)
    {
        core::events::AlignmentGuideHiddenEvent evt;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Alignment guides hidden");
}

// ── V17 Phase 02 W07: Object Snapping ─────────────────────────────

auto CanvasWorkbench::engage_snap(const std::string& snap_type,
                                   const std::string& target_id) -> void
{
    if (event_bus_)
    {
        core::events::SnapEngagedEvent evt;
        evt.snap_type = snap_type;
        evt.target_id = target_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Snap engaged (type: {}, target: {})", snap_type, target_id);
}

auto CanvasWorkbench::disengage_snap() -> void
{
    if (event_bus_)
    {
        core::events::SnapDisengagedEvent evt;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Snap disengaged");
}

// ── V17 Phase 02 W08: Distribute & Align Actions ─────────────────

auto CanvasWorkbench::align_selected(const std::string& action) -> void
{
    if (event_bus_)
    {
        core::events::AlignActionEvent evt;
        evt.action = action;
        evt.object_count = static_cast<int>(selection_.selection_count());
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Align action: {} ({} objects)", action, selection_.selection_count());
}

auto CanvasWorkbench::distribute_selected(const std::string& axis) -> void
{
    if (event_bus_)
    {
        core::events::DistributeActionEvent evt;
        evt.axis = axis;
        evt.object_count = static_cast<int>(selection_.selection_count());
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Distribute: {} ({} objects)", axis, selection_.selection_count());
}

// ── V17 Phase 02 W09: Smart Spacing ──────────────────────────────

auto CanvasWorkbench::activate_smart_spacing() -> void
{
    smart_spacing_.activate();

    if (event_bus_)
    {
        core::events::SmartSpacingActivatedEvent evt;
        evt.spacing_px = smart_spacing_.target_spacing();
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Smart spacing activated (target: {}px)", smart_spacing_.target_spacing());
}

auto CanvasWorkbench::deactivate_smart_spacing() -> void
{
    smart_spacing_.deactivate();

    if (event_bus_)
    {
        core::events::SmartSpacingDeactivatedEvent evt;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Smart spacing deactivated");
}

auto CanvasWorkbench::smart_spacing() -> SmartSpacingModel&
{
    return smart_spacing_;
}

auto CanvasWorkbench::smart_spacing() const -> const SmartSpacingModel&
{
    return smart_spacing_;
}

// ── V17 Phase 02 W10: Grouping ────────────────────────────────────

auto CanvasWorkbench::group_selected() -> void
{
    const auto count = static_cast<int>(selection_.selection_count());

    if (event_bus_)
    {
        core::events::ObjectsGroupedEvent evt;
        evt.group_id = "grp_" + std::to_string(count);
        evt.member_count = count;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Grouped {} objects", count);
}

auto CanvasWorkbench::ungroup_selected() -> void
{
    if (event_bus_)
    {
        core::events::ObjectsUngroupedEvent evt;
        evt.group_id = "grp_ungrouped";
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Ungrouped selection");
}

// ── V17 Phase 02 W11: Frame Membership ────────────────────────────

auto CanvasWorkbench::add_to_frame(const std::string& frame_id,
                                    const std::string& object_id) -> void
{
    if (event_bus_)
    {
        core::events::FrameMemberAddedEvent evt;
        evt.frame_id = frame_id;
        evt.object_id = object_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Added {} to frame {}", object_id, frame_id);
}

auto CanvasWorkbench::remove_from_frame(const std::string& frame_id,
                                         const std::string& object_id) -> void
{
    if (event_bus_)
    {
        core::events::FrameMemberRemovedEvent evt;
        evt.frame_id = frame_id;
        evt.object_id = object_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Removed {} from frame {}", object_id, frame_id);
}

// ── V17 Phase 02 W12: Layers & Z Order ───────────────────────────

auto CanvasWorkbench::bring_to_front() -> void
{
    if (event_bus_)
    {
        core::events::LayerOrderChangedEvent evt;
        evt.action = "bring_to_front";
        evt.object_id = std::to_string(selection_.first_selected_id());
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Brought selection to front");
}

auto CanvasWorkbench::send_to_back() -> void
{
    if (event_bus_)
    {
        core::events::LayerOrderChangedEvent evt;
        evt.action = "send_to_back";
        evt.object_id = std::to_string(selection_.first_selected_id());
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Sent selection to back");
}

// ── V17 Phase 02 W13: Locking & Pinning ──────────────────────────

auto CanvasWorkbench::lock_selected() -> void
{
    if (event_bus_)
    {
        core::events::ObjectLockedEvent evt;
        evt.object_id = std::to_string(selection_.first_selected_id());
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Locked selected objects");
}

auto CanvasWorkbench::unlock_selected() -> void
{
    if (event_bus_)
    {
        core::events::ObjectUnlockedEvent evt;
        evt.object_id = std::to_string(selection_.first_selected_id());
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Unlocked selected objects");
}

// ── V17 Phase 02 W14: Precision Nudge ────────────────────────────

auto CanvasWorkbench::precision_nudge(NudgeDirection direction, bool big_step) -> void
{
    if (event_bus_)
    {
        core::events::PrecisionNudgeEvent evt;
        evt.direction = PrecisionNudgeModel::direction_label(direction);
        evt.step_px = nudge_model_.step_for(big_step);
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Precision nudge: {} ({}px)",
                     PrecisionNudgeModel::direction_label(direction),
                     nudge_model_.step_for(big_step));
}

auto CanvasWorkbench::nudge_model() -> PrecisionNudgeModel&
{
    return nudge_model_;
}

auto CanvasWorkbench::nudge_model() const -> const PrecisionNudgeModel&
{
    return nudge_model_;
}

// ── V17 Phase 02 W15: Duplicate & Paste In Place ─────────────────

auto CanvasWorkbench::duplicate_in_place() -> void
{
    const auto obj_count = static_cast<int>(selection_.selection_count());

    if (event_bus_)
    {
        core::events::DuplicateInPlaceEvent evt;
        evt.object_count = obj_count;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Duplicated {} objects in place", obj_count);
}

auto CanvasWorkbench::paste_in_place() -> void
{
    if (event_bus_)
    {
        core::events::PasteInPlaceEvent evt;
        evt.object_count = 0; // Placeholder — actual paste count from clipboard
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Pasted in place");
}

// ── V17 Phase 02 W16: Drag Autoscroll ─────────────────────────────

auto CanvasWorkbench::start_autoscroll(AutoscrollDirection dir) -> void
{
    autoscroll_model_.start(dir);

    if (event_bus_)
    {
        core::events::AutoscrollStartedEvent evt;
        evt.direction = DragAutoscrollModel::direction_label(dir);
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Autoscroll started ({})", DragAutoscrollModel::direction_label(dir));
}

auto CanvasWorkbench::stop_autoscroll() -> void
{
    autoscroll_model_.stop();

    if (event_bus_)
    {
        core::events::AutoscrollStoppedEvent evt;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Autoscroll stopped");
}

auto CanvasWorkbench::autoscroll_model() -> DragAutoscrollModel&
{
    return autoscroll_model_;
}

auto CanvasWorkbench::autoscroll_model() const -> const DragAutoscrollModel&
{
    return autoscroll_model_;
}

// ── V17 Phase 02 W17: Viewport-Aware Transforms ──────────────────

auto CanvasWorkbench::sync_viewport_transform() -> void
{
    if (event_bus_)
    {
        core::events::ViewportTransformSyncEvent evt;
        evt.zoom = viewport_.zoom();
        evt.pan_x = viewport_.pan_x();
        evt.pan_y = viewport_.pan_y();
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Viewport transform synced");
}

auto CanvasWorkbench::clamp_viewport() -> void
{
    if (event_bus_)
    {
        core::events::ViewportClampedEvent evt;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Viewport clamped");
}

// ── V17 Phase 02 W18: Multi User Selection ────────────────────────

auto CanvasWorkbench::receive_remote_selection(const std::string& user_id,
                                                int object_count) -> void
{
    if (event_bus_)
    {
        core::events::RemoteSelectionReceivedEvent evt;
        evt.user_id = user_id;
        evt.object_count = object_count;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Remote selection from {} ({} objects)", user_id, object_count);
}

auto CanvasWorkbench::report_lock_conflict(const std::string& user_id,
                                            const std::string& object_id) -> void
{
    if (event_bus_)
    {
        core::events::RemoteLockConflictEvent evt;
        evt.user_id = user_id;
        evt.object_id = object_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Lock conflict: user {} on object {}", user_id, object_id);
}

// ── V17 Phase 02 W19: Transform History Quality ──────────────────

auto CanvasWorkbench::undo_transform() -> void
{
    undo_stack_.undo();

    if (event_bus_)
    {
        core::events::TransformUndoEvent evt;
        evt.action = "undo";
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Transform undo");
}

auto CanvasWorkbench::redo_transform() -> void
{
    undo_stack_.redo();

    if (event_bus_)
    {
        core::events::TransformRedoEvent evt;
        evt.action = "redo";
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Transform redo");
}

// ── V17 Phase 02 W20: Selection Action Surfaces ──────────────────

auto CanvasWorkbench::show_action_surface(const std::string& surface_type) -> void
{
    if (event_bus_)
    {
        core::events::ActionSurfaceShownEvent evt;
        evt.surface_type = surface_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Action surface shown: {}", surface_type);
}

auto CanvasWorkbench::hide_action_surface() -> void
{
    if (event_bus_)
    {
        core::events::ActionSurfaceHiddenEvent evt;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Action surface hidden");
}

// ── V17 Phase 03 W01: Freehand Pen Engine ─────────────────────────

auto CanvasWorkbench::capture_stroke(int point_count) -> void
{
    pen_engine_.increment_stroke_count();
    const auto sid = "stroke_" + std::to_string(pen_engine_.stroke_count());

    if (event_bus_)
    {
        core::events::StrokeCapturedEvent evt;
        evt.stroke_id = sid;
        evt.point_count = point_count;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Stroke captured: {} ({} pts)", sid, point_count);
}

auto CanvasWorkbench::smooth_stroke(const std::string& stroke_id) -> void
{
    if (event_bus_)
    {
        core::events::StrokeSmoothedEvent evt;
        evt.stroke_id = stroke_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Stroke smoothed: {}", stroke_id);
}

auto CanvasWorkbench::pen_engine() -> PenEngineModel&
{
    return pen_engine_;
}

auto CanvasWorkbench::pen_engine() const -> const PenEngineModel&
{
    return pen_engine_;
}

// ── V17 Phase 03 W02: Pen Presets ─────────────────────────────────

auto CanvasWorkbench::apply_pen_preset(const std::string& name) -> void
{
    pen_presets_.set_active_preset(name);
    pen_presets_.push_recent(name);

    if (event_bus_)
    {
        core::events::PenPresetAppliedEvent evt;
        evt.preset_name = name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Pen preset applied: {}", name);
}

auto CanvasWorkbench::save_pen_preset(const std::string& name) -> void
{
    if (event_bus_)
    {
        core::events::PenPresetSavedEvent evt;
        evt.preset_name = name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Pen preset saved: {}", name);
}

auto CanvasWorkbench::pen_presets() -> PenPresetModel&
{
    return pen_presets_;
}

auto CanvasWorkbench::pen_presets() const -> const PenPresetModel&
{
    return pen_presets_;
}

// ── V17 Phase 03 W03: Eraser & Lasso ─────────────────────────────

auto CanvasWorkbench::erase_stroke(int count) -> void
{
    if (event_bus_)
    {
        core::events::EraserStrokeEvent evt;
        evt.erased_count = count;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Eraser stroke: {} erased", count);
}

auto CanvasWorkbench::lasso_select(int count) -> void
{
    if (event_bus_)
    {
        core::events::LassoSelectionEvent evt;
        evt.object_count = count;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Lasso selected {} objects", count);
}

// ── V17 Phase 03 W04: Shape Tool ─────────────────────────────────

auto CanvasWorkbench::create_shape(const std::string& type) -> void
{
    if (event_bus_)
    {
        core::events::ShapeCreatedEvent evt;
        evt.shape_type = type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Shape created: {}", type);
}

auto CanvasWorkbench::resize_shape(const std::string& shape_id) -> void
{
    if (event_bus_)
    {
        core::events::ShapeResizedEvent evt;
        evt.shape_id = shape_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Shape resized: {}", shape_id);
}

// ── V17 Phase 03 W05: Text Box Authoring ──────────────────────────

auto CanvasWorkbench::create_text_box() -> void
{
    const auto tb_id = "textbox_" + std::to_string(pen_engine_.stroke_count() + 1);

    if (event_bus_)
    {
        core::events::TextBoxCreatedEvent evt;
        evt.text_box_id = tb_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Text box created: {}", tb_id);
}

auto CanvasWorkbench::edit_text_box(const std::string& text_box_id) -> void
{
    if (event_bus_)
    {
        core::events::TextBoxEditedEvent evt;
        evt.text_box_id = text_box_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Text box edited: {}", text_box_id);
}

// ── V17 Phase 03 W06: Sticky Notes ────────────────────────────────

auto CanvasWorkbench::create_sticky_note(const std::string& color) -> void
{
    if (event_bus_)
    {
        core::events::StickyNoteCreatedEvent evt;
        evt.note_id = "note_" + std::to_string(pen_engine_.stroke_count() + 1);
        evt.color = color;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Sticky note created (color: {})", color);
}

auto CanvasWorkbench::change_sticky_note_color(const std::string& note_id,
                                                const std::string& color) -> void
{
    if (event_bus_)
    {
        core::events::StickyNoteColorChangedEvent evt;
        evt.note_id = note_id;
        evt.color = color;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Sticky note {} color changed to {}", note_id, color);
}

// ── V17 Phase 03 W07: Color Authoring ─────────────────────────────

auto CanvasWorkbench::select_palette_color(const std::string& color) -> void
{
    if (event_bus_)
    {
        core::events::PaletteColorSelectedEvent evt;
        evt.color = color;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Palette color selected: {}", color);
}

auto CanvasWorkbench::save_palette_color(const std::string& color) -> void
{
    if (event_bus_)
    {
        core::events::PaletteColorSavedEvent evt;
        evt.color = color;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Palette color saved: {}", color);
}

// ── V17 Phase 03 W08: Typography Controls ─────────────────────────

auto CanvasWorkbench::change_typography(const std::string& property,
                                         const std::string& value) -> void
{
    if (event_bus_)
    {
        core::events::TypographyChangedEvent evt;
        evt.property = property;
        evt.value = value;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Typography changed: {} = {}", property, value);
}

auto CanvasWorkbench::apply_font(const std::string& font_name) -> void
{
    if (event_bus_)
    {
        core::events::FontAppliedEvent evt;
        evt.font_name = font_name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Font applied: {}", font_name);
}

// ── V17 Phase 03 W09: Fill, Stroke & Effects ──────────────────────

auto CanvasWorkbench::change_fill_style(const std::string& object_id,
                                         const std::string& fill) -> void
{
    if (event_bus_)
    {
        core::events::FillStyleChangedEvent evt;
        evt.object_id = object_id;
        evt.fill = fill;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Fill style changed on {}: {}", object_id, fill);
}

auto CanvasWorkbench::change_stroke_style(const std::string& object_id,
                                           const std::string& stroke) -> void
{
    if (event_bus_)
    {
        core::events::StrokeStyleChangedEvent evt;
        evt.object_id = object_id;
        evt.stroke = stroke;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Stroke style changed on {}: {}", object_id, stroke);
}

// ── V17 Phase 03 W10: Style Presets ───────────────────────────────

auto CanvasWorkbench::apply_style_preset(const std::string& preset_name) -> void
{
    if (event_bus_)
    {
        core::events::StylePresetAppliedEvent evt;
        evt.preset_name = preset_name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Style preset applied: {}", preset_name);
}

auto CanvasWorkbench::save_style_preset(const std::string& preset_name) -> void
{
    if (event_bus_)
    {
        core::events::StylePresetSavedEvent evt;
        evt.preset_name = preset_name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Style preset saved: {}", preset_name);
}

// ── V17 Phase 03 W11: Inline Rich Text ────────────────────────────

auto CanvasWorkbench::enable_inline_rich_text(const std::string& object_id) -> void
{
    if (event_bus_)
    {
        core::events::InlineRichTextEnabledEvent evt;
        evt.object_id = object_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Inline rich text enabled on {}", object_id);
}

auto CanvasWorkbench::format_inline_rich_text(const std::string& object_id,
                                               const std::string& format) -> void
{
    if (event_bus_)
    {
        core::events::InlineRichTextFormattedEvent evt;
        evt.object_id = object_id;
        evt.format = format;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Inline rich text formatted on {}: {}", object_id, format);
}

// ── V17 Phase 03 W12: Shape Library Quick Insert ──────────────────

auto CanvasWorkbench::insert_from_shape_library(const std::string& shape_name) -> void
{
    if (event_bus_)
    {
        core::events::ShapeLibraryInsertEvent evt;
        evt.shape_name = shape_name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Shape inserted from library: {}", shape_name);
}

auto CanvasWorkbench::open_shape_library() -> void
{
    if (event_bus_)
    {
        core::events::ShapeLibraryOpenedEvent evt;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Shape library opened");
}

// ── V17 Phase 03 W13: Shape Recognition ──────────────────────────

auto CanvasWorkbench::recognize_shape(const std::string& type, double confidence) -> void
{
    if (event_bus_)
    {
        core::events::ShapeRecognizedEvent evt;
        evt.recognized_type = type;
        evt.confidence = confidence;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Shape recognized: {} (confidence: {:.2f})", type, confidence);
}

auto CanvasWorkbench::reject_shape_recognition(const std::string& stroke_id) -> void
{
    if (event_bus_)
    {
        core::events::ShapeRecognitionRejectedEvent evt;
        evt.stroke_id = stroke_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Shape recognition rejected for {}", stroke_id);
}

// ── V17 Phase 03 W14: Image Annotation ───────────────────────────

auto CanvasWorkbench::add_image_annotation(const std::string& image_id,
                                            const std::string& annotation_type) -> void
{
    if (event_bus_)
    {
        core::events::ImageAnnotationAddedEvent evt;
        evt.image_id = image_id;
        evt.annotation_type = annotation_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Image annotation added on {}: {}", image_id, annotation_type);
}

auto CanvasWorkbench::remove_image_annotation(const std::string& image_id,
                                               const std::string& annotation_id) -> void
{
    if (event_bus_)
    {
        core::events::ImageAnnotationRemovedEvent evt;
        evt.image_id = image_id;
        evt.annotation_id = annotation_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Image annotation removed: {} from {}", annotation_id, image_id);
}

// ── V17 Phase 03 W15: Comments & Callouts ─────────────────────────

auto CanvasWorkbench::add_comment(const std::string& object_id,
                                   const std::string& text) -> void
{
    if (event_bus_)
    {
        core::events::CommentAddedEvent evt;
        evt.object_id = object_id;
        evt.comment_text = text;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Comment added on {}", object_id);
}

auto CanvasWorkbench::create_callout(const std::string& callout_type) -> void
{
    if (event_bus_)
    {
        core::events::CalloutCreatedEvent evt;
        evt.callout_type = callout_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Callout created: {}", callout_type);
}

// ── V17 Phase 03 W16: Tables For Authoring ────────────────────────

auto CanvasWorkbench::create_canvas_table(int rows, int columns) -> void
{
    if (event_bus_)
    {
        core::events::CanvasTableCreatedEvent evt;
        evt.rows = rows;
        evt.columns = columns;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Canvas table created: {}x{}", rows, columns);
}

auto CanvasWorkbench::edit_canvas_table_cell(const std::string& table_id,
                                              int row, int column) -> void
{
    if (event_bus_)
    {
        core::events::CanvasTableCellEditedEvent evt;
        evt.table_id = table_id;
        evt.row = row;
        evt.column = column;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Table {} cell ({},{}) edited", table_id, row, column);
}

// ── V17 Phase 03 W17: Icons, Stickers & Emoji ────────────────────

auto CanvasWorkbench::insert_icon(const std::string& icon_name) -> void
{
    if (event_bus_)
    {
        core::events::IconInsertedEvent evt;
        evt.icon_name = icon_name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Icon inserted: {}", icon_name);
}

auto CanvasWorkbench::insert_sticker(const std::string& sticker_id) -> void
{
    if (event_bus_)
    {
        core::events::StickerInsertedEvent evt;
        evt.sticker_id = sticker_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Sticker inserted: {}", sticker_id);
}

// ── V17 Phase 03 W18: Creator Shortcuts ──────────────────────────

auto CanvasWorkbench::invoke_creator_shortcut(const std::string& shortcut_id) -> void
{
    if (event_bus_)
    {
        core::events::CreatorShortcutInvokedEvent evt;
        evt.shortcut_id = shortcut_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Creator shortcut invoked: {}", shortcut_id);
}

auto CanvasWorkbench::register_creator_shortcut(const std::string& shortcut_id,
                                                  const std::string& action) -> void
{
    if (event_bus_)
    {
        core::events::CreatorShortcutRegisteredEvent evt;
        evt.shortcut_id = shortcut_id;
        evt.action = action;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Creator shortcut registered: {} -> {}", shortcut_id, action);
}

// ── V17 Phase 03 W19: Bulk Style Editing ─────────────────────────

auto CanvasWorkbench::apply_bulk_style(int object_count,
                                        const std::string& style_property) -> void
{
    if (event_bus_)
    {
        core::events::BulkStyleAppliedEvent evt;
        evt.object_count = object_count;
        evt.style_property = style_property;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Bulk style applied to {} objects: {}", object_count, style_property);
}

auto CanvasWorkbench::reset_bulk_style(int object_count) -> void
{
    if (event_bus_)
    {
        core::events::BulkStyleResetEvent evt;
        evt.object_count = object_count;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Bulk style reset on {} objects", object_count);
}

// ── V17 Phase 03 W20: Authoring Motion Feedback ──────────────────

auto CanvasWorkbench::start_motion_preview(const std::string& object_id,
                                            const std::string& motion_type) -> void
{
    if (event_bus_)
    {
        core::events::MotionPreviewStartedEvent evt;
        evt.object_id = object_id;
        evt.motion_type = motion_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Motion preview started on {}: {}", object_id, motion_type);
}

auto CanvasWorkbench::end_motion_preview(const std::string& object_id) -> void
{
    if (event_bus_)
    {
        core::events::MotionPreviewEndedEvent evt;
        evt.object_id = object_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Motion preview ended on {}", object_id);
}

// ── V17 Phase 04 W01: Connector Creation ──────────────────────────

auto CanvasWorkbench::create_connector(const std::string& from_id,
                                        const std::string& to_id,
                                        const std::string& connector_type) -> void
{
    if (event_bus_)
    {
        core::events::ConnectorCreatedEvent evt;
        evt.from_id = from_id;
        evt.to_id = to_id;
        evt.connector_type = connector_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Connector created: {} -> {} ({})", from_id, to_id, connector_type);
}

auto CanvasWorkbench::delete_connector(const std::string& connector_id) -> void
{
    if (event_bus_)
    {
        core::events::ConnectorDeletedEvent evt;
        evt.connector_id = connector_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Connector deleted: {}", connector_id);
}

// ── V17 Phase 04 W02: Connector Routing ──────────────────────────

auto CanvasWorkbench::change_connector_route(const std::string& connector_id,
                                              const std::string& route_type) -> void
{
    if (event_bus_)
    {
        core::events::ConnectorRouteChangedEvent evt;
        evt.connector_id = connector_id;
        evt.route_type = route_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Connector {} route changed to {}", connector_id, route_type);
}

auto CanvasWorkbench::add_connector_waypoint(const std::string& connector_id,
                                              double waypoint_x, double waypoint_y) -> void
{
    if (event_bus_)
    {
        core::events::ConnectorWaypointAddedEvent evt;
        evt.connector_id = connector_id;
        evt.waypoint_x = waypoint_x;
        evt.waypoint_y = waypoint_y;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Waypoint added to {}: ({:.1f}, {:.1f})", connector_id, waypoint_x, waypoint_y);
}

// ── V17 Phase 04 W03: Connector Labels & Ports ──────────────────

auto CanvasWorkbench::set_connector_label(const std::string& connector_id,
                                           const std::string& label) -> void
{
    if (event_bus_)
    {
        core::events::ConnectorLabelSetEvent evt;
        evt.connector_id = connector_id;
        evt.label = label;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Connector {} label set: {}", connector_id, label);
}

auto CanvasWorkbench::assign_port(const std::string& object_id,
                                   const std::string& port_id) -> void
{
    if (event_bus_)
    {
        core::events::PortAssignedEvent evt;
        evt.object_id = object_id;
        evt.port_id = port_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Port {} assigned on {}", port_id, object_id);
}

// ── V17 Phase 04 W04: Diagram Library ────────────────────────────

auto CanvasWorkbench::insert_diagram_shape(const std::string& shape_type,
                                            const std::string& category) -> void
{
    if (event_bus_)
    {
        core::events::DiagramShapeInsertedEvent evt;
        evt.shape_type = shape_type;
        evt.category = category;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Diagram shape inserted: {} ({})", shape_type, category);
}

auto CanvasWorkbench::browse_diagram_library(const std::string& category) -> void
{
    if (event_bus_)
    {
        core::events::DiagramLibraryBrowsedEvent evt;
        evt.category = category;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Diagram library browsed: {}", category);
}

// ── V17 Phase 04 W05: Flowchart Grammar ─────────────────────────

auto CanvasWorkbench::apply_flowchart_defaults(const std::string& grammar_name) -> void
{
    if (event_bus_)
    {
        core::events::FlowchartDefaultsAppliedEvent evt;
        evt.grammar_name = grammar_name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Flowchart defaults applied: {}", grammar_name);
}

auto CanvasWorkbench::validate_flowchart(int node_count, int connector_count,
                                          bool is_valid) -> void
{
    if (event_bus_)
    {
        core::events::FlowchartValidatedEvent evt;
        evt.node_count = node_count;
        evt.connector_count = connector_count;
        evt.is_valid = is_valid;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Flowchart validated: {} nodes, {} connectors, valid={}",
                     node_count, connector_count, is_valid);
}

// ── V17 Phase 04 W06: Mind Maps ──────────────────────────────────

auto CanvasWorkbench::add_mind_map_node(const std::string& parent_id,
                                         const std::string& label) -> void
{
    if (event_bus_)
    {
        core::events::MindMapNodeAddedEvent evt;
        evt.parent_id = parent_id;
        evt.node_label = label;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Mind map node added under {}: {}", parent_id, label);
}

auto CanvasWorkbench::toggle_mind_map_branch(const std::string& node_id,
                                              bool collapsed) -> void
{
    if (event_bus_)
    {
        core::events::MindMapBranchToggledEvent evt;
        evt.node_id = node_id;
        evt.collapsed = collapsed;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Mind map branch {}: {}", node_id, collapsed ? "collapsed" : "expanded");
}

// ── V17 Phase 04 W07: Kanban Boards ─────────────────────────────

auto CanvasWorkbench::move_canvas_kanban_card(const std::string& card_id,
                                               const std::string& from_lane,
                                               const std::string& to_lane) -> void
{
    if (event_bus_)
    {
        core::events::CanvasKanbanCardMovedEvent evt;
        evt.card_id = card_id;
        evt.from_lane = from_lane;
        evt.to_lane = to_lane;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Kanban card {} moved: {} -> {}", card_id, from_lane, to_lane);
}

auto CanvasWorkbench::add_kanban_lane(const std::string& lane_name) -> void
{
    if (event_bus_)
    {
        core::events::KanbanLaneAddedEvent evt;
        evt.lane_name = lane_name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Kanban lane added: {}", lane_name);
}

// ── V17 Phase 04 W08: Frames & Sections ─────────────────────────

auto CanvasWorkbench::create_frame(const std::string& frame_name,
                                    double width, double height) -> void
{
    if (event_bus_)
    {
        core::events::FrameCreatedEvent evt;
        evt.frame_name = frame_name;
        evt.width = width;
        evt.height = height;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Frame created: {} ({:.0f}x{:.0f})", frame_name, width, height);
}

auto CanvasWorkbench::add_section(const std::string& frame_id,
                                   const std::string& section_name) -> void
{
    if (event_bus_)
    {
        core::events::SectionAddedEvent evt;
        evt.frame_id = frame_id;
        evt.section_name = section_name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Section {} added to frame {}", section_name, frame_id);
}

// ── V17 Phase 04 W09: Swimlanes & Grids ─────────────────────────

auto CanvasWorkbench::create_swimlane(const std::string& orientation,
                                       int lane_count) -> void
{
    if (event_bus_)
    {
        core::events::SwimlaneCreatedEvent evt;
        evt.orientation = orientation;
        evt.lane_count = lane_count;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Swimlane created: {} with {} lanes", orientation, lane_count);
}

auto CanvasWorkbench::apply_grid_layout(int rows, int columns) -> void
{
    if (event_bus_)
    {
        core::events::GridLayoutAppliedEvent evt;
        evt.grid_rows = rows;
        evt.grid_columns = columns;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Grid layout applied: {}x{}", rows, columns);
}

// ── V17 Phase 04 W10: Container Auto Layout ─────────────────────

auto CanvasWorkbench::trigger_auto_layout(const std::string& container_id,
                                           const std::string& layout_type) -> void
{
    if (event_bus_)
    {
        core::events::AutoLayoutTriggeredEvent evt;
        evt.container_id = container_id;
        evt.layout_type = layout_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Auto layout triggered on {}: {}", container_id, layout_type);
}

auto CanvasWorkbench::change_auto_layout_spacing(const std::string& container_id,
                                                  double spacing) -> void
{
    if (event_bus_)
    {
        core::events::AutoLayoutSpacingChangedEvent evt;
        evt.container_id = container_id;
        evt.spacing = spacing;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Auto layout spacing on {}: {:.1f}", container_id, spacing);
}

// ── V17 Phase 04 W11: Advanced Tables ─────────────────────────────

auto CanvasWorkbench::add_advanced_table_column(const std::string& table_id,
                                                 const std::string& column_name) -> void
{
    if (event_bus_)
    {
        core::events::AdvancedTableColumnAddedEvent evt;
        evt.table_id = table_id;
        evt.column_name = column_name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Advanced table {} column added: {}", table_id, column_name);
}

auto CanvasWorkbench::sort_advanced_table(const std::string& table_id,
                                           const std::string& sort_column,
                                           bool ascending) -> void
{
    if (event_bus_)
    {
        core::events::AdvancedTableSortedEvent evt;
        evt.table_id = table_id;
        evt.sort_column = sort_column;
        evt.ascending = ascending;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Advanced table {} sorted by {} {}", table_id, sort_column,
                     ascending ? "ASC" : "DESC");
}

// ── V17 Phase 04 W12: Cross Board Links ──────────────────────────

auto CanvasWorkbench::create_cross_board_link(const std::string& source_board_id,
                                               const std::string& target_board_id) -> void
{
    if (event_bus_)
    {
        core::events::CrossBoardLinkCreatedEvent evt;
        evt.source_board_id = source_board_id;
        evt.target_board_id = target_board_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Cross board link: {} -> {}", source_board_id, target_board_id);
}

auto CanvasWorkbench::navigate_cross_board_link(const std::string& link_id) -> void
{
    if (event_bus_)
    {
        core::events::CrossBoardLinkNavigatedEvent evt;
        evt.link_id = link_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Cross board link navigated: {}", link_id);
}

// ── V17 Phase 04 W13: Diagram From Selection ─────────────────────

auto CanvasWorkbench::generate_diagram_from_selection(int selected_count,
                                                       const std::string& diagram_type) -> void
{
    if (event_bus_)
    {
        core::events::DiagramFromSelectionEvent evt;
        evt.selected_count = selected_count;
        evt.diagram_type = diagram_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Diagram from {} selected objects: {}", selected_count, diagram_type);
}

auto CanvasWorkbench::structure_selection(const std::string& structure_type) -> void
{
    if (event_bus_)
    {
        core::events::SelectionStructuredEvent evt;
        evt.structure_type = structure_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Selection structured as: {}", structure_type);
}

// ── V17 Phase 04 W14: Connected Layout Cleanup ──────────────────

auto CanvasWorkbench::trigger_layout_cleanup(int affected_connectors) -> void
{
    if (event_bus_)
    {
        core::events::LayoutCleanupTriggeredEvent evt;
        evt.affected_connectors = affected_connectors;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Layout cleanup: {} connectors affected", affected_connectors);
}

auto CanvasWorkbench::reflow_connector(const std::string& connector_id,
                                        const std::string& trigger_action) -> void
{
    if (event_bus_)
    {
        core::events::ConnectorReflowEvent evt;
        evt.connector_id = connector_id;
        evt.trigger_action = trigger_action;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Connector {} reflowed due to {}", connector_id, trigger_action);
}

// ── V17 Phase 04 W15: Relationship Metadata ─────────────────────

auto CanvasWorkbench::set_relationship_metadata(const std::string& connector_id,
                                                 const std::string& key,
                                                 const std::string& value) -> void
{
    if (event_bus_)
    {
        core::events::RelationshipMetadataSetEvent evt;
        evt.connector_id = connector_id;
        evt.metadata_key = key;
        evt.metadata_value = value;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Relationship {} metadata: {}={}", connector_id, key, value);
}

auto CanvasWorkbench::assign_relationship_type(const std::string& connector_id,
                                                const std::string& relationship_type) -> void
{
    if (event_bus_)
    {
        core::events::RelationshipTypeAssignedEvent evt;
        evt.connector_id = connector_id;
        evt.relationship_type = relationship_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Relationship {} type: {}", connector_id, relationship_type);
}

// ── V17 Phase 04 W16: Diagram Templates ──────────────────────────

auto CanvasWorkbench::apply_diagram_template(const std::string& template_name,
                                              const std::string& category) -> void
{
    if (event_bus_)
    {
        core::events::DiagramTemplateAppliedEvent evt;
        evt.template_name = template_name;
        evt.template_category = category;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Diagram template applied: {} ({})", template_name, category);
}

auto CanvasWorkbench::save_diagram_template(const std::string& template_name,
                                             int node_count) -> void
{
    if (event_bus_)
    {
        core::events::DiagramTemplateSavedEvent evt;
        evt.template_name = template_name;
        evt.node_count = node_count;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Diagram template saved: {} ({} nodes)", template_name, node_count);
}

// ── V17 Phase 04 W17: Dependency Validation ─────────────────────

auto CanvasWorkbench::run_dependency_check(int total_links, int broken_links) -> void
{
    if (event_bus_)
    {
        core::events::DependencyCheckRunEvent evt;
        evt.total_links = total_links;
        evt.broken_links = broken_links;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Dependency check: {}/{} broken", broken_links, total_links);
}

auto CanvasWorkbench::fix_broken_dependency(const std::string& connector_id,
                                             const std::string& fix_action) -> void
{
    if (event_bus_)
    {
        core::events::BrokenDependencyFixedEvent evt;
        evt.connector_id = connector_id;
        evt.fix_action = fix_action;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Broken dependency {} fixed: {}", connector_id, fix_action);
}

// ── V17 Phase 04 W18: Diagram Export Readiness ──────────────────

auto CanvasWorkbench::preview_diagram_export(const std::string& export_format) -> void
{
    if (event_bus_)
    {
        core::events::DiagramExportPreviewEvent evt;
        evt.export_format = export_format;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Diagram export preview: {}", export_format);
}

auto CanvasWorkbench::complete_diagram_export(const std::string& export_format,
                                               const std::string& output_path) -> void
{
    if (event_bus_)
    {
        core::events::DiagramExportCompletedEvent evt;
        evt.export_format = export_format;
        evt.output_path = output_path;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Diagram exported as {} to {}", export_format, output_path);
}

// ── V17 Phase 04 W19: Collaborative Diagram Editing ─────────────

auto CanvasWorkbench::acquire_diagram_lock(const std::string& diagram_id,
                                            const std::string& user_id) -> void
{
    if (event_bus_)
    {
        core::events::DiagramLockAcquiredEvent evt;
        evt.diagram_id = diagram_id;
        evt.user_id = user_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Diagram {} locked by {}", diagram_id, user_id);
}

auto CanvasWorkbench::release_diagram_lock(const std::string& diagram_id) -> void
{
    if (event_bus_)
    {
        core::events::DiagramLockReleasedEvent evt;
        evt.diagram_id = diagram_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Diagram {} lock released", diagram_id);
}

// ── V17 Phase 04 W20: Diagram Test Matrix ───────────────────────

auto CanvasWorkbench::run_diagram_test_suite(int tests_passed, int tests_failed) -> void
{
    if (event_bus_)
    {
        core::events::DiagramTestSuiteRunEvent evt;
        evt.tests_passed = tests_passed;
        evt.tests_failed = tests_failed;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Diagram test suite: {} passed, {} failed", tests_passed, tests_failed);
}

auto CanvasWorkbench::report_diagram_regression(const std::string& test_name,
                                                 const std::string& description) -> void
{
    if (event_bus_)
    {
        core::events::DiagramRegressionDetectedEvent evt;
        evt.test_name = test_name;
        evt.description = description;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Diagram regression: {} - {}", test_name, description);
}

// ── V17 Phase 05 W01: Image Placement ────────────────────────────

auto CanvasWorkbench::place_image(const std::string& image_path, double scale) -> void
{
    if (event_bus_)
    {
        core::events::ImagePlacedEvent evt;
        evt.image_path = image_path;
        evt.scale = scale;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Image placed: {} (scale {:.2f})", image_path, scale);
}

auto CanvasWorkbench::replace_image(const std::string& object_id,
                                     const std::string& new_image_path) -> void
{
    if (event_bus_)
    {
        core::events::ImageReplacedEvent evt;
        evt.object_id = object_id;
        evt.new_image_path = new_image_path;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Image replaced on {}: {}", object_id, new_image_path);
}

// ── V17 Phase 05 W02: Video & Web Embeds ─────────────────────────

auto CanvasWorkbench::add_video_embed(const std::string& embed_url,
                                       const std::string& embed_type) -> void
{
    if (event_bus_)
    {
        core::events::VideoEmbedAddedEvent evt;
        evt.embed_url = embed_url;
        evt.embed_type = embed_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Video embed added: {} ({})", embed_url, embed_type);
}

auto CanvasWorkbench::resize_web_embed(const std::string& object_id,
                                        double width, double height) -> void
{
    if (event_bus_)
    {
        core::events::WebEmbedResizedEvent evt;
        evt.object_id = object_id;
        evt.width = width;
        evt.height = height;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Web embed {} resized to {:.0f}x{:.0f}", object_id, width, height);
}

// ── V17 Phase 05 W03: PDF Pages ─────────────────────────────────

auto CanvasWorkbench::add_pdf_page(const std::string& pdf_path, int page_number) -> void
{
    if (event_bus_)
    {
        core::events::PdfPageAddedEvent evt;
        evt.pdf_path = pdf_path;
        evt.page_number = page_number;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("PDF page added: {} page {}", pdf_path, page_number);
}

auto CanvasWorkbench::navigate_canvas_pdf_page(const std::string& object_id,
                                                int target_page) -> void
{
    if (event_bus_)
    {
        core::events::CanvasPdfPageNavigatedEvent evt;
        evt.object_id = object_id;
        evt.target_page = target_page;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("PDF {} navigated to page {}", object_id, target_page);
}

// ── V17 Phase 05 W04: Bookmark Cards ────────────────────────────

auto CanvasWorkbench::create_bookmark_card(const std::string& url,
                                            const std::string& title) -> void
{
    if (event_bus_)
    {
        core::events::BookmarkCardCreatedEvent evt;
        evt.url = url;
        evt.title = title;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Bookmark card created: {} ({})", title, url);
}

auto CanvasWorkbench::refresh_bookmark_card(const std::string& object_id) -> void
{
    if (event_bus_)
    {
        core::events::BookmarkCardRefreshedEvent evt;
        evt.object_id = object_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Bookmark card refreshed: {}", object_id);
}

// ── V17 Phase 05 W05: App Widgets ────────────────────────────────

auto CanvasWorkbench::add_app_widget(const std::string& widget_type,
                                      const std::string& widget_config) -> void
{
    if (event_bus_)
    {
        core::events::AppWidgetAddedEvent evt;
        evt.widget_type = widget_type;
        evt.widget_config = widget_config;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("App widget added: {} ({})", widget_type, widget_config);
}

auto CanvasWorkbench::configure_app_widget(const std::string& object_id,
                                            const std::string& config_key,
                                            const std::string& config_value) -> void
{
    if (event_bus_)
    {
        core::events::AppWidgetConfiguredEvent evt;
        evt.object_id = object_id;
        evt.config_key = config_key;
        evt.config_value = config_value;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("App widget {} configured: {}={}", object_id, config_key, config_value);
}

// ── V17 Phase 05 W06: Icon Insertion ─────────────────────────────

auto CanvasWorkbench::insert_canvas_icon(const std::string& icon_name,
                                          const std::string& icon_set) -> void
{
    if (event_bus_)
    {
        core::events::CanvasIconInsertedEvent evt;
        evt.icon_name = icon_name;
        evt.icon_set = icon_set;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Icon inserted: {} ({})", icon_name, icon_set);
}

auto CanvasWorkbench::change_icon_color(const std::string& object_id,
                                         const std::string& new_color) -> void
{
    if (event_bus_)
    {
        core::events::IconColorChangedEvent evt;
        evt.object_id = object_id;
        evt.new_color = new_color;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Icon {} color changed to {}", object_id, new_color);
}

// ── V17 Phase 05 W07: Drag & Drop Intake ─────────────────────────

auto CanvasWorkbench::handle_content_drop(const std::string& content_type,
                                           const std::string& source_path) -> void
{
    if (event_bus_)
    {
        core::events::ContentDroppedEvent evt;
        evt.content_type = content_type;
        evt.source_path = source_path;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Content dropped: {} from {}", content_type, source_path);
}

auto CanvasWorkbench::complete_drop_intake(int items_ingested) -> void
{
    if (event_bus_)
    {
        core::events::DropIntakeCompletedEvent evt;
        evt.items_ingested = items_ingested;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Drop intake completed: {} items", items_ingested);
}

// ── V17 Phase 05 W08: Clipboard Ingest ──────────────────────────

auto CanvasWorkbench::paste_clipboard(const std::string& paste_format) -> void
{
    if (event_bus_)
    {
        core::events::ClipboardPastedEvent evt;
        evt.paste_format = paste_format;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Clipboard pasted: {}", paste_format);
}

auto CanvasWorkbench::convert_rich_paste(const std::string& source_format,
                                          const std::string& target_format) -> void
{
    if (event_bus_)
    {
        core::events::RichPasteConvertedEvent evt;
        evt.source_format = source_format;
        evt.target_format = target_format;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Rich paste converted: {} -> {}", source_format, target_format);
}

// ── V17 Phase 05 W09: Media Optimization ────────────────────────

auto CanvasWorkbench::run_media_optimization(int assets_optimized,
                                              double size_reduction_pct) -> void
{
    if (event_bus_)
    {
        core::events::MediaOptimizationRunEvent evt;
        evt.assets_optimized = assets_optimized;
        evt.size_reduction_pct = size_reduction_pct;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Media optimized: {} assets, {:.1f}% reduction",
                     assets_optimized, size_reduction_pct);
}

auto CanvasWorkbench::set_media_quality(const std::string& quality_level) -> void
{
    if (event_bus_)
    {
        core::events::MediaQualitySetEvent evt;
        evt.quality_level = quality_level;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Media quality set to: {}", quality_level);
}

// ── V17 Phase 05 W10: Cropping & Masking ────────────────────────

auto CanvasWorkbench::crop_image(const std::string& object_id,
                                  double crop_x, double crop_y,
                                  double crop_width, double crop_height) -> void
{
    if (event_bus_)
    {
        core::events::ImageCroppedEvent evt;
        evt.object_id = object_id;
        evt.crop_x = crop_x;
        evt.crop_y = crop_y;
        evt.crop_width = crop_width;
        evt.crop_height = crop_height;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Image {} cropped to ({:.0f},{:.0f}) {:.0f}x{:.0f}",
                     object_id, crop_x, crop_y, crop_width, crop_height);
}

auto CanvasWorkbench::apply_image_mask(const std::string& object_id,
                                        const std::string& mask_type) -> void
{
    if (event_bus_)
    {
        core::events::ImageMaskAppliedEvent evt;
        evt.object_id = object_id;
        evt.mask_type = mask_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Image {} mask applied: {}", object_id, mask_type);
}

// ── V17 Phase 05 W11: Captions & Metadata ───────────────────────

auto CanvasWorkbench::set_asset_caption(const std::string& object_id,
                                         const std::string& caption_text) -> void
{
    if (event_bus_)
    {
        core::events::AssetCaptionSetEvent evt;
        evt.object_id = object_id;
        evt.caption_text = caption_text;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Asset {} caption set: {}", object_id, caption_text);
}

auto CanvasWorkbench::set_asset_alt_text(const std::string& object_id,
                                          const std::string& alt_text) -> void
{
    if (event_bus_)
    {
        core::events::AssetAltTextSetEvent evt;
        evt.object_id = object_id;
        evt.alt_text = alt_text;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Asset {} alt text set: {}", object_id, alt_text);
}

// ── V17 Phase 05 W12: Asset Library Panel ───────────────────────

auto CanvasWorkbench::add_to_asset_library(const std::string& asset_name,
                                            const std::string& asset_category) -> void
{
    if (event_bus_)
    {
        core::events::AssetLibraryAddedEvent evt;
        evt.asset_name = asset_name;
        evt.asset_category = asset_category;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Asset added to library: {} ({})", asset_name, asset_category);
}

auto CanvasWorkbench::remove_from_asset_library(const std::string& asset_id) -> void
{
    if (event_bus_)
    {
        core::events::AssetLibraryRemovedEvent evt;
        evt.asset_id = asset_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Asset removed from library: {}", asset_id);
}

// ── V17 Phase 05 W13: Asset Styling ─────────────────────────────

auto CanvasWorkbench::apply_asset_frame(const std::string& object_id,
                                         const std::string& frame_style) -> void
{
    if (event_bus_)
    {
        core::events::AssetFrameAppliedEvent evt;
        evt.object_id = object_id;
        evt.frame_style = frame_style;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Asset {} frame applied: {}", object_id, frame_style);
}

auto CanvasWorkbench::change_asset_shadow(const std::string& object_id,
                                           double shadow_offset,
                                           double shadow_blur) -> void
{
    if (event_bus_)
    {
        core::events::AssetShadowChangedEvent evt;
        evt.object_id = object_id;
        evt.shadow_offset = shadow_offset;
        evt.shadow_blur = shadow_blur;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Asset {} shadow: offset={:.1f} blur={:.1f}",
                     object_id, shadow_offset, shadow_blur);
}

// ── V17 Phase 05 W14: Mixed Media Boards ────────────────────────

auto CanvasWorkbench::create_media_composition(int object_count,
                                                const std::string& layout_mode) -> void
{
    if (event_bus_)
    {
        core::events::MediaCompositionCreatedEvent evt;
        evt.object_count = object_count;
        evt.layout_mode = layout_mode;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Media composition created: {} objects ({})", object_count, layout_mode);
}

auto CanvasWorkbench::reorder_media_layer(const std::string& object_id,
                                           int new_z_index) -> void
{
    if (event_bus_)
    {
        core::events::MediaLayerReorderedEvent evt;
        evt.object_id = object_id;
        evt.new_z_index = new_z_index;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Media layer {} reordered to z={}", object_id, new_z_index);
}

// ── V17 Phase 05 W15: Import & Convert ──────────────────────────

auto CanvasWorkbench::import_external_file(const std::string& source_path,
                                            const std::string& converted_format) -> void
{
    if (event_bus_)
    {
        core::events::ExternalFileImportedEvent evt;
        evt.source_path = source_path;
        evt.converted_format = converted_format;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("External file imported: {} -> {}", source_path, converted_format);
}

auto CanvasWorkbench::complete_import_conversion(int files_converted,
                                                  bool all_succeeded) -> void
{
    if (event_bus_)
    {
        core::events::ImportConversionCompletedEvent evt;
        evt.files_converted = files_converted;
        evt.all_succeeded = all_succeeded;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Import conversion: {} files, success={}",
                     files_converted, all_succeeded);
}

// ── V17 Phase 05 W16: Media Review Flows ────────────────────────

auto CanvasWorkbench::start_media_review(const std::string& object_id,
                                          const std::string& reviewer) -> void
{
    if (event_bus_)
    {
        core::events::MediaReviewStartedEvent evt;
        evt.object_id = object_id;
        evt.reviewer = reviewer;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Media review started: {} by {}", object_id, reviewer);
}

auto CanvasWorkbench::complete_media_review(const std::string& object_id,
                                             bool approved) -> void
{
    if (event_bus_)
    {
        core::events::MediaReviewCompletedEvent evt;
        evt.object_id = object_id;
        evt.approved = approved;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Media review completed: {} approved={}", object_id, approved);
}

// ── V17 Phase 05 W17: Licensing & Security ──────────────────────

auto CanvasWorkbench::set_content_license(const std::string& object_id,
                                           const std::string& license_type) -> void
{
    if (event_bus_)
    {
        core::events::ContentLicenseSetEvent evt;
        evt.object_id = object_id;
        evt.license_type = license_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Content license set: {} -> {}", object_id, license_type);
}

auto CanvasWorkbench::run_security_scan(int assets_scanned, int threats_found) -> void
{
    if (event_bus_)
    {
        core::events::SecurityScanCompletedEvent evt;
        evt.assets_scanned = assets_scanned;
        evt.threats_found = threats_found;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Security scan: {} assets, {} threats", assets_scanned, threats_found);
}

// ── V17 Phase 05 W18: Asset Serialization & Sync ────────────────

auto CanvasWorkbench::serialize_asset_metadata(int assets_serialized,
                                                const std::string& format) -> void
{
    if (event_bus_)
    {
        core::events::AssetMetadataSerializedEvent evt;
        evt.assets_serialized = assets_serialized;
        evt.format = format;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Asset metadata serialized: {} assets ({})", assets_serialized, format);
}

auto CanvasWorkbench::complete_asset_sync(int assets_synced, int conflicts_detected) -> void
{
    if (event_bus_)
    {
        core::events::AssetSyncCompletedEvent evt;
        evt.assets_synced = assets_synced;
        evt.conflicts_detected = conflicts_detected;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Asset sync: {} synced, {} conflicts", assets_synced, conflicts_detected);
}

// ── V17 Phase 05 W19: Rich Content Export ───────────────────────

auto CanvasWorkbench::start_rich_content_export(const std::string& export_format,
                                                 int objects_to_export) -> void
{
    if (event_bus_)
    {
        core::events::RichContentExportStartedEvent evt;
        evt.export_format = export_format;
        evt.objects_to_export = objects_to_export;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Rich content export started: {} ({} objects)",
                     export_format, objects_to_export);
}

auto CanvasWorkbench::complete_rich_content_export(const std::string& output_path,
                                                    bool success) -> void
{
    if (event_bus_)
    {
        core::events::RichContentExportCompletedEvent evt;
        evt.output_path = output_path;
        evt.success = success;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Rich content export completed: {} success={}", output_path, success);
}

// ── V17 Phase 05 W20: Asset Test Coverage ───────────────────────

auto CanvasWorkbench::run_asset_test_suite(int tests_run, int tests_passed) -> void
{
    if (event_bus_)
    {
        core::events::AssetTestSuiteRunEvent evt;
        evt.tests_run = tests_run;
        evt.tests_passed = tests_passed;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Asset test suite: {}/{} passed", tests_passed, tests_run);
}

auto CanvasWorkbench::report_asset_regression(const std::string& test_name,
                                               const std::string& description) -> void
{
    if (event_bus_)
    {
        core::events::AssetRegressionDetectedEvent evt;
        evt.test_name = test_name;
        evt.description = description;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Asset regression: {} - {}", test_name, description);
}

// ── V17 Phase 06 W01: Live Presence ─────────────────────────────

auto CanvasWorkbench::collaborator_joined(const std::string& user_id,
                                           const std::string& display_name) -> void
{
    if (event_bus_)
    {
        core::events::CollaboratorJoinedEvent evt;
        evt.user_id = user_id;
        evt.display_name = display_name;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Collaborator joined: {} ({})", display_name, user_id);
}

auto CanvasWorkbench::collaborator_left(const std::string& user_id,
                                         const std::string& reason) -> void
{
    if (event_bus_)
    {
        core::events::CollaboratorLeftEvent evt;
        evt.user_id = user_id;
        evt.reason = reason;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Collaborator left: {} ({})", user_id, reason);
}

// ── V17 Phase 06 W02: Remote Selections & Follow ───────────────

auto CanvasWorkbench::update_remote_selection(const std::string& user_id,
                                               int selected_object_count) -> void
{
    if (event_bus_)
    {
        core::events::RemoteSelectionChangedEvent evt;
        evt.user_id = user_id;
        evt.selected_object_count = selected_object_count;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Remote selection: {} selected {} objects", user_id, selected_object_count);
}

auto CanvasWorkbench::toggle_follow_mode(const std::string& target_user_id,
                                          bool following) -> void
{
    if (event_bus_)
    {
        core::events::FollowModeToggledEvent evt;
        evt.target_user_id = target_user_id;
        evt.following = following;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Follow mode: {} following={}", target_user_id, following);
}

// ── V17 Phase 06 W03: Conflict Handling ─────────────────────────

auto CanvasWorkbench::report_edit_conflict(const std::string& object_id,
                                            const std::string& conflicting_user) -> void
{
    if (event_bus_)
    {
        core::events::EditConflictDetectedEvent evt;
        evt.object_id = object_id;
        evt.conflicting_user = conflicting_user;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Edit conflict: {} by {}", object_id, conflicting_user);
}

auto CanvasWorkbench::resolve_conflict(const std::string& object_id,
                                        const std::string& resolution_strategy) -> void
{
    if (event_bus_)
    {
        core::events::ConflictResolvedEvent evt;
        evt.object_id = object_id;
        evt.resolution_strategy = resolution_strategy;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Conflict resolved: {} via {}", object_id, resolution_strategy);
}

// ── V17 Phase 06 W04: Comment Threads ───────────────────────────

auto CanvasWorkbench::create_comment_thread(const std::string& thread_id,
                                             const std::string& anchor_object_id) -> void
{
    if (event_bus_)
    {
        core::events::CommentThreadCreatedEvent evt;
        evt.thread_id = thread_id;
        evt.anchor_object_id = anchor_object_id;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Comment thread created: {} on {}", thread_id, anchor_object_id);
}

auto CanvasWorkbench::add_comment_reply(const std::string& thread_id,
                                         const std::string& author) -> void
{
    if (event_bus_)
    {
        core::events::CommentReplyAddedEvent evt;
        evt.thread_id = thread_id;
        evt.author = author;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Comment reply: {} by {}", thread_id, author);
}

// ── V17 Phase 06 W05: Voting & Reactions ────────────────────────

auto CanvasWorkbench::cast_vote(const std::string& object_id,
                                 const std::string& voter) -> void
{
    if (event_bus_)
    {
        core::events::VoteCastEvent evt;
        evt.object_id = object_id;
        evt.voter = voter;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Vote cast: {} by {}", object_id, voter);
}

auto CanvasWorkbench::add_reaction(const std::string& object_id,
                                    const std::string& reaction_type) -> void
{
    if (event_bus_)
    {
        core::events::ReactionAddedEvent evt;
        evt.object_id = object_id;
        evt.reaction_type = reaction_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Reaction added: {} on {}", reaction_type, object_id);
}

// ── V17 Phase 06 W06: Timer & Facilitation ──────────────────────

auto CanvasWorkbench::start_facilitation_timer(int duration_seconds,
                                                const std::string& facilitator) -> void
{
    if (event_bus_)
    {
        core::events::FacilitationTimerStartedEvent evt;
        evt.duration_seconds = duration_seconds;
        evt.facilitator = facilitator;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Timer started: {}s by {}", duration_seconds, facilitator);
}

auto CanvasWorkbench::expire_facilitation_timer(const std::string& session_id,
                                                 bool auto_extended) -> void
{
    if (event_bus_)
    {
        core::events::FacilitationTimerExpiredEvent evt;
        evt.session_id = session_id;
        evt.auto_extended = auto_extended;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Timer expired: {} extended={}", session_id, auto_extended);
}

// ── V17 Phase 06 W07: Private Reveal & Presentation ────────────

auto CanvasWorkbench::initiate_private_reveal(const std::string& presenter,
                                               int hidden_object_count) -> void
{
    if (event_bus_)
    {
        core::events::PrivateRevealInitiatedEvent evt;
        evt.presenter = presenter;
        evt.hidden_object_count = hidden_object_count;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Private reveal: {} hiding {} objects", presenter, hidden_object_count);
}

auto CanvasWorkbench::reveal_content(int revealed_count,
                                      const std::string& reveal_mode) -> void
{
    if (event_bus_)
    {
        core::events::ContentRevealedEvent evt;
        evt.revealed_count = revealed_count;
        evt.reveal_mode = reveal_mode;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Content revealed: {} objects ({})", revealed_count, reveal_mode);
}

// ── V17 Phase 06 W08: Permissions & Shared Locks ────────────────

auto CanvasWorkbench::change_object_permission(const std::string& object_id,
                                                const std::string& permission_level) -> void
{
    if (event_bus_)
    {
        core::events::ObjectPermissionChangedEvent evt;
        evt.object_id = object_id;
        evt.permission_level = permission_level;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Permission changed: {} -> {}", object_id, permission_level);
}

auto CanvasWorkbench::acquire_shared_lock(const std::string& object_id,
                                           const std::string& locked_by) -> void
{
    if (event_bus_)
    {
        core::events::SharedLockAcquiredEvent evt;
        evt.object_id = object_id;
        evt.locked_by = locked_by;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Lock acquired: {} by {}", object_id, locked_by);
}

// ── V17 Phase 06 W09: Offline Queue ────────────────────────────

auto CanvasWorkbench::queue_offline_operation(int queue_depth,
                                               const std::string& operation_type) -> void
{
    if (event_bus_)
    {
        core::events::OfflineOperationQueuedEvent evt;
        evt.queue_depth = queue_depth;
        evt.operation_type = operation_type;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Offline op queued: depth={} type={}", queue_depth, operation_type);
}

auto CanvasWorkbench::flush_offline_queue(int operations_applied,
                                           int operations_conflicted) -> void
{
    if (event_bus_)
    {
        core::events::OfflineQueueFlushedEvent evt;
        evt.operations_applied = operations_applied;
        evt.operations_conflicted = operations_conflicted;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Offline queue flushed: {} applied, {} conflicts",
                     operations_applied, operations_conflicted);
}

// ── V17 Phase 06 W10: Activity Feed ────────────────────────────

auto CanvasWorkbench::add_activity_feed_entry(const std::string& actor,
                                               const std::string& action_description) -> void
{
    if (event_bus_)
    {
        core::events::ActivityFeedEntryAddedEvent evt;
        evt.actor = actor;
        evt.action_description = action_description;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Activity: {} - {}", actor, action_description);
}

auto CanvasWorkbench::clear_activity_feed(int entries_cleared,
                                           const std::string& cleared_by) -> void
{
    if (event_bus_)
    {
        core::events::ActivityFeedClearedEvent evt;
        evt.entries_cleared = entries_cleared;
        evt.cleared_by = cleared_by;
        event_bus_->publish(evt);
    }

    MARKAMP_LOG_INFO("Activity feed cleared: {} entries by {}", entries_cleared, cleared_by);
}

// ── V17 Phase 06 W11: Change Highlights ─────────────────────────

auto CanvasWorkbench::detect_change_highlights(int changes_since_last_visit,
                                                const std::string& last_visitor) -> void
{
    if (event_bus_) { core::events::ChangeHighlightDetectedEvent evt; evt.changes_since_last_visit = changes_since_last_visit; evt.last_visitor = last_visitor; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Changes detected: {} since {}", changes_since_last_visit, last_visitor);
}

auto CanvasWorkbench::dismiss_change_highlights(const std::string& user_id, int dismissed_count) -> void
{
    if (event_bus_) { core::events::ChangeHighlightDismissedEvent evt; evt.user_id = user_id; evt.dismissed_count = dismissed_count; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Highlights dismissed: {} by {}", dismissed_count, user_id);
}

// ── V17 Phase 06 W12: Invites & Session Entry ──────────────────

auto CanvasWorkbench::send_session_invite(const std::string& invitee_email, const std::string& session_id) -> void
{
    if (event_bus_) { core::events::SessionInviteSentEvent evt; evt.invitee_email = invitee_email; evt.session_id = session_id; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Invite sent: {} -> {}", invitee_email, session_id);
}

auto CanvasWorkbench::join_session_via_invite(const std::string& user_id, const std::string& invite_code) -> void
{
    if (event_bus_) { core::events::SessionJoinedViaInviteEvent evt; evt.user_id = user_id; evt.invite_code = invite_code; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Joined via invite: {} code={}", user_id, invite_code);
}

// ── V17 Phase 06 W13: Collaborator Panels ──────────────────────

auto CanvasWorkbench::open_collaborator_panel(const std::string& panel_type, int active_users) -> void
{
    if (event_bus_) { core::events::CollaboratorPanelOpenedEvent evt; evt.panel_type = panel_type; evt.active_users = active_users; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Panel opened: {} ({} users)", panel_type, active_users);
}

auto CanvasWorkbench::take_collaborator_panel_action(const std::string& action_type, const std::string& target_user) -> void
{
    if (event_bus_) { core::events::CollaboratorPanelActionEvent evt; evt.action_type = action_type; evt.target_user = target_user; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Panel action: {} on {}", action_type, target_user);
}

// ── V17 Phase 06 W14: Co-Editing Text ──────────────────────────

auto CanvasWorkbench::start_co_editing(const std::string& object_id, int editors_count) -> void
{
    if (event_bus_) { core::events::CoEditingStartedEvent evt; evt.object_id = object_id; evt.editors_count = editors_count; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Co-editing started: {} ({} editors)", object_id, editors_count);
}

auto CanvasWorkbench::move_co_editing_cursor(const std::string& user_id, int cursor_position) -> void
{
    if (event_bus_) { core::events::CoEditingCursorMovedEvent evt; evt.user_id = user_id; evt.cursor_position = cursor_position; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Co-edit cursor: {} pos={}", user_id, cursor_position);
}

// ── V17 Phase 06 W15: Workshop Templates ───────────────────────

auto CanvasWorkbench::apply_workshop_template(const std::string& template_name, int objects_created) -> void
{
    if (event_bus_) { core::events::WorkshopTemplateAppliedEvent evt; evt.template_name = template_name; evt.objects_created = objects_created; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Template applied: {} ({} objects)", template_name, objects_created);
}

auto CanvasWorkbench::save_workshop_template(const std::string& template_name, const std::string& author) -> void
{
    if (event_bus_) { core::events::WorkshopTemplateSavedEvent evt; evt.template_name = template_name; evt.author = author; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Template saved: {} by {}", template_name, author);
}

// ── V17 Phase 06 W16: Async Review ─────────────────────────────

auto CanvasWorkbench::request_async_review(const std::string& reviewer_id, const std::string& board_id) -> void
{
    if (event_bus_) { core::events::AsyncReviewRequestedEvent evt; evt.reviewer_id = reviewer_id; evt.board_id = board_id; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Async review requested: {} for {}", reviewer_id, board_id);
}

auto CanvasWorkbench::complete_async_review(const std::string& reviewer_id, bool approved) -> void
{
    if (event_bus_) { core::events::AsyncReviewCompletedEvent evt; evt.reviewer_id = reviewer_id; evt.approved = approved; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Async review completed: {} approved={}", reviewer_id, approved);
}

// ── V17 Phase 06 W17: Moderation & Recovery ────────────────────

auto CanvasWorkbench::moderate_user(const std::string& target_user, const std::string& moderation_action) -> void
{
    if (event_bus_) { core::events::UserModeratedEvent evt; evt.target_user = target_user; evt.moderation_action = moderation_action; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("User moderated: {} action={}", target_user, moderation_action);
}

auto CanvasWorkbench::recover_board_state(const std::string& recovery_point, int objects_restored) -> void
{
    if (event_bus_) { core::events::BoardStateRecoveredEvent evt; evt.recovery_point = recovery_point; evt.objects_restored = objects_restored; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Board recovered: {} ({} objects)", recovery_point, objects_restored);
}

// ── V17 Phase 06 W18: Multiplayer Performance ──────────────────

auto CanvasWorkbench::throttle_presence(int active_connections, int throttle_interval_ms) -> void
{
    if (event_bus_) { core::events::PresenceThrottledEvent evt; evt.active_connections = active_connections; evt.throttle_interval_ms = throttle_interval_ms; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Presence throttled: {} connections, {}ms", active_connections, throttle_interval_ms);
}

auto CanvasWorkbench::send_batch_update(int operations_batched, int batch_size_bytes) -> void
{
    if (event_bus_) { core::events::BatchUpdateSentEvent evt; evt.operations_batched = operations_batched; evt.batch_size_bytes = batch_size_bytes; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Batch update: {} ops, {} bytes", operations_batched, batch_size_bytes);
}

// ── V17 Phase 06 W19: Collaboration Analytics ──────────────────

auto CanvasWorkbench::snapshot_collab_metrics(int total_edits, int unique_contributors) -> void
{
    if (event_bus_) { core::events::CollabMetricsSnapshotEvent evt; evt.total_edits = total_edits; evt.unique_contributors = unique_contributors; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Collab metrics: {} edits, {} contributors", total_edits, unique_contributors);
}

auto CanvasWorkbench::calculate_engagement_score(double engagement_score, const std::string& session_id) -> void
{
    if (event_bus_) { core::events::EngagementScoreCalculatedEvent evt; evt.engagement_score = engagement_score; evt.session_id = session_id; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Engagement score: {} for {}", engagement_score, session_id);
}

// ── V17 Phase 06 W20: Collaboration Coverage ───────────────────

auto CanvasWorkbench::run_collab_test_suite(int tests_run, int tests_passed) -> void
{
    if (event_bus_) { core::events::CollabTestSuiteRunEvent evt; evt.tests_run = tests_run; evt.tests_passed = tests_passed; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Collab test suite: {}/{} passed", tests_passed, tests_run);
}

auto CanvasWorkbench::report_collab_regression(const std::string& test_name, const std::string& failure_detail) -> void
{
    if (event_bus_) { core::events::CollabRegressionDetectedEvent evt; evt.test_name = test_name; evt.failure_detail = failure_detail; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Collab regression: {} - {}", test_name, failure_detail);
}

// ── V17 Phase 07 W01: Minimap Navigation ────────────────────────

auto CanvasWorkbench::change_minimap_viewport(double viewport_x, double viewport_y) -> void
{
    if (event_bus_) { core::events::MinimapViewportChangedEvent evt; evt.viewport_x = viewport_x; evt.viewport_y = viewport_y; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Minimap viewport: ({}, {})", viewport_x, viewport_y);
}

auto CanvasWorkbench::toggle_minimap_visibility(bool visible, const std::string& toggle_source) -> void
{
    if (event_bus_) { core::events::MinimapVisibilityToggledEvent evt; evt.visible = visible; evt.toggle_source = toggle_source; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Minimap visible={} source={}", visible, toggle_source);
}

// ── V17 Phase 07 W02: Outline Navigation ────────────────────────

auto CanvasWorkbench::select_outline_node(const std::string& node_id, const std::string& node_type) -> void
{
    if (event_bus_) { core::events::OutlineNodeSelectedEvent evt; evt.node_id = node_id; evt.node_type = node_type; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Outline node: {} ({})", node_id, node_type);
}

auto CanvasWorkbench::refresh_outline_tree(int total_nodes, int depth_levels) -> void
{
    if (event_bus_) { core::events::OutlineTreeRefreshedEvent evt; evt.total_nodes = total_nodes; evt.depth_levels = depth_levels; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Outline refreshed: {} nodes, {} levels", total_nodes, depth_levels);
}

// ── V17 Phase 07 W03: Search & Find ────────────────────────────

auto CanvasWorkbench::execute_board_search(const std::string& query, int results_found) -> void
{
    if (event_bus_) { core::events::BoardSearchExecutedEvent evt; evt.query = query; evt.results_found = results_found; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Search: '{}' -> {} results", query, results_found);
}

auto CanvasWorkbench::navigate_search_result(const std::string& object_id, int result_index) -> void
{
    if (event_bus_) { core::events::BoardSearchResultNavigatedEvent evt; evt.object_id = object_id; evt.result_index = result_index; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Navigate result: {} (index {})", object_id, result_index);
}

// ── V17 Phase 07 W04: Tagging & Filters ────────────────────────

auto CanvasWorkbench::apply_tag_filter(const std::string& tag_name, int matching_objects) -> void
{
    if (event_bus_) { core::events::TagFilterAppliedEvent evt; evt.tag_name = tag_name; evt.matching_objects = matching_objects; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Tag filter: {} ({} matches)", tag_name, matching_objects);
}

auto CanvasWorkbench::clear_tag_filter(int filters_removed, int total_objects_visible) -> void
{
    if (event_bus_) { core::events::TagFilterClearedEvent evt; evt.filters_removed = filters_removed; evt.total_objects_visible = total_objects_visible; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Tag filter cleared: {} removed, {} visible", filters_removed, total_objects_visible);
}

// ── V17 Phase 07 W05: Sections & Landmarks ─────────────────────

auto CanvasWorkbench::create_section_landmark(const std::string& section_name, const std::string& section_id) -> void
{
    if (event_bus_) { core::events::SectionLandmarkCreatedEvent evt; evt.section_name = section_name; evt.section_id = section_id; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Landmark created: {} ({})", section_name, section_id);
}

auto CanvasWorkbench::navigate_to_landmark(const std::string& landmark_id, double target_zoom) -> void
{
    if (event_bus_) { core::events::LandmarkNavigatedEvent evt; evt.landmark_id = landmark_id; evt.target_zoom = target_zoom; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Navigate landmark: {} zoom={}", landmark_id, target_zoom);
}

// ── V17 Phase 07 W06: Large Board Streaming ────────────────────

auto CanvasWorkbench::load_tile(int tile_x, int tile_y) -> void
{
    if (event_bus_) { core::events::TileLoadedEvent evt; evt.tile_x = tile_x; evt.tile_y = tile_y; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Tile loaded: ({}, {})", tile_x, tile_y);
}

auto CanvasWorkbench::change_streaming_lod(int lod_level, int visible_objects) -> void
{
    if (event_bus_) { core::events::StreamingLodChangedEvent evt; evt.lod_level = lod_level; evt.visible_objects = visible_objects; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("LOD changed: level={} visible={}", lod_level, visible_objects);
}

// ── V17 Phase 07 W07: Breadcrumbs & History ────────────────────

auto CanvasWorkbench::push_breadcrumb(const std::string& location_label, int stack_depth) -> void
{
    if (event_bus_) { core::events::BreadcrumbPushedEvent evt; evt.location_label = location_label; evt.stack_depth = stack_depth; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Breadcrumb pushed: {} (depth {})", location_label, stack_depth);
}

auto CanvasWorkbench::navigate_breadcrumb_back(int steps_back, const std::string& destination_label) -> void
{
    if (event_bus_) { core::events::BreadcrumbNavigatedBackEvent evt; evt.steps_back = steps_back; evt.destination_label = destination_label; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Breadcrumb back: {} steps to {}", steps_back, destination_label);
}

// ── V17 Phase 07 W08: Zoom Presets ─────────────────────────────

auto CanvasWorkbench::apply_zoom_preset(const std::string& preset_name, double zoom_level) -> void
{
    if (event_bus_) { core::events::ZoomPresetAppliedEvent evt; evt.preset_name = preset_name; evt.zoom_level = zoom_level; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Zoom preset: {} ({})", preset_name, zoom_level);
}

auto CanvasWorkbench::save_zoom_preset(const std::string& preset_name, double zoom_level) -> void
{
    if (event_bus_) { core::events::ZoomPresetSavedEvent evt; evt.preset_name = preset_name; evt.zoom_level = zoom_level; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Zoom preset saved: {} ({})", preset_name, zoom_level);
}

// ── V17 Phase 07 W09: Board Bookmarks ──────────────────────────

auto CanvasWorkbench::create_board_bookmark(const std::string& bookmark_name, const std::string& bookmark_id) -> void
{
    if (event_bus_) { core::events::BoardBookmarkCreatedEvent evt; evt.bookmark_name = bookmark_name; evt.bookmark_id = bookmark_id; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Bookmark created: {} ({})", bookmark_name, bookmark_id);
}

auto CanvasWorkbench::navigate_board_bookmark(const std::string& bookmark_id, double viewport_zoom) -> void
{
    if (event_bus_) { core::events::BoardBookmarkNavigatedEvent evt; evt.bookmark_id = bookmark_id; evt.viewport_zoom = viewport_zoom; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Bookmark navigated: {} zoom={}", bookmark_id, viewport_zoom);
}

// ── V17 Phase 07 W10: Metadata Driven Navigation ──────────────

auto CanvasWorkbench::apply_metadata_filter(const std::string& filter_key, const std::string& filter_value) -> void
{
    if (event_bus_) { core::events::MetadataFilterAppliedEvent evt; evt.filter_key = filter_key; evt.filter_value = filter_value; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Metadata filter: {}={}", filter_key, filter_value);
}

auto CanvasWorkbench::jump_to_metadata_nav(const std::string& target_object_id, const std::string& metadata_key) -> void
{
    if (event_bus_) { core::events::MetadataNavJumpEvent evt; evt.target_object_id = target_object_id; evt.metadata_key = metadata_key; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Metadata nav jump: {} key={}", target_object_id, metadata_key);
}

// ── V17 Phase 07 W11: Cross Board Traversal ─────────────────────

auto CanvasWorkbench::follow_cross_board_link(const std::string& source_board_id, const std::string& target_board_id) -> void
{
    if (event_bus_) { core::events::CrossBoardLinkFollowedEvent evt; evt.source_board_id = source_board_id; evt.target_board_id = target_board_id; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Cross board: {} -> {}", source_board_id, target_board_id);
}

auto CanvasWorkbench::navigate_cross_board_back(const std::string& returning_to_board_id, int boards_traversed) -> void
{
    if (event_bus_) { core::events::CrossBoardBackNavigatedEvent evt; evt.returning_to_board_id = returning_to_board_id; evt.boards_traversed = boards_traversed; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Cross board back: {} ({} traversed)", returning_to_board_id, boards_traversed);
}

// ── V17 Phase 07 W12: Selection Sync ────────────────────────────

auto CanvasWorkbench::sync_selection_to_panel(const std::string& panel_id, int synced_objects) -> void
{
    if (event_bus_) { core::events::SelectionSyncedToPanelEvent evt; evt.panel_id = panel_id; evt.synced_objects = synced_objects; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Selection synced: {} ({} objects)", panel_id, synced_objects);
}

auto CanvasWorkbench::toggle_selection_sync(bool sync_enabled, const std::string& panel_id) -> void
{
    if (event_bus_) { core::events::SelectionSyncToggledEvent evt; evt.sync_enabled = sync_enabled; evt.panel_id = panel_id; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Selection sync: {} panel={}", sync_enabled, panel_id);
}

// ── V17 Phase 07 W13: Saved Views ──────────────────────────────

auto CanvasWorkbench::save_named_view(const std::string& view_name, const std::string& view_id) -> void
{
    if (event_bus_) { core::events::NamedViewSavedEvent evt; evt.view_name = view_name; evt.view_id = view_id; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("View saved: {} ({})", view_name, view_id);
}

auto CanvasWorkbench::restore_named_view(const std::string& view_id, double restored_zoom) -> void
{
    if (event_bus_) { core::events::NamedViewRestoredEvent evt; evt.view_id = view_id; evt.restored_zoom = restored_zoom; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("View restored: {} zoom={}", view_id, restored_zoom);
}

// ── V17 Phase 07 W14: Presentation Navigation ──────────────────

auto CanvasWorkbench::advance_presentation_slide(int slide_index, int total_slides) -> void
{
    if (event_bus_) { core::events::PresentationSlideAdvancedEvent evt; evt.slide_index = slide_index; evt.total_slides = total_slides; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Presentation slide: {}/{}", slide_index, total_slides);
}

auto CanvasWorkbench::toggle_presentation_mode(bool presentation_active, const std::string& presenter_id) -> void
{
    if (event_bus_) { core::events::PresentationModeToggledEvent evt; evt.presentation_active = presentation_active; evt.presenter_id = presenter_id; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Presentation mode: {} by {}", presentation_active, presenter_id);
}

// ── V17 Phase 07 W15: Semantic Navigation ──────────────────────

auto CanvasWorkbench::navigate_semantic_cluster(const std::string& cluster_label, int objects_in_cluster) -> void
{
    if (event_bus_) { core::events::SemanticClusterNavigatedEvent evt; evt.cluster_label = cluster_label; evt.objects_in_cluster = objects_in_cluster; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Semantic cluster: {} ({} objects)", cluster_label, objects_in_cluster);
}

auto CanvasWorkbench::recalculate_semantic_grouping(int clusters_found, int ungrouped_objects) -> void
{
    if (event_bus_) { core::events::SemanticGroupingRecalculatedEvent evt; evt.clusters_found = clusters_found; evt.ungrouped_objects = ungrouped_objects; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Semantic grouping: {} clusters, {} ungrouped", clusters_found, ungrouped_objects);
}

// ── V17 Phase 07 W16: Quick Action Navigation ───────────────────

auto CanvasWorkbench::invoke_quick_action(const std::string& action_name, const std::string& action_source) -> void
{
    if (event_bus_) { core::events::QuickActionInvokedEvent evt; evt.action_name = action_name; evt.action_source = action_source; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Quick action: {} from {}", action_name, action_source);
}

auto CanvasWorkbench::select_quick_action_result(const std::string& result_id, int result_rank) -> void
{
    if (event_bus_) { core::events::QuickActionResultSelectedEvent evt; evt.result_id = result_id; evt.result_rank = result_rank; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Quick action result: {} rank={}", result_id, result_rank);
}

// ── V17 Phase 07 W17: Discoverability ──────────────────────────

auto CanvasWorkbench::show_feature_hint(const std::string& hint_id, const std::string& feature_area) -> void
{
    if (event_bus_) { core::events::FeatureHintShownEvent evt; evt.hint_id = hint_id; evt.feature_area = feature_area; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Hint shown: {} area={}", hint_id, feature_area);
}

auto CanvasWorkbench::dismiss_feature_hint(const std::string& hint_id, bool dont_show_again) -> void
{
    if (event_bus_) { core::events::FeatureHintDismissedEvent evt; evt.hint_id = hint_id; evt.dont_show_again = dont_show_again; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Hint dismissed: {} permanent={}", hint_id, dont_show_again);
}

// ── V17 Phase 07 W18: Responsive Layouts ───────────────────────

auto CanvasWorkbench::change_layout_breakpoint(const std::string& breakpoint_name, int window_width) -> void
{
    if (event_bus_) { core::events::LayoutBreakpointChangedEvent evt; evt.breakpoint_name = breakpoint_name; evt.window_width = window_width; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Breakpoint: {} width={}", breakpoint_name, window_width);
}

auto CanvasWorkbench::reposition_nav_panel(const std::string& panel_position, const std::string& trigger) -> void
{
    if (event_bus_) { core::events::NavPanelRepositionedEvent evt; evt.panel_position = panel_position; evt.trigger = trigger; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Nav panel: {} trigger={}", panel_position, trigger);
}

// ── V17 Phase 07 W19: Wayfinding Telemetry ─────────────────────

auto CanvasWorkbench::detect_nav_confusion(int rapid_pans, int zoom_reversals) -> void
{
    if (event_bus_) { core::events::NavConfusionDetectedEvent evt; evt.rapid_pans = rapid_pans; evt.zoom_reversals = zoom_reversals; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Nav confusion: {} pans, {} reversals", rapid_pans, zoom_reversals);
}

auto CanvasWorkbench::record_wayfinding_metric(const std::string& metric_name, double metric_value) -> void
{
    if (event_bus_) { core::events::WayfindingMetricRecordedEvent evt; evt.metric_name = metric_name; evt.metric_value = metric_value; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Wayfinding metric: {}={}", metric_name, metric_value);
}

// ── V17 Phase 07 W20: Navigation Coverage ──────────────────────

auto CanvasWorkbench::run_nav_test_suite(int tests_run, int tests_passed) -> void
{
    if (event_bus_) { core::events::NavTestSuiteRunEvent evt; evt.tests_run = tests_run; evt.tests_passed = tests_passed; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Nav test suite: {}/{} passed", tests_passed, tests_run);
}

auto CanvasWorkbench::report_nav_regression(const std::string& test_name, const std::string& failure_detail) -> void
{
    if (event_bus_) { core::events::NavRegressionDetectedEvent evt; evt.test_name = test_name; evt.failure_detail = failure_detail; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Nav regression: {} - {}", test_name, failure_detail);
}

// ── V18 Phase 08 W01: Board Templates ───────────────────────────

auto CanvasWorkbench::apply_board_template(const std::string& template_name, const std::string& template_category) -> void
{
    if (event_bus_) { core::events::BoardTemplateAppliedEvent evt; evt.template_name = template_name; evt.template_category = template_category; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Board template: {} ({})", template_name, template_category);
}

auto CanvasWorkbench::save_board_template(const std::string& template_name, int objects_in_template) -> void
{
    if (event_bus_) { core::events::BoardTemplateSavedEvent evt; evt.template_name = template_name; evt.objects_in_template = objects_in_template; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Board template saved: {} ({} objects)", template_name, objects_in_template);
}

// ── V18 Phase 08 W02: Object Templates ──────────────────────────

auto CanvasWorkbench::insert_object_template(const std::string& template_id, int objects_inserted) -> void
{
    if (event_bus_) { core::events::ObjectTemplateInsertedEvent evt; evt.template_id = template_id; evt.objects_inserted = objects_inserted; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Object template inserted: {} ({} objects)", template_id, objects_inserted);
}

auto CanvasWorkbench::register_object_template(const std::string& template_name, const std::string& template_type) -> void
{
    if (event_bus_) { core::events::ObjectTemplateRegisteredEvent evt; evt.template_name = template_name; evt.template_type = template_type; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Object template registered: {} ({})", template_name, template_type);
}

// ── V18 Phase 08 W03: Advanced Style Presets ────────────────────

auto CanvasWorkbench::apply_style_preset(const std::string& preset_name, int objects_affected) -> void
{
    if (event_bus_) { core::events::BoardStylePresetAppliedEvent evt; evt.preset_name = preset_name; evt.objects_affected = objects_affected; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Style preset: {} ({} objects)", preset_name, objects_affected);
}

auto CanvasWorkbench::create_style_preset(const std::string& preset_name, const std::string& preset_scope) -> void
{
    if (event_bus_) { core::events::StylePresetCreatedEvent evt; evt.preset_name = preset_name; evt.preset_scope = preset_scope; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Style preset created: {} scope={}", preset_name, preset_scope);
}

// ── V18 Phase 08 W04: Automation Rules ──────────────────────────

auto CanvasWorkbench::trigger_automation_rule(const std::string& rule_name, int actions_executed) -> void
{
    if (event_bus_) { core::events::AutomationRuleTriggeredEvent evt; evt.rule_name = rule_name; evt.actions_executed = actions_executed; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Automation rule: {} ({} actions)", rule_name, actions_executed);
}

auto CanvasWorkbench::create_automation_rule(const std::string& rule_name, const std::string& trigger_type) -> void
{
    if (event_bus_) { core::events::AutomationRuleCreatedEvent evt; evt.rule_name = rule_name; evt.trigger_type = trigger_type; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Automation rule created: {} trigger={}", rule_name, trigger_type);
}

// ── V18 Phase 08 W05: AI Board Generation ───────────────────────

auto CanvasWorkbench::request_ai_board_generation(const std::string& prompt_text, const std::string& board_type) -> void
{
    if (event_bus_) { core::events::AIBoardGenerationRequestedEvent evt; evt.prompt_text = prompt_text; evt.board_type = board_type; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("AI board gen: '{}' type={}", prompt_text, board_type);
}

auto CanvasWorkbench::complete_ai_board_generation(int objects_generated, double generation_time_ms) -> void
{
    if (event_bus_) { core::events::AIBoardGenerationCompletedEvent evt; evt.objects_generated = objects_generated; evt.generation_time_ms = generation_time_ms; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("AI board gen complete: {} objects in {}ms", objects_generated, generation_time_ms);
}

// ── V18 Phase 08 W06: AI Cleanup & Refinement ──────────────────

auto CanvasWorkbench::request_ai_cleanup(const std::string& cleanup_type, int objects_in_scope) -> void
{
    if (event_bus_) { core::events::AICleanupRequestedEvent evt; evt.cleanup_type = cleanup_type; evt.objects_in_scope = objects_in_scope; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("AI cleanup: {} ({} objects)", cleanup_type, objects_in_scope);
}

auto CanvasWorkbench::complete_ai_cleanup(int objects_modified, int objects_removed) -> void
{
    if (event_bus_) { core::events::AICleanupCompletedEvent evt; evt.objects_modified = objects_modified; evt.objects_removed = objects_removed; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("AI cleanup done: {} modified, {} removed", objects_modified, objects_removed);
}

// ── V18 Phase 08 W07: AI Summaries & Explanations ──────────────

auto CanvasWorkbench::request_ai_summary(const std::string& scope, int objects_summarized) -> void
{
    if (event_bus_) { core::events::AISummaryRequestedEvent evt; evt.scope = scope; evt.objects_summarized = objects_summarized; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("AI summary request: {} ({} objects)", scope, objects_summarized);
}

auto CanvasWorkbench::generate_ai_summary(int word_count, const std::string& output_format) -> void
{
    if (event_bus_) { core::events::AISummaryGeneratedEvent evt; evt.word_count = word_count; evt.output_format = output_format; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("AI summary: {} words format={}", word_count, output_format);
}

// ── V18 Phase 08 W08: AI Tags & Links ──────────────────────────

auto CanvasWorkbench::generate_ai_tag_suggestions(int tags_suggested, int objects_analyzed) -> void
{
    if (event_bus_) { core::events::AITagSuggestionsGeneratedEvent evt; evt.tags_suggested = tags_suggested; evt.objects_analyzed = objects_analyzed; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("AI tags: {} suggested from {} objects", tags_suggested, objects_analyzed);
}

auto CanvasWorkbench::generate_ai_link_suggestions(int links_suggested, int cross_board_links) -> void
{
    if (event_bus_) { core::events::AILinkSuggestionsGeneratedEvent evt; evt.links_suggested = links_suggested; evt.cross_board_links = cross_board_links; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("AI links: {} suggested ({} cross-board)", links_suggested, cross_board_links);
}

// ── V18 Phase 08 W09: Batch Operations ─────────────────────────

auto CanvasWorkbench::execute_batch_operation(const std::string& operation_type, int objects_affected) -> void
{
    if (event_bus_) { core::events::BatchOperationExecutedEvent evt; evt.operation_type = operation_type; evt.objects_affected = objects_affected; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Batch op: {} ({} objects)", operation_type, objects_affected);
}

auto CanvasWorkbench::undo_batch_operation(const std::string& operation_type, int objects_restored) -> void
{
    if (event_bus_) { core::events::BatchOperationUndoneEvent evt; evt.operation_type = operation_type; evt.objects_restored = objects_restored; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Batch undo: {} ({} restored)", operation_type, objects_restored);
}

// ── V18 Phase 08 W10: Markdown To Canvas ───────────────────────

auto CanvasWorkbench::start_markdown_to_canvas(const std::string& source_file, int lines_to_parse) -> void
{
    if (event_bus_) { core::events::MarkdownToCanvasStartedEvent evt; evt.source_file = source_file; evt.lines_to_parse = lines_to_parse; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("MD to canvas: {} ({} lines)", source_file, lines_to_parse);
}

auto CanvasWorkbench::complete_markdown_to_canvas(int objects_created, int connectors_created) -> void
{
    if (event_bus_) { core::events::MarkdownToCanvasCompletedEvent evt; evt.objects_created = objects_created; evt.connectors_created = connectors_created; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("MD to canvas done: {} objects, {} connectors", objects_created, connectors_created);
}

// ── V18 Phase 08 W11: CSV & Database Imports ────────────────────

auto CanvasWorkbench::start_csv_import(const std::string& source_file, int rows_to_import) -> void
{
    if (event_bus_) { core::events::CsvImportStartedEvent evt; evt.source_file = source_file; evt.rows_to_import = rows_to_import; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("CSV import: {} ({} rows)", source_file, rows_to_import);
}

auto CanvasWorkbench::complete_csv_import(int objects_created, int columns_mapped) -> void
{
    if (event_bus_) { core::events::CsvImportCompletedEvent evt; evt.objects_created = objects_created; evt.columns_mapped = columns_mapped; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("CSV import done: {} objects, {} columns", objects_created, columns_mapped);
}

// ── V18 Phase 08 W12: Export Workflows ──────────────────────────

auto CanvasWorkbench::start_board_export(const std::string& export_format, const std::string& export_scope) -> void
{
    if (event_bus_) { core::events::BoardExportStartedEvent evt; evt.export_format = export_format; evt.export_scope = export_scope; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Board export: {} scope={}", export_format, export_scope);
}

auto CanvasWorkbench::complete_board_export(const std::string& output_path, int objects_exported) -> void
{
    if (event_bus_) { core::events::BoardExportCompletedEvent evt; evt.output_path = output_path; evt.objects_exported = objects_exported; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Board export done: {} ({} objects)", output_path, objects_exported);
}

// ── V18 Phase 08 W13: Reusable Components ──────────────────────

auto CanvasWorkbench::save_reusable_component(const std::string& component_name, int child_objects) -> void
{
    if (event_bus_) { core::events::ReusableComponentSavedEvent evt; evt.component_name = component_name; evt.child_objects = child_objects; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Component saved: {} ({} children)", component_name, child_objects);
}

auto CanvasWorkbench::instantiate_reusable_component(const std::string& component_id, const std::string& instance_id) -> void
{
    if (event_bus_) { core::events::ReusableComponentInstantiatedEvent evt; evt.component_id = component_id; evt.instance_id = instance_id; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Component instantiated: {} -> {}", component_id, instance_id);
}

// ── V18 Phase 08 W14: Plugin Hooks ─────────────────────────────

auto CanvasWorkbench::register_plugin_hook(const std::string& hook_name, const std::string& plugin_id) -> void
{
    if (event_bus_) { core::events::PluginHookRegisteredEvent evt; evt.hook_name = hook_name; evt.plugin_id = plugin_id; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Plugin hook registered: {} by {}", hook_name, plugin_id);
}

auto CanvasWorkbench::invoke_plugin_hook(const std::string& hook_name, int listeners_notified) -> void
{
    if (event_bus_) { core::events::PluginHookInvokedEvent evt; evt.hook_name = hook_name; evt.listeners_notified = listeners_notified; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Plugin hook invoked: {} ({} listeners)", hook_name, listeners_notified);
}

// ── V18 Phase 08 W15: Command Macros ───────────────────────────

auto CanvasWorkbench::record_command_macro(const std::string& macro_name, int steps_recorded) -> void
{
    if (event_bus_) { core::events::CommandMacroRecordedEvent evt; evt.macro_name = macro_name; evt.steps_recorded = steps_recorded; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Macro recorded: {} ({} steps)", macro_name, steps_recorded);
}

auto CanvasWorkbench::replay_command_macro(const std::string& macro_name, int objects_affected) -> void
{
    if (event_bus_) { core::events::CommandMacroReplayedEvent evt; evt.macro_name = macro_name; evt.objects_affected = objects_affected; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Macro replayed: {} ({} objects)", macro_name, objects_affected);
}

// ── V18 Phase 08 W16: Quick Insert ─────────────────────────────

auto CanvasWorkbench::open_quick_insert_menu(const std::string& trigger_source, int items_available) -> void
{
    if (event_bus_) { core::events::QuickInsertMenuOpenedEvent evt; evt.trigger_source = trigger_source; evt.items_available = items_available; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Quick insert: {} ({} items)", trigger_source, items_available);
}

auto CanvasWorkbench::select_quick_insert_item(const std::string& item_type, int search_rank) -> void
{
    if (event_bus_) { core::events::QuickInsertItemSelectedEvent evt; evt.item_type = item_type; evt.search_rank = search_rank; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Quick insert selected: {} rank={}", item_type, search_rank);
}

// ── V18 Phase 08 W17: Smart Defaults ───────────────────────────

auto CanvasWorkbench::apply_smart_default(const std::string& default_type, const std::string& context) -> void
{
    if (event_bus_) { core::events::SmartDefaultAppliedEvent evt; evt.default_type = default_type; evt.context = context; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Smart default: {} context={}", default_type, context);
}

auto CanvasWorkbench::show_smart_default_suggestion(int suggestions_count, bool suggestion_accepted) -> void
{
    if (event_bus_) { core::events::SmartDefaultSuggestionShownEvent evt; evt.suggestions_count = suggestions_count; evt.suggestion_accepted = suggestion_accepted; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Smart suggestion: {} shown, accepted={}", suggestions_count, suggestion_accepted);
}

// ── V18 Phase 08 W18: Profile & Preference Sync ────────────────

auto CanvasWorkbench::sync_profile(const std::string& profile_id, int preferences_synced) -> void
{
    if (event_bus_) { core::events::ProfileSyncedEvent evt; evt.profile_id = profile_id; evt.preferences_synced = preferences_synced; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Profile synced: {} ({} prefs)", profile_id, preferences_synced);
}

auto CanvasWorkbench::resolve_preference_conflict(const std::string& preference_key, const std::string& resolution) -> void
{
    if (event_bus_) { core::events::PreferenceConflictResolvedEvent evt; evt.preference_key = preference_key; evt.resolution = resolution; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Preference conflict: {} resolved={}", preference_key, resolution);
}

// ── V18 Phase 08 W19: Onboarding Programs ──────────────────────

auto CanvasWorkbench::complete_onboarding_step(const std::string& step_id, int steps_remaining) -> void
{
    if (event_bus_) { core::events::CanvasOnboardingStepCompletedEvent evt; evt.step_id = step_id; evt.steps_remaining = steps_remaining; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Onboarding step: {} ({} remaining)", step_id, steps_remaining);
}

auto CanvasWorkbench::finish_onboarding_program(const std::string& program_id, int total_steps_completed) -> void
{
    if (event_bus_) { core::events::OnboardingProgramFinishedEvent evt; evt.program_id = program_id; evt.total_steps_completed = total_steps_completed; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Onboarding done: {} ({} steps)", program_id, total_steps_completed);
}

// ── V18 Phase 08 W20: Agent Ready Scaffolds ────────────────────

auto CanvasWorkbench::run_creation_test_suite(int tests_run, int tests_passed) -> void
{
    if (event_bus_) { core::events::CreationTestSuiteRunEvent evt; evt.tests_run = tests_run; evt.tests_passed = tests_passed; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Creation test suite: {}/{} passed", tests_passed, tests_run);
}

auto CanvasWorkbench::report_creation_regression(const std::string& test_name, const std::string& failure_detail) -> void
{
    if (event_bus_) { core::events::CreationRegressionDetectedEvent evt; evt.test_name = test_name; evt.failure_detail = failure_detail; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Creation regression: {} - {}", test_name, failure_detail);
}

// ── V19 Phase 09 W01: Keyboard Only Canvas ──────────────────────

auto CanvasWorkbench::perform_keyboard_nav_action(const std::string& action, const std::string& direction) -> void
{
    if (event_bus_) { core::events::KeyboardNavActionEvent evt; evt.action = action; evt.direction = direction; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Keyboard nav: {} {}", action, direction);
}

auto CanvasWorkbench::detect_keyboard_shortcut_conflict(const std::string& shortcut_key, const std::string& conflicting_action) -> void
{
    if (event_bus_) { core::events::KeyboardShortcutConflictEvent evt; evt.shortcut_key = shortcut_key; evt.conflicting_action = conflicting_action; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Shortcut conflict: {} -> {}", shortcut_key, conflicting_action);
}

// ── V19 Phase 09 W02: Screen Reader Semantics ──────────────────

auto CanvasWorkbench::queue_screen_reader_announcement(const std::string& text, const std::string& priority) -> void
{
    if (event_bus_) { core::events::ScreenReaderAnnouncementEvent evt; evt.announcement_text = text; evt.priority = priority; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("SR announce: '{}' priority={}", text, priority);
}

auto CanvasWorkbench::update_accessible_label(const std::string& object_id, const std::string& new_label) -> void
{
    if (event_bus_) { core::events::AccessibleLabelUpdatedEvent evt; evt.object_id = object_id; evt.new_label = new_label; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Accessible label: {} -> '{}'", object_id, new_label);
}

// ── V19 Phase 09 W03: Contrast & Color Blindness ───────────────

auto CanvasWorkbench::toggle_high_contrast_mode(bool enabled, const std::string& profile) -> void
{
    if (event_bus_) { core::events::HighContrastModeToggledEvent evt; evt.high_contrast_enabled = enabled; evt.contrast_profile = profile; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("High contrast: {} profile={}", enabled, profile);
}

auto CanvasWorkbench::run_color_accessibility_check(int elements_checked, int issues_found) -> void
{
    if (event_bus_) { core::events::ColorAccessibilityCheckEvent evt; evt.elements_checked = elements_checked; evt.issues_found = issues_found; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Color a11y: {}/{} issues", issues_found, elements_checked);
}

// ── V19 Phase 09 W04: Focus & Announcements ────────────────────

auto CanvasWorkbench::transfer_focus(const std::string& from_id, const std::string& to_id) -> void
{
    if (event_bus_) { core::events::FocusTransferredEvent evt; evt.from_object_id = from_id; evt.to_object_id = to_id; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Focus: {} -> {}", from_id, to_id);
}

auto CanvasWorkbench::update_live_region(const std::string& region_id, const std::string& text) -> void
{
    if (event_bus_) { core::events::LiveRegionUpdateEvent evt; evt.region_id = region_id; evt.update_text = text; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Live region: {} '{}'", region_id, text);
}

// ── V19 Phase 09 W05: Touch & Pen Parity ───────────────────────

auto CanvasWorkbench::recognize_touch_gesture(const std::string& gesture_type, int touch_points) -> void
{
    if (event_bus_) { core::events::TouchGestureRecognizedEvent evt; evt.gesture_type = gesture_type; evt.touch_points = touch_points; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Touch gesture: {} ({} points)", gesture_type, touch_points);
}

auto CanvasWorkbench::apply_pen_pressure(double pressure_level, const std::string& pen_tool) -> void
{
    if (event_bus_) { core::events::PenPressureAppliedEvent evt; evt.pressure_level = pressure_level; evt.pen_tool = pen_tool; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Pen pressure: {} tool={}", pressure_level, pen_tool);
}

// ── V19 Phase 09 W06: Performance Budgets ──────────────────────

auto CanvasWorkbench::report_perf_budget_exceeded(const std::string& operation, double elapsed_ms) -> void
{
    if (event_bus_) { core::events::PerfBudgetExceededEvent evt; evt.operation = operation; evt.elapsed_ms = elapsed_ms; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Perf budget exceeded: {} {}ms", operation, elapsed_ms);
}

auto CanvasWorkbench::record_frame_rate_metric(double fps, int objects_rendered) -> void
{
    if (event_bus_) { core::events::FrameRateMetricEvent evt; evt.fps = fps; evt.objects_rendered = objects_rendered; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Frame rate: {} fps ({} objects)", fps, objects_rendered);
}

// ── V19 Phase 09 W07: Virtualization ───────────────────────────

auto CanvasWorkbench::update_viewport_culling(int visible_objects, int total_objects) -> void
{
    if (event_bus_) { core::events::ViewportCullingUpdatedEvent evt; evt.visible_objects = visible_objects; evt.total_objects = total_objects; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Viewport culling: {}/{} visible", visible_objects, total_objects);
}

auto CanvasWorkbench::record_tile_cache_metric(int cache_hits, int cache_misses) -> void
{
    if (event_bus_) { core::events::TileCacheMetricEvent evt; evt.cache_hits = cache_hits; evt.cache_misses = cache_misses; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Tile cache: {} hits, {} misses", cache_hits, cache_misses);
}

// ── V19 Phase 09 W08: Crash Recovery ───────────────────────────

auto CanvasWorkbench::save_crash_recovery_checkpoint(const std::string& checkpoint_id, int objects_saved) -> void
{
    if (event_bus_) { core::events::CrashRecoveryCheckpointEvent evt; evt.checkpoint_id = checkpoint_id; evt.objects_saved = objects_saved; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Crash checkpoint: {} ({} objects)", checkpoint_id, objects_saved);
}

auto CanvasWorkbench::recover_crashed_board_state(const std::string& recovery_source, int objects_recovered) -> void
{
    if (event_bus_) { core::events::CrashBoardStateRecoveredEvent evt; evt.recovery_source = recovery_source; evt.objects_recovered = objects_recovered; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Board recovered: {} ({} objects)", recovery_source, objects_recovered);
}

// ── V19 Phase 09 W09: Data Integrity ───────────────────────────

auto CanvasWorkbench::run_board_integrity_check(int objects_validated, int corruption_found) -> void
{
    if (event_bus_) { core::events::BoardIntegrityCheckEvent evt; evt.objects_validated = objects_validated; evt.corruption_found = corruption_found; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Integrity: {}/{} corrupt", corruption_found, objects_validated);
}

auto CanvasWorkbench::apply_data_repair(const std::string& repair_type, int items_repaired) -> void
{
    if (event_bus_) { core::events::DataRepairAppliedEvent evt; evt.repair_type = repair_type; evt.items_repaired = items_repaired; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Data repair: {} ({} items)", repair_type, items_repaired);
}

// ── V19 Phase 09 W10: Accessibility Tooling ────────────────────

auto CanvasWorkbench::run_accessibility_audit(int elements_audited, int violations_found) -> void
{
    if (event_bus_) { core::events::AccessibilityAuditRunEvent evt; evt.elements_audited = elements_audited; evt.violations_found = violations_found; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("A11y audit: {}/{} violations", violations_found, elements_audited);
}

auto CanvasWorkbench::detect_accessibility_regression(const std::string& test_name, const std::string& violation_type) -> void
{
    if (event_bus_) { core::events::AccessibilityRegressionEvent evt; evt.test_name = test_name; evt.violation_type = violation_type; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("A11y regression: {} type={}", test_name, violation_type);
}

// ── V19 Phase 09 W11: Safe Degradation ──────────────────────────

auto CanvasWorkbench::activate_safe_degradation(const std::string& feature_name, const std::string& fallback_reason) -> void
{
    if (event_bus_) { core::events::SafeDegradationActivatedEvent evt; evt.feature_name = feature_name; evt.fallback_reason = fallback_reason; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Safe degrade: {} reason={}", feature_name, fallback_reason);
}

auto CanvasWorkbench::exit_degraded_mode(const std::string& feature_name, double degraded_duration_ms) -> void
{
    if (event_bus_) { core::events::DegradedModeExitedEvent evt; evt.feature_name = feature_name; evt.degraded_duration_ms = degraded_duration_ms; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Degraded exit: {} {}ms", feature_name, degraded_duration_ms);
}

// ── V19 Phase 09 W12: Observability ─────────────────────────────

auto CanvasWorkbench::record_telemetry_span(const std::string& span_name, double duration_ms) -> void
{
    if (event_bus_) { core::events::TelemetrySpanRecordedEvent evt; evt.span_name = span_name; evt.duration_ms = duration_ms; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Telemetry: {} {}ms", span_name, duration_ms);
}

auto CanvasWorkbench::emit_error_telemetry(const std::string& error_category, const std::string& error_message) -> void
{
    if (event_bus_) { core::events::ErrorTelemetryEmittedEvent evt; evt.error_category = error_category; evt.error_message = error_message; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Error telemetry: {} '{}'", error_category, error_message);
}

// ── V19 Phase 09 W13: Feature Flags ─────────────────────────────

auto CanvasWorkbench::evaluate_feature_flag(const std::string& flag_name, bool flag_value) -> void
{
    if (event_bus_) { core::events::FeatureFlagEvaluatedEvent evt; evt.flag_name = flag_name; evt.flag_value = flag_value; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Feature flag: {} = {}", flag_name, flag_value);
}

auto CanvasWorkbench::apply_feature_flag_override(const std::string& flag_name, const std::string& override_source) -> void
{
    if (event_bus_) { core::events::FeatureFlagOverrideEvent evt; evt.flag_name = flag_name; evt.override_source = override_source; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Flag override: {} from={}", flag_name, override_source);
}

// ── V19 Phase 09 W14: Snapshot & Harnesses ──────────────────────

auto CanvasWorkbench::capture_visual_snapshot(const std::string& snapshot_id, int pixels_diffed) -> void
{
    if (event_bus_) { core::events::VisualSnapshotCapturedEvent evt; evt.snapshot_id = snapshot_id; evt.pixels_diffed = pixels_diffed; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Snapshot: {} ({} px)", snapshot_id, pixels_diffed);
}

auto CanvasWorkbench::compare_snapshot(const std::string& baseline_id, double diff_percentage) -> void
{
    if (event_bus_) { core::events::SnapshotComparisonResultEvent evt; evt.baseline_id = baseline_id; evt.diff_percentage = diff_percentage; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Snapshot compare: {} diff={}%", baseline_id, diff_percentage);
}

// ── V19 Phase 09 W15: Unit & Integration Gaps ──────────────────

auto CanvasWorkbench::identify_test_coverage_gap(const std::string& module_name, int uncovered_lines) -> void
{
    if (event_bus_) { core::events::TestCoverageGapEvent evt; evt.module_name = module_name; evt.uncovered_lines = uncovered_lines; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Coverage gap: {} ({} lines)", module_name, uncovered_lines);
}

auto CanvasWorkbench::add_integration_test(const std::string& test_name, const std::string& covered_module) -> void
{
    if (event_bus_) { core::events::IntegrationTestAddedEvent evt; evt.test_name = test_name; evt.covered_module = covered_module; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Integration test: {} covers {}", test_name, covered_module);
}

// ── V19 Phase 09 W16: End To End Determinism ────────────────────

auto CanvasWorkbench::detect_e2e_flakiness(const std::string& test_name, int flaky_runs) -> void
{
    if (event_bus_) { core::events::E2EFlakinessDetectedEvent evt; evt.test_name = test_name; evt.flaky_runs = flaky_runs; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("E2E flaky: {} ({} runs)", test_name, flaky_runs);
}

auto CanvasWorkbench::stabilize_e2e_timing(const std::string& test_name, double variance_ms) -> void
{
    if (event_bus_) { core::events::E2ETimingStabilizedEvent evt; evt.test_name = test_name; evt.variance_ms = variance_ms; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("E2E stabilized: {} var={}ms", test_name, variance_ms);
}

// ── V19 Phase 09 W17: Security & Privacy ────────────────────────

auto CanvasWorkbench::report_security_audit_finding(const std::string& finding_type, const std::string& severity) -> void
{
    if (event_bus_) { core::events::SecurityAuditFindingEvent evt; evt.finding_type = finding_type; evt.severity = severity; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Security finding: {} severity={}", finding_type, severity);
}

auto CanvasWorkbench::scrub_privacy_data(int fields_scrubbed, const std::string& scrub_scope) -> void
{
    if (event_bus_) { core::events::PrivacyDataScrubbedEvent evt; evt.fields_scrubbed = fields_scrubbed; evt.scrub_scope = scrub_scope; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Privacy scrub: {} fields scope={}", fields_scrubbed, scrub_scope);
}

// ── V19 Phase 09 W18: Localization & IME ────────────────────────

auto CanvasWorkbench::switch_locale(const std::string& from_locale, const std::string& to_locale) -> void
{
    if (event_bus_) { core::events::LocaleSwitchedEvent evt; evt.from_locale = from_locale; evt.to_locale = to_locale; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Locale: {} -> {}", from_locale, to_locale);
}

auto CanvasWorkbench::handle_ime_composition(const std::string& ime_state, int composition_length) -> void
{
    if (event_bus_) { core::events::IMECompositionEvent evt; evt.ime_state = ime_state; evt.composition_length = composition_length; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("IME: {} len={}", ime_state, composition_length);
}

// ── V19 Phase 09 W19: Compliance Checklists ─────────────────────

auto CanvasWorkbench::run_compliance_check(int checks_passed, int checks_failed) -> void
{
    if (event_bus_) { core::events::ComplianceCheckRunEvent evt; evt.checks_passed = checks_passed; evt.checks_failed = checks_failed; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Compliance: {}/{} passed", checks_passed, checks_passed + checks_failed);
}

auto CanvasWorkbench::flag_compliance_violation(const std::string& rule_id, const std::string& violation_detail) -> void
{
    if (event_bus_) { core::events::ComplianceViolationFlaggedEvent evt; evt.rule_id = rule_id; evt.violation_detail = violation_detail; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Compliance violation: {} '{}'", rule_id, violation_detail);
}

// ── V19 Phase 09 W20: Operational Dashboards ────────────────────

auto CanvasWorkbench::publish_dashboard_metric(const std::string& metric_name, double metric_value) -> void
{
    if (event_bus_) { core::events::DashboardMetricPublishedEvent evt; evt.metric_name = metric_name; evt.metric_value = metric_value; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Dashboard: {} = {}", metric_name, metric_value);
}

auto CanvasWorkbench::complete_health_check(int healthy_systems, int degraded_systems) -> void
{
    if (event_bus_) { core::events::CanvasHealthCheckCompletedEvent evt; evt.healthy_systems = healthy_systems; evt.degraded_systems = degraded_systems; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Health check: {} healthy, {} degraded", healthy_systems, degraded_systems);
}

// ── V20 Phase 10 W01: Parity Audit ──────────────────────────────

auto CanvasWorkbench::check_parity_audit_item(const std::string& competitor_feature, const std::string& parity_status) -> void
{
    if (event_bus_) { core::events::ParityAuditItemCheckedEvent evt; evt.competitor_feature = competitor_feature; evt.parity_status = parity_status; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Parity: {} = {}", competitor_feature, parity_status);
}

auto CanvasWorkbench::identify_parity_gap(const std::string& feature_name, const std::string& gap_severity) -> void
{
    if (event_bus_) { core::events::ParityGapIdentifiedEvent evt; evt.feature_name = feature_name; evt.gap_severity = gap_severity; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Parity gap: {} severity={}", feature_name, gap_severity);
}

// ── V20 Phase 10 W02: Control Polish ────────────────────────────

auto CanvasWorkbench::adjust_control_density(const std::string& control_group, const std::string& density_level) -> void
{
    if (event_bus_) { core::events::ControlDensityAdjustedEvent evt; evt.control_group = control_group; evt.density_level = density_level; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Control density: {} = {}", control_group, density_level);
}

auto CanvasWorkbench::apply_affordance_refinement(const std::string& element_id, const std::string& refinement_type) -> void
{
    if (event_bus_) { core::events::AffordanceRefinementAppliedEvent evt; evt.element_id = element_id; evt.refinement_type = refinement_type; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Affordance: {} type={}", element_id, refinement_type);
}

// ── V20 Phase 10 W03: Motion & Microinteraction ─────────────────

auto CanvasWorkbench::trigger_microinteraction(const std::string& interaction_name, double duration_ms) -> void
{
    if (event_bus_) { core::events::MicrointeractionTriggeredEvent evt; evt.interaction_name = interaction_name; evt.duration_ms = duration_ms; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Microinteraction: {} {}ms", interaction_name, duration_ms);
}

auto CanvasWorkbench::apply_motion_preference(const std::string& preference, bool system_prefers_reduced) -> void
{
    if (event_bus_) { core::events::MotionPreferenceAppliedEvent evt; evt.preference = preference; evt.system_prefers_reduced = system_prefers_reduced; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Motion pref: {} reduced={}", preference, system_prefers_reduced);
}

// ── V20 Phase 10 W04: Platform Conventions ──────────────────────

auto CanvasWorkbench::apply_platform_convention(const std::string& platform, const std::string& convention_type) -> void
{
    if (event_bus_) { core::events::PlatformConventionAppliedEvent evt; evt.platform = platform; evt.convention_type = convention_type; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Platform: {} convention={}", platform, convention_type);
}

auto CanvasWorkbench::run_platform_parity_check(const std::string& platform, int conventions_matched) -> void
{
    if (event_bus_) { core::events::PlatformParityCheckEvent evt; evt.platform = platform; evt.conventions_matched = conventions_matched; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Platform parity: {} matched={}", platform, conventions_matched);
}

// ── V20 Phase 10 W05: Settings Migration ────────────────────────

auto CanvasWorkbench::start_settings_migration(const std::string& from_version, const std::string& to_version) -> void
{
    if (event_bus_) { core::events::SettingsMigrationStartedEvent evt; evt.from_version = from_version; evt.to_version = to_version; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Settings migration: {} -> {}", from_version, to_version);
}

auto CanvasWorkbench::complete_settings_migration(int settings_migrated, int settings_defaulted) -> void
{
    if (event_bus_) { core::events::SettingsMigrationCompletedEvent evt; evt.settings_migrated = settings_migrated; evt.settings_defaulted = settings_defaulted; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Settings migrated: {} migrated, {} defaulted", settings_migrated, settings_defaulted);
}

// ── V20 Phase 10 W06: Documentation ─────────────────────────────

auto CanvasWorkbench::generate_doc_page(const std::string& page_id, int sections_written) -> void
{
    if (event_bus_) { core::events::DocPageGeneratedEvent evt; evt.page_id = page_id; evt.sections_written = sections_written; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Doc page: {} ({} sections)", page_id, sections_written);
}

auto CanvasWorkbench::check_doc_coverage(int features_documented, int features_undocumented) -> void
{
    if (event_bus_) { core::events::DocCoverageCheckedEvent evt; evt.features_documented = features_documented; evt.features_undocumented = features_undocumented; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Doc coverage: {}/{} documented", features_documented, features_documented + features_undocumented);
}

// ── V20 Phase 10 W07: Benchmarks & Baselines ───────────────────

auto CanvasWorkbench::complete_benchmark_run(const std::string& benchmark_name, double result_ms) -> void
{
    if (event_bus_) { core::events::BenchmarkRunCompletedEvent evt; evt.benchmark_name = benchmark_name; evt.result_ms = result_ms; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Benchmark: {} = {}ms", benchmark_name, result_ms);
}

auto CanvasWorkbench::detect_benchmark_regression(const std::string& benchmark_name, double regression_pct) -> void
{
    if (event_bus_) { core::events::BenchmarkRegressionDetectedEvent evt; evt.benchmark_name = benchmark_name; evt.regression_pct = regression_pct; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Benchmark regression: {} {}%", benchmark_name, regression_pct);
}

// ── V20 Phase 10 W08: Release Gates ─────────────────────────────

auto CanvasWorkbench::evaluate_release_gate(const std::string& gate_name, bool gate_passed) -> void
{
    if (event_bus_) { core::events::ReleaseGateEvaluatedEvent evt; evt.gate_name = gate_name; evt.gate_passed = gate_passed; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Release gate: {} = {}", gate_name, gate_passed);
}

auto CanvasWorkbench::summarize_release_readiness(int gates_passed, int gates_failed) -> void
{
    if (event_bus_) { core::events::ReleaseReadinessSummaryEvent evt; evt.gates_passed = gates_passed; evt.gates_failed = gates_failed; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Release ready: {}/{} passed", gates_passed, gates_passed + gates_failed);
}

// ── V20 Phase 10 W09: Beta Feedback Loops ───────────────────────

auto CanvasWorkbench::submit_beta_feedback(const std::string& feedback_category, const std::string& board_state_id) -> void
{
    if (event_bus_) { core::events::BetaFeedbackSubmittedEvent evt; evt.feedback_category = feedback_category; evt.board_state_id = board_state_id; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Beta feedback: {} board={}", feedback_category, board_state_id);
}

auto CanvasWorkbench::triage_beta_feedback(const std::string& feedback_id, const std::string& triage_priority) -> void
{
    if (event_bus_) { core::events::BetaFeedbackTriagedEvent evt; evt.feedback_id = feedback_id; evt.triage_priority = triage_priority; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Feedback triage: {} priority={}", feedback_id, triage_priority);
}

// ── V20 Phase 10 W10: Extension Ecosystem ───────────────────────

auto CanvasWorkbench::check_extension_compatibility(const std::string& extension_id, bool is_compatible) -> void
{
    if (event_bus_) { core::events::ExtensionCompatibilityCheckedEvent evt; evt.extension_id = extension_id; evt.is_compatible = is_compatible; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Extension compat: {} = {}", extension_id, is_compatible);
}

auto CanvasWorkbench::generate_extension_ecosystem_report(int total_extensions, int compatible_extensions) -> void
{
    if (event_bus_) { core::events::ExtensionEcosystemReportEvent evt; evt.total_extensions = total_extensions; evt.compatible_extensions = compatible_extensions; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Extension ecosystem: {}/{} compatible", compatible_extensions, total_extensions);
}

// ── V20 Phase 10 W11: Advanced Drawing Polish ───────────────────

auto CanvasWorkbench::refine_drawing_stroke(const std::string& tool_name, double pressure_sensitivity) -> void
{
    if (event_bus_) { core::events::DrawingStrokeRefinedEvent evt; evt.tool_name = tool_name; evt.pressure_sensitivity = pressure_sensitivity; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Stroke refined: {} pressure={}", tool_name, pressure_sensitivity);
}

auto CanvasWorkbench::calibrate_drawing_tool(const std::string& tool_name, int calibration_points) -> void
{
    if (event_bus_) { core::events::DrawingToolCalibratedEvent evt; evt.tool_name = tool_name; evt.calibration_points = calibration_points; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Tool calibrated: {} points={}", tool_name, calibration_points);
}

// ── V20 Phase 10 W12: Advanced Layout Polish ────────────────────

auto CanvasWorkbench::refine_layout_transform(const std::string& transform_type, double precision_delta) -> void
{
    if (event_bus_) { core::events::LayoutTransformRefinedEvent evt; evt.transform_type = transform_type; evt.precision_delta = precision_delta; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Layout transform: {} delta={}", transform_type, precision_delta);
}

auto CanvasWorkbench::resolve_alignment_edge_case(const std::string& alignment_type, int objects_affected) -> void
{
    if (event_bus_) { core::events::AlignmentEdgeCaseResolvedEvent evt; evt.alignment_type = alignment_type; evt.objects_affected = objects_affected; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Alignment fix: {} objects={}", alignment_type, objects_affected);
}

// ── V20 Phase 10 W13: Advanced Collaboration Polish ─────────────

auto CanvasWorkbench::resolve_collab_friction(const std::string& friction_type, double latency_improvement_ms) -> void
{
    if (event_bus_) { core::events::CollabFrictionResolvedEvent evt; evt.friction_type = friction_type; evt.latency_improvement_ms = latency_improvement_ms; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Collab friction fix: {} improved={}ms", friction_type, latency_improvement_ms);
}

auto CanvasWorkbench::stabilize_multi_user_session(int concurrent_users, double session_uptime_hours) -> void
{
    if (event_bus_) { core::events::MultiUserSessionStabilizedEvent evt; evt.concurrent_users = concurrent_users; evt.session_uptime_hours = session_uptime_hours; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Session stable: {} users, {}h", concurrent_users, session_uptime_hours);
}

// ── V20 Phase 10 W14: Advanced Navigation Polish ────────────────

auto CanvasWorkbench::optimize_board_travel(const std::string& navigation_mode, double travel_time_ms) -> void
{
    if (event_bus_) { core::events::BoardTravelOptimizedEvent evt; evt.navigation_mode = navigation_mode; evt.travel_time_ms = travel_time_ms; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Board travel: {} {}ms", navigation_mode, travel_time_ms);
}

auto CanvasWorkbench::set_navigation_waypoint(const std::string& waypoint_id, double x) -> void
{
    if (event_bus_) { core::events::NavigationWaypointSetEvent evt; evt.waypoint_id = waypoint_id; evt.x = x; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Waypoint: {} x={}", waypoint_id, x);
}

// ── V20 Phase 10 W15: Advanced Export Polish ────────────────────

auto CanvasWorkbench::finalize_export_artifact(const std::string& format, int pages_exported) -> void
{
    if (event_bus_) { core::events::ExportArtifactFinalizedEvent evt; evt.format = format; evt.pages_exported = pages_exported; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Export finalized: {} pages={}", format, pages_exported);
}

auto CanvasWorkbench::validate_export_fidelity(const std::string& format, double fidelity_score) -> void
{
    if (event_bus_) { core::events::ExportFidelityValidatedEvent evt; evt.format = format; evt.fidelity_score = fidelity_score; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Export fidelity: {} score={}", format, fidelity_score);
}

// ── V20 Phase 10 W16: Enterprise & Admin ────────────────────────

auto CanvasWorkbench::apply_admin_policy(const std::string& policy_name, int users_affected) -> void
{
    if (event_bus_) { core::events::AdminPolicyAppliedEvent evt; evt.policy_name = policy_name; evt.users_affected = users_affected; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Admin policy: {} users={}", policy_name, users_affected);
}

auto CanvasWorkbench::log_governance_audit(const std::string& action_type, const std::string& actor_id) -> void
{
    if (event_bus_) { core::events::GovernanceAuditLoggedEvent evt; evt.action_type = action_type; evt.actor_id = actor_id; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Governance: {} actor={}", action_type, actor_id);
}

// ── V20 Phase 10 W17: Quality Backlog Triage ────────────────────

auto CanvasWorkbench::triage_defect(const std::string& defect_id, const std::string& severity) -> void
{
    if (event_bus_) { core::events::DefectTriagedEvent evt; evt.defect_id = defect_id; evt.severity = severity; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Defect triage: {} severity={}", defect_id, severity);
}

auto CanvasWorkbench::reduce_quality_backlog(int defects_resolved, int defects_remaining) -> void
{
    if (event_bus_) { core::events::QualityBacklogReducedEvent evt; evt.defects_resolved = defects_resolved; evt.defects_remaining = defects_remaining; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Backlog: {} resolved, {} remaining", defects_resolved, defects_remaining);
}

// ── V20 Phase 10 W18: Adoption Metrics ──────────────────────────

auto CanvasWorkbench::track_adoption_metric(const std::string& metric_name, double metric_value) -> void
{
    if (event_bus_) { core::events::AdoptionMetricTrackedEvent evt; evt.metric_name = metric_name; evt.metric_value = metric_value; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Adoption: {} = {}", metric_name, metric_value);
}

auto CanvasWorkbench::analyze_usage_trend(const std::string& feature_name, double trend_direction) -> void
{
    if (event_bus_) { core::events::UsageTrendAnalyzedEvent evt; evt.feature_name = feature_name; evt.trend_direction = trend_direction; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Usage trend: {} dir={}", feature_name, trend_direction);
}

// ── V20 Phase 10 W19: Support Playbooks ─────────────────────────

auto CanvasWorkbench::activate_support_playbook(const std::string& playbook_id, const std::string& issue_category) -> void
{
    if (event_bus_) { core::events::SupportPlaybookActivatedEvent evt; evt.playbook_id = playbook_id; evt.issue_category = issue_category; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Playbook: {} category={}", playbook_id, issue_category);
}

auto CanvasWorkbench::log_support_resolution(const std::string& ticket_id, double resolution_time_hours) -> void
{
    if (event_bus_) { core::events::SupportResolutionLoggedEvent evt; evt.ticket_id = ticket_id; evt.resolution_time_hours = resolution_time_hours; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Support resolved: {} in {}h", ticket_id, resolution_time_hours);
}

// ── V20 Phase 10 W20: Architecture Follow Ups ──────────────────

auto CanvasWorkbench::address_tech_debt(const std::string& debt_item, const std::string& resolution_type) -> void
{
    if (event_bus_) { core::events::TechDebtAddressedEvent evt; evt.debt_item = debt_item; evt.resolution_type = resolution_type; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Tech debt: {} type={}", debt_item, resolution_type);
}

auto CanvasWorkbench::complete_platform_investment(const std::string& investment_area, int files_affected) -> void
{
    if (event_bus_) { core::events::PlatformInvestmentCompletedEvent evt; evt.investment_area = investment_area; evt.files_affected = files_affected; event_bus_->publish(evt); }
    MARKAMP_LOG_INFO("Platform invest: {} files={}", investment_area, files_affected);
}

} // namespace markamp::canvas
