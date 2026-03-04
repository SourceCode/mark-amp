#pragma once

/// @file ImagePreviewToolbar.h
/// @brief V13 Phase 30 — Toolbar for image preview actions (zoom, rotate, copy).

#include "FloatingToolbar.h"

#include <functional>

namespace markamp::ui
{

/// Floating toolbar for image preview operations:
/// Zoom In/Out, Fit, Rotate Left/Right, Copy, Open.
class ImagePreviewToolbar : public FloatingToolbar
{
public:
    ImagePreviewToolbar(wxWindow* parent,
                        core::ThemeEngine& theme_engine,
                        core::EventBus& event_bus);

    /// Show the toolbar positioned near an image preview.
    void ShowForImage(const wxPoint& position);

    using ZoomCallback = std::function<void(float factor)>;
    using RotateCallback = std::function<void(int degrees)>;

    void set_on_zoom(ZoomCallback callback);
    void set_on_rotate(RotateCallback callback);

    [[nodiscard]] auto current_zoom() const -> float;
    [[nodiscard]] auto current_rotation() const -> int;

private:
    ZoomCallback on_zoom_;
    RotateCallback on_rotate_;
    float current_zoom_{1.0F};
    int current_rotation_{0};

    void BuildButtons();
};

} // namespace markamp::ui
