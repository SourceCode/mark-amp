// ============================================================================
// File: src/ui/PDFAnnotationSidebar.cpp
// Phase 31: PDF Annotation System — Annotation sidebar implementation (stub)
// ============================================================================
#include "PDFAnnotationSidebar.h"

#include <wx/dcclient.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(PDFAnnotationSidebar, wxPanel) EVT_PAINT(PDFAnnotationSidebar::on_paint)
    wxEND_EVENT_TABLE()

        PDFAnnotationSidebar::PDFAnnotationSidebar(wxWindow* parent,
                                                   core::EventBus& event_bus,
                                                   core::PDFAnnotationStore& annotation_store)
    : wxPanel(parent, wxID_ANY)
    , event_bus_(event_bus)
    , annotation_store_(annotation_store)
{
    SetMinSize(wxSize(200, -1));
    SetBackgroundColour(wxColour(250, 250, 250));
}

void PDFAnnotationSidebar::refresh_for_document(const std::string& pdf_path)
{
    current_pdf_ = pdf_path;
    Refresh();
}

void PDFAnnotationSidebar::select_annotation(const std::string& /*annotation_id*/)
{
    // Stub: scroll to and highlight the annotation in the list
    Refresh();
}

void PDFAnnotationSidebar::on_paint(wxPaintEvent& /*evt*/)
{
    wxPaintDC dc(this);
    dc.SetTextForeground(wxColour(128, 128, 128));
    dc.DrawText("Annotations: " + current_pdf_, 10, 10);
}

} // namespace markamp::ui
