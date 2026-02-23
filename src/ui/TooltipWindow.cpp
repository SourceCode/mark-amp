#include "TooltipWindow.h"

#include "TypographyScale.h"
#include "core/Logger.h"
#include "core/ThemeEngine.h"

#include <wx/app.h>
#include <wx/dcbuffer.h>
#include <wx/sizer.h>

namespace markamp::ui
{

static TooltipWindow* g_tooltip_window = nullptr;

TooltipWindow* TooltipWindow::GetOrCreate(wxWindow* parent, DesignSystemContext& ds)
{
    if (g_tooltip_window == nullptr)
    {
        // Use a frame that doesn't steal focus or show up in the taskbar
        g_tooltip_window = new TooltipWindow(parent, ds);
    }
    return g_tooltip_window;
}

TooltipWindow::TooltipWindow(wxWindow* parent, DesignSystemContext& ds)
    : wxFrame(parent,
              wxID_ANY,
              "",
              wxDefaultPosition,
              wxDefaultSize,
              wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT | wxBORDER_NONE)
    , ds_(ds)
    , transition_manager_(this)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    label_ = new wxStaticText(this, wxID_ANY, "");
    sizer->Add(label_, 1, wxEXPAND | wxALL, 6);
    SetSizer(sizer);

    Bind(wxEVT_PAINT, &TooltipWindow::OnPaint, this);

    animation::AnimationConfig show_cfg;
    show_cfg.duration = std::chrono::milliseconds(150);
    show_cfg.easing_type = animation::EasingType::EaseOutQuad;
    transition_manager_.register_transition("tooltip_fade_in", show_cfg);

    animation::AnimationConfig hide_cfg;
    hide_cfg.duration = std::chrono::milliseconds(150);
    hide_cfg.easing_type = animation::EasingType::EaseInQuad;
    transition_manager_.register_transition("tooltip_fade_out", hide_cfg);

    ApplyTheme();

    Bind(wxEVT_DESTROY,
         [this](wxWindowDestroyEvent& event)
         {
             if (event.GetEventObject() == this)
             {
                 g_tooltip_window = nullptr;
             }
             event.Skip();
         });
}

TooltipWindow::~TooltipWindow()
{
    if (g_tooltip_window == this)
    {
        g_tooltip_window = nullptr;
    }
}

void TooltipWindow::ApplyTheme()
{
    SetBackgroundColour(ds_.theme.resolve_token("bg.header")
                            .value_or(ds_.theme.color(core::ThemeColorToken::BgHeader)));
    label_->SetForegroundColour(ds_.theme.resolve_token("text.main")
                                    .value_or(ds_.theme.color(core::ThemeColorToken::TextMain)));
    label_->SetFont(ds_.typography.font(TypeSlot::kBody));
}

void TooltipWindow::ShowTooltip(const wxString& text, const wxPoint& screen_pos)
{
    if (is_showing_ && label_->GetLabel() == text)
    {
        SetPosition(screen_pos);
        return;
    }

    ApplyTheme(); // Ensure theme is fresh
    label_->SetLabel(text);
    GetSizer()->Fit(this);
    SetPosition(screen_pos);

    is_showing_ = true;

    // Start fade-in
    SetTransparent(0);
    ShowWithoutActivating(); // Safe method to show without stealing focus

    transition_manager_.start<float>(
        "tooltip_fade_in",
        0.0F,
        1.0F,
        [this](float alpha)
        {
            SetTransparent(static_cast<wxByte>(255.0F * alpha));
            Refresh();
        },
        nullptr);
}

void TooltipWindow::HideTooltip()
{
    if (!is_showing_)
    {
        return;
    }

    is_showing_ = false;

    transition_manager_.start<float>(
        "tooltip_fade_out",
        1.0F,
        0.0F,
        [this](float alpha)
        {
            SetTransparent(static_cast<wxByte>(255.0F * alpha));
            Refresh();
        },
        [this]()
        {
            if (!is_showing_)
            {
                Hide();
            }
        });
}

void TooltipWindow::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(this);

    auto bg_color = ds_.theme.resolve_token("bg.header")
                        .value_or(ds_.theme.color(core::ThemeColorToken::BgHeader));
    auto border_color = ds_.theme.resolve_token("border.light")
                            .value_or(ds_.theme.color(core::ThemeColorToken::BorderLight));

    paint_dc.SetBrush(wxBrush(bg_color));
    paint_dc.SetPen(wxPen(border_color));

    auto size = GetClientSize();
    paint_dc.DrawRoundedRectangle(0, 0, size.GetWidth(), size.GetHeight(), 4.0);
}

} // namespace markamp::ui
