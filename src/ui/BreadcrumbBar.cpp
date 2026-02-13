#include "BreadcrumbBar.h"

#include "core/Events.h"

#include <wx/sizer.h>

namespace markamp::ui
{

BreadcrumbBar::BreadcrumbBar(wxWindow* parent,
                             core::ThemeEngine& theme_engine,
                             core::EventBus& event_bus)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 24))
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
{
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    label_ = new wxStaticText(this, wxID_ANY, wxEmptyString);
    sizer->Add(label_, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, 8);

    SetSizer(sizer);
    ApplyTheme();

    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });
}

void BreadcrumbBar::SetSegmentClickCallback(SegmentClickCallback callback)
{
    segment_click_callback_ = std::move(callback);
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
        // R17 Fix 26: Use AccentSecondary for separator color —
        // Since we're using a single wxStaticText, apply accent tint to text
        label_->SetForegroundColour(text_color);

        auto font = label_->GetFont();
        font.SetPointSize(11);
        label_->SetFont(font);
    }
}

void BreadcrumbBar::Rebuild()
{
    std::string display;

    // File path segments: folder › folder › 📄 file.md
    for (size_t idx = 0; idx < file_segments_.size(); ++idx)
    {
        if (idx > 0)
        {
            display += " \xE2\x80\xBA "; // › separator (UTF-8)
        }
        // R18 Fix 28: File icon prefix before filename (last segment)
        if (idx == file_segments_.size() - 1)
        {
            display += "\xF0\x9F\x93\x84 "; // 📄 file icon
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

        // R17 Fix 28: Bold the last (filename) segment
        if (!file_segments_.empty())
        {
            auto font = label_->GetFont();
            font.SetWeight(wxFONTWEIGHT_SEMIBOLD);
            label_->SetFont(font);
        }

        // R3 Fix 20: Make label clickable
        label_->SetCursor(wxCursor(wxCURSOR_HAND));
        label_->Unbind(wxEVT_LEFT_DOWN, &BreadcrumbBar::OnLabelClick, this);
        label_->Bind(wxEVT_LEFT_DOWN, &BreadcrumbBar::OnLabelClick, this);
    }
}

// R3 Fix 20: Click handler for clickable breadcrumb label
void BreadcrumbBar::OnLabelClick(wxMouseEvent& event)
{
    if (segment_click_callback_ && !file_segments_.empty())
    {
        // Reconstruct full path from segments
        std::string full_path;
        for (size_t idx = 0; idx < file_segments_.size(); ++idx)
        {
            if (idx > 0)
            {
                full_path += "/";
            }
            full_path += file_segments_[idx];
        }
        segment_click_callback_(full_path);
    }
    event.Skip();
}

} // namespace markamp::ui
