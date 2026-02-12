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

        btn->Bind(wxEVT_BUTTON,
                  [this, action = spec.action](wxCommandEvent& /*evt*/)
                  { OnButtonClicked(action); });

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
