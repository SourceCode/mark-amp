#include "RunDebugPanel.h"

#include "DesignSystemContext.h"
#include "SidebarSection.h"

#include <spdlog/spdlog.h>

namespace markamp::ui
{

RunDebugPanel::RunDebugPanel(wxWindow* parent,
                             core::ThemeEngine& theme_engine,
                             core::EventBus& event_bus,
                             core::DebugSessionManager& session_mgr,
                             DesignSystemContext& design_system,
                             IconManager& icon_manager,
                             core::Config* config)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , session_mgr_(session_mgr)
    , ds_(design_system)
    , icon_manager_(icon_manager)
    , config_(config)
{
    CreateLayout();
    ApplyTheme();
}

void RunDebugPanel::CreateLayout()
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // ── Task 3: Launch configuration selector ──
    auto* config_sizer = new wxBoxSizer(wxHORIZONTAL);

    config_selector_ = new wxChoice(this, wxID_ANY);
    config_selector_->SetMinSize(wxSize(120, 28));
    config_selector_->SetFont(theme_engine_.font(core::ThemeFontToken::UISmall));

    play_button_ =
        new wxButton(this, wxID_ANY, "\u25B6", wxDefaultPosition, wxSize(32, 28), wxBORDER_NONE);
    play_button_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular));
    play_button_->SetToolTip("Start Debugging (F5)");
    play_button_->Bind(wxEVT_BUTTON,
                       [this](wxCommandEvent& /*unused*/)
                       {
                           if (on_launch_ && config_selector_->GetSelection() != wxNOT_FOUND)
                           {
                               on_launch_(config_selector_->GetStringSelection().ToStdString());
                           }
                       });

    // Task 25: Accessibility
    config_selector_->SetName("Launch Configuration");
    play_button_->SetName("Start Debugging");

    config_sizer->Add(config_selector_, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    config_sizer->Add(play_button_, 0, wxALIGN_CENTER_VERTICAL);
    main_sizer->Add(config_sizer, 0, wxEXPAND | wxALL, 8);

    // ── Task 6: Debug controls ──
    controls_panel_ = new wxPanel(this, wxID_ANY);
    auto* ctrl_sizer = new wxBoxSizer(wxHORIZONTAL);

    auto make_ctrl_btn = [this](const std::string& label, const std::string& tooltip) -> wxButton*
    {
        auto* btn = new wxButton(
            controls_panel_, wxID_ANY, label, wxDefaultPosition, wxSize(28, 28), wxBORDER_NONE);
        btn->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular));
        btn->SetToolTip(tooltip);
        return btn;
    };

    btn_continue_ = make_ctrl_btn("\u25B6", "Continue (F5)");
    btn_pause_ = make_ctrl_btn("\u23F8", "Pause (F6)");
    btn_step_over_ = make_ctrl_btn("\u2B8F", "Step Over (F10)");
    btn_step_into_ = make_ctrl_btn("\u2B8E", "Step Into (F11)");
    btn_step_out_ = make_ctrl_btn("\u2B8D", "Step Out (Shift+F11)");
    btn_restart_ = make_ctrl_btn("\u21BB", "Restart (Ctrl+Shift+F5)");
    btn_stop_ = make_ctrl_btn("\u25A0", "Stop (Shift+F5)");

    // Wire callbacks
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

    ctrl_sizer->AddStretchSpacer();
    ctrl_sizer->Add(btn_continue_, 0, wxRIGHT, 2);
    ctrl_sizer->Add(btn_pause_, 0, wxRIGHT, 2);
    ctrl_sizer->Add(btn_step_over_, 0, wxRIGHT, 2);
    ctrl_sizer->Add(btn_step_into_, 0, wxRIGHT, 2);
    ctrl_sizer->Add(btn_step_out_, 0, wxRIGHT, 2);
    ctrl_sizer->Add(btn_restart_, 0, wxRIGHT, 2);
    ctrl_sizer->Add(btn_stop_, 0);
    ctrl_sizer->AddStretchSpacer();

    controls_panel_->SetSizer(ctrl_sizer);
    controls_panel_->Hide(); // Only shown when debugging
    main_sizer->Add(controls_panel_, 0, wxEXPAND | wxLEFT | wxRIGHT, 8);

    // ── Task 9: Variables section ──
    variables_section_ =
        new SidebarSection(this, ds_, icon_manager_, event_bus_, config_, "VARIABLES");
    variables_scroll_ = new wxScrolledWindow(variables_section_, wxID_ANY);
    variables_scroll_->SetScrollRate(0, 10);
    variables_sizer_ = new wxBoxSizer(wxVERTICAL);
    auto* no_vars = new wxStaticText(variables_scroll_, wxID_ANY, "Not paused");
    no_vars->SetFont(theme_engine_.font(core::ThemeFontToken::UISmall));
    no_vars->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
    variables_sizer_->Add(no_vars, 0, wxALL, 8);
    variables_scroll_->SetSizer(variables_sizer_);
    variables_section_->set_content(variables_scroll_);
    main_sizer->Add(variables_section_, 1, wxEXPAND);

    // ── Task 10: Watch section ──
    watch_section_ = new SidebarSection(this, ds_, icon_manager_, event_bus_, config_, "WATCH");
    watch_scroll_ = new wxScrolledWindow(watch_section_, wxID_ANY);
    watch_scroll_->SetScrollRate(0, 10);
    watch_sizer_ = new wxBoxSizer(wxVERTICAL);
    auto* add_watch = new wxStaticText(watch_scroll_, wxID_ANY, "+ Add Expression...");
    add_watch->SetFont(theme_engine_.font(core::ThemeFontToken::UISmall));
    add_watch->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::AccentPrimary));
    watch_sizer_->Add(add_watch, 0, wxALL, 8);
    watch_scroll_->SetSizer(watch_sizer_);
    watch_section_->set_content(watch_scroll_);
    main_sizer->Add(watch_section_, 1, wxEXPAND);

    // ── Task 8: Call Stack section ──
    callstack_section_ =
        new SidebarSection(this, ds_, icon_manager_, event_bus_, config_, "CALL STACK");
    callstack_scroll_ = new wxScrolledWindow(callstack_section_, wxID_ANY);
    callstack_scroll_->SetScrollRate(0, 10);
    callstack_sizer_ = new wxBoxSizer(wxVERTICAL);
    auto* no_stack = new wxStaticText(callstack_scroll_, wxID_ANY, "Not paused");
    no_stack->SetFont(theme_engine_.font(core::ThemeFontToken::UISmall));
    no_stack->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
    callstack_sizer_->Add(no_stack, 0, wxALL, 8);
    callstack_scroll_->SetSizer(callstack_sizer_);
    callstack_section_->set_content(callstack_scroll_);
    main_sizer->Add(callstack_section_, 1, wxEXPAND);

    // ── Task 7: Breakpoints section ──
    breakpoints_section_ =
        new SidebarSection(this, ds_, icon_manager_, event_bus_, config_, "BREAKPOINTS");
    breakpoints_scroll_ = new wxScrolledWindow(breakpoints_section_, wxID_ANY);
    breakpoints_scroll_->SetScrollRate(0, 10);
    breakpoints_sizer_ = new wxBoxSizer(wxVERTICAL);
    auto* no_bps = new wxStaticText(breakpoints_scroll_, wxID_ANY, "No breakpoints set");
    no_bps->SetFont(theme_engine_.font(core::ThemeFontToken::UISmall));
    no_bps->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
    breakpoints_sizer_->Add(no_bps, 0, wxALL, 8);
    breakpoints_scroll_->SetSizer(breakpoints_sizer_);
    breakpoints_section_->set_content(breakpoints_scroll_);
    main_sizer->Add(breakpoints_section_, 1, wxEXPAND);

    SetSizer(main_sizer);
}

void RunDebugPanel::ApplyTheme()
{
    auto bg_col = theme_engine_.color(core::ThemeColorToken::BgPanel);
    SetBackgroundColour(bg_col);

    if (controls_panel_ != nullptr)
    {
        controls_panel_->SetBackgroundColour(bg_col);
    }

    if (play_button_ != nullptr)
    {
        play_button_->SetBackgroundColour(
            theme_engine_.color(core::ThemeColorToken::AccentPrimary));
        play_button_->SetForegroundColour(*wxWHITE);
    }

    Refresh();
}

void RunDebugPanel::RefreshAll()
{
    UpdateControlButtons();
    Layout();
    Refresh();
}

void RunDebugPanel::SetLaunchConfigs(const std::vector<core::LaunchConfig>& configs)
{
    if (config_selector_ == nullptr)
    {
        return;
    }

    config_selector_->Clear();
    for (const auto& cfg : configs)
    {
        config_selector_->Append(cfg.name);
    }
    if (!configs.empty())
    {
        config_selector_->SetSelection(0);
    }
}

void RunDebugPanel::UpdateBreakpoints(const std::vector<std::string>& breakpoint_labels)
{
    if (breakpoints_sizer_ == nullptr || breakpoints_scroll_ == nullptr)
    {
        return;
    }

    breakpoints_sizer_->Clear(true);

    if (breakpoint_labels.empty())
    {
        auto* label = new wxStaticText(breakpoints_scroll_, wxID_ANY, "No breakpoints set");
        label->SetFont(theme_engine_.font(core::ThemeFontToken::UISmall));
        label->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
        breakpoints_sizer_->Add(label, 0, wxALL, 8);
    }
    else
    {
        for (const auto& bp_label : breakpoint_labels)
        {
            auto* label = new wxStaticText(breakpoints_scroll_, wxID_ANY, bp_label);
            label->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));
            label->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMain));
            breakpoints_sizer_->Add(label, 0, wxLEFT | wxTOP, 8);
        }
    }

    breakpoints_scroll_->FitInside();
    breakpoints_scroll_->Layout();
}

void RunDebugPanel::UpdateCallStack(const std::vector<core::StackFrame>& frames)
{
    if (callstack_sizer_ == nullptr || callstack_scroll_ == nullptr)
    {
        return;
    }

    callstack_sizer_->Clear(true);

    if (frames.empty())
    {
        auto* label = new wxStaticText(callstack_scroll_, wxID_ANY, "Not paused");
        label->SetFont(theme_engine_.font(core::ThemeFontToken::UISmall));
        label->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
        callstack_sizer_->Add(label, 0, wxALL, 8);
    }
    else
    {
        bool is_first = true;
        for (const auto& frame : frames)
        {
            std::string display =
                frame.name + " (" + frame.source_file + ":" + std::to_string(frame.line) + ")";
            auto* label = new wxStaticText(callstack_scroll_, wxID_ANY, display);
            auto font = theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F);
            if (is_first)
            {
                font = font.Bold();
            }
            label->SetFont(font);

            auto color = frame.is_external ? theme_engine_.color(core::ThemeColorToken::TextMuted)
                                           : theme_engine_.color(core::ThemeColorToken::TextMain);
            label->SetForegroundColour(color);

            if (is_first)
            {
                // Current frame: accent indicator
                auto* row = new wxBoxSizer(wxHORIZONTAL);
                auto* indicator = new wxStaticText(callstack_scroll_, wxID_ANY, "\u25B6 ");
                indicator->SetFont(font);
                indicator->SetForegroundColour(
                    theme_engine_.color(core::ThemeColorToken::AccentPrimary));
                row->Add(indicator, 0, wxALIGN_CENTER_VERTICAL);
                row->Add(label, 1, wxALIGN_CENTER_VERTICAL);
                callstack_sizer_->Add(row, 0, wxLEFT | wxTOP, 8);
            }
            else
            {
                callstack_sizer_->Add(label, 0, wxLEFT | wxTOP, 8);
            }
            is_first = false;
        }
    }

    callstack_scroll_->FitInside();
    callstack_scroll_->Layout();
}

void RunDebugPanel::UpdateVariables(const std::vector<core::DebugVariable>& variables)
{
    if (variables_sizer_ == nullptr || variables_scroll_ == nullptr)
    {
        return;
    }

    variables_sizer_->Clear(true);

    if (variables.empty())
    {
        auto* label = new wxStaticText(variables_scroll_, wxID_ANY, "Not paused");
        label->SetFont(theme_engine_.font(core::ThemeFontToken::UISmall));
        label->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
        variables_sizer_->Add(label, 0, wxALL, 8);
    }
    else
    {
        for (const auto& var : variables)
        {
            auto* row = new wxBoxSizer(wxHORIZONTAL);

            // Variable name
            auto* name_label = new wxStaticText(variables_scroll_, wxID_ANY, var.name + ": ");
            name_label->SetFont(
                theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));
            name_label->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMain));

            // Type in muted
            auto* type_label = new wxStaticText(variables_scroll_, wxID_ANY, var.type + " = ");
            type_label->SetFont(
                theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));
            type_label->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::SyntaxType));

            // Value in string color
            auto* value_label = new wxStaticText(variables_scroll_, wxID_ANY, var.value);
            value_label->SetFont(
                theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));
            value_label->SetForegroundColour(
                theme_engine_.color(core::ThemeColorToken::SyntaxString));

            row->Add(name_label, 0, wxALIGN_CENTER_VERTICAL);
            row->Add(type_label, 0, wxALIGN_CENTER_VERTICAL);
            row->Add(value_label, 0, wxALIGN_CENTER_VERTICAL);

            variables_sizer_->Add(row, 0, wxLEFT | wxTOP, 8);
        }
    }

    variables_scroll_->FitInside();
    variables_scroll_->Layout();
}

void RunDebugPanel::UpdateControlButtons()
{
    auto debug_state = session_mgr_.state();
    bool is_debugging = session_mgr_.is_active();

    if (controls_panel_ != nullptr)
    {
        controls_panel_->Show(is_debugging);
    }

    if (btn_continue_ != nullptr)
    {
        btn_continue_->Show(debug_state == core::DebugState::kPaused);
    }
    if (btn_pause_ != nullptr)
    {
        btn_pause_->Show(debug_state == core::DebugState::kRunning);
    }

    bool can_step = (debug_state == core::DebugState::kPaused);
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
}

// ── Callback setters ──

void RunDebugPanel::SetOnLaunch(std::function<void(const std::string&)> callback)
{
    on_launch_ = std::move(callback);
}

void RunDebugPanel::SetOnContinue(std::function<void()> callback)
{
    on_continue_ = std::move(callback);
}

void RunDebugPanel::SetOnPause(std::function<void()> callback)
{
    on_pause_ = std::move(callback);
}

void RunDebugPanel::SetOnStepOver(std::function<void()> callback)
{
    on_step_over_ = std::move(callback);
}

void RunDebugPanel::SetOnStepInto(std::function<void()> callback)
{
    on_step_into_ = std::move(callback);
}

void RunDebugPanel::SetOnStepOut(std::function<void()> callback)
{
    on_step_out_ = std::move(callback);
}

void RunDebugPanel::SetOnStop(std::function<void()> callback)
{
    on_stop_ = std::move(callback);
}

void RunDebugPanel::SetOnRestart(std::function<void()> callback)
{
    on_restart_ = std::move(callback);
}

} // namespace markamp::ui
