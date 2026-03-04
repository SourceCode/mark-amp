#pragma once

#include "ThemeAwareWindow.h"

#include <wx/timer.h>

namespace markamp::ui
{

/// Phase 44: Semi-transparent backdrop overlay behind modal dialogs.
class DialogBackdrop : public ThemeAwareWindow
{
public:
    DialogBackdrop(wxWindow* parent, core::ThemeEngine& theme_engine);

    void show_with_fade(int duration_ms = 200);
    void hide_with_fade(int duration_ms = 200);

    void set_opacity_target(double opacity)
    {
        target_opacity_ = opacity;
    }

    [[nodiscard]] auto is_animating() const -> bool
    {
        return is_animating_;
    }

private:
    void on_paint(wxPaintEvent& event);
    void on_click(wxMouseEvent& event);
    void on_timer(wxTimerEvent& event);

    double current_opacity_{0.0};
    double target_opacity_{0.5};
    bool is_animating_{false};
    bool is_fading_in_{false};
    wxTimer fade_timer_;
    int fade_step_ms_{16};
    double fade_rate_{0.05};
};

/// Phase 44: Base themed dialog with chrome, button bar, focus trapping.
class ThemedDialog : public ThemeAwareWindow
{
public:
    ThemedDialog(wxWindow* parent,
                 core::ThemeEngine& theme_engine,
                 const wxString& title,
                 const wxSize& size = wxDefaultSize);

    void set_buttons(const std::vector<wxString>& labels);
    void set_closable(bool closable)
    {
        closable_ = closable;
    }

    [[nodiscard]] auto result_index() const -> int
    {
        return result_index_;
    }

protected:
    virtual void on_button_clicked(int button_index);
    void on_key_down(wxKeyEvent& event);

private:
    void on_paint(wxPaintEvent& event);

    bool closable_{true};
    int result_index_{-1};
    std::vector<wxString> button_labels_;
};

/// Phase 44: Confirmation dialog with "Don't ask again" option.
class ConfirmDialog : public ThemedDialog
{
public:
    ConfirmDialog(wxWindow* parent,
                  core::ThemeEngine& theme_engine,
                  const wxString& title,
                  const wxString& message,
                  bool show_dont_ask = false);

    [[nodiscard]] auto dont_ask_again() const -> bool
    {
        return dont_ask_;
    }
    [[nodiscard]] auto was_confirmed() const -> bool
    {
        return confirmed_;
    }

protected:
    void on_button_clicked(int button_index) override;

private:
    wxString message_;
    [[maybe_unused]] bool show_dont_ask_{false};
    bool dont_ask_{false};
    bool confirmed_{false};
};

/// Phase 44: Input dialog with validation.
class InputDialog : public ThemedDialog
{
public:
    InputDialog(wxWindow* parent,
                core::ThemeEngine& theme_engine,
                const wxString& title,
                const wxString& prompt,
                const wxString& default_value = {});

    using ValidatorFn = std::function<std::string(const std::string&)>;
    void set_validator(ValidatorFn validator)
    {
        validator_ = std::move(validator);
    }

    [[nodiscard]] auto value() const -> wxString
    {
        return value_;
    }

protected:
    void on_button_clicked(int button_index) override;

private:
    wxString prompt_;
    wxString value_;
    ValidatorFn validator_;
};

/// Phase 44: Progress dialog with cancel button.
class ProgressDialog : public ThemedDialog
{
public:
    ProgressDialog(wxWindow* parent,
                   core::ThemeEngine& theme_engine,
                   const wxString& title,
                   bool show_cancel = false);

    void set_progress(double normalized); ///< 0.0–1.0
    void set_message(const wxString& message);
    void set_indeterminate(bool indeterminate);

    [[nodiscard]] auto was_cancelled() const -> bool
    {
        return cancelled_;
    }

protected:
    void on_button_clicked(int button_index) override;

private:
    double progress_{0.0};
    wxString message_;
    bool indeterminate_{false};
    bool cancelled_{false};
};

/// Phase 44: Multi-step wizard dialog.
class WizardDialog : public ThemedDialog
{
public:
    WizardDialog(wxWindow* parent,
                 core::ThemeEngine& theme_engine,
                 const wxString& title,
                 int step_count);

    void set_current_step(int step);
    void set_step_title(int step, const wxString& title);

    [[nodiscard]] auto current_step() const -> int
    {
        return current_step_;
    }
    [[nodiscard]] auto was_completed() const -> bool
    {
        return completed_;
    }

protected:
    void on_button_clicked(int button_index) override;

private:
    int step_count_{0};
    int current_step_{0};
    bool completed_{false};
    std::vector<wxString> step_titles_;
};

} // namespace markamp::ui
