// ============================================================================
// File: src/ui/PDFThumbnailStrip.cpp
// Phase 31: PDF Annotation System — Thumbnail strip implementation (stub)
// ============================================================================
#include "PDFThumbnailStrip.h"

#include <wx/dcclient.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(PDFThumbnailStrip, wxPanel) EVT_PAINT(PDFThumbnailStrip::on_paint)
    EVT_LEFT_UP(PDFThumbnailStrip::on_mouse_left_up) wxEND_EVENT_TABLE()

        PDFThumbnailStrip::PDFThumbnailStrip(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    SetMinSize(wxSize(-1, 80));
    SetBackgroundColour(wxColour(240, 240, 240));
}

void PDFThumbnailStrip::set_page_count(int count)
{
    page_count_ = count;
    Refresh();
}

void PDFThumbnailStrip::set_current_page(int page_number)
{
    current_page_ = page_number;
    Refresh();
}

void PDFThumbnailStrip::set_page_selected_callback(PageSelectedCallback callback)
{
    on_page_selected_ = std::move(callback);
}

void PDFThumbnailStrip::on_paint(wxPaintEvent& /*evt*/)
{
    wxPaintDC dc(this);
    constexpr int kThumbWidth = 60;
    constexpr int kThumbHeight = 70;
    constexpr int kPadding = 4;

    for (int idx = 0; idx < page_count_; ++idx)
    {
        const int pos_x = kPadding + idx * (kThumbWidth + kPadding);
        const int pos_y = kPadding;

        // Draw thumbnail placeholder
        if (idx == current_page_)
        {
            dc.SetPen(wxPen(wxColour(66, 133, 244), 2));
        }
        else
        {
            dc.SetPen(wxPen(wxColour(200, 200, 200), 1));
        }

        dc.SetBrush(*wxWHITE_BRUSH);
        dc.DrawRectangle(pos_x, pos_y, kThumbWidth, kThumbHeight);

        dc.SetTextForeground(wxColour(128, 128, 128));
        dc.DrawText(wxString::Format("%d", idx + 1),
                    pos_x + kThumbWidth / 2 - 4,
                    pos_y + kThumbHeight / 2 - 6);
    }
}

void PDFThumbnailStrip::on_mouse_left_up(wxMouseEvent& evt)
{
    constexpr int kThumbWidth = 60;
    constexpr int kPadding = 4;

    const int clicked_page = (evt.GetX() - kPadding) / (kThumbWidth + kPadding);

    if (clicked_page >= 0 && clicked_page < page_count_)
    {
        set_current_page(clicked_page);
        if (on_page_selected_)
        {
            on_page_selected_(clicked_page);
        }
    }
}

} // namespace markamp::ui
