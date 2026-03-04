#include "ThemedTextInput.h"

#include <wx/dcclient.h>
#include <wx/graphics.h>

namespace markamp::ui
{

// ── TextInputModel ─────────────────────────────────────────────────

void TextInputModel::set_value(const std::string& value)
{
    if (max_length_ > 0 && static_cast<int>(value.size()) > max_length_)
    {
        value_ = value.substr(0, static_cast<size_t>(max_length_));
    }
    else
    {
        value_ = value;
    }
}

auto TextInputModel::value() const -> const std::string&
{
    return value_;
}

void TextInputModel::set_placeholder(const std::string& placeholder)
{
    placeholder_ = placeholder;
}

auto TextInputModel::placeholder() const -> const std::string&
{
    return placeholder_;
}

void TextInputModel::set_validation(TextInputValidation state, const std::string& message)
{
    validation_ = state;
    validation_message_ = message;
}

auto TextInputModel::validation_state() const -> TextInputValidation
{
    return validation_;
}

auto TextInputModel::validation_message() const -> const std::string&
{
    return validation_message_;
}

void TextInputModel::set_max_length(int max_length)
{
    max_length_ = max_length;
}

auto TextInputModel::max_length() const -> int
{
    return max_length_;
}

auto TextInputModel::is_empty() const -> bool
{
    return value_.empty();
}

auto TextInputModel::char_count() const -> int
{
    return static_cast<int>(value_.size());
}

// ── ThemedTextInput ────────────────────────────────────────────────

ThemedTextInput::ThemedTextInput(wxWindow* parent,
                                 core::ThemeEngine& theme_engine,
                                 const std::string& placeholder)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxTAB_TRAVERSAL | wxWANTS_CHARS)
{
    model_.set_placeholder(placeholder);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(-1, FromDIP(kHeight)));

    Bind(wxEVT_PAINT, &ThemedTextInput::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &ThemedTextInput::OnMouseDown, this);
    Bind(wxEVT_ENTER_WINDOW, &ThemedTextInput::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &ThemedTextInput::OnMouseLeave, this);
    Bind(wxEVT_SET_FOCUS, &ThemedTextInput::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &ThemedTextInput::OnKillFocus, this);
    Bind(wxEVT_CHAR, &ThemedTextInput::OnChar, this);
    Bind(wxEVT_KEY_DOWN, &ThemedTextInput::OnKeyDown, this);
}

void ThemedTextInput::set_value(const std::string& value)
{
    model_.set_value(value);
    cursor_pos_ = static_cast<int>(model_.value().size());
    Refresh();
}

auto ThemedTextInput::value() const -> const std::string&
{
    return model_.value();
}

void ThemedTextInput::set_placeholder(const std::string& placeholder)
{
    model_.set_placeholder(placeholder);
    Refresh();
}

void ThemedTextInput::set_validation(TextInputValidation state, const std::string& message)
{
    model_.set_validation(state, message);
    Refresh();
}

void ThemedTextInput::set_leading_icon(const std::string& icon_name)
{
    leading_icon_ = icon_name;
    Refresh();
}

void ThemedTextInput::set_trailing_icon(const std::string& icon_name)
{
    trailing_icon_ = icon_name;
    Refresh();
}

void ThemedTextInput::set_on_change(ChangeCallback callback)
{
    on_change_ = std::move(callback);
}

void ThemedTextInput::set_enabled(bool enabled)
{
    state_.set_disabled(!enabled);
    Refresh();
}

auto ThemedTextInput::model() const -> const TextInputModel&
{
    return model_;
}

void ThemedTextInput::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void ThemedTextInput::OnPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
        return;

    const auto sz = GetClientSize();
    const double r = FromDIP(kBorderRadius);
    const int padH = FromDIP(kPaddingH);

    // Background
    auto bg_token =
        state_.is_focused() ? core::ThemeColorToken::BgInput : core::ThemeColorToken::BgPanel;
    gc->SetBrush(wxBrush(theme_engine().color(bg_token)));
    gc->SetPen(*wxTRANSPARENT_PEN);
    auto path = gc->CreatePath();
    path.AddRoundedRectangle(0, 0, sz.x, sz.y, r);
    gc->FillPath(path);

    // Border
    auto border_token = core::ThemeColorToken::BorderLight;
    if (model_.validation_state() == TextInputValidation::kError)
        border_token = core::ThemeColorToken::ErrorColor;
    else if (model_.validation_state() == TextInputValidation::kWarning)
        border_token = core::ThemeColorToken::SuccessColor;
    else if (state_.is_focused())
        border_token = core::ThemeColorToken::FocusRingColor;

    gc->SetPen(wxPen(theme_engine().color(border_token), 1));
    gc->SetBrush(*wxTRANSPARENT_BRUSH);
    auto border_path = gc->CreatePath();
    border_path.AddRoundedRectangle(0.5, 0.5, sz.x - 1, sz.y - 1, r);
    gc->StrokePath(border_path);

    // Text
    const auto& text = model_.value();
    const bool show_placeholder = text.empty() && !state_.is_focused();

    auto fg_token =
        show_placeholder ? core::ThemeColorToken::TextMuted : core::ThemeColorToken::TextMain;
    if (state_.is_disabled())
        fg_token = core::ThemeColorToken::ControlFgDisabled;

    gc->SetFont(theme_engine().font(core::ThemeFontToken::UILabel),
                theme_engine().color(fg_token));

    const auto& display_text = show_placeholder ? model_.placeholder() : text;
    double tw = 0, th = 0;
    gc->GetTextExtent(wxString::FromUTF8(display_text), &tw, &th);

    int text_x = padH;
    if (!leading_icon_.empty())
        text_x += FromDIP(kIconSize + 4);

    const double ty = (sz.y - th) / 2.0;
    gc->DrawText(wxString::FromUTF8(display_text), text_x, ty);

    // Cursor
    if (state_.is_focused() && is_editing_)
    {
        const auto cursor_text = text.substr(0, static_cast<size_t>(cursor_pos_));
        double cw = 0, ch = 0;
        gc->GetTextExtent(wxString::FromUTF8(cursor_text), &cw, &ch);
        gc->SetPen(wxPen(theme_engine().color(core::ThemeColorToken::TextMain), 1));
        gc->StrokeLine(text_x + cw, ty, text_x + cw, ty + th);
    }

    // Validation message
    if (model_.validation_state() != TextInputValidation::kNone &&
        !model_.validation_message().empty())
    {
        auto msg_token = model_.validation_state() == TextInputValidation::kError
                             ? core::ThemeColorToken::ErrorColor
                             : core::ThemeColorToken::SuccessColor;
        gc->SetFont(theme_engine().font(core::ThemeFontToken::UISmall),
                    theme_engine().color(msg_token));
        // Validation message is drawn below the control by parent layout
    }

    state_.acknowledge_change();
}

void ThemedTextInput::OnMouseDown(wxMouseEvent& /*event*/)
{
    if (!state_.is_disabled())
    {
        SetFocus();
    }
}

void ThemedTextInput::OnMouseEnter(wxMouseEvent& /*event*/)
{
    if (!state_.is_disabled())
    {
        state_.on_mouse_enter();
        SetControlCursor(ControlCursorType::kIBeam);
        Refresh();
    }
}

void ThemedTextInput::OnMouseLeave(wxMouseEvent& /*event*/)
{
    state_.on_mouse_leave();
    SetControlCursor(ControlCursorType::kArrow);
    Refresh();
}

void ThemedTextInput::OnSetFocus(wxFocusEvent& /*event*/)
{
    state_.on_focus();
    is_editing_ = true;
    cursor_pos_ = static_cast<int>(model_.value().size());
    Refresh();
}

void ThemedTextInput::OnKillFocus(wxFocusEvent& /*event*/)
{
    state_.on_blur();
    is_editing_ = false;
    Refresh();
}

void ThemedTextInput::OnChar(wxKeyEvent& event)
{
    if (state_.is_disabled())
        return;

    const auto ch = event.GetUnicodeKey();
    if (ch == WXK_NONE || ch < 32)
    {
        event.Skip();
        return;
    }

    auto text = model_.value();
    text.insert(text.begin() + cursor_pos_, static_cast<char>(ch));
    model_.set_value(text);
    cursor_pos_ = std::min(cursor_pos_ + 1, static_cast<int>(model_.value().size()));

    if (on_change_)
        on_change_(model_.value());
    Refresh();
}

void ThemedTextInput::OnKeyDown(wxKeyEvent& event)
{
    if (state_.is_disabled())
    {
        event.Skip();
        return;
    }

    switch (event.GetKeyCode())
    {
        case WXK_BACK:
        {
            if (cursor_pos_ > 0)
            {
                auto text = model_.value();
                text.erase(static_cast<size_t>(cursor_pos_ - 1), 1);
                model_.set_value(text);
                --cursor_pos_;
                if (on_change_)
                    on_change_(model_.value());
                Refresh();
            }
            break;
        }
        case WXK_DELETE:
        {
            auto text = model_.value();
            if (cursor_pos_ < static_cast<int>(text.size()))
            {
                text.erase(static_cast<size_t>(cursor_pos_), 1);
                model_.set_value(text);
                if (on_change_)
                    on_change_(model_.value());
                Refresh();
            }
            break;
        }
        case WXK_LEFT:
            if (cursor_pos_ > 0)
            {
                --cursor_pos_;
                Refresh();
            }
            break;
        case WXK_RIGHT:
            if (cursor_pos_ < static_cast<int>(model_.value().size()))
            {
                ++cursor_pos_;
                Refresh();
            }
            break;
        case WXK_HOME:
            cursor_pos_ = 0;
            Refresh();
            break;
        case WXK_END:
            cursor_pos_ = static_cast<int>(model_.value().size());
            Refresh();
            break;
        default:
            event.Skip();
            break;
    }
}

} // namespace markamp::ui
