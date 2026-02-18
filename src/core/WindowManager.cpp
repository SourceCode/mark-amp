/// @file WindowManager.cpp
/// @brief V9 Phase 46 — WindowManager implementation.

#include "WindowManager.h"

#include <algorithm>

namespace markamp::core
{

auto WindowManager::create_window(const std::string& title) -> std::string
{
    WindowInfo win;
    win.window_id = "win_" + std::to_string(next_id_++);
    win.title = title;
    win.created_at = std::chrono::system_clock::now();

    // First window gets focus
    if (windows_.empty())
    {
        win.is_focused = true;
    }

    windows_.push_back(std::move(win));
    return windows_.back().window_id;
}

auto WindowManager::close_window(const std::string& window_id) -> bool
{
    auto iter = std::remove_if(windows_.begin(),
                               windows_.end(),
                               [&](const WindowInfo& win) { return win.window_id == window_id; });
    if (iter == windows_.end())
    {
        return false;
    }
    windows_.erase(iter, windows_.end());

    // If we removed the focused window, focus the last one
    if (!windows_.empty())
    {
        bool has_focus = false;
        for (const auto& win : windows_)
        {
            if (win.is_focused)
            {
                has_focus = true;
                break;
            }
        }
        if (!has_focus)
        {
            windows_.back().is_focused = true;
        }
    }
    return true;
}

auto WindowManager::focus_window(const std::string& window_id) -> bool
{
    auto* target = find_window_mut(window_id);
    if (target == nullptr)
    {
        return false;
    }
    for (auto& win : windows_)
    {
        win.is_focused = false;
    }
    target->is_focused = true;
    return true;
}

auto WindowManager::focused_window() const -> const WindowInfo*
{
    for (const auto& win : windows_)
    {
        if (win.is_focused)
        {
            return &win;
        }
    }
    return nullptr;
}

auto WindowManager::set_window_state(const std::string& window_id, WindowState state) -> bool
{
    auto* win = find_window_mut(window_id);
    if (win == nullptr)
    {
        return false;
    }
    win->state = state;
    return true;
}

auto WindowManager::set_window_bounds(const std::string& window_id, const WindowBounds& bounds)
    -> bool
{
    auto* win = find_window_mut(window_id);
    if (win == nullptr)
    {
        return false;
    }
    win->bounds = bounds;
    return true;
}

auto WindowManager::get_window(const std::string& window_id) const -> const WindowInfo*
{
    for (const auto& win : windows_)
    {
        if (win.window_id == window_id)
        {
            return &win;
        }
    }
    return nullptr;
}

auto WindowManager::all_windows() const -> std::vector<const WindowInfo*>
{
    std::vector<const WindowInfo*> result;
    result.reserve(windows_.size());
    for (const auto& win : windows_)
    {
        result.push_back(&win);
    }
    return result;
}

auto WindowManager::window_count() const -> int
{
    return static_cast<int>(windows_.size());
}

void WindowManager::clear_all()
{
    windows_.clear();
}

auto WindowManager::find_window_mut(const std::string& window_id) -> WindowInfo*
{
    for (auto& win : windows_)
    {
        if (win.window_id == window_id)
        {
            return &win;
        }
    }
    return nullptr;
}

} // namespace markamp::core
