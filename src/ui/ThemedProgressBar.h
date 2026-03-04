#pragma once

#include "ControlState.h"
#include "ThemeAwareWindow.h"

#include <cstdint>
#include <string>

namespace markamp::ui
{

/// Phase 41 Task 11: Progress bar style.
enum class ProgressBarStyle : uint8_t
{
    kLinear,       ///< Horizontal bar
    kCircular,     ///< Circular spinner
    kIndeterminate ///< Animated indeterminate
};

/// Phase 41 Task 11: Testable model for progress bar.
class ProgressBarModel
{
public:
    void set_value(double value);
    [[nodiscard]] auto value() const -> double;

    void set_max(double max);
    [[nodiscard]] auto max() const -> double;

    [[nodiscard]] auto normalized() const -> double;
    [[nodiscard]] auto percentage() const -> int;

    void set_style(ProgressBarStyle style);
    [[nodiscard]] auto style() const -> ProgressBarStyle;

    void set_label(const std::string& label);
    [[nodiscard]] auto label() const -> const std::string&;

    [[nodiscard]] auto is_complete() const -> bool;

private:
    double value_{0.0};
    double max_{100.0};
    ProgressBarStyle style_{ProgressBarStyle::kLinear};
    std::string label_;
};

/// Phase 41 Task 11: Themed progress bar (linear + circular).
class ThemedProgressBar : public ThemeAwareWindow
{
public:
    ThemedProgressBar(wxWindow* parent,
                      core::ThemeEngine& theme_engine,
                      ProgressBarStyle style = ProgressBarStyle::kLinear);
    ~ThemedProgressBar() override = default;

    void set_value(double value);
    void set_max(double max);
    void set_style(ProgressBarStyle style);
    void set_label(const std::string& label);

    [[nodiscard]] auto model() const -> const ProgressBarModel&;

    static constexpr int kLinearHeight = 4;
    static constexpr int kCircularSize = 24;
    static constexpr int kBorderRadius = 2;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    ProgressBarModel model_;

    void OnPaint(wxPaintEvent& event);
};

} // namespace markamp::ui
