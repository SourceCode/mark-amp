// ============================================================================
// File: src/canvas/CanvasWorkbench.cpp
// Phase 11: Canvas Workbench Shell — implementation
// ============================================================================
#include "CanvasWorkbench.h"

#include "canvas/Board.h"
#include "canvas/BoardSerializer.h"
#include "canvas/DrawTool.h"
#include "canvas/PanTool.h"
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
    return file.good();
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

auto CanvasWorkbench::emit_board_opened(const std::string& /*board_id*/) -> void
{
    // Event emission via EventBus — Phase 11 integration point
}

auto CanvasWorkbench::emit_board_closed(const std::string& /*board_id*/) -> void
{
    // Event emission via EventBus — Phase 11 integration point
}

auto CanvasWorkbench::emit_tool_changed(ToolMode /*old_mode*/, ToolMode /*new_mode*/) -> void
{
    // Event emission via EventBus — Phase 11 integration point
}

} // namespace markamp::canvas
