#include "BreadcrumbBar.h"

#include <wx/sizer.h>

namespace markamp::ui
{

BreadcrumbBar::BreadcrumbBar(wxWindow* parent, core::ThemeEngine& theme_engine)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 24))
    , theme_engine_(theme_engine)
{
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    label_ = new wxStaticText(this, wxID_ANY, wxEmptyString);
    sizer->Add(label_, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, 8);

    SetSizer(sizer);
    ApplyTheme();
}

void BreadcrumbBar::SetFilePath(const std::vector<std::string>& segments)
{
    file_segments_ = segments;
    Rebuild();
}

void BreadcrumbBar::SetHeadingPath(const std::vector<std::string>& headings)
{
    heading_segments_ = headings;
    Rebuild();
}

void BreadcrumbBar::ApplyTheme()
{
    auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    auto text_color = theme_engine_.color(core::ThemeColorToken::TextMuted);

    SetBackgroundColour(bg_color);
    if (label_ != nullptr)
    {
        label_->SetForegroundColour(text_color);

        auto font = label_->GetFont();
        font.SetPointSize(11);
        label_->SetFont(font);
    }
}

void BreadcrumbBar::Rebuild()
{
    std::string display;

    // File path segments: folder > folder > file.md
    for (size_t idx = 0; idx < file_segments_.size(); ++idx)
    {
        if (idx > 0)
        {
            display += " \xE2\x80\xBA "; // › separator (UTF-8)
        }
        display += file_segments_[idx];
    }

    // Heading path: separated by >
    if (!heading_segments_.empty())
    {
        if (!display.empty())
        {
            display += "  \xE2\x80\x94  "; // — separator (UTF-8)
        }
        for (size_t idx = 0; idx < heading_segments_.size(); ++idx)
        {
            if (idx > 0)
            {
                display += " \xE2\x80\xBA "; // › separator
            }
            display += heading_segments_[idx];
        }
    }

    if (label_ != nullptr)
    {
        label_->SetLabel(wxString::FromUTF8(display));
    }
}

} // namespace markamp::ui
