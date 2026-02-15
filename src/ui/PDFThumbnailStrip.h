// ============================================================================
// File: src/ui/PDFThumbnailStrip.h
// Phase 31: PDF Annotation System — Page thumbnail strip (stub)
// ============================================================================
#pragma once

#include <wx/panel.h>

#include <functional>
#include <string>

namespace markamp::ui
{

/// Stub panel showing a horizontal strip of page thumbnails
/// for quick PDF page navigation.
class PDFThumbnailStrip : public wxPanel
{
public:
    explicit PDFThumbnailStrip(wxWindow* parent);

    /// Set the total number of pages.
    void set_page_count(int count);

    /// Highlight the currently viewed page.
    void set_current_page(int page_number);

    /// Register a callback for when a thumbnail is clicked.
    using PageSelectedCallback = std::function<void(int page_number)>;
    void set_page_selected_callback(PageSelectedCallback callback);

private:
    int page_count_{0};
    int current_page_{0};
    PageSelectedCallback on_page_selected_;

    void on_paint(wxPaintEvent& evt);
    void on_mouse_left_up(wxMouseEvent& evt);

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
