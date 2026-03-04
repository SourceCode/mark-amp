// ============================================================================
// File: src/ui/SpotlightOverlay.cpp
// Phase 48: Welcome and Onboarding — Spotlight overlay model
// ============================================================================
#include "SpotlightOverlay.h"

namespace markamp::ui
{

void SpotlightOverlayModel::show(const std::string& target_element_id,
                                 int x,
                                 int y,
                                 int width,
                                 int height,
                                 const std::string& callout_text,
                                 CalloutPosition position)
{
    target_element_id_ = target_element_id;
    target_x_ = x;
    target_y_ = y;
    target_width_ = width;
    target_height_ = height;
    callout_text_ = callout_text;
    position_ = position;
    is_visible_ = true;
}

void SpotlightOverlayModel::hide()
{
    is_visible_ = false;
}

} // namespace markamp::ui
