#include "ui/IconGalleryDialog.h"

#include "ui/IconManager.h"

#include <wx/dcbuffer.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/wrapsizer.h>

namespace markamp::ui
{

namespace
{
/// A simple panel that draws an icon and displays its name.
class IconItemPanel : public wxPanel
{
public:
    IconItemPanel(wxWindow* parent, const std::string& icon_name, core::ThemeEngine& theme_engine)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(120, 100))
        , icon_name_(icon_name)
        , theme_engine_(theme_engine)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);

        // Subtitle text representing the icon name
        auto* label = new wxStaticText(this,
                                       wxID_ANY,
                                       wxString::FromUTF8(icon_name),
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       wxALIGN_CENTER_HORIZONTAL);

        auto font = label->GetFont();
        font.SetPointSize(10);
        label->SetFont(font);
        label->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));

        auto* sizer = new wxBoxSizer(wxVERTICAL);
        // Add stretchable space to push the icon/text downwards
        sizer->AddStretchSpacer(1);
        // Icon drawing is handled in OnPaint, so we just add the label at the bottom
        sizer->Add(label, 0, wxEXPAND | wxBOTTOM, 8);

        SetSizer(sizer);

        Bind(wxEVT_PAINT, &IconItemPanel::OnPaint, this);
    }

private:
    std::string icon_name_;
    core::ThemeEngine& theme_engine_;

    void OnPaint(wxPaintEvent& /*event*/)
    {
        wxAutoBufferedPaintDC dc(this);

        auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
        dc.SetBrush(wxBrush(bg_color));
        dc.SetPen(wxPen(theme_engine_.color(core::ThemeColorToken::BorderLight)));
        dc.DrawRectangle(GetClientRect());

        // Draw icon in the center
        int icon_size = 32;
        wxSize sz = GetClientSize();
        int icon_x = (sz.GetWidth() - icon_size) / 2;
        int icon_y = (sz.GetHeight() - icon_size - 20) / 2; // Offset to make room for text

        auto icon_color = theme_engine_.color(core::ThemeColorToken::TextMain);
        IconManager::get().draw_icon(dc,
                                     icon_name_,
                                     icon_x,
                                     icon_y,
                                     wxSize(icon_size, icon_size),
                                     icon_color,
                                     GetDPIScaleFactor());
    }
};
} // namespace

IconGalleryDialog::IconGalleryDialog(wxWindow* parent, core::ThemeEngine& theme_engine)
    : wxDialog(parent,
               wxID_ANY,
               "Icon Gallery",
               wxDefaultPosition,
               wxSize(800, 600),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    , theme_engine_(theme_engine)
{
    SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::BgApp));
    setupLayout();
    renderIcons();
    CenterOnParent();
}

void IconGalleryDialog::setupLayout()
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    scroll_panel_ =
        new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    scroll_panel_->SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::BgApp));
    scroll_panel_->SetScrollRate(0, 20);

    auto* wrap_sizer = new wxWrapSizer(wxHORIZONTAL);
    scroll_panel_->SetSizer(wrap_sizer);

    main_sizer->Add(scroll_panel_, 1, wxEXPAND | wxALL, 16);
    SetSizer(main_sizer);
}

void IconGalleryDialog::renderIcons()
{
    auto icon_names = IconManager::get().registry().get_icon_names();
    auto* sizer = scroll_panel_->GetSizer();

    for (const auto& name : icon_names)
    {
        auto* item = new IconItemPanel(scroll_panel_, name, theme_engine_);
        sizer->Add(item, 0, wxALL, 4);
    }

    scroll_panel_->Layout();
}

} // namespace markamp::ui
