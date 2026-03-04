// ============================================================================
// File: src/ui/InteractiveTutorial.cpp
// Phase 48: Welcome and Onboarding — Interactive tutorial engine model
// ============================================================================
#include "InteractiveTutorial.h"

#include <algorithm>

namespace markamp::ui
{

void InteractiveTutorialEngine::add_tutorial(Tutorial tutorial)
{
    tutorials_.push_back(std::move(tutorial));
}

void InteractiveTutorialEngine::start(const std::string& tutorial_id)
{
    auto* tut = find_tutorial(tutorial_id);
    if (tut && !tut->steps.empty())
    {
        active_tutorial_id_ = tutorial_id;
        current_step_index_ = 0;
        is_active_ = true;
    }
}

auto InteractiveTutorialEngine::current_step() const -> const TutorialStep*
{
    if (!is_active_)
    {
        return nullptr;
    }
    for (const auto& tut : tutorials_)
    {
        if (tut.id == active_tutorial_id_ &&
            current_step_index_ < static_cast<int>(tut.steps.size()))
        {
            return &tut.steps[static_cast<size_t>(current_step_index_)];
        }
    }
    return nullptr;
}

void InteractiveTutorialEngine::advance()
{
    auto* tut = find_tutorial(active_tutorial_id_);
    if (!tut || !is_active_)
    {
        return;
    }

    // Mark current step completed.
    if (current_step_index_ < static_cast<int>(tut->steps.size()))
    {
        tut->steps[static_cast<size_t>(current_step_index_)].is_completed = true;
    }

    ++current_step_index_;
    if (current_step_index_ >= static_cast<int>(tut->steps.size()))
    {
        is_active_ = false;
    }
}

void InteractiveTutorialEngine::skip()
{
    is_active_ = false;
    active_tutorial_id_.clear();
}

auto InteractiveTutorialEngine::completed_tutorial_count() const -> int
{
    int count = 0;
    for (const auto& tut : tutorials_)
    {
        if (tut.is_complete())
        {
            ++count;
        }
    }
    return count;
}

auto InteractiveTutorialEngine::find_tutorial(const std::string& id) -> Tutorial*
{
    for (auto& tut : tutorials_)
    {
        if (tut.id == id)
        {
            return &tut;
        }
    }
    return nullptr;
}

} // namespace markamp::ui
