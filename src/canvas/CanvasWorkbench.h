// ============================================================================
// File: src/canvas/CanvasWorkbench.h
// Phase 11: Canvas Workbench Shell — workbench mode, board lifecycle, tool switching
// ============================================================================
#pragma once

#include "canvas/Board.h"
#include "canvas/BoardSerializer.h"
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

    // Persistent state
    CanvasWorkbenchState state_;

    // Helpers
    auto add_to_recent(const std::string& board_id) -> void;
    auto emit_board_opened(const std::string& board_id) -> void;
    auto emit_board_closed(const std::string& board_id) -> void;
    auto emit_tool_changed(ToolMode old_mode, ToolMode new_mode) -> void;
};

} // namespace markamp::canvas
