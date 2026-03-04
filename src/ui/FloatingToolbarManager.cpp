#include "FloatingToolbarManager.h"

#include <algorithm>

namespace markamp::ui
{

FloatingToolbarManager::FloatingToolbarManager(wxWindow* parent,
                                               core::ThemeEngine& theme_engine,
                                               core::EventBus& event_bus)
    : parent_(parent)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
{
}

void FloatingToolbarManager::RegisterToolbar(const std::string& toolbar_id,
                                             std::unique_ptr<FloatingToolbar> toolbar)
{
    toolbars_[toolbar_id] = std::move(toolbar);
}

void FloatingToolbarManager::ShowToolbar(const std::string& toolbar_id, const wxPoint& position)
{
    auto iter = toolbars_.find(toolbar_id);
    if (iter == toolbars_.end())
    {
        return;
    }

    // Use saved position if available and no explicit position provided
    wxPoint show_pos = position;
    auto saved_iter = saved_positions_.find(toolbar_id);
    if (saved_iter != saved_positions_.end() && position == wxPoint(-1, -1))
    {
        show_pos = saved_iter->second;
    }

    iter->second->ShowAt(show_pos);
    BringToFront(toolbar_id);
}

void FloatingToolbarManager::HideToolbar(const std::string& toolbar_id)
{
    auto iter = toolbars_.find(toolbar_id);
    if (iter == toolbars_.end())
    {
        return;
    }

    if (iter->second->is_visible())
    {
        saved_positions_[toolbar_id] = iter->second->last_position();
        iter->second->HideToolbar();
    }
}

void FloatingToolbarManager::HideAll()
{
    for (auto& [toolbar_id, toolbar] : toolbars_)
    {
        if (toolbar->is_visible())
        {
            saved_positions_[toolbar_id] = toolbar->last_position();
            toolbar->HideToolbar();
        }
    }
}

auto FloatingToolbarManager::GetToolbar(const std::string& toolbar_id) -> FloatingToolbar*
{
    auto iter = toolbars_.find(toolbar_id);
    if (iter == toolbars_.end())
    {
        return nullptr;
    }
    return iter->second.get();
}

auto FloatingToolbarManager::IsVisible(const std::string& toolbar_id) const -> bool
{
    auto iter = toolbars_.find(toolbar_id);
    if (iter == toolbars_.end())
    {
        return false;
    }
    return iter->second->is_visible();
}

auto FloatingToolbarManager::toolbar_count() const -> int
{
    return static_cast<int>(toolbars_.size());
}

auto FloatingToolbarManager::visible_count() const -> int
{
    int count = 0;
    for (const auto& [toolbar_id, toolbar] : toolbars_)
    {
        if (toolbar->is_visible())
        {
            ++count;
        }
    }
    return count;
}

void FloatingToolbarManager::BringToFront(const std::string& toolbar_id)
{
    auto iter = std::find(z_order_.begin(), z_order_.end(), toolbar_id);
    if (iter != z_order_.end())
    {
        z_order_.erase(iter);
    }
    z_order_.insert(z_order_.begin(), toolbar_id);

    // Reorder windows — raise from back to front so front ends on top
    for (auto rit = z_order_.rbegin(); rit != z_order_.rend(); ++rit)
    {
        auto tb_iter = toolbars_.find(*rit);
        if (tb_iter != toolbars_.end() && tb_iter->second->is_visible())
        {
            tb_iter->second->Raise();
        }
    }
}

void FloatingToolbarManager::SavePositions()
{
    for (const auto& [toolbar_id, toolbar] : toolbars_)
    {
        if (toolbar->is_visible())
        {
            saved_positions_[toolbar_id] = toolbar->last_position();
        }
    }
}

void FloatingToolbarManager::RestorePositions()
{
    // Positions are automatically restored via ShowToolbar's saved_positions_ lookup
}

auto FloatingToolbarManager::z_order() const -> const std::vector<std::string>&
{
    return z_order_;
}

} // namespace markamp::ui
