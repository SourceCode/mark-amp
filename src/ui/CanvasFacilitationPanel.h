#pragma once

#include "core/CanvasCollabService.h"
#include "core/EventBus.h"

#include <wx/panel.h>
#include <wx/stattext.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Facilitation panel for collaborative sessions on the canvas.
/// Provides timer, voting, private/reveal modes, and board/object
/// locking controls matching whiteboard facilitation workflows.
class CanvasFacilitationPanel : public wxPanel
{
public:
    CanvasFacilitationPanel(wxWindow* parent,
                            core::EventBus& event_bus,
                            core::CanvasCollabService& collab_service);
    ~CanvasFacilitationPanel() override = default;

    // Rule-of-five
    CanvasFacilitationPanel(const CanvasFacilitationPanel&) = delete;
    auto operator=(const CanvasFacilitationPanel&) -> CanvasFacilitationPanel& = delete;
    CanvasFacilitationPanel(CanvasFacilitationPanel&&) = delete;
    auto operator=(CanvasFacilitationPanel&&) -> CanvasFacilitationPanel& = delete;

    // ── Timer controls ─────────────────────────────────────────────

    /// Start a countdown timer with the given duration.
    auto start_timer(int duration_seconds) -> void;

    /// Stop the running timer.
    auto stop_timer() -> void;

    /// Update the timer display (called on each tick).
    auto update_timer_display(int remaining_seconds, bool is_running, bool is_expired) -> void;

    // ── Voting controls ────────────────────────────────────────────

    /// Start a new vote with topic and options.
    auto start_vote(const std::string& topic, const std::vector<std::string>& options) -> void;

    /// Cast the local user's vote.
    auto cast_vote(const std::string& option) -> void;

    /// Close the active vote.
    auto close_vote() -> void;

    /// Update the vote display from service state.
    auto refresh_vote_display() -> void;

    // ── Mode controls ──────────────────────────────────────────────

    /// Toggle private mode (hide remote work until reveal).
    auto set_private_mode(bool enabled) -> void;
    [[nodiscard]] auto is_private_mode() const -> bool;

    /// Reveal all hidden objects (end private mode).
    auto reveal_all() -> void;

    /// Lock the entire board (prevent editing).
    auto set_board_locked(bool locked) -> void;
    [[nodiscard]] auto is_board_locked() const -> bool;

private:
    core::EventBus& event_bus_;
    core::CanvasCollabService& collab_service_;

    bool private_mode_{false};
    bool board_locked_{false};

    std::vector<core::Subscription> subscriptions_;

    auto create_layout() -> void;
    auto subscribe_to_events() -> void;

    wxStaticText* timer_label_{nullptr};
    wxStaticText* vote_label_{nullptr};
};

} // namespace markamp::ui
