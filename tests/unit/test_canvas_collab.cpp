/// @file test_canvas_collab.cpp
/// @brief V8 Phase 7: Realtime Collaboration — compile guardrail + unit tests.
///
/// Tests collaboration event types, CanvasCollabService session/vote/timer
/// lifecycle, CommentObject thread management, and UndoRedoStack remote
/// patch support.

#include "canvas/CanvasCommands.h"
#include "canvas/CommentObject.h"
#include "canvas/UndoRedoStack.h"
#include "core/CanvasCollabService.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <type_traits>
#include <vector>

// ── Event compilation tests ────────────────────────────────────────

TEST_CASE("Collaboration events compile and default-construct", "[collab]")
{
    using namespace markamp::core::events;

    CanvasSessionJoinedEvent joined;
    REQUIRE(joined.session_id.empty());
    REQUIRE(joined.participant_id.empty());

    CanvasSessionLeftEvent left;
    REQUIRE(left.session_id.empty());

    CanvasPresenceUpdatedEvent presence;
    REQUIRE(presence.status.empty());

    CanvasRemoteCursorMovedEvent cursor;
    REQUIRE(cursor.cursor_x == 0.0);
    REQUIRE(cursor.cursor_y == 0.0);

    CanvasRemoteSelectionChangedEvent selection;
    REQUIRE(selection.selected_object_ids.empty());

    CanvasRemoteObjectPatchedEvent patch;
    REQUIRE(patch.object_id.empty());
    REQUIRE(patch.patch_type.empty());

    CanvasVoteStartedEvent vote_start;
    REQUIRE(vote_start.options.empty());

    CanvasVoteUpdatedEvent vote_update;
    REQUIRE_FALSE(vote_update.is_closed);

    CanvasTimerUpdatedEvent timer;
    REQUIRE(timer.remaining_seconds == 0);
    REQUIRE_FALSE(timer.is_running);
    REQUIRE_FALSE(timer.is_expired);
}

// ── CanvasCollabService tests ──────────────────────────────────────

TEST_CASE("CanvasCollabService session lifecycle", "[collab]")
{
    markamp::core::EventBus bus;
    markamp::core::CanvasCollabService svc(bus);

    REQUIRE_FALSE(svc.is_in_session());
    REQUIRE(svc.participant_count() == 0);

    svc.join_session("session_1", "user_alice", "Alice");

    REQUIRE(svc.is_in_session());
    REQUIRE(svc.session_id() == "session_1");
    REQUIRE(svc.local_participant_id() == "user_alice");
    REQUIRE(svc.participant_count() == 1);

    const auto* alice = svc.find_participant("user_alice");
    REQUIRE(alice != nullptr);
    REQUIRE(alice->display_name == "Alice");

    svc.leave_session();

    REQUIRE_FALSE(svc.is_in_session());
    REQUIRE(svc.participant_count() == 0);
}

TEST_CASE("CanvasCollabService voting workflow", "[collab]")
{
    markamp::core::EventBus bus;
    markamp::core::CanvasCollabService svc(bus);
    svc.join_session("s1", "user_bob", "Bob");

    REQUIRE(svc.active_vote() == nullptr);

    const auto vote_id = svc.start_vote("Best color?", {"Red", "Blue", "Green"});
    REQUIRE_FALSE(vote_id.empty());

    const auto* vote = svc.active_vote();
    REQUIRE(vote != nullptr);
    REQUIRE(vote->topic == "Best color?");
    REQUIRE(vote->options.size() == 3);
    REQUIRE_FALSE(vote->is_closed);

    svc.cast_vote(vote_id, "Blue");
    REQUIRE(svc.active_vote()->votes.size() == 1);

    svc.close_vote(vote_id);
    REQUIRE(svc.active_vote()->is_closed);

    svc.leave_session();
}

TEST_CASE("CanvasCollabService timer lifecycle", "[collab]")
{
    markamp::core::EventBus bus;
    markamp::core::CanvasCollabService svc(bus);
    svc.join_session("s2", "user_carol", "Carol");

    REQUIRE(svc.active_timer() == nullptr);

    const auto timer_id = svc.start_timer(5);
    REQUIRE_FALSE(timer_id.empty());

    const auto* timer = svc.active_timer();
    REQUIRE(timer != nullptr);
    REQUIRE(timer->duration_seconds == 5);
    REQUIRE(timer->remaining_seconds == 5);
    REQUIRE(timer->is_running);

    svc.tick_timer();
    REQUIRE(svc.active_timer()->remaining_seconds == 4);

    svc.stop_timer(timer_id);
    REQUIRE_FALSE(svc.active_timer()->is_running);

    svc.leave_session();
}

TEST_CASE("CanvasCollabService cursor and selection updates", "[collab]")
{
    markamp::core::EventBus bus;
    markamp::core::CanvasCollabService svc(bus);
    svc.join_session("s3", "user_dave", "Dave");

    svc.update_local_cursor(100.0, 200.0);
    const auto* dave = svc.find_participant("user_dave");
    REQUIRE(dave != nullptr);
    REQUIRE(dave->cursor_x == 100.0);
    REQUIRE(dave->cursor_y == 200.0);

    svc.update_local_selection({"obj_1", "obj_2"});
    REQUIRE(dave->selected_object_ids.size() == 2);

    svc.leave_session();
}

// ── CommentObject tests ────────────────────────────────────────────

TEST_CASE("CommentObject thread management", "[collab]")
{
    markamp::canvas::CommentObject comment_obj;
    REQUIRE(comment_obj.type_name() == "comment");
    REQUIRE(comment_obj.comment_count() == 0);
    REQUIRE(comment_obj.root_comment() == nullptr);
    REQUIRE_FALSE(comment_obj.is_resolved());

    markamp::canvas::Comment root;
    root.comment_id = "c1";
    root.author_id = "alice";
    root.author_name = "Alice";
    root.body = "This needs work.";
    comment_obj.add_comment(root);

    REQUIRE(comment_obj.comment_count() == 1);
    REQUIRE(comment_obj.root_comment()->body == "This needs work.");

    markamp::canvas::Comment reply;
    reply.comment_id = "c2";
    reply.author_id = "bob";
    reply.body = "Fixed!";
    comment_obj.add_comment(reply);

    REQUIRE(comment_obj.comment_count() == 2);

    comment_obj.resolve();
    REQUIRE(comment_obj.is_resolved());

    comment_obj.unresolve();
    REQUIRE_FALSE(comment_obj.is_resolved());
}

TEST_CASE("CommentObject clone", "[collab]")
{
    markamp::canvas::CommentObject original;
    original.set_author("alice", "Alice");
    markamp::canvas::Comment root;
    root.comment_id = "clone_test";
    root.body = "Original comment";
    original.add_comment(root);
    original.resolve();

    auto cloned = original.clone();
    REQUIRE(cloned != nullptr);

    auto* cloned_comment = dynamic_cast<markamp::canvas::CommentObject*>(cloned.get());
    REQUIRE(cloned_comment != nullptr);
    REQUIRE(cloned_comment->is_resolved());
    REQUIRE(cloned_comment->comment_count() == 1);
    REQUIRE(cloned_comment->author_id() == "alice");
}

TEST_CASE("CommentObject local_bounds", "[collab]")
{
    markamp::canvas::CommentObject comment_obj;
    const auto bounds = comment_obj.local_bounds();
    REQUIRE(bounds.width() == 24.0);
    REQUIRE(bounds.height() == 24.0);
}

// ── UndoRedoStack remote awareness tests ───────────────────────────

TEST_CASE("ICanvasCommand remote flag", "[collab]")
{
    // Use a simple concrete command to test the flag
    class TestCommand : public markamp::canvas::ICanvasCommand
    {
    public:
        auto execute() -> void override {}
        auto undo() -> void override {}
        [[nodiscard]] auto description() const -> std::string override
        {
            return "test";
        }
    };

    auto cmd = std::make_unique<TestCommand>();
    REQUIRE_FALSE(cmd->is_remote());
    cmd->set_remote(true);
    REQUIRE(cmd->is_remote());
}
