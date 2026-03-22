/// @file test_v24_p16_rendering_export.cpp
/// @brief V24 Phase 16 tests: Export/import types, format support, publishing.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/ExportTypes.h"
#include "../../src/core/ImportTypes.h"

using namespace markamp::core;

// P16-T01: Export format enumeration
TEST_CASE("P16-T01 export format values", "[v24][p16]") {
    REQUIRE(ExportFormat::Markdown != ExportFormat::HTML);
    REQUIRE(ExportFormat::HTML != ExportFormat::PDF);
    REQUIRE(ExportFormat::PDF != ExportFormat::DOCX);
}

TEST_CASE("P16-T01 export options defaults", "[v24][p16]") {
    ExportOptions opts;
    REQUIRE(opts.format == ExportFormat::Markdown);
    REQUIRE(opts.include_frontmatter);
    REQUIRE(opts.include_assets);
}

// P16-T02: Import format enumeration
TEST_CASE("P16-T02 import format values", "[v24][p16]") {
    REQUIRE(ImportFormat::Markdown != ImportFormat::HTML);
    REQUIRE(ImportFormat::HTML != ImportFormat::DOCX);
}

TEST_CASE("P16-T02 import options structure", "[v24][p16]") {
    ImportOptions opts;
    opts.format = ImportFormat::Markdown;
    opts.source_path = "/docs/import.md";
    REQUIRE(opts.format == ImportFormat::Markdown);
    REQUIRE(opts.import_assets);
}

// P16-T03: Export result
TEST_CASE("P16-T03 export result structure", "[v24][p16]") {
    ExportResult result;
    result.success = true;
    result.output_path = "/tmp/out.html";
    result.elapsed_ms = 42;
    REQUIRE(result.success);
    REQUIRE(result.elapsed_ms > 0);
}

TEST_CASE("P16-T03 export result failure", "[v24][p16]") {
    ExportResult result;
    result.success = false;
    result.error_message = "Pandoc not found";
    REQUIRE_FALSE(result.success);
    REQUIRE_FALSE(result.error_message.empty());
}

// P16-T04: Import result
TEST_CASE("P16-T04 import result success", "[v24][p16]") {
    ImportResult result;
    result.success = true;
    result.documents_imported = 3;
    REQUIRE(result.success);
    REQUIRE(result.documents_imported == 3);
}

TEST_CASE("P16-T04 import result failure", "[v24][p16]") {
    ImportResult result;
    result.success = false;
    result.errors.push_back("Unsupported format");
    REQUIRE_FALSE(result.success);
    REQUIRE(result.errors_count == 0); // errors_count is separate from errors vector
}

// P16-T05: Export scope and format coverage
TEST_CASE("P16-T05 export scope values", "[v24][p16]") {
    REQUIRE(ExportScope::CurrentBlock != ExportScope::CurrentDocument);
    REQUIRE(ExportScope::CurrentDocument != ExportScope::SelectedBlocks);
    REQUIRE(ExportScope::SelectedBlocks != ExportScope::Workspace);
}

TEST_CASE("P16-T05 import conflict handling", "[v24][p16]") {
    REQUIRE(ImportConflict::Skip != ImportConflict::Overwrite);
    REQUIRE(ImportConflict::Overwrite != ImportConflict::Rename);
    REQUIRE(ImportConflict::Rename != ImportConflict::Merge);
}
