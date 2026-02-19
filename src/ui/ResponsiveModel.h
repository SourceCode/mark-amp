#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Platform type.
enum class Platform : uint8_t
{
    kMacOS,
    kWindows,
    kLinux,
};

/// DPI scale factor descriptor.
struct ScaleInfo
{
    double scale_factor{1.0};
    int base_padding{4};
    int base_icon_size{16};
};

/// Testable model for Responsive / DPI / Platform-Adaptive Controls (Phase 36).
///
/// Encapsulates:
/// - DPI-aware metric scaling (padding, icon size, font size)
/// - Compact-width overflow detection
/// - Platform convention adherence
/// - Live DPI change handling
class ResponsiveModel
{
public:
    // ── Scale ───────────────────────────────────────────────────────

    void set_scale(ScaleInfo info);
    [[nodiscard]] auto scale() const -> const ScaleInfo&;

    /// Effective padding = base_padding * scale_factor, rounded.
    [[nodiscard]] auto effective_padding() const -> int;

    /// Effective icon size = base_icon_size * scale_factor, rounded.
    [[nodiscard]] auto effective_icon_size() const -> int;

    // ── Compact width ───────────────────────────────────────────────

    void set_window_width(int pixels);
    [[nodiscard]] auto window_width() const -> int;

    void set_compact_threshold(int pixels);
    [[nodiscard]] auto compact_threshold() const -> int;

    [[nodiscard]] auto is_compact() const -> bool;

    /// Number of toolbar items that overflow at current width.
    void set_toolbar_item_count(int count);
    [[nodiscard]] auto visible_toolbar_items() const -> int;
    [[nodiscard]] auto overflow_count() const -> int;

    // ── Platform ────────────────────────────────────────────────────

    void set_platform(Platform platform);
    [[nodiscard]] auto platform() const -> Platform;

    /// Platform-specific close button position ("left" for macOS, "right" otherwise).
    [[nodiscard]] auto close_button_side() const -> std::string;

    /// Platform-specific modifier key label ("⌘" for macOS, "Ctrl" otherwise).
    [[nodiscard]] auto modifier_label() const -> std::string;

private:
    ScaleInfo scale_{1.0, 4, 16};
    int window_width_{1280};
    int compact_threshold_{600};
    int toolbar_item_count_{10};
    Platform platform_{Platform::kMacOS};
};

} // namespace markamp::ui
