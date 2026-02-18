/// @file test_phase42_pdf_import.cpp
/// @brief Comprehensive tests for Phase 42 — PDF Viewer & Document Import.

#include "core/CommandRegistry.h"
#include "core/DocumentImporter.h"
#include "core/Events.h"
#include "core/ImportCommandProvider.h"
#include "core/ImportPresetRegistry.h"
#include "core/PdfAnnotationManager.h"
#include "core/PdfCommandProvider.h"
#include "core/PdfViewerService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;

// ═══════════════════════════════════════════════════════════════════
// PdfViewerService Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("PdfViewerService: load and find document", "[phase42][pdf]")
{
    PdfViewerService svc;
    auto doc_id = svc.load_document("/docs/report.pdf");

    REQUIRE(svc.document_count() == 1);
    const auto* doc = svc.find_document(doc_id);
    REQUIRE(doc != nullptr);
    REQUIRE(doc->title == "report.pdf");
    REQUIRE(doc->total_pages == 10);
    REQUIRE(doc->loaded == true);
}

TEST_CASE("PdfViewerService: close document", "[phase42][pdf]")
{
    PdfViewerService svc;
    auto doc_id = svc.load_document("/docs/report.pdf");
    REQUIRE(svc.close_document(doc_id) == true);
    REQUIRE(svc.document_count() == 0);
    REQUIRE(svc.close_document("nonexistent") == false);
}

TEST_CASE("PdfViewerService: page navigation", "[phase42][pdf]")
{
    PdfViewerService svc;
    auto doc_id = svc.load_document("/docs/report.pdf");

    REQUIRE(svc.go_to_page(doc_id, 5) == true);
    REQUIRE(svc.find_document(doc_id)->current_page == 5);

    REQUIRE(svc.next_page(doc_id) == true);
    REQUIRE(svc.find_document(doc_id)->current_page == 6);

    REQUIRE(svc.previous_page(doc_id) == true);
    REQUIRE(svc.find_document(doc_id)->current_page == 5);

    // Bounds
    REQUIRE(svc.go_to_page(doc_id, 10) == true);
    REQUIRE(svc.next_page(doc_id) == false);
    REQUIRE(svc.go_to_page(doc_id, 1) == true);
    REQUIRE(svc.previous_page(doc_id) == false);
    REQUIRE(svc.go_to_page(doc_id, 0) == false);
    REQUIRE(svc.go_to_page(doc_id, 11) == false);
}

TEST_CASE("PdfViewerService: zoom control", "[phase42][pdf]")
{
    PdfViewerService svc;
    auto doc_id = svc.load_document("/docs/report.pdf");

    REQUIRE(svc.set_zoom(doc_id, 2.0) == true);
    REQUIRE(svc.find_document(doc_id)->zoom_level == 2.0);
    REQUIRE(svc.find_document(doc_id)->zoom_mode == PdfZoomMode::kCustom);

    REQUIRE(svc.set_zoom(doc_id, 0.05) == false); // Too small
    REQUIRE(svc.set_zoom(doc_id, 11.0) == false); // Too large
}

TEST_CASE("PdfViewerService: zoom mode", "[phase42][pdf]")
{
    PdfViewerService svc;
    auto doc_id = svc.load_document("/docs/report.pdf");

    REQUIRE(svc.set_zoom_mode(doc_id, PdfZoomMode::kActualSize) == true);
    REQUIRE(svc.find_document(doc_id)->zoom_level == 1.0);
}

TEST_CASE("PdfViewerService: zoom in/out", "[phase42][pdf]")
{
    PdfViewerService svc;
    auto doc_id = svc.load_document("/docs/report.pdf");

    double before = svc.find_document(doc_id)->zoom_level;
    REQUIRE(svc.zoom_in(doc_id) == true);
    REQUIRE(svc.find_document(doc_id)->zoom_level > before);

    before = svc.find_document(doc_id)->zoom_level;
    REQUIRE(svc.zoom_out(doc_id) == true);
    REQUIRE(svc.find_document(doc_id)->zoom_level < before);
}

TEST_CASE("PdfViewerService: extract text", "[phase42][pdf]")
{
    PdfViewerService svc;
    auto doc_id = svc.load_document("/docs/report.pdf");

    auto text = svc.extract_text(doc_id, 1);
    REQUIRE(text == "Page 1 content");

    auto all_text = svc.extract_all_text(doc_id);
    REQUIRE(all_text.find("Page 1 content") != std::string::npos);
    REQUIRE(all_text.find("Page 10 content") != std::string::npos);
}

TEST_CASE("PdfViewerService: render quality", "[phase42][pdf]")
{
    PdfViewerService svc;
    auto doc_id = svc.load_document("/docs/report.pdf");

    REQUIRE(svc.set_render_quality(doc_id, PdfRenderQuality::kHigh) == true);
    REQUIRE(svc.find_document(doc_id)->quality == PdfRenderQuality::kHigh);
}

TEST_CASE("PdfViewerService: multiple documents", "[phase42][pdf]")
{
    PdfViewerService svc;
    svc.load_document("/docs/a.pdf");
    svc.load_document("/docs/b.pdf");

    REQUIRE(svc.document_count() == 2);
    REQUIRE(svc.all_documents().size() == 2);
    REQUIRE(svc.open_document_ids().size() == 2);

    svc.close_all();
    REQUIRE(svc.document_count() == 0);
}

// ═══════════════════════════════════════════════════════════════════
// DocumentImporter Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("DocumentImporter: import file", "[phase42][import]")
{
    DocumentImporter importer;
    auto imp_id = importer.import_file("/docs/report.docx", ImportFormat::kDocx);

    REQUIRE(importer.import_count() == 1);
    const auto* result = importer.find_import(imp_id);
    REQUIRE(result != nullptr);
    REQUIRE(result->status == ImportStatus::kCompleted);
    REQUIRE(result->converted_markdown.find("# report") != std::string::npos);
}

TEST_CASE("DocumentImporter: import with metadata", "[phase42][import]")
{
    DocumentImporter importer;
    ImportOptions opts;
    opts.include_metadata = true;
    auto imp_id = importer.import_file("/docs/notes.html", ImportFormat::kHtml, opts);

    const auto* result = importer.find_import(imp_id);
    REQUIRE(result->converted_markdown.find("source:") != std::string::npos);
}

TEST_CASE("DocumentImporter: format detection", "[phase42][import]")
{
    REQUIRE(DocumentImporter::detect_format("/docs/file.docx") == ImportFormat::kDocx);
    REQUIRE(DocumentImporter::detect_format("/docs/file.html") == ImportFormat::kHtml);
    REQUIRE(DocumentImporter::detect_format("/docs/file.csv") == ImportFormat::kCsv);
    REQUIRE(DocumentImporter::detect_format("/docs/file.rtf") == ImportFormat::kRtf);
    REQUIRE(DocumentImporter::detect_format("/docs/file.epub") == ImportFormat::kEpub);
    REQUIRE(DocumentImporter::detect_format("/docs/file.tex") == ImportFormat::kLatex);
    REQUIRE(DocumentImporter::detect_format("/docs/file.org") == ImportFormat::kOrg);
    REQUIRE(DocumentImporter::detect_format("/docs/file.txt") == ImportFormat::kTxt);
    REQUIRE(DocumentImporter::detect_format("/docs/noext") == ImportFormat::kTxt);
}

TEST_CASE("DocumentImporter: supported extensions", "[phase42][import]")
{
    auto exts = DocumentImporter::supported_extensions();
    REQUIRE(exts.size() == 10);
}

TEST_CASE("DocumentImporter: status counts", "[phase42][import]")
{
    DocumentImporter importer;
    importer.import_file("/a.txt", ImportFormat::kTxt);
    importer.import_file("/b.csv", ImportFormat::kCsv);

    REQUIRE(importer.completed_count() == 2);
    REQUIRE(importer.pending_count() == 0);
}

TEST_CASE("DocumentImporter: imports_by_status", "[phase42][import]")
{
    DocumentImporter importer;
    importer.import_file("/a.txt", ImportFormat::kTxt);

    auto completed = importer.imports_by_status(ImportStatus::kCompleted);
    REQUIRE(completed.size() == 1);
}

TEST_CASE("DocumentImporter: clear_history", "[phase42][import]")
{
    DocumentImporter importer;
    importer.import_file("/a.txt", ImportFormat::kTxt);
    importer.clear_history();
    REQUIRE(importer.import_count() == 0);
}

TEST_CASE("DocumentImporter: format names", "[phase42][import]")
{
    REQUIRE(import_format_name(ImportFormat::kDocx) == "docx");
    REQUIRE(import_format_name(ImportFormat::kHtml) == "html");
    REQUIRE(import_format_name(ImportFormat::kCsv) == "csv");
}

// ═══════════════════════════════════════════════════════════════════
// PdfAnnotationManager Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("PdfAnnotationManager: add and find", "[phase42][annotations]")
{
    PdfAnnotationManager mgr;
    PdfAnnotation ann;
    ann.document_id = "doc1";
    ann.page_number = 1;
    ann.annotation_type = PdfAnnotationType::kHighlight;
    ann.text = "Important text";
    ann.color = "#FFD700";
    auto ann_id = mgr.add_annotation(std::move(ann));

    REQUIRE(mgr.annotation_count() == 1);
    const auto* found = mgr.find_annotation(ann_id);
    REQUIRE(found != nullptr);
    REQUIRE(found->text == "Important text");
}

TEST_CASE("PdfAnnotationManager: remove", "[phase42][annotations]")
{
    PdfAnnotationManager mgr;
    PdfAnnotation ann;
    ann.document_id = "doc1";
    auto ann_id = mgr.add_annotation(std::move(ann));

    REQUIRE(mgr.remove_annotation(ann_id) == true);
    REQUIRE(mgr.annotation_count() == 0);
    REQUIRE(mgr.remove_annotation("nonexistent") == false);
}

TEST_CASE("PdfAnnotationManager: annotations_for_document", "[phase42][annotations]")
{
    PdfAnnotationManager mgr;
    PdfAnnotation a1;
    a1.document_id = "doc1";
    mgr.add_annotation(std::move(a1));

    PdfAnnotation a2;
    a2.document_id = "doc2";
    mgr.add_annotation(std::move(a2));

    auto doc1_anns = mgr.annotations_for_document("doc1");
    REQUIRE(doc1_anns.size() == 1);
}

TEST_CASE("PdfAnnotationManager: annotations_for_page", "[phase42][annotations]")
{
    PdfAnnotationManager mgr;
    PdfAnnotation a1;
    a1.document_id = "doc1";
    a1.page_number = 3;
    mgr.add_annotation(std::move(a1));

    PdfAnnotation a2;
    a2.document_id = "doc1";
    a2.page_number = 5;
    mgr.add_annotation(std::move(a2));

    REQUIRE(mgr.annotations_for_page("doc1", 3).size() == 1);
    REQUIRE(mgr.annotations_for_page("doc1", 5).size() == 1);
    REQUIRE(mgr.annotations_for_page("doc1", 1).empty());
}

TEST_CASE("PdfAnnotationManager: annotations_by_type", "[phase42][annotations]")
{
    PdfAnnotationManager mgr;
    PdfAnnotation a1;
    a1.annotation_type = PdfAnnotationType::kHighlight;
    mgr.add_annotation(std::move(a1));

    PdfAnnotation a2;
    a2.annotation_type = PdfAnnotationType::kComment;
    mgr.add_annotation(std::move(a2));

    REQUIRE(mgr.annotations_by_type(PdfAnnotationType::kHighlight).size() == 1);
    REQUIRE(mgr.annotations_by_type(PdfAnnotationType::kComment).size() == 1);
}

TEST_CASE("PdfAnnotationManager: export_annotations", "[phase42][annotations]")
{
    PdfAnnotationManager mgr;
    PdfAnnotation ann;
    ann.document_id = "doc1";
    ann.page_number = 2;
    ann.text = "Highlighted text";
    ann.comment = "Note about this";
    mgr.add_annotation(std::move(ann));

    auto exported = mgr.export_annotations("doc1");
    REQUIRE(exported.find("Page 2") != std::string::npos);
    REQUIRE(exported.find("Highlighted text") != std::string::npos);
    REQUIRE(exported.find("Note about this") != std::string::npos);
}

TEST_CASE("PdfAnnotationManager: clear_all", "[phase42][annotations]")
{
    PdfAnnotationManager mgr;
    PdfAnnotation ann;
    mgr.add_annotation(std::move(ann));
    mgr.clear_all();
    REQUIRE(mgr.annotation_count() == 0);
}

// ═══════════════════════════════════════════════════════════════════
// ImportPresetRegistry Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("ImportPresetRegistry: register and find", "[phase42][presets]")
{
    ImportPresetRegistry reg;
    ImportPreset preset;
    preset.preset_id = "custom1";
    preset.name = "My Preset";
    reg.register_preset(std::move(preset));

    REQUIRE(reg.preset_count() == 1);
    REQUIRE(reg.find_preset("custom1")->name == "My Preset");
}

TEST_CASE("ImportPresetRegistry: remove", "[phase42][presets]")
{
    ImportPresetRegistry reg;
    ImportPreset preset;
    preset.preset_id = "custom1";
    reg.register_preset(std::move(preset));

    REQUIRE(reg.remove_preset("custom1") == true);
    REQUIRE(reg.preset_count() == 0);
}

TEST_CASE("ImportPresetRegistry: load_defaults", "[phase42][presets]")
{
    ImportPresetRegistry reg;
    reg.load_defaults();

    REQUIRE(reg.preset_count() == 3);
    REQUIRE(reg.find_preset("default_docx") != nullptr);
    REQUIRE(reg.find_preset("default_html") != nullptr);
    REQUIRE(reg.find_preset("default_txt") != nullptr);
}

TEST_CASE("ImportPresetRegistry: set_default", "[phase42][presets]")
{
    ImportPresetRegistry reg;
    reg.load_defaults();

    REQUIRE(reg.set_default("default_html") == true);
    REQUIRE(reg.default_preset()->preset_id == "default_html");
}

TEST_CASE("ImportPresetRegistry: mark_used", "[phase42][presets]")
{
    ImportPresetRegistry reg;
    ImportPreset preset;
    preset.preset_id = "p1";
    reg.register_preset(std::move(preset));

    REQUIRE(reg.mark_used("p1") == true);
    REQUIRE(reg.mark_used("p1") == true);
    REQUIRE(reg.find_preset("p1")->usage_count == 2);
}

TEST_CASE("ImportPresetRegistry: presets_for_format", "[phase42][presets]")
{
    ImportPresetRegistry reg;
    reg.load_defaults();

    auto docx_presets = reg.presets_for_format(ImportFormat::kDocx);
    REQUIRE(docx_presets.size() == 1);
}

// ═══════════════════════════════════════════════════════════════════
// PdfCommandProvider Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("PdfCommandProvider: count and ids", "[phase42][pdf-commands]")
{
    REQUIRE(PdfCommandProvider::command_count() == 8);
    auto ids = PdfCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "pdf.openFile");
}

TEST_CASE("PdfCommandProvider: register to registry", "[phase42][pdf-commands]")
{
    CommandRegistry registry;
    PdfCommandProvider provider;
    provider.register_commands(registry);

    REQUIRE(registry.get_command("pdf.openFile") != nullptr);
    REQUIRE(registry.get_command("pdf.openFile")->category == "PDF");
}

// ═══════════════════════════════════════════════════════════════════
// ImportCommandProvider Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("ImportCommandProvider: count and ids", "[phase42][import-commands]")
{
    REQUIRE(ImportCommandProvider::command_count() == 8);
    auto ids = ImportCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "import.file");
}

TEST_CASE("ImportCommandProvider: register to registry", "[phase42][import-commands]")
{
    CommandRegistry registry;
    ImportCommandProvider provider;
    provider.register_commands(registry);

    REQUIRE(registry.get_command("import.file") != nullptr);
    REQUIRE(registry.get_command("import.file")->category == "Import");
}

// ═══════════════════════════════════════════════════════════════════
// Phase 42 Events Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Phase 42 Events: PdfDocumentLoadedEvent", "[phase42][events]")
{
    PdfDocumentLoadedEvent event;
    event.document_id = "pdf_1";
    event.file_path = "/docs/report.pdf";
    event.total_pages = 25;
    REQUIRE(event.total_pages == 25);
}

TEST_CASE("Phase 42 Events: PdfPageNavigatedEvent", "[phase42][events]")
{
    PdfPageNavigatedEvent event;
    event.document_id = "pdf_1";
    event.page_number = 7;
    REQUIRE(event.page_number == 7);
}

TEST_CASE("Phase 42 Events: PdfAnnotationChangedEvent", "[phase42][events]")
{
    PdfAnnotationChangedEvent event;
    event.annotation_id = "ann_1";
    event.document_id = "pdf_1";
    event.action = "added";
    REQUIRE(event.action == "added");
}

TEST_CASE("Phase 42 Events: DocumentImportCompletedEvent", "[phase42][events]")
{
    DocumentImportCompletedEvent event;
    event.import_id = "imp_1";
    event.source_path = "/docs/paper.docx";
    event.format = "docx";
    event.success = true;
    REQUIRE(event.success == true);
}

TEST_CASE("Phase 42 Events: ImportPresetChangedEvent", "[phase42][events]")
{
    ImportPresetChangedEvent event;
    event.preset_id = "p1";
    event.action = "created";
    REQUIRE(event.action == "created");
}

TEST_CASE("Phase 42 Events: PdfTextExtractedEvent", "[phase42][events]")
{
    PdfTextExtractedEvent event;
    event.document_id = "pdf_1";
    event.page_count = 10;
    event.text_length = 5000;
    REQUIRE(event.text_length == 5000);
}
