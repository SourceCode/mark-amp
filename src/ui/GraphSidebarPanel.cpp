#include "GraphSidebarPanel.h"

#include "core/Events.h"
#include "core/Logger.h"

#include <wx/sizer.h>

namespace markamp::ui
{

GraphSidebarPanel::GraphSidebarPanel(wxWindow* parent,
                                     core::ThemeEngine& theme_engine,
                                     core::EventBus& event_bus)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Title
    title_label_ = new wxStaticText(this, wxID_ANY, "GRAPH VIEW");
    auto title_font = title_label_->GetFont();
    title_font.SetWeight(wxFONTWEIGHT_BOLD);
    title_label_->SetFont(title_font);
    main_sizer->Add(title_label_, 0, wxALL, 12);

    // Status indicator
    status_label_ = new wxStaticText(
        this, wxID_ANY, "\xE2\x97\x8B Coming Soon \u2014 Backlink graph visualization");
    main_sizer->Add(status_label_, 0, wxLEFT | wxRIGHT | wxBOTTOM, 12);

    // Document links list
    link_list_ = new wxListBox(this, wxID_ANY);
    main_sizer->Add(link_list_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);

    SetSizer(main_sizer);

    ApplyTheme();

    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });
}

void GraphSidebarPanel::AddDocumentLink(const std::string& label)
{
    if (link_list_ != nullptr)
    {
        link_list_->Append(label);
    }
}

void GraphSidebarPanel::ClearLinks()
{
    if (link_list_ != nullptr)
    {
        link_list_->Clear();
    }
}

void GraphSidebarPanel::ApplyTheme()
{
    SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::BgPanel));

    if (title_label_ != nullptr)
    {
        title_label_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMain));
    }

    if (status_label_ != nullptr)
    {
        status_label_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
    }

    Refresh();
}

} // namespace markamp::ui
