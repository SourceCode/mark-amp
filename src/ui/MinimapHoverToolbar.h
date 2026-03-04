#pragma once

/// @file MinimapHoverToolbar.h
/// @brief V13 Phase 30 — Toolbar that appears when hovering over the minimap.

#include "FloatingToolbar.h"

namespace markamp::ui
{

/// Small floating toolbar for minimap operations:
/// Zoom to Fit, Navigate, Toggle Minimap, Switch Side.
class MinimapHoverToolbar : public FloatingToolbar
{
public:
    MinimapHoverToolbar(wxWindow* parent,
                        core::ThemeEngine& theme_engine,
                        core::EventBus& event_bus);

    /// Show adjacent to the minimap at the given position.
    void ShowNearMinimap(const wxPoint& minimap_pos);

private:
    void BuildButtons();
};

} // namespace markamp::ui
