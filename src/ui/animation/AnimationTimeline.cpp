#include "AnimationTimeline.h"

#include <algorithm>

namespace markamp::ui::animation
{

bool AnimationTimeline::reduced_motion_ = false;

wxBEGIN_EVENT_TABLE(AnimationTimeline, wxEvtHandler)
    EVT_TIMER(wxID_ANY, AnimationTimeline::on_timer) wxEND_EVENT_TABLE()

        AnimationTimeline::AnimationTimeline()
    : timer_(this)
{
}

AnimationTimeline::~AnimationTimeline()
{
    stop_all();
}

void AnimationTimeline::stop_all()
{
    if (timer_.IsRunning())
    {
        timer_.Stop();
    }
    tracks_.clear();
}

void AnimationTimeline::finish_all()
{
    if (timer_.IsRunning())
    {
        timer_.Stop();
    }

    for (auto& track : tracks_)
    {
        track->finish();
    }

    tracks_.clear();
}

void AnimationTimeline::set_reduced_motion(bool enabled)
{
    reduced_motion_ = enabled;
}

auto AnimationTimeline::is_reduced_motion() -> bool
{
    return reduced_motion_;
}

void AnimationTimeline::on_timer(wxTimerEvent& /*event*/)
{
    if (tracks_.empty())
    {
        timer_.Stop();
        return;
    }

    auto now = std::chrono::steady_clock::now();

    // Use erase-remove idiom to remove finished tracks
    tracks_.erase(std::remove_if(tracks_.begin(),
                                 tracks_.end(),
                                 [now](auto& track) { return track->tick(now); }),
                  tracks_.end());

    if (tracks_.empty())
    {
        timer_.Stop();
    }
}

} // namespace markamp::ui::animation
