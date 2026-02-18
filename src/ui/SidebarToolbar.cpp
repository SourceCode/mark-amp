#include "SidebarToolbar.h"

#include "core/Logger.h"

#include <wx/button.h>

namespace markamp::ui
{

SidebarToolbar::SidebarToolbar(wxWindow* parent,
                               core::ThemeEngine& theme_engine,
                               core::EventBus& event_bus)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
{
    SetMinSize(wxSize(-1, 32));

    main_sizer_ = new wxBoxSizer(wxHORIZONTAL);

    // Title label (left-aligned, bold)
    title_label_ = new wxStaticText(this, wxID_ANY, "EXPLORER");
    auto title_font = title_label_->GetFont();
    title_font.SetPointSize(10);
    title_font.MakeBold();
    title_label_->SetFont(title_font);

    main_sizer_->AddSpacer(12);
    main_sizer_->Add(title_label_, 0, wxALIGN_CENTER_VERTICAL);
    main_sizer_->AddStretchSpacer();

    // Actions sizer (right-aligned action buttons)
    actions_sizer_ = new wxBoxSizer(wxHORIZONTAL);
    main_sizer_->Add(actions_sizer_, 0, wxALIGN_CENTER_VERTICAL);
    main_sizer_->AddSpacer(8);

    SetSizer(main_sizer_);
    ApplyTheme();

    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });
}

void SidebarToolbar::SetTitle(const std::string& title)
{
    if (title_label_ != nullptr)
    {
        title_label_->SetLabel(title);
        Layout();
    }
}

void SidebarToolbar::SetActions(const std::vector<ToolbarAction>& actions)
{
    RebuildActions(actions);
}

void SidebarToolbar::ClearActions()
{
    if (actions_sizer_ != nullptr)
    {
        actions_sizer_->Clear(true);
        Layout();
    }
}

void SidebarToolbar::ApplyTheme()
{
    SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::BgHeader));

    if (title_label_ != nullptr)
    {
        title_label_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
    }

    Refresh();
}

void SidebarToolbar::RebuildActions(const std::vector<ToolbarAction>& actions)
{
    if (actions_sizer_ == nullptr)
    {
        return;
    }

    // Remove existing action buttons
    actions_sizer_->Clear(true);

    // Create new action buttons
    for (const auto& action : actions)
    {
        auto* btn = new wxButton(this,
                                 wxID_ANY,
                                 action.label,
                                 wxDefaultPosition,
                                 wxSize(24, 24),
                                 wxBU_EXACTFIT | wxBORDER_NONE);
        btn->SetToolTip(action.tooltip);
        btn->SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::BgHeader));
        btn->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));

        auto font = btn->GetFont();
        font.SetPointSize(11);
        btn->SetFont(font);

        // Capture callback for button click
        auto callback = action.on_click;
        btn->Bind(wxEVT_BUTTON,
                  [callback](wxCommandEvent& /*cmd*/)
                  {
                      if (callback)
                      {
                          callback();
                      }
                  });

        actions_sizer_->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    }

    Layout();
}

} // namespace markamp::ui
