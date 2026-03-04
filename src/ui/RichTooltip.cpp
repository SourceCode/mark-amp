#include "RichTooltip.h"

#include "core/ThemeEngine.h"
#include "core/ThemeTokens.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

namespace markamp::ui
{

RichTooltip::RichTooltip(wxWindow* parent, core::ThemeEngine& theme_engine)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxSize(300, 80), wxBORDER_NONE)
    , show_timer_(this)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &RichTooltip::on_paint, this);
    Bind(wxEVT_TIMER, &RichTooltip::on_timer, this);
    Hide();
}

void RichTooltip::show_at(int screen_x, int screen_y)
{
    pending_x_ = screen_x;
    pending_y_ = screen_y;
    if (delay_ms_ > 0)
    {
        show_timer_.StartOnce(delay_ms_);
    }
    else
    {
        do_show();
    }
}

void RichTooltip::dismiss()
{
    show_timer_.Stop();
    is_showing_ = false;
    Hide();
}

void RichTooltip::do_show()
{
    SetPosition(wxPoint(pending_x_, pending_y_));
    is_showing_ = true;
    Show();
    Refresh();
}

void RichTooltip::on_timer(wxTimerEvent& /*event*/)
{
    do_show();
}

void RichTooltip::on_paint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC pdc(this);
    auto graphics_ctx = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!graphics_ctx)
    {
        return;
    }

    const auto client_rect = GetClientRect();
    const int pad = FromDIP(8);

    // Background
    graphics_ctx->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::BgPanel)));
    graphics_ctx->SetPen(wxPen(theme_engine().color(core::ThemeColorToken::BorderLight), 1));
    graphics_ctx->DrawRoundedRectangle(0, 0, client_rect.width, client_rect.height, FromDIP(4));

    int text_y = pad;

    // Title
    if (!title_.empty())
    {
        graphics_ctx->SetFont(theme_engine().font(core::ThemeFontToken::SansSemiBold),
                              theme_engine().color(core::ThemeColorToken::TextMain));
        graphics_ctx->DrawText(wxString::FromUTF8(title_), pad, text_y);
        text_y += FromDIP(18);
    }

    // Body
    if (!body_.empty())
    {
        graphics_ctx->SetFont(theme_engine().font(core::ThemeFontToken::SansRegular),
                              theme_engine().color(core::ThemeColorToken::TextMuted));
        graphics_ctx->DrawText(wxString::FromUTF8(body_), pad, text_y);
        text_y += FromDIP(16);
    }

    // Shortcut
    if (!shortcut_.empty())
    {
        graphics_ctx->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular),
                              theme_engine().color(core::ThemeColorToken::TextMuted));
        const auto shortcut_str = wxString::FromUTF8(shortcut_);
        double text_width = 0;
        double text_height = 0;
        graphics_ctx->GetTextExtent(shortcut_str, &text_width, &text_height);
        graphics_ctx->DrawText(
            shortcut_str, client_rect.width - pad - static_cast<int>(text_width), text_y);
    }
}

} // namespace markamp::ui
