// ============================================================================
// File: tests/unit/test_phase13_canvas_collab.cpp
// Phase 13: Canvas Collaboration & Facilitation — comprehensive tests
// ============================================================================
#include "canvas/BoardLockController.h"
#include "canvas/CollabEventLogger.h"
#include "canvas/CollabUndoRedoManager.h"
#include "canvas/FollowModeController.h"
#include "canvas/OfflineSyncQueue.h"
#include "canvas/PrivateRevealController.h"
#include "canvas/RemoteCursorOverlay.h"
#include "canvas/RemoteSelectionOverlay.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::canvas;
using namespace markamp::core;

// ── RemoteCursorOverlay ───────────────────────────────────────────

TEST_CASE("RemoteCursorOverlay: default has no cursors", "[phase13][cursor]")
{
    RemoteCursorOverlay overlay;
    REQUIRE(overlay.cursor_count() == 0);
    REQUIRE(overlay.visible_cursors().empty());
}

TEST_CASE("RemoteCursorOverlay: add and find cursor", "[phase13][cursor]")
{
    RemoteCursorOverlay overlay;
    overlay.update_cursor("user1", "Alice", {100.0, 200.0}, {0, 120, 215, 255});

    REQUIRE(overlay.cursor_count() == 1);

    const auto* cursor = overlay.find_cursor("user1");
    REQUIRE(cursor != nullptr);
    CHECK(cursor->display_name == "Alice");
    CHECK(cursor->position.x == 100.0);
    CHECK(cursor->position.y == 200.0);
}

TEST_CASE("RemoteCursorOverlay: update existing cursor", "[phase13][cursor]")
{
    RemoteCursorOverlay overlay;
    overlay.update_cursor("user1", "Alice", {100.0, 200.0}, {0, 120, 215, 255});
    overlay.update_cursor("user1", "Alice", {150.0, 250.0}, {0, 120, 215, 255});

    REQUIRE(overlay.cursor_count() == 1);
    const auto* cursor = overlay.find_cursor("user1");
    REQUIRE(cursor != nullptr);
    CHECK(cursor->target_position.x == 150.0);
    CHECK(cursor->target_position.y == 250.0);
}

TEST_CASE("RemoteCursorOverlay: remove cursor", "[phase13][cursor]")
{
    RemoteCursorOverlay overlay;
    overlay.update_cursor("user1", "Alice", {0, 0}, {0, 120, 215, 255});
    overlay.update_cursor("user2", "Bob", {0, 0}, {232, 65, 24, 255});
    REQUIRE(overlay.cursor_count() == 2);

    overlay.remove_cursor("user1");
    REQUIRE(overlay.cursor_count() == 1);
    CHECK(overlay.find_cursor("user1") == nullptr);
    CHECK(overlay.find_cursor("user2") != nullptr);
}

TEST_CASE("RemoteCursorOverlay: clear all cursors", "[phase13][cursor]")
{
    RemoteCursorOverlay overlay;
    overlay.update_cursor("user1", "Alice", {0, 0}, {0, 0, 0, 255});
    overlay.update_cursor("user2", "Bob", {0, 0}, {0, 0, 0, 255});
    overlay.clear();
    REQUIRE(overlay.cursor_count() == 0);
}

TEST_CASE("RemoteCursorOverlay: tool names", "[phase13][cursor]")
{
    CHECK(RemoteCursorOverlay::tool_name(RemoteToolType::kPointer) == "Pointer");
    CHECK(RemoteCursorOverlay::tool_name(RemoteToolType::kDraw) == "Draw");
    CHECK(RemoteCursorOverlay::tool_name(RemoteToolType::kLaser) == "Laser");
}

TEST_CASE("RemoteCursorOverlay: participant colors are distinct", "[phase13][cursor]")
{
    auto color0 = RemoteCursorOverlay::participant_color(0);
    auto color1 = RemoteCursorOverlay::participant_color(1);
    CHECK(color0.r != color1.r);
}

TEST_CASE("RemoteCursorOverlay: label visibility toggle", "[phase13][cursor]")
{
    RemoteCursorOverlay overlay;
    CHECK(overlay.labels_visible() == true);
    overlay.set_labels_visible(false);
    CHECK(overlay.labels_visible() == false);
}

// ── RemoteSelectionOverlay ────────────────────────────────────────

TEST_CASE("RemoteSelectionOverlay: default has no selections", "[phase13][selection]")
{
    RemoteSelectionOverlay overlay;
    REQUIRE(overlay.active_selection_count() == 0);
    REQUIRE(overlay.total_selected_objects() == 0);
}

TEST_CASE("RemoteSelectionOverlay: update selection", "[phase13][selection]")
{
    RemoteSelectionOverlay overlay;
    overlay.update_selection("user1", "Alice", {1, 2, 3}, {0, 120, 215, 255});

    REQUIRE(overlay.active_selection_count() == 1);
    REQUIRE(overlay.total_selected_objects() == 3);

    const auto* sel = overlay.find_selection("user1");
    REQUIRE(sel != nullptr);
    CHECK(sel->selected_ids.size() == 3);
}

TEST_CASE("RemoteSelectionOverlay: is_remotely_selected", "[phase13][selection]")
{
    RemoteSelectionOverlay overlay;
    overlay.update_selection("user1", "Alice", {10, 20}, {0, 0, 0, 255});

    CHECK(overlay.is_remotely_selected(10) == true);
    CHECK(overlay.is_remotely_selected(99) == false);
}

TEST_CASE("RemoteSelectionOverlay: conflict detection", "[phase13][selection]")
{
    RemoteSelectionOverlay overlay;
    overlay.update_selection("user1", "Alice", {10, 20}, {0, 0, 0, 255});
    overlay.update_selection("user2", "Bob", {10, 30}, {255, 0, 0, 255});

    auto conflicts = overlay.conflicts();
    REQUIRE(conflicts.size() == 1);
    CHECK(conflicts[0].object_id == 10);
    CHECK(conflicts[0].participant_ids.size() == 2);
}

TEST_CASE("RemoteSelectionOverlay: clear selection", "[phase13][selection]")
{
    RemoteSelectionOverlay overlay;
    overlay.update_selection("user1", "Alice", {1, 2}, {0, 0, 0, 255});
    overlay.clear_selection("user1");

    REQUIRE(overlay.active_selection_count() == 0);
}

// ── FollowModeController ──────────────────────────────────────────

TEST_CASE("FollowModeController: default is not following", "[phase13][follow]")
{
    EventBus bus;
    FollowModeController ctrl(bus);

    CHECK(ctrl.status() == FollowStatus::kNone);
    CHECK(ctrl.is_following() == false);
    CHECK(ctrl.is_presenting() == false);
}

TEST_CASE("FollowModeController: follow and unfollow", "[phase13][follow]")
{
    EventBus bus;
    FollowModeController ctrl(bus);

    ctrl.follow("user2");
    CHECK(ctrl.is_following() == true);
    CHECK(ctrl.following_id() == "user2");

    ctrl.unfollow();
    CHECK(ctrl.is_following() == false);
    CHECK(ctrl.status() == FollowStatus::kNone);
}

TEST_CASE("FollowModeController: presenter mode", "[phase13][follow]")
{
    EventBus bus;
    FollowModeController ctrl(bus);

    ctrl.start_presenting();
    CHECK(ctrl.is_presenting() == true);
    CHECK(ctrl.status() == FollowStatus::kPresenting);

    ctrl.stop_presenting();
    CHECK(ctrl.is_presenting() == false);
}

TEST_CASE("FollowModeController: break on pan", "[phase13][follow]")
{
    EventBus bus;
    FollowModeController ctrl(bus);

    ctrl.follow("user2");
    CHECK(ctrl.is_following() == true);

    ctrl.on_user_pan();
    CHECK(ctrl.is_following() == false);
}

TEST_CASE("FollowModeController: status names", "[phase13][follow]")
{
    CHECK(FollowModeController::status_name(FollowStatus::kNone) == "Not Following");
    CHECK(FollowModeController::status_name(FollowStatus::kFollowing) == "Following");
    CHECK(FollowModeController::status_name(FollowStatus::kPresenting) == "Presenting");
}

// ── BoardLockController ───────────────────────────────────────────

TEST_CASE("BoardLockController: default is unlocked", "[phase13][lock]")
{
    EventBus bus;
    BoardLockController ctrl(bus);

    CHECK(ctrl.is_board_locked() == false);
    CHECK(ctrl.lock_scope() == LockScope::kNone);
}

TEST_CASE("BoardLockController: lock and unlock board", "[phase13][lock]")
{
    EventBus bus;
    BoardLockController ctrl(bus);

    auto result = ctrl.lock_board("host1");
    CHECK(result.success == true);
    CHECK(ctrl.is_board_locked() == true);
    CHECK(ctrl.locked_by() == "host1");

    // Only host can unlock
    auto fail_result = ctrl.unlock_board("user2");
    CHECK(fail_result.success == false);

    auto unlock_result = ctrl.unlock_board("host1");
    CHECK(unlock_result.success == true);
    CHECK(ctrl.is_board_locked() == false);
}

TEST_CASE("BoardLockController: region lock", "[phase13][lock]")
{
    EventBus bus;
    BoardLockController ctrl(bus);

    AABB region_bounds;
    region_bounds.min_x = 0.0;
    region_bounds.min_y = 0.0;
    region_bounds.max_x = 100.0;
    region_bounds.max_y = 100.0;

    auto result = ctrl.lock_region("user1", "region_a", region_bounds);
    CHECK(result.success == true);
    CHECK(ctrl.region_lock_count() == 1);
    CHECK(ctrl.lock_scope() == LockScope::kRegion);
}

TEST_CASE("BoardLockController: can_edit permission check", "[phase13][lock]")
{
    EventBus bus;
    BoardLockController ctrl(bus);

    ctrl.lock_board("host1");

    // Host can edit
    CHECK(ctrl.can_edit("host1", {50.0, 50.0}) == true);

    // Others cannot
    CHECK(ctrl.can_edit("user2", {50.0, 50.0}) == false);
}

TEST_CASE("BoardLockController: scope names", "[phase13][lock]")
{
    CHECK(BoardLockController::scope_name(LockScope::kNone) == "Unlocked");
    CHECK(BoardLockController::scope_name(LockScope::kFullBoard) == "Full Board Locked");
    CHECK(BoardLockController::scope_name(LockScope::kRegion) == "Region Locked");
}

// ── PrivateRevealController ───────────────────────────────────────

TEST_CASE("PrivateRevealController: no active round initially", "[phase13][reveal]")
{
    EventBus bus;
    Board board;
    PrivateRevealController ctrl(board, bus);

    CHECK(ctrl.is_round_active() == false);
    CHECK(ctrl.current_round() == nullptr);
    CHECK(ctrl.round_count() == 0);
}

TEST_CASE("PrivateRevealController: start private round", "[phase13][reveal]")
{
    EventBus bus;
    Board board;
    PrivateRevealController ctrl(board, bus);

    auto round_id = ctrl.start_private_round("host1", "Brainstorm ideas");
    CHECK_FALSE(round_id.empty());
    CHECK(ctrl.is_round_active() == true);
    CHECK(ctrl.round_count() == 1);

    const auto* round = ctrl.current_round();
    REQUIRE(round != nullptr);
    CHECK(round->topic == "Brainstorm ideas");
    CHECK(round->state == RevealRoundState::kPrivatePhase);
}

TEST_CASE("PrivateRevealController: reveal changes state", "[phase13][reveal]")
{
    EventBus bus;
    Board board;
    PrivateRevealController ctrl(board, bus);

    ctrl.start_private_round("host1", "Topic");
    auto result = ctrl.reveal_all("host1");
    CHECK(result.success == true);

    const auto* round = ctrl.current_round();
    REQUIRE(round != nullptr);
    CHECK(round->state == RevealRoundState::kRevealed);
}

TEST_CASE("PrivateRevealController: only host can reveal", "[phase13][reveal]")
{
    EventBus bus;
    Board board;
    PrivateRevealController ctrl(board, bus);

    ctrl.start_private_round("host1", "Topic");
    auto result = ctrl.reveal_all("user2");
    CHECK(result.success == false);
}

TEST_CASE("PrivateRevealController: state names", "[phase13][reveal]")
{
    CHECK(PrivateRevealController::state_name(RevealRoundState::kNotStarted) == "Not Started");
    CHECK(PrivateRevealController::state_name(RevealRoundState::kPrivatePhase) == "Private Phase");
    CHECK(PrivateRevealController::state_name(RevealRoundState::kRevealed) == "Revealed");
    CHECK(PrivateRevealController::state_name(RevealRoundState::kCompleted) == "Completed");
}

// ── CollabUndoRedoManager ─────────────────────────────────────────

TEST_CASE("CollabUndoRedoManager: nothing to undo initially", "[phase13][undo]")
{
    EventBus bus;
    Board board;
    CollabUndoRedoManager mgr(board, bus);

    CHECK(mgr.can_undo("user1") == false);
    CHECK(mgr.can_redo("user1") == false);
    CHECK(mgr.total_operations() == 0);
}

TEST_CASE("CollabUndoRedoManager: record and undo", "[phase13][undo]")
{
    EventBus bus;
    Board board;
    CollabUndoRedoManager mgr(board, bus);

    mgr.record_operation("user1", "move", 1, R"({"dx":10})", R"({"dx":-10})");
    CHECK(mgr.can_undo("user1") == true);
    CHECK(mgr.undo_depth("user1") == 1);

    auto result = mgr.undo_for_user("user1");
    CHECK(result.success == true);
    CHECK(mgr.undo_depth("user1") == 0);
    CHECK(mgr.can_redo("user1") == true);
}

TEST_CASE("CollabUndoRedoManager: redo after undo", "[phase13][undo]")
{
    EventBus bus;
    Board board;
    CollabUndoRedoManager mgr(board, bus);

    mgr.record_operation("user1", "create", 5, "{}", "{}");
    mgr.undo_for_user("user1");

    auto result = mgr.redo_for_user("user1");
    CHECK(result.success == true);
    CHECK(mgr.undo_depth("user1") == 1);
    CHECK(mgr.redo_depth("user1") == 0);
}

TEST_CASE("CollabUndoRedoManager: conflict detection", "[phase13][undo]")
{
    EventBus bus;
    Board board;
    CollabUndoRedoManager mgr(board, bus);

    // User1 modifies object 10, then user2 modifies same object
    mgr.record_operation("user1", "move", 10, "{}", "{}");
    mgr.record_operation("user2", "resize", 10, "{}", "{}");

    // User1 undoes — should detect conflict since user2 modified object 10
    auto result = mgr.undo_for_user("user1");
    CHECK(result.success == true);
    CHECK(result.had_conflict == true);
}

TEST_CASE("CollabUndoRedoManager: object ownership tracking", "[phase13][undo]")
{
    EventBus bus;
    Board board;
    CollabUndoRedoManager mgr(board, bus);

    mgr.record_operation("user1", "create", 5, "{}", "{}");
    CHECK(mgr.object_last_modifier(5) == "user1");

    mgr.record_operation("user2", "modify", 5, "{}", "{}");
    CHECK(mgr.object_last_modifier(5) == "user2");
}

TEST_CASE("CollabUndoRedoManager: max undo depth", "[phase13][undo]")
{
    EventBus bus;
    Board board;
    CollabUndoRedoManager mgr(board, bus);

    mgr.set_max_undo_depth(3);

    for (int idx = 0; idx < 5; ++idx)
    {
        mgr.record_operation("user1", "move", static_cast<ObjectId>(idx), "{}", "{}");
    }

    CHECK(mgr.undo_depth("user1") == 3);
}

// ── CollabEventLogger ─────────────────────────────────────────────

TEST_CASE("CollabEventLogger: empty initially", "[phase13][logger]")
{
    CollabEventLogger logger;
    CHECK(logger.entry_count() == 0);
}

TEST_CASE("CollabEventLogger: log events", "[phase13][logger]")
{
    CollabEventLogger logger;
    logger.log_session("participant_joined", "user1");
    logger.log_session("participant_joined", "user2");
    logger.log_edit("user1", "obj_1", "create");

    CHECK(logger.entry_count() == 3);
}

TEST_CASE("CollabEventLogger: filter by category", "[phase13][logger]")
{
    CollabEventLogger logger;
    logger.log_session("join", "user1");
    logger.log_edit("user1", "obj_1", "create");

    auto session_entries = logger.entries_by_category(CollabEventCategory::kSession);
    CHECK(session_entries.size() == 1);

    auto edit_entries = logger.entries_by_category(CollabEventCategory::kEdit);
    CHECK(edit_entries.size() == 1);
}

TEST_CASE("CollabEventLogger: filter by participant", "[phase13][logger]")
{
    CollabEventLogger logger;
    logger.log_session("join", "user1");
    logger.log_session("join", "user2");
    logger.log_edit("user1", "obj_1", "create");

    auto user1_entries = logger.entries_by_participant("user1");
    CHECK(user1_entries.size() == 2);
}

TEST_CASE("CollabEventLogger: presence events skipped by default", "[phase13][logger]")
{
    CollabEventLogger logger;
    logger.log_event(CollabEventCategory::kPresence, "cursor_move", "user1", "{}");
    CHECK(logger.entry_count() == 0); // Skipped because log_presence_events is false
}

TEST_CASE("CollabEventLogger: export JSON", "[phase13][logger]")
{
    CollabEventLogger logger;
    logger.log_session("join", "user1");

    auto json = logger.export_json();
    CHECK_FALSE(json.empty());
    CHECK(json.front() == '[');
    CHECK(json.back() == ']');
}

TEST_CASE("CollabEventLogger: category names", "[phase13][logger]")
{
    CHECK(CollabEventLogger::category_name(CollabEventCategory::kSession) == "session");
    CHECK(CollabEventLogger::category_name(CollabEventCategory::kEdit) == "edit");
    CHECK(CollabEventLogger::category_name(CollabEventCategory::kVote) == "vote");
    CHECK(CollabEventLogger::category_name(CollabEventCategory::kSystem) == "system");
}

// ── OfflineSyncQueue ──────────────────────────────────────────────

TEST_CASE("OfflineSyncQueue: empty initially", "[phase13][offline]")
{
    OfflineSyncQueue queue;
    CHECK(queue.is_empty() == true);
    CHECK(queue.queue_size() == 0);
    CHECK(queue.is_offline() == false);
}

TEST_CASE("OfflineSyncQueue: enqueue operations", "[phase13][offline]")
{
    OfflineSyncQueue queue;
    auto op_id = queue.enqueue_operation(QueuedOperationType::kCreate, "user1", 1, "{}");

    CHECK_FALSE(op_id.empty());
    CHECK(queue.queue_size() == 1);
}

TEST_CASE("OfflineSyncQueue: replay queue no conflicts", "[phase13][offline]")
{
    OfflineSyncQueue queue;
    queue.enqueue_operation(QueuedOperationType::kCreate, "user1", 1, "{}");
    queue.enqueue_operation(QueuedOperationType::kModify, "user1", 2, "{}");

    auto reports = queue.replay_queue();
    REQUIRE(reports.size() == 2);
    CHECK(reports[0].result == SyncResult::kApplied);
    CHECK(reports[1].result == SyncResult::kApplied);
    CHECK(queue.is_empty() == true);
}

TEST_CASE("OfflineSyncQueue: go offline/online", "[phase13][offline]")
{
    OfflineSyncQueue queue;

    queue.go_offline();
    CHECK(queue.is_offline() == true);

    queue.enqueue_operation(QueuedOperationType::kMove, "user1", 1, "{}");

    auto reports = queue.go_online();
    CHECK(queue.is_offline() == false);
    CHECK(reports.size() == 1);
}

TEST_CASE("OfflineSyncQueue: conflict detection with checker", "[phase13][offline]")
{
    OfflineSyncQueue queue;

    // Always-conflict checker
    queue.set_conflict_checker([](ObjectId /*obj_id*/, const std::string& /*patch*/)
                               { return true; });

    queue.enqueue_operation(QueuedOperationType::kModify, "user1", 1, "{}");

    auto reports = queue.replay_queue();
    REQUIRE(reports.size() == 1);
    CHECK(reports[0].result == SyncResult::kConflict);
    // Conflicted operation stays in queue
    CHECK(queue.queue_size() == 1);
}

TEST_CASE("OfflineSyncQueue: remove operation", "[phase13][offline]")
{
    OfflineSyncQueue queue;
    auto op_id = queue.enqueue_operation(QueuedOperationType::kCreate, "user1", 1, "{}");

    CHECK(queue.remove_operation(op_id) == true);
    CHECK(queue.is_empty() == true);
}

TEST_CASE("OfflineSyncQueue: operation type names", "[phase13][offline]")
{
    CHECK(OfflineSyncQueue::operation_type_name(QueuedOperationType::kCreate) == "Create");
    CHECK(OfflineSyncQueue::operation_type_name(QueuedOperationType::kDelete) == "Delete");
    CHECK(OfflineSyncQueue::operation_type_name(QueuedOperationType::kMove) == "Move");
    CHECK(OfflineSyncQueue::operation_type_name(QueuedOperationType::kStyleChange) ==
          "Style Change");
}
