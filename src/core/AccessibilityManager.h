#pragma once

#include "Config.h"
#include "EventBus.h"
#include "platform/PlatformAbstraction.h"

#include <algorithm>
#include <cmath>

namespace markamp::core
{

/// Centralized manager for accessibility state: UI scaling, input mode tracking,
/// and platform accessibility queries (high contrast, reduced motion, SR announce).
class AccessibilityManager
{
public:
    AccessibilityManager(EventBus& event_bus,
                         Config& config,
                         platform::PlatformAbstraction& platform);

    // ── Scale factor (0.75 – 2.0) ──

    /// Current UI scale factor.
    [[nodiscard]] auto scale_factor() const -> float
    {
        return scale_factor_;
    }

    /// Set scale factor (clamped to [0.75, 2.0]).
    void set_scale_factor(float factor);

    /// Increase scale by 10%.
    void zoom_in();

    /// Decrease scale by 10%.
    void zoom_out();

    /// Reset scale to 100%.
    void zoom_reset();

    /// Scale a base pixel value by the current factor.
    [[nodiscard]] auto scaled(int base_px) const -> int
    {
        return static_cast<int>(std::round(static_cast<float>(base_px) * scale_factor_));
    }

    // ── Input mode tracking ──

    /// Returns true if the user is navigating with the keyboard.
    [[nodiscard]] auto is_using_keyboard() const -> bool
    {
        return using_keyboard_;
    }

    /// Call when a key event is received (sets keyboard mode).
    void on_key_input();

    /// Call when a mouse event is received (clears keyboard mode).
    void on_mouse_input();

    // ── Platform accessibility state ──

    /// Returns true if the OS is in high contrast mode.
    [[nodiscard]] auto is_high_contrast() const -> bool;

    /// Returns true if the OS prefers reduced motion.
    [[nodiscard]] auto prefers_reduced_motion() const -> bool;

    /// Announce a message to screen readers.
    void announce(wxWindow* window, const wxString& message);

    // ── Config persistence ──

    /// Load scale factor from config.
    void load_from_config();

    /// Save scale factor to config.
    void save_to_config();

    // ── Constants ──

    static constexpr float kMinScale = 0.75F;
    static constexpr float kMaxScale = 2.0F;
    static constexpr float kScaleStep = 0.1F;
    static constexpr float kDefaultScale = 1.0F;

private:
    EventBus& event_bus_;
    Config& config_;
    platform::PlatformAbstraction& platform_;

    float scale_factor_{kDefaultScale};
    bool using_keyboard_{false};
};

} // namespace markamp::core
