#include "ThemedDialog.h"

#include "core/ThemeEngine.h"
#include "core/ThemeTokens.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

namespace markamp::ui
{

// ── DialogBackdrop ─────────────────────────────────────────────────

DialogBackdrop::DialogBackdrop(wxWindow* parent, core::ThemeEngine& theme_engine)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
    , fade_timer_(this)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &DialogBackdrop::on_paint, this);
    Bind(wxEVT_LEFT_DOWN, &DialogBackdrop::on_click, this);
    Bind(wxEVT_TIMER, &DialogBackdrop::on_timer, this);
    Hide();
}

void DialogBackdrop::show_with_fade(int /*duration_ms*/)
{
    current_opacity_ = 0.0;
    is_fading_in_ = true;
    is_animating_ = true;
    Show();
    fade_timer_.Start(fade_step_ms_);
}

void DialogBackdrop::hide_with_fade(int /*duration_ms*/)
{
    is_fading_in_ = false;
    is_animating_ = true;
    fade_timer_.Start(fade_step_ms_);
}

void DialogBackdrop::on_paint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC pdc(this);
    const auto rect = GetClientRect();
    const auto alpha = static_cast<int>(current_opacity_ * 255.0);
    pdc.SetBrush(wxBrush(wxColour(0, 0, 0, static_cast<unsigned char>(alpha))));
    pdc.SetPen(*wxTRANSPARENT_PEN);
    pdc.DrawRectangle(rect);
}

void DialogBackdrop::on_click(wxMouseEvent& /*event*/)
{
    // Clicking backdrop doesn't close by default; controlled by dialog owner.
}

void DialogBackdrop::on_timer(wxTimerEvent& /*event*/)
{
    if (is_fading_in_)
    {
        current_opacity_ += fade_rate_;
        if (current_opacity_ >= target_opacity_)
        {
            current_opacity_ = target_opacity_;
            is_animating_ = false;
            fade_timer_.Stop();
        }
    }
    else
    {
        current_opacity_ -= fade_rate_;
        if (current_opacity_ <= 0.0)
        {
            current_opacity_ = 0.0;
            is_animating_ = false;
            fade_timer_.Stop();
            Hide();
        }
    }
    Refresh();
}

// ── ThemedDialog ───────────────────────────────────────────────────

ThemedDialog::ThemedDialog(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           const wxString& title,
                           const wxSize& size)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       size.IsFullySpecified() ? size : wxSize(400, 250),
                       wxBORDER_NONE)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &ThemedDialog::on_paint, this);
    Bind(wxEVT_KEY_DOWN, &ThemedDialog::on_key_down, this);
    SetLabel(title);
}

void ThemedDialog::set_buttons(const std::vector<wxString>& labels)
{
    button_labels_ = labels;
    Refresh();
}

void ThemedDialog::on_button_clicked(int button_index)
{
    result_index_ = button_index;
}

void ThemedDialog::on_key_down(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_ESCAPE && closable_)
    {
        result_index_ = -1;
        Hide();
        return;
    }
    event.Skip();
}

void ThemedDialog::on_paint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
    {
        return;
    }

    const auto rect = GetClientRect();

    // Background
    gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::BgPanel)));
    gc->SetPen(wxPen(theme_engine().color(core::ThemeColorToken::BorderLight), 1));
    gc->DrawRoundedRectangle(0, 0, rect.width, rect.height, FromDIP(8));

    // Title bar
    gc->SetFont(theme_engine().font(core::ThemeFontToken::SansSemiBold),
                theme_engine().color(core::ThemeColorToken::TextMain));
    gc->DrawText(GetLabel(), FromDIP(16), FromDIP(12));
}

// ── ConfirmDialog ──────────────────────────────────────────────────

ConfirmDialog::ConfirmDialog(wxWindow* parent,
                             core::ThemeEngine& theme_engine,
                             const wxString& title,
                             const wxString& message,
                             bool show_dont_ask)
    : ThemedDialog(parent, theme_engine, title, wxSize(400, 200))
    , message_(message)
    , show_dont_ask_(show_dont_ask)
{
    set_buttons({wxT("Cancel"), wxT("OK")});
}

void ConfirmDialog::on_button_clicked(int button_index)
{
    ThemedDialog::on_button_clicked(button_index);
    confirmed_ = (button_index == 1);
}

// ── InputDialog ────────────────────────────────────────────────────

InputDialog::InputDialog(wxWindow* parent,
                         core::ThemeEngine& theme_engine,
                         const wxString& title,
                         const wxString& prompt,
                         const wxString& default_value)
    : ThemedDialog(parent, theme_engine, title, wxSize(400, 200))
    , prompt_(prompt)
    , value_(default_value)
{
    set_buttons({wxT("Cancel"), wxT("OK")});
}

void InputDialog::on_button_clicked(int button_index)
{
    if (button_index == 1 && validator_)
    {
        auto error = validator_(value_.ToStdString());
        if (!error.empty())
        {
            return; // Validation failed.
        }
    }
    ThemedDialog::on_button_clicked(button_index);
}

// ── ProgressDialog ─────────────────────────────────────────────────

ProgressDialog::ProgressDialog(wxWindow* parent,
                               core::ThemeEngine& theme_engine,
                               const wxString& title,
                               bool show_cancel)
    : ThemedDialog(parent, theme_engine, title, wxSize(400, 150))
{
    if (show_cancel)
    {
        set_buttons({wxT("Cancel")});
    }
}

void ProgressDialog::set_progress(double normalized)
{
    progress_ = std::clamp(normalized, 0.0, 1.0);
    Refresh();
}

void ProgressDialog::set_message(const wxString& message)
{
    message_ = message;
    Refresh();
}

void ProgressDialog::set_indeterminate(bool indeterminate)
{
    indeterminate_ = indeterminate;
    Refresh();
}

void ProgressDialog::on_button_clicked(int button_index)
{
    ThemedDialog::on_button_clicked(button_index);
    cancelled_ = (button_index == 0);
}

// ── WizardDialog ───────────────────────────────────────────────────

WizardDialog::WizardDialog(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           const wxString& title,
                           int step_count)
    : ThemedDialog(parent, theme_engine, title, wxSize(500, 400))
    , step_count_(step_count)
    , step_titles_(static_cast<size_t>(step_count))
{
    set_buttons({wxT("Back"), wxT("Next")});
}

void WizardDialog::set_current_step(int step)
{
    if (step >= 0 && step < step_count_)
    {
        current_step_ = step;
        Refresh();
    }
}

void WizardDialog::set_step_title(int step, const wxString& title)
{
    if (step >= 0 && step < step_count_)
    {
        step_titles_[static_cast<size_t>(step)] = title;
    }
}

void WizardDialog::on_button_clicked(int button_index)
{
    if (button_index == 0 && current_step_ > 0)
    {
        --current_step_;
        Refresh();
    }
    else if (button_index == 1)
    {
        if (current_step_ < step_count_ - 1)
        {
            ++current_step_;
            Refresh();
        }
        else
        {
            completed_ = true;
            ThemedDialog::on_button_clicked(button_index);
        }
    }
}

} // namespace markamp::ui
