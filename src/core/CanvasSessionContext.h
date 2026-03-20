/// @file CanvasSessionContext.h
/// @brief P07-T03: Canvas session context for comments, facilitation, and collab.
///
/// Binds adjunct panels to the active board/session rather than isolated
/// local state. Clears state on board switch or canvas mode exit.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;

/// A participant in a collaboration session.
struct SessionParticipant
{
    std::string user_id;
    std::string display_name;
    bool is_local{false};
};

/// Canvas session context consumed by comments, facilitation, and collab panels.
class CanvasSessionContext
{
public:
    explicit CanvasSessionContext(EventBus& bus);

    /// Set the active board for this session.
    void set_board(const std::string& board_id);

    /// Clear session context (board switch or mode exit).
    void clear();

    /// Get the active board.
    [[nodiscard]] auto board_id() const -> const std::string& { return board_id_; }

    /// Check if a session is active.
    [[nodiscard]] auto has_session() const -> bool { return session_active_; }

    /// Join a collaboration session.
    void join_session(const std::string& session_id);

    /// Leave the current session.
    void leave_session();

    /// Get session ID.
    [[nodiscard]] auto session_id() const -> const std::string& { return session_id_; }

    /// Add a participant.
    void add_participant(const SessionParticipant& participant);

    /// Remove a participant.
    void remove_participant(const std::string& user_id);

    /// Get participant count.
    [[nodiscard]] auto participant_count() const -> int
    {
        return static_cast<int>(participants_.size());
    }

    /// Get participants.
    [[nodiscard]] auto participants() const -> const std::vector<SessionParticipant>&
    {
        return participants_;
    }

private:
    EventBus& event_bus_;
    std::string board_id_;
    std::string session_id_;
    bool session_active_{false};
    std::vector<SessionParticipant> participants_;
};

} // namespace markamp::core
