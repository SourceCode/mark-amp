// ============================================================================
// File: src/ui/PDFViewerPanel.cpp
// Phase 31: PDF Annotation System — PDF viewer panel implementation (stub)
// ============================================================================
#include "PDFViewerPanel.h"

#include "core/Theme.h"

#include <wx/dcclient.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(PDFViewerPanel, wxPanel) EVT_PAINT(PDFViewerPanel::on_paint)
    EVT_LEFT_UP(PDFViewerPanel::on_mouse_left_up) EVT_KEY_DOWN(PDFViewerPanel::on_key_down)
        wxEND_EVENT_TABLE()

            PDFViewerPanel::PDFViewerPanel(wxWindow* parent,
                                           core::EventBus& event_bus,
                                           core::PDFAnnotationStore& annotation_store)
    : wxPanel(parent, wxID_ANY)
    , event_bus_(event_bus)
    , annotation_store_(annotation_store)
{
    const core::Theme default_theme;
    SetBackgroundColour(default_theme.colors.bg_panel.to_wx_colour());
}

auto PDFViewerPanel::load_document(const std::string& path) -> bool
{
    document_path_ = path;
    current_page_ = 0;

    // Improvement 75: Set initial page count (would be from PDF library in production)
    page_count_ = 1;
    if (!path.empty())
    {
        // Heuristic: if file exists, assume at least 1 page
        page_count_ = std::max(1, page_count_);
    }

    Refresh();
    return true;
}

void PDFViewerPanel::go_to_page(int page_number)
{
    if (page_number >= 0 && page_number < page_count_)
    {
        current_page_ = page_number;
        Refresh();
    }
}

auto PDFViewerPanel::current_page() const -> int
{
    return current_page_;
}

auto PDFViewerPanel::page_count() const -> int
{
    return page_count_;
}

void PDFViewerPanel::set_zoom(double zoom_level)
{
    zoom_level_ = std::clamp(zoom_level, 0.25, 5.0);
    Refresh();
}

auto PDFViewerPanel::zoom() const -> double
{
    return zoom_level_;
}

void PDFViewerPanel::on_paint(wxPaintEvent& /*evt*/)
{
    wxPaintDC dc(this);
    const core::Theme default_theme;
    dc.SetTextForeground(default_theme.colors.text_muted.to_wx_colour());
    dc.DrawText(wxString::Format("PDF Viewer: %s (Page %d/%d, Zoom %.0f%%)",
                                 document_path_,
                                 current_page_ + 1,
                                 page_count_,
                                 zoom_level_ * 100.0),
                20,
                20);
}

void PDFViewerPanel::on_mouse_left_up(wxMouseEvent& evt)
{
    // Improvement 76: Handle annotation creation at click position
    const auto click_pos = evt.GetPosition();
    const double doc_x = static_cast<double>(click_pos.x) / zoom_level_;
    const double doc_y = static_cast<double>(click_pos.y) / zoom_level_;

    // Store click position for annotation — actual annotation creation
    // is delegated to the annotation store via the event bus
    last_click_x_ = doc_x;
    last_click_y_ = doc_y;
    Refresh();
}

void PDFViewerPanel::on_key_down(wxKeyEvent& evt)
{
    // Page navigation via Page Up/Down
    if (evt.GetKeyCode() == WXK_PAGEDOWN)
    {
        go_to_page(current_page_ + 1);
    }
    else if (evt.GetKeyCode() == WXK_PAGEUP)
    {
        go_to_page(current_page_ - 1);
    }
    else
    {
        evt.Skip();
    }
}

} // namespace markamp::ui
