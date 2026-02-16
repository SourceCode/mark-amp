#include "CanvasFacilitationPanel.h"

#include "core/Events.h"
#include "core/Logger.h"

#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

namespace markamp::ui
{

CanvasFacilitationPanel::CanvasFacilitationPanel(wxWindow* parent,
                                                 core::EventBus& event_bus,
                                                 core::CanvasCollabService& collab_service)
    : wxPanel(parent, wxID_ANY)
    , event_bus_(event_bus)
    , collab_service_(collab_service)
{
    create_layout();
    subscribe_to_events();
}

// ── Timer controls ─────────────────────────────────────────────────

auto CanvasFacilitationPanel::start_timer(int duration_seconds) -> void
{
    collab_service_.start_timer(duration_seconds);
    MARKAMP_LOG_INFO("Facilitation timer started for {}s", duration_seconds);
}

auto CanvasFacilitationPanel::stop_timer() -> void
{
    const auto* timer = collab_service_.active_timer();
    if (timer != nullptr)
    {
        collab_service_.stop_timer(timer->timer_id);
    }
}

auto CanvasFacilitationPanel::update_timer_display(int remaining_seconds,
                                                   bool is_running,
                                                   bool is_expired) -> void
{
    MARKAMP_LOG_DEBUG("Timer display: {}s remaining, running={}, expired={}",
                      remaining_seconds,
                      is_running ? "true" : "false",
                      is_expired ? "true" : "false");
    // TODO: update wxStaticText labels with remaining time
}

// ── Voting controls ────────────────────────────────────────────────

auto CanvasFacilitationPanel::start_vote(const std::string& topic,
                                         const std::vector<std::string>& options) -> void
{
    collab_service_.start_vote(topic, options);
}

auto CanvasFacilitationPanel::cast_vote(const std::string& option) -> void
{
    const auto* vote = collab_service_.active_vote();
    if (vote != nullptr)
    {
        collab_service_.cast_vote(vote->vote_id, option);
    }
}

auto CanvasFacilitationPanel::close_vote() -> void
{
    const auto* vote = collab_service_.active_vote();
    if (vote != nullptr)
    {
        collab_service_.close_vote(vote->vote_id);
    }
}

auto CanvasFacilitationPanel::refresh_vote_display() -> void
{
    const auto* vote = collab_service_.active_vote();
    if (vote == nullptr)
    {
        MARKAMP_LOG_DEBUG("No active vote to display");
        return;
    }

    MARKAMP_LOG_DEBUG("Vote '{}': {} votes cast, closed={}",
                      vote->topic,
                      vote->votes.size(),
                      vote->is_closed ? "true" : "false");
    // TODO: rebuild vote option buttons/results
}

// ── Mode controls ──────────────────────────────────────────────────

auto CanvasFacilitationPanel::set_private_mode(bool enabled) -> void
{
    private_mode_ = enabled;
    MARKAMP_LOG_INFO("Private mode: {}", enabled ? "on" : "off");
}

auto CanvasFacilitationPanel::is_private_mode() const -> bool
{
    return private_mode_;
}

auto CanvasFacilitationPanel::reveal_all() -> void
{
    private_mode_ = false;
    MARKAMP_LOG_INFO("Revealed all — private mode off");
}

auto CanvasFacilitationPanel::set_board_locked(bool locked) -> void
{
    board_locked_ = locked;
    MARKAMP_LOG_INFO("Board locked: {}", locked ? "yes" : "no");
}

auto CanvasFacilitationPanel::is_board_locked() const -> bool
{
    return board_locked_;
}

// ── Private helpers ────────────────────────────────────────────────

auto CanvasFacilitationPanel::create_layout() -> void
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    auto* header = new wxStaticText(this, wxID_ANY, "Facilitation");
    sizer->Add(header, 0, wxEXPAND | wxALL, 8);

    sizer->AddStretchSpacer(1);

    SetSizer(sizer);
}

auto CanvasFacilitationPanel::subscribe_to_events() -> void
{
    subscriptions_.push_back(event_bus_.subscribe<core::events::CanvasTimerUpdatedEvent>(
        [this](const core::events::CanvasTimerUpdatedEvent& evt)
        { update_timer_display(evt.remaining_seconds, evt.is_running, evt.is_expired); }));

    subscriptions_.push_back(event_bus_.subscribe<core::events::CanvasVoteUpdatedEvent>(
        [this](const core::events::CanvasVoteUpdatedEvent& /*evt*/) { refresh_vote_display(); }));

    subscriptions_.push_back(event_bus_.subscribe<core::events::CanvasVoteStartedEvent>(
        [this](const core::events::CanvasVoteStartedEvent& /*evt*/) { refresh_vote_display(); }));
}

} // namespace markamp::ui
