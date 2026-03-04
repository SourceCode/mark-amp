#pragma once

#include "ControlState.h"
#include "ThemeAwareWindow.h"

#include <functional>

namespace markamp::ui
{

/// Phase 41 Task 17: Testable model for number stepper.
class NumberStepperModel
{
public:
    void set_value(double value);
    [[nodiscard]] auto value() const -> double;

    void set_range(double min_val, double max_val);
    [[nodiscard]] auto min_value() const -> double;
    [[nodiscard]] auto max_value() const -> double;

    void set_step(double step);
    [[nodiscard]] auto step() const -> double;

    void increment();
    void decrement();

    [[nodiscard]] auto can_increment() const -> bool;
    [[nodiscard]] auto can_decrement() const -> bool;

    /// Display string for current value.
    [[nodiscard]] auto display_text() const -> std::string;

    void set_precision(int decimal_places);
    [[nodiscard]] auto precision() const -> int;

private:
    double value_{0.0};
    double min_{0.0};
    double max_{100.0};
    double step_{1.0};
    int precision_{0}; ///< Decimal places for display
};

/// Phase 41 Task 17: Themed number input with stepper buttons.
class NumberStepper : public ThemeAwareWindow
{
public:
    using ChangeCallback = std::function<void(double value)>;

    NumberStepper(wxWindow* parent,
                  core::ThemeEngine& theme_engine,
                  double min_val = 0.0,
                  double max_val = 100.0,
                  double step = 1.0);
    ~NumberStepper() override = default;

    void set_value(double value);
    [[nodiscard]] auto value() const -> double;
    void set_range(double min_val, double max_val);
    void set_step(double step);
    void set_precision(int decimal_places);
    void set_on_change(ChangeCallback callback);
    void set_enabled(bool enabled);

    [[nodiscard]] auto model() const -> const NumberStepperModel&;

    static constexpr int kHeight = 28;
    static constexpr int kButtonWidth = 24;
    static constexpr int kPaddingH = 8;
    static constexpr int kBorderRadius = 4;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    NumberStepperModel model_;
    ControlStateTracker state_;
    ChangeCallback on_change_;

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
};

} // namespace markamp::ui
