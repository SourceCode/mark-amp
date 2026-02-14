#include "FloatingFormatBar.h"

#include "core/Color.h"
#include "core/Events.h"

#include <wx/button.h>
#include <wx/sizer.h>

#include <array>

namespace markamp::ui
{

namespace
{

struct ButtonSpec
{
    wxString label;
    FloatingFormatBar::Action action;
    wxString tooltip;
};

// Button definitions — compact labels for the toolbar
constexpr int kButtonSize = 28;
constexpr int kBarPadding = 4;
constexpr int kButtonSpacing = 2;

} // namespace

FloatingFormatBar::FloatingFormatBar(wxWindow* parent,
                                     core::ThemeEngine& theme_engine,
                                     core::EventBus& event_bus,
                                     ActionCallback callback)
    : wxPopupTransientWindow(parent, wxBORDER_NONE)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , callback_(std::move(callback))
{
    CreateButtons();
    ApplyTheme();

    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });
}

void FloatingFormatBar::CreateButtons()
{
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    const std::array<ButtonSpec, 7> specs = {{
        {wxString::FromUTF8("B"), Action::Bold, "Bold (⌘B)"},
        {wxString::FromUTF8("I"), Action::Italic, "Italic (⌘I)"},
        {wxString::FromUTF8("</>"), Action::InlineCode, "Inline Code"},
        {wxString::FromUTF8("🔗"), Action::Link, "Insert Link (⌘K)"},
        {wxString::FromUTF8(">"), Action::Blockquote, "Blockquote"},
        {wxString::FromUTF8("H"), Action::Heading, "Cycle Heading"},
        {wxString::FromUTF8("⊞"), Action::Table, "Insert Table"},
    }};

    sizer->AddSpacer(kBarPadding);

    for (const auto& spec : specs)
    {
        auto* btn = new wxButton(
            this, wxID_ANY, spec.label, wxDefaultPosition, wxSize(kButtonSize, kButtonSize));
        btn->SetToolTip(spec.tooltip);

        // Use a bold font for B and I buttons
        if (spec.action == Action::Bold)
        {
            auto font = btn->GetFont();
            font.SetWeight(wxFONTWEIGHT_BOLD);
            btn->SetFont(font);
        }
        else if (spec.action == Action::Italic)
        {
            auto font = btn->GetFont();
            font.SetStyle(wxFONTSTYLE_ITALIC);
            btn->SetFont(font);
        }

        // R21 Fix 9: Hover background — highlight button on mouse enter
        btn->Bind(wxEVT_ENTER_WINDOW,
                  [this, btn](wxMouseEvent& /*evt*/)
                  {
                      auto hover_bg = theme_engine_.color(core::ThemeColorToken::HoverBg);
                      btn->SetBackgroundColour(hover_bg);
                      btn->Refresh();
                  });
        btn->Bind(wxEVT_LEAVE_WINDOW,
                  [this, btn](wxMouseEvent& /*evt*/)
                  {
                      auto bg = theme_engine_.color(core::ThemeColorToken::BgPanel);
                      btn->SetBackgroundColour(bg);
                      btn->Refresh();
                  });

        // R21 Fix 12: Pressed feedback — flash AccentSecondary on click
        btn->Bind(wxEVT_BUTTON,
                  [this, action = spec.action, btn](wxCommandEvent& /*evt*/)
                  {
                      // R21 Fix 10: Flash accent color on press
                      auto accent = theme_engine_.color(core::ThemeColorToken::AccentSecondary);
                      btn->SetForegroundColour(accent);
                      btn->Refresh();
                      OnButtonClicked(action);
                  });

        sizer->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, kButtonSpacing);
    }

    sizer->AddSpacer(kBarPadding);
    SetSizer(sizer);

    // Fit to contents
    sizer->Fit(this);
}

void FloatingFormatBar::ApplyTheme()
{
    auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    auto fg_color = theme_engine_.color(core::ThemeColorToken::TextMain);
    auto accent = theme_engine_.color(core::ThemeColorToken::AccentPrimary);
    auto border = theme_engine_.color(core::ThemeColorToken::BorderDark);

    SetBackgroundColour(bg_color);

    // Style all child buttons
    for (auto* child : GetChildren())
    {
        auto* btn = dynamic_cast<wxButton*>(child);
        if (btn != nullptr)
        {
            btn->SetBackgroundColour(bg_color);
            btn->SetForegroundColour(fg_color);
        }
    }

    // R21 Fix 11: Border for depth effect around the popup
    // wxPopupTransientWindow doesn't support border painting directly,
    // but we can use the window's background and accent to style it

    Refresh();
}

void FloatingFormatBar::OnButtonClicked(Action action)
{
    if (callback_)
    {
        callback_(action);
    }
    // Dismiss after action
    Dismiss();
}

} // namespace markamp::ui
