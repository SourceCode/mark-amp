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
    // Improvement 73: Update timer display labels
    if (timer_label_ != nullptr)
    {
        int minutes = remaining_seconds / 60;
        int seconds = remaining_seconds % 60;
        wxString time_text = wxString::Format("%02d:%02d", minutes, seconds);

        if (is_expired)
        {
            timer_label_->SetLabel("⏰ Time's up!");
        }
        else if (is_running)
        {
            timer_label_->SetLabel("⏱ " + time_text);
        }
        else
        {
            timer_label_->SetLabel("Timer: " + time_text + " (paused)");
        }
    }
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
    // Improvement 74: Rebuild vote option buttons with tally display
    if (vote_label_ != nullptr)
    {
        wxString vote_text = "Vote: " + wxString(vote->topic) + "\n";
        for (const auto& [option, count] : vote->votes)
        {
            vote_text += "  " + wxString(option) + ": " + wxString::Format("%zu", count) + "\n";
        }
        if (vote->is_closed)
        {
            vote_text += "(Voting closed)";
        }
        vote_label_->SetLabel(vote_text);
    }
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

    // Improvement 71: Header with icon
    auto* header = new wxStaticText(this, wxID_ANY, "\xF0\x9F\x8E\xAF Facilitation");
    header->SetFont(header->GetFont().Bold().Scaled(1.1f));
    sizer->Add(header, 0, wxEXPAND | wxALL, 8);

    // ── Timer section ──

    auto* timer_section = new wxStaticText(this, wxID_ANY, "Timer");
    timer_section->SetFont(timer_section->GetFont().Bold());
    sizer->Add(timer_section, 0, wxLEFT | wxTOP, 8);

    // Improvement 73: Timer display label
    timer_label_ = new wxStaticText(this, wxID_ANY, "Timer: 00:00");
    timer_label_->SetFont(timer_label_->GetFont().Scaled(1.5f));
    sizer->Add(timer_label_, 0, wxALIGN_CENTER | wxALL, 4);

    auto* timer_btns = new wxBoxSizer(wxHORIZONTAL);

    auto* start_timer_btn = new wxButton(this, wxID_ANY, "\xE2\x8F\xB1 Start 5min",
        wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxBU_EXACTFIT);
    start_timer_btn->Bind(wxEVT_BUTTON,
        [this](wxCommandEvent& /*evt*/) { start_timer(300); });
    timer_btns->Add(start_timer_btn, 1, wxEXPAND | wxRIGHT, 2);

    auto* stop_timer_btn = new wxButton(this, wxID_ANY, "\xE2\x8F\xB9 Stop",
        wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxBU_EXACTFIT);
    stop_timer_btn->Bind(wxEVT_BUTTON,
        [this](wxCommandEvent& /*evt*/) { stop_timer(); });
    timer_btns->Add(stop_timer_btn, 1, wxEXPAND);

    sizer->Add(timer_btns, 0, wxEXPAND | wxLEFT | wxRIGHT, 8);

    // ── Vote section ──

    sizer->AddSpacer(12);
    auto* vote_section = new wxStaticText(this, wxID_ANY, "Voting");
    vote_section->SetFont(vote_section->GetFont().Bold());
    sizer->Add(vote_section, 0, wxLEFT, 8);

    // Improvement 74: Vote display label
    vote_label_ = new wxStaticText(this, wxID_ANY, "No active vote");
    sizer->Add(vote_label_, 0, wxLEFT | wxRIGHT | wxTOP, 8);

    auto* vote_btns = new wxBoxSizer(wxHORIZONTAL);

    auto* start_vote_btn = new wxButton(this, wxID_ANY, "\xF0\x9F\x97\xB3 New Vote",
        wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxBU_EXACTFIT);
    start_vote_btn->Bind(wxEVT_BUTTON,
        [this](wxCommandEvent& /*evt*/)
        {
            start_vote("Quick poll", {"Yes", "No", "Abstain"});
        });
    vote_btns->Add(start_vote_btn, 1, wxEXPAND | wxRIGHT, 2);

    auto* close_vote_btn = new wxButton(this, wxID_ANY, "\xE2\x9C\x96 Close",
        wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxBU_EXACTFIT);
    close_vote_btn->Bind(wxEVT_BUTTON,
        [this](wxCommandEvent& /*evt*/) { close_vote(); });
    vote_btns->Add(close_vote_btn, 1, wxEXPAND);

    sizer->Add(vote_btns, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 8);

    // ── Mode section ──

    sizer->AddSpacer(12);
    auto* mode_section = new wxStaticText(this, wxID_ANY, "Board Controls");
    mode_section->SetFont(mode_section->GetFont().Bold());
    sizer->Add(mode_section, 0, wxLEFT, 8);

    // Improvement 75: Private mode toggle
    auto* private_btn = new wxButton(this, wxID_ANY, "\xF0\x9F\x94\x92 Toggle Private Mode",
        wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    private_btn->Bind(wxEVT_BUTTON,
        [this](wxCommandEvent& /*evt*/)
        {
            set_private_mode(!is_private_mode());
        });
    sizer->Add(private_btn, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 8);

    // Improvement 75: Reveal all button
    auto* reveal_btn = new wxButton(this, wxID_ANY, "\xF0\x9F\x91\x81 Reveal All",
        wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    reveal_btn->Bind(wxEVT_BUTTON,
        [this](wxCommandEvent& /*evt*/) { reveal_all(); });
    sizer->Add(reveal_btn, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 4);

    // Improvement 76: Board lock toggle
    auto* lock_btn = new wxButton(this, wxID_ANY, "\xF0\x9F\x94\x93 Toggle Board Lock",
        wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    lock_btn->Bind(wxEVT_BUTTON,
        [this](wxCommandEvent& /*evt*/)
        {
            set_board_locked(!is_board_locked());
        });
    sizer->Add(lock_btn, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 4);

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
