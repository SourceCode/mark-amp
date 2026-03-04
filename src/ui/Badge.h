#pragma once

#include "ThemeAwareWindow.h"

#include <cstdint>
#include <string>

namespace markamp::ui
{

/// Phase 41 Task 12: Testable model for badge.
class BadgeModel
{
public:
    void set_count(int count);
    [[nodiscard]] auto count() const -> int;

    void set_max_count(int max_count);
    [[nodiscard]] auto max_count() const -> int;

    /// Display string (e.g. "99+" when count exceeds max).
    [[nodiscard]] auto display_text() const -> std::string;

    void set_dot_only(bool dot_only);
    [[nodiscard]] auto is_dot_only() const -> bool;

    [[nodiscard]] auto is_visible() const -> bool;

private:
    int count_{0};
    int max_count_{99};
    bool dot_only_{false};
};

/// Phase 41 Task 12: Themed badge component.
class Badge : public ThemeAwareWindow
{
public:
    Badge(wxWindow* parent, core::ThemeEngine& theme_engine);
    ~Badge() override = default;

    void set_count(int count);
    void set_max_count(int max_count);
    void set_dot_only(bool dot_only);

    [[nodiscard]] auto model() const -> const BadgeModel&;

    static constexpr int kMinWidth = 18;
    static constexpr int kHeight = 18;
    static constexpr int kDotSize = 8;
    static constexpr int kPaddingH = 6;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    BadgeModel model_;

    void OnPaint(wxPaintEvent& event);
};

} // namespace markamp::ui
