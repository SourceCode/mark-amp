#pragma once

#include <wx/wx.h>

namespace markamp::ui
{

/// A panel that draws a beveled border to create the retro-futuristic inset/outset effect.
/// This is a core visual component of the MarkAmp aesthetic.
class BevelPanel : public wxPanel
{
public:
    enum class Style
    {
        Raised, ///< top/left highlight, bottom/right shadow
        Sunken, ///< top/left shadow, bottom/right highlight
        Flat    ///< no bevel, just background fill
    };

    BevelPanel(wxWindow* parent,
               wxWindowID id = wxID_ANY,
               Style style = Style::Sunken,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize);

    void set_style(Style style);
    [[nodiscard]] auto get_style() const -> Style;

    /// Override to allow custom shadow/highlight colours.
    void set_shadow_colour(const wxColour& colour);
    void set_highlight_colour(const wxColour& colour);

    [[nodiscard]] auto HasTransparentBackground() -> bool override
    {
        return true;
    }

protected:
    void OnPaint(wxPaintEvent& event);

private:
    Style style_;
    wxColour shadow_colour_{0, 0, 0, 51};          // black @ 20% opacity
    wxColour highlight_colour_{255, 255, 255, 13}; // white @ 5% opacity

    void drawBevel(wxDC& dc, const wxSize& size);
};

} // namespace markamp::ui
