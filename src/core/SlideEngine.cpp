/// @file SlideEngine.cpp
/// @brief V9 Phase 43 — SlideEngine implementation.

#include "SlideEngine.h"

#include <algorithm>

namespace markamp::core
{

auto SlideEngine::add_slide(Slide slide) -> std::string
{
    if (slide.slide_id.empty())
    {
        slide.slide_id = "slide_" + std::to_string(next_id_++);
    }
    slide.slide_number = static_cast<int>(slides_.size()) + 1;
    auto sid = slide.slide_id;
    slides_.push_back(std::move(slide));
    return sid;
}

auto SlideEngine::remove_slide(const std::string& slide_id) -> bool
{
    auto iter = std::remove_if(slides_.begin(),
                               slides_.end(),
                               [&](const Slide& slide) { return slide.slide_id == slide_id; });
    if (iter == slides_.end())
    {
        return false;
    }
    slides_.erase(iter, slides_.end());
    renumber_slides();
    return true;
}

auto SlideEngine::find_slide(const std::string& slide_id) const -> const Slide*
{
    for (const auto& slide : slides_)
    {
        if (slide.slide_id == slide_id)
        {
            return &slide;
        }
    }
    return nullptr;
}

auto SlideEngine::slide_count() const -> int
{
    return static_cast<int>(slides_.size());
}

auto SlideEngine::move_slide(const std::string& slide_id, int new_position) -> bool
{
    int old_idx = -1;
    for (int idx = 0; idx < static_cast<int>(slides_.size()); ++idx)
    {
        if (slides_[static_cast<size_t>(idx)].slide_id == slide_id)
        {
            old_idx = idx;
            break;
        }
    }
    if (old_idx < 0 || new_position < 0 || new_position >= static_cast<int>(slides_.size()))
    {
        return false;
    }
    auto slide = std::move(slides_[static_cast<size_t>(old_idx)]);
    slides_.erase(slides_.begin() + old_idx);
    slides_.insert(slides_.begin() + new_position, std::move(slide));
    renumber_slides();
    return true;
}

auto SlideEngine::swap_slides(const std::string& slide_id_a, const std::string& slide_id_b) -> bool
{
    Slide* slide_a = find_mut(slide_id_a);
    Slide* slide_b = find_mut(slide_id_b);
    if (slide_a == nullptr || slide_b == nullptr)
    {
        return false;
    }
    std::swap(slide_a->slide_number, slide_b->slide_number);
    std::swap(*slide_a, *slide_b);
    return true;
}

auto SlideEngine::ordered_slides() const -> std::vector<const Slide*>
{
    std::vector<const Slide*> result;
    result.reserve(slides_.size());
    for (const auto& slide : slides_)
    {
        result.push_back(&slide);
    }
    return result;
}

auto SlideEngine::add_block(const std::string& slide_id, SlideBlock block) -> bool
{
    auto* slide = find_mut(slide_id);
    if (slide == nullptr)
    {
        return false;
    }
    if (block.block_id.empty())
    {
        block.block_id = "block_" + std::to_string(slide->blocks.size());
    }
    block.sort_order = static_cast<int>(slide->blocks.size());
    slide->blocks.push_back(std::move(block));
    return true;
}

auto SlideEngine::remove_block(const std::string& slide_id, const std::string& block_id) -> bool
{
    auto* slide = find_mut(slide_id);
    if (slide == nullptr)
    {
        return false;
    }
    auto iter = std::remove_if(slide->blocks.begin(),
                               slide->blocks.end(),
                               [&](const SlideBlock& blk) { return blk.block_id == block_id; });
    if (iter == slide->blocks.end())
    {
        return false;
    }
    slide->blocks.erase(iter, slide->blocks.end());
    return true;
}

auto SlideEngine::block_count(const std::string& slide_id) const -> int
{
    const auto* slide = find_slide(slide_id);
    if (slide == nullptr)
    {
        return 0;
    }
    return static_cast<int>(slide->blocks.size());
}

auto SlideEngine::set_transition(const std::string& slide_id, SlideTransition transition) -> bool
{
    auto* slide = find_mut(slide_id);
    if (slide == nullptr)
    {
        return false;
    }
    slide->transition = transition;
    return true;
}

auto SlideEngine::set_all_transitions(SlideTransition transition) -> int
{
    int count = 0;
    for (auto& slide : slides_)
    {
        slide.transition = transition;
        ++count;
    }
    return count;
}

auto SlideEngine::hide_slide(const std::string& slide_id) -> bool
{
    auto* slide = find_mut(slide_id);
    if (slide == nullptr)
    {
        return false;
    }
    slide->hidden = true;
    return true;
}

auto SlideEngine::show_slide(const std::string& slide_id) -> bool
{
    auto* slide = find_mut(slide_id);
    if (slide == nullptr)
    {
        return false;
    }
    slide->hidden = false;
    return true;
}

auto SlideEngine::visible_slides() const -> std::vector<const Slide*>
{
    std::vector<const Slide*> result;
    for (const auto& slide : slides_)
    {
        if (!slide.hidden)
        {
            result.push_back(&slide);
        }
    }
    return result;
}

auto SlideEngine::set_speaker_notes(const std::string& slide_id, const std::string& notes) -> bool
{
    auto* slide = find_mut(slide_id);
    if (slide == nullptr)
    {
        return false;
    }
    slide->speaker_notes = notes;
    return true;
}

void SlideEngine::clear_all()
{
    slides_.clear();
}

auto SlideEngine::find_mut(const std::string& slide_id) -> Slide*
{
    for (auto& slide : slides_)
    {
        if (slide.slide_id == slide_id)
        {
            return &slide;
        }
    }
    return nullptr;
}

void SlideEngine::renumber_slides()
{
    for (int idx = 0; idx < static_cast<int>(slides_.size()); ++idx)
    {
        slides_[static_cast<size_t>(idx)].slide_number = idx + 1;
    }
}

} // namespace markamp::core
