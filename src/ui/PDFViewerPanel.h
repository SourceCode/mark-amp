// ============================================================================
// File: src/ui/PDFViewerPanel.h
// Phase 31: PDF Annotation System — PDF viewer panel (stub)
// ============================================================================
#pragma once

#include <wx/panel.h>

#include <functional>
#include <string>

namespace markamp::core
{
class EventBus;
class PDFAnnotationStore;
} // namespace markamp::core

namespace markamp::ui
{

/// Stub panel for viewing PDF documents with annotation overlay.
/// Full rendering requires integration with a PDF rendering library
/// (e.g., Poppler, MuPDF, or PDFium).
class PDFViewerPanel : public wxPanel
{
public:
    PDFViewerPanel(wxWindow* parent,
                   core::EventBus& event_bus,
                   core::PDFAnnotationStore& annotation_store);

    /// Load a PDF document from a file path.
    auto load_document(const std::string& path) -> bool;

    /// Navigate to a specific page (0-based).
    void go_to_page(int page_number);

    /// Get the currently displayed page number (0-based).
    [[nodiscard]] auto current_page() const -> int;

    /// Get the total number of pages.
    [[nodiscard]] auto page_count() const -> int;

    /// Set the zoom level (1.0 = 100%).
    void set_zoom(double zoom_level);

    /// Get the current zoom level.
    [[nodiscard]] auto zoom() const -> double;

private:
    core::EventBus& event_bus_;
    core::PDFAnnotationStore& annotation_store_;
    std::string document_path_;
    int current_page_{0};
    int page_count_{0};
    double zoom_level_{1.0};
    double last_click_x_{0.0};
    double last_click_y_{0.0};

    void on_paint(wxPaintEvent& evt);
    void on_mouse_left_up(wxMouseEvent& evt);
    void on_key_down(wxKeyEvent& evt);

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
