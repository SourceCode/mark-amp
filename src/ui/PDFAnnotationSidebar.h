// ============================================================================
// File: src/ui/PDFAnnotationSidebar.h
// Phase 31: PDF Annotation System — Annotation list sidebar (stub)
// ============================================================================
#pragma once

#include <wx/panel.h>

#include <string>

namespace markamp::core
{
class EventBus;
class PDFAnnotationStore;
} // namespace markamp::core

namespace markamp::ui
{

/// Stub sidebar panel displaying a list of PDF annotations
/// grouped by page. Supports clicking an annotation to scroll
/// the PDF viewer to that location.
class PDFAnnotationSidebar : public wxPanel
{
public:
    PDFAnnotationSidebar(wxWindow* parent,
                         core::EventBus& event_bus,
                         core::PDFAnnotationStore& annotation_store);

    /// Refresh the annotation list for a specific PDF.
    void refresh_for_document(const std::string& pdf_path);

    /// Select an annotation by ID in the list.
    void select_annotation(const std::string& annotation_id);

private:
    core::EventBus& event_bus_;
    core::PDFAnnotationStore& annotation_store_;
    std::string current_pdf_;
    std::string selected_annotation_id_;

    void on_paint(wxPaintEvent& evt);

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
