#include "canvas/Board.h"
#include "canvas/PDFImporter.h"
#include "canvas/PDFPageObject.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

using namespace markamp::canvas;

namespace
{

auto create_fake_pdf(const std::filesystem::path& path) -> void
{
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path);
    file << "%PDF-1.4 fake";
}

} // anonymous namespace

TEST_CASE("PDFPageObject: construction defaults", "[pdf_import]")
{
    PDFPageObject page;
    REQUIRE(page.page_number() == 0);
    REQUIRE(page.total_pages() == 0);
    REQUIRE(page.width() == 842.0);
    REQUIRE(page.height() == 1191.0);
    REQUIRE(page.type() == CanvasObjectType::PDFPage);
}

TEST_CASE("PDFPageObject: set page number", "[pdf_import]")
{
    PDFPageObject page;
    page.set_page_number(5);
    page.set_total_pages(10);
    REQUIRE(page.page_number() == 5);
    REQUIRE(page.total_pages() == 10);
}

TEST_CASE("PDFPageObject: JSON round-trip", "[pdf_import]")
{
    PDFPageObject page;
    page.set_page_number(2);
    page.set_total_pages(8);
    page.set_source_pdf("/docs/report.pdf");

    const auto json = page.to_json();
    REQUIRE(json.find("\"page_number\":2") != std::string::npos);
    REQUIRE(json.find("\"total_pages\":8") != std::string::npos);
    REQUIRE(json.find("report.pdf") != std::string::npos);
}

TEST_CASE("PDFImportOptions: defaults", "[pdf_import]")
{
    PDFImportOptions options;
    REQUIRE(options.dpi == 150);
    REQUIRE(options.page_gap == 50.0);
    REQUIRE(options.scale == 1.0);
    REQUIRE_FALSE(options.horizontal_layout);
    REQUIRE(options.create_frames);
}

TEST_CASE("PDFImporter: vertical layout positions", "[pdf_import]")
{
    const auto temp_dir = std::filesystem::temp_directory_path() / "markamp_pdf_test";
    const auto pdf_path = temp_dir / "test.pdf";
    create_fake_pdf(pdf_path);

    Board board;
    PDFImporter importer;
    PDFImportOptions options;
    options.scale = 1.0;

    const bool result = importer.import_pdf(pdf_path, board, options);
    REQUIRE(result);
    REQUIRE(board.objects().size() >= 1);

    // If multiple pages, they should be stacked vertically.
    if (board.objects().size() >= 2)
    {
        const auto& first = *board.objects()[0];
        const auto& second = *board.objects()[1];
        REQUIRE(first.position().y < second.position().y);
    }

    std::filesystem::remove_all(temp_dir);
}

TEST_CASE("PDFImporter: horizontal layout", "[pdf_import]")
{
    const auto temp_dir = std::filesystem::temp_directory_path() / "markamp_pdf_test_h";
    const auto pdf_path = temp_dir / "test.pdf";
    create_fake_pdf(pdf_path);

    Board board;
    PDFImporter importer;
    PDFImportOptions options;
    options.horizontal_layout = true;

    importer.import_pdf(pdf_path, board, options);
    REQUIRE(board.objects().size() >= 1);

    // If multiple pages, they should be stacked horizontally.
    if (board.objects().size() >= 2)
    {
        const auto& first = *board.objects()[0];
        const auto& second = *board.objects()[1];
        REQUIRE(first.position().x < second.position().x);
        REQUIRE(first.position().y == second.position().y);
    }

    std::filesystem::remove_all(temp_dir);
}
