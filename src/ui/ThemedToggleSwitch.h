#pragma once

#include "ControlState.h"
#include "ThemeAwareWindow.h"

#include <functional>
#include <string>

namespace markamp::ui
{

/// Phase 41 Task 09: Testable model for toggle switch.
class ToggleSwitchModel
{
public:
    void set_on(bool on);
    [[nodiscard]] auto is_on() const -> bool;
    void toggle();

    void set_label(const std::string& label);
    [[nodiscard]] auto label() const -> const std::string&;

private:
    bool on_{false};
    std::string label_;
};

/// Phase 41 Task 09: Themed toggle switch (animated pill).
class ThemedToggleSwitch : public ThemeAwareWindow
{
public:
    using ChangeCallback = std::function<void(bool on)>;

    ThemedToggleSwitch(wxWindow* parent,
                       core::ThemeEngine& theme_engine,
                       const std::string& label = {});
    ~ThemedToggleSwitch() override = default;

    void set_on(bool on);
    [[nodiscard]] auto is_on() const -> bool;
    void set_label(const std::string& label);
    void set_on_change(ChangeCallback callback);
    void set_enabled(bool enabled);

    [[nodiscard]] auto model() const -> const ToggleSwitchModel&;

    static constexpr int kTrackWidth = 36;
    static constexpr int kTrackHeight = 20;
    static constexpr int kKnobSize = 16;
    static constexpr int kGap = 8;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    ToggleSwitchModel model_;
    ControlStateTracker state_;
    ChangeCallback on_change_;
    float knob_position_{0.0F}; ///< 0.0 = off, 1.0 = on (for animation)

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
};

} // namespace markamp::ui
