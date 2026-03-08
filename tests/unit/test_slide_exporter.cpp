// test_slide_exporter.cpp — 10 tests for SlideExporter types and operations
#include "core/SlideExporter.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SlideExportOptions defaults", "[presentation][export]")
{
    SlideExportOptions options;
    CHECK(options.format == SlideExportFormat::kPdf);
    CHECK(options.output_path.empty());
    CHECK_FALSE(options.include_speaker_notes);
    CHECK_FALSE(options.include_hidden_slides);
    CHECK(options.image_dpi == 150);
}

TEST_CASE("SlideExportResult defaults", "[presentation][export]")
{
    SlideExportResult result;
    CHECK(result.export_id.empty());
    CHECK(result.format == SlideExportFormat::kPdf);
    CHECK(result.status == ExportStatus::kPending);
    CHECK(result.slides_exported == 0);
    CHECK(result.file_size_bytes == 0);
    CHECK(result.error_message.empty());
}

TEST_CASE("SlideExportFormat enum values", "[presentation][export]")
{
    CHECK(SlideExportFormat::kPdf != SlideExportFormat::kHtml);
    CHECK(SlideExportFormat::kPng != SlideExportFormat::kSvg);
    CHECK(SlideExportFormat::kPptx != SlideExportFormat::kPdf);
}

TEST_CASE("ExportStatus enum values", "[presentation][export]")
{
    CHECK(ExportStatus::kPending != ExportStatus::kInProgress);
    CHECK(ExportStatus::kCompleted != ExportStatus::kFailed);
}

TEST_CASE("slide_export_format_name returns names", "[presentation][export]")
{
    auto pdf = slide_export_format_name(SlideExportFormat::kPdf);
    auto html = slide_export_format_name(SlideExportFormat::kHtml);
    CHECK_FALSE(pdf.empty());
    CHECK_FALSE(html.empty());
    CHECK(pdf != html);
}

TEST_CASE("SlideExporter starts empty", "[presentation][export]")
{
    SlideExporter exporter;
    CHECK(exporter.export_count() == 0);
}

TEST_CASE("SlideExporter export_presentation creates record", "[presentation][export]")
{
    SlideExporter exporter;
    SlideExportOptions options;
    options.format = SlideExportFormat::kHtml;
    options.output_path = "/tmp/slides.html";
    auto export_id = exporter.export_presentation("pres-1", options);
    CHECK_FALSE(export_id.empty());
    CHECK(exporter.export_count() == 1);
}

TEST_CASE("SlideExporter find_export", "[presentation][export]")
{
    SlideExporter exporter;
    SlideExportOptions options;
    options.format = SlideExportFormat::kPdf;
    auto eid = exporter.export_presentation("pres-1", options);
    auto found = exporter.find_export(eid);
    REQUIRE(found != nullptr);
    CHECK(found->format == SlideExportFormat::kPdf);
}

TEST_CASE("SlideExporter clear_history", "[presentation][export]")
{
    SlideExporter exporter;
    SlideExportOptions options;
    exporter.export_presentation("pres-1", options);
    exporter.clear_history();
    CHECK(exporter.export_count() == 0);
}

TEST_CASE("SlideExporter exports_by_format", "[presentation][export]")
{
    SlideExporter exporter;
    SlideExportOptions pdf_opts;
    pdf_opts.format = SlideExportFormat::kPdf;
    SlideExportOptions html_opts;
    html_opts.format = SlideExportFormat::kHtml;
    exporter.export_presentation("pres-1", pdf_opts);
    exporter.export_presentation("pres-2", html_opts);
    auto pdfs = exporter.exports_by_format(SlideExportFormat::kPdf);
    CHECK(pdfs.size() == 1);
}
