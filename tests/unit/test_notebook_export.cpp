/// @file test_notebook_export.cpp
/// @brief V4 Phase 37 – NotebookExportEngine tests.

#include "core/EventBus.h"
#include "core/NotebookExportEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct ExportFixture
{
    EventBus event_bus;
    NotebookExportEngine engine{event_bus};

    std::vector<ExportCell> make_cells()
    {
        std::vector<ExportCell> cells;

        ExportCell md_cell;
        md_cell.cell_id = "cell-1";
        md_cell.cell_type = "markdown";
        md_cell.source = "# Introduction\n\nThis is a test notebook.";
        cells.push_back(std::move(md_cell));

        ExportCell code_cell;
        code_cell.cell_id = "cell-2";
        code_cell.cell_type = "code";
        code_cell.source = "print('hello')";
        code_cell.output_text = "hello";
        code_cell.execution_count = 1;
        cells.push_back(std::move(code_cell));

        ExportCell md_cell2;
        md_cell2.cell_id = "cell-3";
        md_cell2.cell_type = "markdown";
        md_cell2.source = "## Results\n\nAnalysis complete.";
        cells.push_back(std::move(md_cell2));

        return cells;
    }
};

TEST_CASE("NotebookExportEngine: export to HTML", "[notebook_export]")
{
    ExportFixture fixture;
    auto cells = fixture.make_cells();
    auto html = fixture.engine.export_to_html(cells);

    REQUIRE(html.find("<!DOCTYPE html>") != std::string::npos);
    REQUIRE(html.find("print('hello')") != std::string::npos);
    REQUIRE(html.find("In [1]:") != std::string::npos);
    REQUIRE(html.find("hello") != std::string::npos);
}

TEST_CASE("NotebookExportEngine: export to Markdown", "[notebook_export]")
{
    ExportFixture fixture;
    auto cells = fixture.make_cells();
    auto md = fixture.engine.export_to_markdown(cells);

    REQUIRE(md.find("```python") != std::string::npos);
    REQUIRE(md.find("print('hello')") != std::string::npos);
    REQUIRE(md.find("**In [1]:**") != std::string::npos);
    REQUIRE(md.find("**Out:**") != std::string::npos);
}

TEST_CASE("NotebookExportEngine: export to LaTeX", "[notebook_export]")
{
    ExportFixture fixture;
    auto cells = fixture.make_cells();
    auto latex = fixture.engine.export_to_latex(cells);

    REQUIRE(latex.find("\\documentclass{article}") != std::string::npos);
    REQUIRE(latex.find("\\begin{lstlisting}") != std::string::npos);
    REQUIRE(latex.find("\\end{document}") != std::string::npos);
}

TEST_CASE("NotebookExportEngine: export to slides", "[notebook_export]")
{
    ExportFixture fixture;
    auto cells = fixture.make_cells();
    auto slides = fixture.engine.export_to_slides(cells);

    REQUIRE(slides.find("class=\"slide\"") != std::string::npos);
    REQUIRE(slides.find("slide-1") != std::string::npos);
}

TEST_CASE("NotebookExportEngine: export_notebook dispatcher", "[notebook_export]")
{
    ExportFixture fixture;
    auto cells = fixture.make_cells();
    auto result = fixture.engine.export_notebook(cells, ExportFormat::kHtml);

    REQUIRE(result.success);
    REQUIRE(result.format == ExportFormat::kHtml);
    REQUIRE(!result.content.empty());
}

TEST_CASE("NotebookExportEngine: PDF format returns error", "[notebook_export]")
{
    ExportFixture fixture;
    auto cells = fixture.make_cells();
    auto result = fixture.engine.export_notebook(cells, ExportFormat::kPdf);

    REQUIRE(!result.success);
    REQUIRE(!result.error_message.empty());
}

TEST_CASE("NotebookExportEngine: generate TOC", "[notebook_export]")
{
    ExportFixture fixture;
    auto cells = fixture.make_cells();
    auto toc = fixture.engine.generate_toc(cells);

    REQUIRE(toc.size() == 2); // "# Introduction" and "## Results"
    REQUIRE(toc[0].heading == "Introduction");
    REQUIRE(toc[0].level == 1);
    REQUIRE(toc[1].heading == "Results");
    REQUIRE(toc[1].level == 2);
}

TEST_CASE("NotebookExportEngine: strip outputs", "[notebook_export]")
{
    ExportFixture fixture;
    auto cells = fixture.make_cells();
    auto stripped = fixture.engine.strip_outputs(cells);

    REQUIRE(stripped.size() == 3);
    REQUIRE(stripped[1].output_text.empty());
    REQUIRE(stripped[1].execution_count == 0);
    REQUIRE(stripped[1].source == "print('hello')"); // Source preserved.
}

TEST_CASE("NotebookExportEngine: HTML with TOC", "[notebook_export]")
{
    ExportFixture fixture;
    auto cells = fixture.make_cells();
    ExportOptions options;
    options.include_toc = true;
    auto html = fixture.engine.export_to_html(cells, options);

    REQUIRE(html.find("class=\"toc\"") != std::string::npos);
    REQUIRE(html.find("Introduction") != std::string::npos);
}

TEST_CASE("NotebookExportEngine: HTML with title", "[notebook_export]")
{
    ExportFixture fixture;
    auto cells = fixture.make_cells();
    ExportOptions options;
    options.title = "My Notebook";
    auto html = fixture.engine.export_to_html(cells, options);

    REQUIRE(html.find("<title>My Notebook</title>") != std::string::npos);
}
