#pragma once

#include "ControlState.h"
#include "ThemeAwareWindow.h"

#include <functional>

namespace markamp::ui
{

/// Phase 41 Task 10: Testable model for slider.
class SliderModel
{
public:
    void set_range(double min_val, double max_val);
    [[nodiscard]] auto min_value() const -> double;
    [[nodiscard]] auto max_value() const -> double;

    void set_value(double value);
    [[nodiscard]] auto value() const -> double;

    void set_step(double step);
    [[nodiscard]] auto step() const -> double;

    /// Normalized position [0.0, 1.0].
    [[nodiscard]] auto normalized() const -> double;

    /// Snap to nearest step.
    void snap_to_step();

    /// Increment/decrement by step.
    void increment();
    void decrement();

private:
    double min_{0.0};
    double max_{100.0};
    double value_{0.0};
    double step_{1.0};
};

/// Phase 41 Task 10: Themed slider with value tooltip.
class ThemedSlider : public ThemeAwareWindow
{
public:
    using ChangeCallback = std::function<void(double value)>;

    ThemedSlider(wxWindow* parent,
                 core::ThemeEngine& theme_engine,
                 double min_val = 0.0,
                 double max_val = 100.0);
    ~ThemedSlider() override = default;

    void set_value(double value);
    [[nodiscard]] auto value() const -> double;
    void set_range(double min_val, double max_val);
    void set_step(double step);
    void set_on_change(ChangeCallback callback);
    void set_enabled(bool enabled);

    [[nodiscard]] auto model() const -> const SliderModel&;

    static constexpr int kTrackHeight = 4;
    static constexpr int kThumbSize = 14;
    static constexpr int kHeight = 24;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    SliderModel model_;
    ControlStateTracker state_;
    ChangeCallback on_change_;
    bool is_dragging_{false};

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);

    [[nodiscard]] auto pixel_to_value(int x) const -> double;
    [[nodiscard]] auto value_to_pixel(double val) const -> int;
};

} // namespace markamp::ui
