#pragma once

#include "AnimationConfig.h"
#include "ValueInterpolator.h"

#include <wx/event.h>
#include <wx/timer.h>

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

namespace markamp::ui::animation
{

class AnimationTrackBase
{
public:
    virtual ~AnimationTrackBase() = default;
    virtual auto tick(std::chrono::steady_clock::time_point now) -> bool = 0;
    virtual void finish() = 0;
};

template <typename T>
class AnimationTrack : public AnimationTrackBase
{
public:
    using UpdateCallback = std::function<void(const T&)>;
    using FinishCallback = std::function<void()>;

    AnimationTrack(const T& start,
                   const T& end,
                   const AnimationConfig& config,
                   std::chrono::steady_clock::time_point start_time,
                   UpdateCallback on_update,
                   FinishCallback on_finish = nullptr)
        : start_(start)
        , end_(end)
        , config_(config)
        , start_time_(start_time)
        , on_update_(std::move(on_update))
        , on_finish_(std::move(on_finish))
    {
    }

    auto tick(std::chrono::steady_clock::time_point now) -> bool override
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_);

        if (elapsed < config_.delay)
            return false;

        auto active_time = elapsed - config_.delay;

        float t = 0.0f;
        float cycle_count = 0.0f;
        if (config_.duration.count() > 0)
        {
            cycle_count = static_cast<float>(active_time.count()) /
                          static_cast<float>(config_.duration.count());
            t = cycle_count;
        }
        else
        {
            t = 1.0f;
        }

        bool finished = false;
        if (t >= 1.0f)
        {
            if (config_.repeat_count != 0)
            {
                if (config_.repeat_count > 0 &&
                    cycle_count >= static_cast<float>(config_.repeat_count + 1))
                {
                    t = 1.0f;
                    finished = true;
                }
                else
                {
                    t = std::fmod(cycle_count, 1.0f);
                    if (config_.auto_reverse && (static_cast<int>(cycle_count) % 2 != 0))
                    {
                        t = 1.0f - t;
                    }
                }
            }
            else
            {
                t = 1.0f;
                finished = true;
            }
        }

        float eased_t = Easing::calculate(t, config_.easing_type);
        T current = ValueInterpolator::interpolate(start_, end_, eased_t);

        if (on_update_)
            on_update_(current);

        if (finished && on_finish_)
            on_finish_();

        return finished;
    }

    void finish() override
    {
        if (on_update_)
            on_update_(end_);
        if (on_finish_)
            on_finish_();
    }

private:
    T start_;
    T end_;
    AnimationConfig config_;
    std::chrono::steady_clock::time_point start_time_;
    UpdateCallback on_update_;
    FinishCallback on_finish_;
};

class AnimationTimeline : public wxEvtHandler
{
public:
    AnimationTimeline();
    ~AnimationTimeline() override;

    template <typename T>
    void animate(const T& start,
                 const T& end,
                 const AnimationConfig& config,
                 typename AnimationTrack<T>::UpdateCallback on_update,
                 typename AnimationTrack<T>::FinishCallback on_finish = nullptr)
    {
        if (reduced_motion_)
        {
            // Instantly resolve if reduced motion is on
            if (on_update)
                on_update(end);
            if (on_finish)
                on_finish();
            return;
        }

        auto now = std::chrono::steady_clock::now();
        tracks_.push_back(
            std::make_unique<AnimationTrack<T>>(start, end, config, now, on_update, on_finish));

        if (!timer_.IsRunning())
        {
            timer_.Start(16); // ~60fps
        }
    }

    void stop_all();
    void finish_all();

    static void set_reduced_motion(bool enabled);
    static auto is_reduced_motion() -> bool;

private:
    void on_timer(wxTimerEvent& event);

    wxTimer timer_;
    std::vector<std::unique_ptr<AnimationTrackBase>> tracks_;
    static bool reduced_motion_;

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui::animation
