#include "DebugToolbar.h"

namespace markamp::ui
{

DebugToolbar::DebugToolbar(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           core::EventBus& event_bus,
                           core::DebugSessionManager& session_mgr)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , session_mgr_(session_mgr)
{
    CreateLayout();
    ApplyTheme();
}

void DebugToolbar::CreateLayout()
{
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    auto make_btn = [this](const std::string& label, const std::string& tooltip) -> wxButton*
    {
        auto* btn =
            new wxButton(this, wxID_ANY, label, wxDefaultPosition, wxSize(28, 28), wxBORDER_NONE);
        btn->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular));
        btn->SetToolTip(tooltip);
        btn->SetName(tooltip); // Accessibility
        return btn;
    };

    btn_continue_ = make_btn("\u25B6", "Continue (F5)");
    btn_pause_ = make_btn("\u23F8", "Pause (F6)");
    btn_step_over_ = make_btn("\u2B8F", "Step Over (F10)");
    btn_step_into_ = make_btn("\u2B8E", "Step Into (F11)");
    btn_step_out_ = make_btn("\u2B8D", "Step Out (Shift+F11)");
    btn_restart_ = make_btn("\u21BB", "Restart (Ctrl+Shift+F5)");
    btn_stop_ = make_btn("\u25A0", "Stop (Shift+F5)");

    btn_continue_->Bind(wxEVT_BUTTON,
                        [this](wxCommandEvent& /*unused*/)
                        {
                            if (on_continue_)
                            {
                                on_continue_();
                            }
                        });
    btn_pause_->Bind(wxEVT_BUTTON,
                     [this](wxCommandEvent& /*unused*/)
                     {
                         if (on_pause_)
                         {
                             on_pause_();
                         }
                     });
    btn_step_over_->Bind(wxEVT_BUTTON,
                         [this](wxCommandEvent& /*unused*/)
                         {
                             if (on_step_over_)
                             {
                                 on_step_over_();
                             }
                         });
    btn_step_into_->Bind(wxEVT_BUTTON,
                         [this](wxCommandEvent& /*unused*/)
                         {
                             if (on_step_into_)
                             {
                                 on_step_into_();
                             }
                         });
    btn_step_out_->Bind(wxEVT_BUTTON,
                        [this](wxCommandEvent& /*unused*/)
                        {
                            if (on_step_out_)
                            {
                                on_step_out_();
                            }
                        });
    btn_restart_->Bind(wxEVT_BUTTON,
                       [this](wxCommandEvent& /*unused*/)
                       {
                           if (on_restart_)
                           {
                               on_restart_();
                           }
                       });
    btn_stop_->Bind(wxEVT_BUTTON,
                    [this](wxCommandEvent& /*unused*/)
                    {
                        if (on_stop_)
                        {
                            on_stop_();
                        }
                    });

    sizer->Add(btn_continue_, 0, wxRIGHT, 2);
    sizer->Add(btn_pause_, 0, wxRIGHT, 2);
    sizer->Add(btn_step_over_, 0, wxRIGHT, 2);
    sizer->Add(btn_step_into_, 0, wxRIGHT, 2);
    sizer->Add(btn_step_out_, 0, wxRIGHT, 2);
    sizer->Add(btn_restart_, 0, wxRIGHT, 2);
    sizer->Add(btn_stop_, 0);

    SetSizer(sizer);
    SetMinSize(wxSize(220, 32));
}

void DebugToolbar::ApplyTheme()
{
    auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    SetBackgroundColour(bg_color);

    // Accent green for continue, red for stop
    if (btn_continue_ != nullptr)
    {
        btn_continue_->SetBackgroundColour(
            theme_engine_.color(core::ThemeColorToken::AccentPrimary));
        btn_continue_->SetForegroundColour(*wxWHITE);
    }
    if (btn_stop_ != nullptr)
    {
        btn_stop_->SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::ErrorColor));
        btn_stop_->SetForegroundColour(*wxWHITE);
    }

    Refresh();
}

void DebugToolbar::UpdateState()
{
    const auto state = session_mgr_.state();
    const bool is_active = session_mgr_.is_active();

    Show(is_active);

    if (btn_continue_ != nullptr)
    {
        btn_continue_->Show(state == core::DebugState::kPaused);
    }
    if (btn_pause_ != nullptr)
    {
        btn_pause_->Show(state == core::DebugState::kRunning);
    }

    const bool can_step = (state == core::DebugState::kPaused);
    if (btn_step_over_ != nullptr)
    {
        btn_step_over_->Enable(can_step);
    }
    if (btn_step_into_ != nullptr)
    {
        btn_step_into_->Enable(can_step);
    }
    if (btn_step_out_ != nullptr)
    {
        btn_step_out_->Enable(can_step);
    }

    Layout();
}

// ── Callback setters ──

void DebugToolbar::SetOnContinue(std::function<void()> callback)
{
    on_continue_ = std::move(callback);
}
void DebugToolbar::SetOnPause(std::function<void()> callback)
{
    on_pause_ = std::move(callback);
}
void DebugToolbar::SetOnStepOver(std::function<void()> callback)
{
    on_step_over_ = std::move(callback);
}
void DebugToolbar::SetOnStepInto(std::function<void()> callback)
{
    on_step_into_ = std::move(callback);
}
void DebugToolbar::SetOnStepOut(std::function<void()> callback)
{
    on_step_out_ = std::move(callback);
}
void DebugToolbar::SetOnRestart(std::function<void()> callback)
{
    on_restart_ = std::move(callback);
}
void DebugToolbar::SetOnStop(std::function<void()> callback)
{
    on_stop_ = std::move(callback);
}

} // namespace markamp::ui
