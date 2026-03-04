#include "ThemedCheckbox.h"

#include <wx/dcclient.h>
#include <wx/graphics.h>

namespace markamp::ui
{

// ── CheckboxModel ──────────────────────────────────────────────────

void CheckboxModel::set_state(CheckState state)
{
    state_ = state;
}
auto CheckboxModel::state() const -> CheckState
{
    return state_;
}

void CheckboxModel::toggle(bool allow_indeterminate)
{
    switch (state_)
    {
        case CheckState::kUnchecked:
            state_ = CheckState::kChecked;
            break;
        case CheckState::kChecked:
            state_ = allow_indeterminate ? CheckState::kIndeterminate : CheckState::kUnchecked;
            break;
        case CheckState::kIndeterminate:
            state_ = CheckState::kUnchecked;
            break;
    }
}

auto CheckboxModel::is_checked() const -> bool
{
    return state_ == CheckState::kChecked;
}
auto CheckboxModel::is_indeterminate() const -> bool
{
    return state_ == CheckState::kIndeterminate;
}

void CheckboxModel::set_label(const std::string& label)
{
    label_ = label;
}
auto CheckboxModel::label() const -> const std::string&
{
    return label_;
}

// ── ThemedCheckbox ─────────────────────────────────────────────────

ThemedCheckbox::ThemedCheckbox(wxWindow* parent,
                               core::ThemeEngine& theme_engine,
                               const std::string& label,
                               bool allow_indeterminate)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxTAB_TRAVERSAL | wxWANTS_CHARS)
    , allow_indeterminate_(allow_indeterminate)
{
    model_.set_label(label);
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &ThemedCheckbox::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &ThemedCheckbox::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &ThemedCheckbox::OnMouseUp, this);
    Bind(wxEVT_ENTER_WINDOW, &ThemedCheckbox::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &ThemedCheckbox::OnMouseLeave, this);
    Bind(wxEVT_KEY_DOWN, &ThemedCheckbox::OnKeyDown, this);
    Bind(wxEVT_SET_FOCUS, &ThemedCheckbox::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &ThemedCheckbox::OnKillFocus, this);

    SetMinSize(wxSize(FromDIP(kBoxSize + kGap) + 80, FromDIP(kBoxSize + 4)));
}

void ThemedCheckbox::set_state(CheckState state)
{
    model_.set_state(state);
    Refresh();
}
auto ThemedCheckbox::check_state() const -> CheckState
{
    return model_.state();
}

void ThemedCheckbox::set_label(const std::string& label)
{
    model_.set_label(label);
    Refresh();
}

void ThemedCheckbox::set_on_change(ChangeCallback callback)
{
    on_change_ = std::move(callback);
}

void ThemedCheckbox::set_enabled(bool enabled)
{
    state_.set_disabled(!enabled);
    Refresh();
}

auto ThemedCheckbox::model() const -> const CheckboxModel&
{
    return model_;
}

void ThemedCheckbox::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void ThemedCheckbox::OnPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
        return;

    const auto sz = GetClientSize();
    const int box = FromDIP(kBoxSize);
    const int gap = FromDIP(kGap);
    const int box_y = (sz.y - box) / 2;
    const double r = FromDIP(kBorderRadius);

    // Box background
    auto box_bg = model_.is_checked() || model_.is_indeterminate()
                      ? core::ThemeColorToken::AccentPrimary
                      : core::ThemeColorToken::BgInput;
    if (state_.is_disabled())
        box_bg = core::ThemeColorToken::ControlBgDisabled;

    gc->SetBrush(wxBrush(theme_engine().color(box_bg)));
    gc->SetPen(*wxTRANSPARENT_PEN);
    auto path = gc->CreatePath();
    path.AddRoundedRectangle(0, box_y, box, box, r);
    gc->FillPath(path);

    // Box border
    auto border = state_.is_focused() ? core::ThemeColorToken::FocusRingColor
                                      : core::ThemeColorToken::BorderLight;
    gc->SetPen(wxPen(theme_engine().color(border), 1));
    gc->SetBrush(*wxTRANSPARENT_BRUSH);
    auto bp = gc->CreatePath();
    bp.AddRoundedRectangle(0.5, box_y + 0.5, box - 1, box - 1, r);
    gc->StrokePath(bp);

    // Check mark / indeterminate line
    const auto mark_color = theme_engine().color(core::ThemeColorToken::AccentSecondary);
    gc->SetPen(wxPen(mark_color, FromDIP(2)));

    if (model_.is_checked())
    {
        gc->StrokeLine(box * 0.2, box_y + box * 0.5, box * 0.4, box_y + box * 0.75);
        gc->StrokeLine(box * 0.4, box_y + box * 0.75, box * 0.8, box_y + box * 0.25);
    }
    else if (model_.is_indeterminate())
    {
        gc->StrokeLine(box * 0.25, box_y + box * 0.5, box * 0.75, box_y + box * 0.5);
    }

    // Label
    auto fg = state_.is_disabled() ? core::ThemeColorToken::ControlFgDisabled
                                   : core::ThemeColorToken::TextMain;
    gc->SetFont(theme_engine().font(core::ThemeFontToken::UILabel), theme_engine().color(fg));
    double tw = 0, th = 0;
    const auto label_str = wxString::FromUTF8(model_.label());
    gc->GetTextExtent(label_str, &tw, &th);
    gc->DrawText(label_str, box + gap, (sz.y - th) / 2.0);

    state_.acknowledge_change();
}

void ThemedCheckbox::OnMouseDown(wxMouseEvent& /*event*/)
{
    if (!state_.is_disabled())
    {
        state_.on_mouse_down();
        CaptureMouse();
        Refresh();
    }
}

void ThemedCheckbox::OnMouseUp(wxMouseEvent& event)
{
    if (HasCapture())
        ReleaseMouse();
    const bool inside = GetClientRect().Contains(event.GetPosition());
    state_.on_mouse_up(inside);
    if (inside && !state_.is_disabled())
    {
        model_.toggle(allow_indeterminate_);
        if (on_change_)
            on_change_(model_.state());
    }
    Refresh();
}

void ThemedCheckbox::OnMouseEnter(wxMouseEvent& /*event*/)
{
    if (!state_.is_disabled())
    {
        state_.on_mouse_enter();
        SetControlCursor(ControlCursorType::kHand);
        Refresh();
    }
}

void ThemedCheckbox::OnMouseLeave(wxMouseEvent& /*event*/)
{
    state_.on_mouse_leave();
    Refresh();
}

void ThemedCheckbox::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_SPACE && !state_.is_disabled())
    {
        model_.toggle(allow_indeterminate_);
        if (on_change_)
            on_change_(model_.state());
        Refresh();
    }
    else
    {
        event.Skip();
    }
}

void ThemedCheckbox::OnSetFocus(wxFocusEvent& /*event*/)
{
    state_.on_focus();
    Refresh();
}
void ThemedCheckbox::OnKillFocus(wxFocusEvent& /*event*/)
{
    state_.on_blur();
    Refresh();
}

} // namespace markamp::ui
