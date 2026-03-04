#pragma once

#include "ControlState.h"
#include "ThemeAwareWindow.h"

#include <cstdint>
#include <functional>
#include <string>

namespace markamp::ui
{

/// Phase 41 Task 07: Tri-state checkbox state.
enum class CheckState : uint8_t
{
    kUnchecked,
    kChecked,
    kIndeterminate,
};

/// Phase 41 Task 07: Testable model for themed checkbox.
class CheckboxModel
{
public:
    void set_state(CheckState state);
    [[nodiscard]] auto state() const -> CheckState;

    /// Toggle through states: unchecked → checked → (indeterminate →) unchecked.
    void toggle(bool allow_indeterminate = false);

    [[nodiscard]] auto is_checked() const -> bool;
    [[nodiscard]] auto is_indeterminate() const -> bool;

    void set_label(const std::string& label);
    [[nodiscard]] auto label() const -> const std::string&;

private:
    CheckState state_{CheckState::kUnchecked};
    std::string label_;
};

/// Phase 41 Task 07: Themed checkbox with indeterminate state.
class ThemedCheckbox : public ThemeAwareWindow
{
public:
    using ChangeCallback = std::function<void(CheckState)>;

    ThemedCheckbox(wxWindow* parent,
                   core::ThemeEngine& theme_engine,
                   const std::string& label = {},
                   bool allow_indeterminate = false);

    ~ThemedCheckbox() override = default;

    void set_state(CheckState state);
    [[nodiscard]] auto check_state() const -> CheckState;

    void set_label(const std::string& label);
    void set_on_change(ChangeCallback callback);
    void set_enabled(bool enabled);

    [[nodiscard]] auto model() const -> const CheckboxModel&;

    static constexpr int kBoxSize = 16;
    static constexpr int kGap = 8;
    static constexpr int kBorderRadius = 3;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    CheckboxModel model_;
    bool allow_indeterminate_{false};
    ControlStateTracker state_;
    ChangeCallback on_change_;

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
