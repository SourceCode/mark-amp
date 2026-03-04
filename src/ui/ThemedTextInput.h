#pragma once

#include "ComponentVariants.h"
#include "ControlState.h"
#include "ThemeAwareWindow.h"

#include <functional>
#include <string>

namespace markamp::ui
{

/// Phase 41 Task 05: Validation state for text input fields.
enum class TextInputValidation : uint8_t
{
    kNone,    ///< No validation state
    kValid,   ///< Input passes validation
    kWarning, ///< Input has warnings
    kError,   ///< Input fails validation
};

/// Phase 41 Task 05: Testable model for themed text input.
class TextInputModel
{
public:
    void set_value(const std::string& value);
    [[nodiscard]] auto value() const -> const std::string&;

    void set_placeholder(const std::string& placeholder);
    [[nodiscard]] auto placeholder() const -> const std::string&;

    void set_validation(TextInputValidation state, const std::string& message = {});
    [[nodiscard]] auto validation_state() const -> TextInputValidation;
    [[nodiscard]] auto validation_message() const -> const std::string&;

    void set_max_length(int max_length);
    [[nodiscard]] auto max_length() const -> int;

    [[nodiscard]] auto is_empty() const -> bool;
    [[nodiscard]] auto char_count() const -> int;

private:
    std::string value_;
    std::string placeholder_;
    TextInputValidation validation_{TextInputValidation::kNone};
    std::string validation_message_;
    int max_length_{0}; ///< 0 = unlimited
};

/// Phase 41 Task 05: Themed text input with icon slots and inline validation.
class ThemedTextInput : public ThemeAwareWindow
{
public:
    using ChangeCallback = std::function<void(const std::string&)>;

    ThemedTextInput(wxWindow* parent,
                    core::ThemeEngine& theme_engine,
                    const std::string& placeholder = {});

    ~ThemedTextInput() override = default;

    void set_value(const std::string& value);
    [[nodiscard]] auto value() const -> const std::string&;

    void set_placeholder(const std::string& placeholder);
    void set_validation(TextInputValidation state, const std::string& message = {});

    void set_leading_icon(const std::string& icon_name);
    void set_trailing_icon(const std::string& icon_name);

    void set_on_change(ChangeCallback callback);
    void set_enabled(bool enabled);

    [[nodiscard]] auto model() const -> const TextInputModel&;

    static constexpr int kHeight = 28;
    static constexpr int kPaddingH = 8;
    static constexpr int kBorderRadius = 4;
    static constexpr int kIconSize = 16;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    TextInputModel model_;
    std::string leading_icon_;
    std::string trailing_icon_;
    ControlStateTracker state_;
    ChangeCallback on_change_;
    int cursor_pos_{0};

    bool is_editing_{false};

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnKeyDown(wxKeyEvent& event);
};

} // namespace markamp::ui
