// ============================================================================
// File: src/ui/PeekView.cpp
// Phase 47: Peek View System — Peek view container model
// ============================================================================
#include "PeekView.h"

#include <algorithm>

namespace markamp::ui
{

void PeekViewModel::open(core::PeekCommand command,
                         const std::string& source_file,
                         int source_line,
                         int source_column,
                         std::vector<core::PeekLocation> results)
{
    stack_.clear();
    stack_.push_back({.command = command,
                      .source_file = source_file,
                      .source_line = source_line,
                      .source_column = source_column,
                      .results = std::move(results),
                      .selected_index = 0});
    state_ = PeekViewState::Open;
}

void PeekViewModel::close()
{
    state_ = PeekViewState::Closed;
    stack_.clear();
}

void PeekViewModel::push(core::PeekCommand command,
                         const std::string& source_file,
                         int source_line,
                         int source_column,
                         std::vector<core::PeekLocation> results)
{
    if (static_cast<int>(stack_.size()) >= kMaxStackDepth)
    {
        // Remove oldest to make room.
        stack_.erase(stack_.begin());
    }
    stack_.push_back({.command = command,
                      .source_file = source_file,
                      .source_line = source_line,
                      .source_column = source_column,
                      .results = std::move(results),
                      .selected_index = 0});
}

void PeekViewModel::pop()
{
    if (stack_.size() > 1)
    {
        stack_.pop_back();
    }
    else
    {
        close();
    }
}

auto PeekViewModel::current_frame() const -> const PeekFrame*
{
    return stack_.empty() ? nullptr : &stack_.back();
}

void PeekViewModel::set_height(int height)
{
    height_ = std::clamp(height, kMinHeight, kMaxHeight);
}

void PeekViewModel::select_result(int index)
{
    if (!stack_.empty())
    {
        auto& frame = stack_.back();
        if (index >= 0 && index < static_cast<int>(frame.results.size()))
        {
            frame.selected_index = index;
        }
    }
}

void PeekViewModel::next_result()
{
    if (!stack_.empty())
    {
        auto& frame = stack_.back();
        if (!frame.results.empty())
        {
            frame.selected_index =
                (frame.selected_index + 1) % static_cast<int>(frame.results.size());
        }
    }
}

void PeekViewModel::prev_result()
{
    if (!stack_.empty())
    {
        auto& frame = stack_.back();
        if (!frame.results.empty())
        {
            frame.selected_index = frame.selected_index <= 0
                                       ? static_cast<int>(frame.results.size()) - 1
                                       : frame.selected_index - 1;
        }
    }
}

auto PeekViewModel::selected_index() const -> int
{
    auto* frame = current_frame();
    return frame ? frame->selected_index : -1;
}

auto PeekViewModel::result_count() const -> int
{
    auto* frame = current_frame();
    return frame ? static_cast<int>(frame->results.size()) : 0;
}

auto PeekViewModel::selected_location() const -> const core::PeekLocation*
{
    auto* frame = current_frame();
    if (!frame || frame->results.empty())
    {
        return nullptr;
    }
    return &frame->results[static_cast<size_t>(frame->selected_index)];
}

} // namespace markamp::ui
