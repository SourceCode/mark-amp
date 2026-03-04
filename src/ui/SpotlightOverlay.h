// ============================================================================
// File: src/ui/SpotlightOverlay.h
// Phase 48: Welcome and Onboarding — Spotlight overlay model
// ============================================================================
#pragma once

#include <string>

namespace markamp::ui
{

/// Position of the callout relative to the spotlight cutout.
enum class CalloutPosition : uint8_t
{
    Above,
    Below,
    Left,
    Right,
    Auto ///< Automatically position based on available space
};

/// Model for the spotlight overlay used during tutorials.
class SpotlightOverlayModel
{
public:
    SpotlightOverlayModel() = default;

    /// Show the spotlight on a target element.
    void show(const std::string& target_element_id,
              int x,
              int y,
              int width,
              int height,
              const std::string& callout_text,
              CalloutPosition position = CalloutPosition::Auto);

    /// Hide the spotlight.
    void hide();

    /// Whether the overlay is visible.
    [[nodiscard]] auto is_visible() const -> bool
    {
        return is_visible_;
    }

    /// Target rect.
    [[nodiscard]] auto target_x() const -> int
    {
        return target_x_;
    }
    [[nodiscard]] auto target_y() const -> int
    {
        return target_y_;
    }
    [[nodiscard]] auto target_width() const -> int
    {
        return target_width_;
    }
    [[nodiscard]] auto target_height() const -> int
    {
        return target_height_;
    }

    /// Callout text.
    [[nodiscard]] auto callout_text() const -> const std::string&
    {
        return callout_text_;
    }

    /// Callout position.
    [[nodiscard]] auto callout_position() const -> CalloutPosition
    {
        return position_;
    }

    /// Dim opacity (0.0 – 1.0).
    void set_dim_opacity(float opacity)
    {
        dim_opacity_ = opacity;
    }
    [[nodiscard]] auto dim_opacity() const -> float
    {
        return dim_opacity_;
    }

private:
    bool is_visible_{false};
    std::string target_element_id_;
    int target_x_{0};
    int target_y_{0};
    int target_width_{0};
    int target_height_{0};
    std::string callout_text_;
    CalloutPosition position_{CalloutPosition::Auto};
    float dim_opacity_{0.6f};
};

} // namespace markamp::ui
