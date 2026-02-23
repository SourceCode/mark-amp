#pragma once

#include "AnimationTimeline.h"

#include <wx/window.h>

#include <string>
#include <unordered_map>

namespace markamp::ui::animation
{

/// A higher-level state transition coordinator that triggers redraws on a target window.
class TransitionManager
{
public:
    explicit TransitionManager(wxWindow* target);
    ~TransitionManager() = default;

    /// Registers a named transition configuration
    void register_transition(const std::string& name, const AnimationConfig& config);

    /// Starts a state transition, interpolating the value from start to end
    template <typename T>
    void start(const std::string& name,
               const T& start_val,
               const T& end_val,
               std::function<void(const T&)> setter,
               std::function<void()> on_finish = nullptr)
    {
        auto config = configs_.count(name) ? configs_[name] : AnimationConfig{};

        timeline_.animate<T>(
            start_val,
            end_val,
            config,
            [this, setter](const T& val)
            {
                setter(val);
                if (target_)
                    target_->Refresh();
            },
            on_finish);
    }

    void stop_all()
    {
        timeline_.stop_all();
    }
    void finish_all()
    {
        timeline_.finish_all();
    }

private:
    wxWindow* target_;
    AnimationTimeline timeline_;
    std::unordered_map<std::string, AnimationConfig> configs_;
};

} // namespace markamp::ui::animation
