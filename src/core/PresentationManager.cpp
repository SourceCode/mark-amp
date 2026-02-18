/// @file PresentationManager.cpp
/// @brief V9 Phase 43 — PresentationManager implementation.

#include "PresentationManager.h"

#include <algorithm>

namespace markamp::core
{

auto PresentationManager::create_presentation(const std::string& title, int slide_count)
    -> std::string
{
    Presentation pres;
    pres.presentation_id = "pres_" + std::to_string(next_id_++);
    pres.title = title;
    pres.slide_count = slide_count;
    pres.created_at = std::chrono::system_clock::now();

    auto pres_id = pres.presentation_id;
    presentations_.push_back(std::move(pres));
    return pres_id;
}

auto PresentationManager::close_presentation(const std::string& presentation_id) -> bool
{
    auto iter = std::remove_if(presentations_.begin(),
                               presentations_.end(),
                               [&](const Presentation& pres)
                               { return pres.presentation_id == presentation_id; });
    if (iter == presentations_.end())
    {
        return false;
    }
    presentations_.erase(iter, presentations_.end());
    return true;
}

auto PresentationManager::find_presentation(const std::string& presentation_id) const
    -> const Presentation*
{
    for (const auto& pres : presentations_)
    {
        if (pres.presentation_id == presentation_id)
        {
            return &pres;
        }
    }
    return nullptr;
}

auto PresentationManager::presentation_count() const -> int
{
    return static_cast<int>(presentations_.size());
}

auto PresentationManager::start_presentation(const std::string& presentation_id) -> bool
{
    auto* pres = find_mut(presentation_id);
    if (pres == nullptr || pres->state == PresentationState::kPlaying)
    {
        return false;
    }
    pres->state = PresentationState::kPlaying;
    pres->started_at = std::chrono::system_clock::now();
    return true;
}

auto PresentationManager::pause_presentation(const std::string& presentation_id) -> bool
{
    auto* pres = find_mut(presentation_id);
    if (pres == nullptr || pres->state != PresentationState::kPlaying)
    {
        return false;
    }
    pres->state = PresentationState::kPaused;
    return true;
}

auto PresentationManager::stop_presentation(const std::string& presentation_id) -> bool
{
    auto* pres = find_mut(presentation_id);
    if (pres == nullptr || pres->state == PresentationState::kIdle)
    {
        return false;
    }
    pres->state = PresentationState::kIdle;
    pres->current_slide = 1;
    return true;
}

auto PresentationManager::next_slide(const std::string& presentation_id) -> bool
{
    auto* pres = find_mut(presentation_id);
    if (pres == nullptr || pres->current_slide >= pres->slide_count)
    {
        if (pres != nullptr && pres->loop && pres->current_slide >= pres->slide_count)
        {
            pres->current_slide = 1;
            return true;
        }
        return false;
    }
    pres->current_slide++;
    return true;
}

auto PresentationManager::previous_slide(const std::string& presentation_id) -> bool
{
    auto* pres = find_mut(presentation_id);
    if (pres == nullptr || pres->current_slide <= 1)
    {
        return false;
    }
    pres->current_slide--;
    return true;
}

auto PresentationManager::go_to_slide(const std::string& presentation_id, int slide_num) -> bool
{
    auto* pres = find_mut(presentation_id);
    if (pres == nullptr || slide_num < 1 || slide_num > pres->slide_count)
    {
        return false;
    }
    pres->current_slide = slide_num;
    return true;
}

auto PresentationManager::toggle_presenter_mode(const std::string& presentation_id) -> bool
{
    auto* pres = find_mut(presentation_id);
    if (pres == nullptr)
    {
        return false;
    }
    pres->presenter_mode = !pres->presenter_mode;
    return true;
}

auto PresentationManager::set_loop(const std::string& presentation_id, bool loop_enabled) -> bool
{
    auto* pres = find_mut(presentation_id);
    if (pres == nullptr)
    {
        return false;
    }
    pres->loop = loop_enabled;
    return true;
}

auto PresentationManager::set_auto_advance(const std::string& presentation_id, int seconds) -> bool
{
    auto* pres = find_mut(presentation_id);
    if (pres == nullptr || seconds < 0)
    {
        return false;
    }
    pres->auto_advance_seconds = seconds;
    return true;
}

auto PresentationManager::active_presentations() const -> std::vector<const Presentation*>
{
    std::vector<const Presentation*> result;
    for (const auto& pres : presentations_)
    {
        if (pres.state != PresentationState::kIdle)
        {
            result.push_back(&pres);
        }
    }
    return result;
}

auto PresentationManager::all_presentations() const -> std::vector<const Presentation*>
{
    std::vector<const Presentation*> result;
    result.reserve(presentations_.size());
    for (const auto& pres : presentations_)
    {
        result.push_back(&pres);
    }
    return result;
}

void PresentationManager::close_all()
{
    presentations_.clear();
}

auto PresentationManager::find_mut(const std::string& presentation_id) -> Presentation*
{
    for (auto& pres : presentations_)
    {
        if (pres.presentation_id == presentation_id)
        {
            return &pres;
        }
    }
    return nullptr;
}

} // namespace markamp::core
