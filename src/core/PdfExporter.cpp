// ============================================================================
// File: src/core/PdfExporter.cpp
// Phase 39: Multi-Format Export — PdfExporter implementation
// ============================================================================

#include "PdfExporter.h"

#include "HtmlExporter.h"

#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>

namespace markamp::core
{

auto PdfExporter::descriptor() const -> ExportFormatDescriptor
{
    return {.format = ExportFormat::PDF,
            .name = "PDF",
            .extension = ".pdf",
            .mime_type = "application/pdf",
            .description = "PDF document via wkhtmltopdf or Pandoc",
            .requires_pandoc = false, // Can use wkhtmltopdf as alternative
            .supports_assets = true,
            .supports_toc = true};
}

auto PdfExporter::export_content(const std::string& markdown_source,
                                 const ExportOptions& options) const
    -> std::expected<std::string, std::string>
{
    if (options.output_path.empty())
    {
        return std::unexpected("PDF export requires an output path");
    }

    // Try wkhtmltopdf first, fall back to Pandoc.
    if (options.pdf_engine == "wkhtmltopdf" || options.pdf_engine.empty())
    {
        // Convert MD to HTML first.
        auto html = HtmlExporter::md_to_html(markdown_source);
        auto result = generate_via_wkhtmltopdf(html, options.output_path, options);
        if (result)
        {
            // Read and return the PDF for consistency.
            return "PDF generated at: " + options.output_path.string();
        }
    }

    // Fall back to Pandoc.
    auto pandoc_result = generate_via_pandoc(markdown_source, options.output_path, options);
    if (pandoc_result)
    {
        return "PDF generated at: " + options.output_path.string();
    }

    return std::unexpected("PDF generation failed. Install wkhtmltopdf or Pandoc.");
}

auto PdfExporter::check_dependencies() const -> std::expected<void, std::string>
{
    // Check for wkhtmltopdf or pandoc.
    FILE* pipe = popen("which wkhtmltopdf 2>/dev/null", "r");
    if (pipe)
    {
        std::array<char, 256> buf{};
        std::string result;
        while (fgets(buf.data(), static_cast<int>(buf.size()), pipe))
            result += buf.data();
        if (pclose(pipe) == 0 && !result.empty())
            return {};
    }

    pipe = popen("which pandoc 2>/dev/null", "r");
    if (pipe)
    {
        std::array<char, 256> buf{};
        std::string result;
        while (fgets(buf.data(), static_cast<int>(buf.size()), pipe))
            result += buf.data();
        if (pclose(pipe) == 0 && !result.empty())
            return {};
    }

    return std::unexpected("PDF export requires wkhtmltopdf or Pandoc. "
                           "Install one from: https://wkhtmltopdf.org or https://pandoc.org");
}

auto PdfExporter::generate_via_wkhtmltopdf(const std::string& html_content,
                                           const std::filesystem::path& output_path,
                                           const ExportOptions& options)
    -> std::expected<void, std::string>
{
    // Write HTML to temp file.
    auto temp_html = std::filesystem::temp_directory_path() / "markamp_pdf_input.html";
    {
        std::ofstream out(temp_html);
        if (!out)
            return std::unexpected("Cannot create temp HTML");
        out << "<!DOCTYPE html><html><head>"
            << "<meta charset='UTF-8'>"
            << "<style>"
            << "body { font-family: -apple-system, sans-serif; line-height: 1.6; }"
            << "h1,h2,h3 { page-break-after: avoid; }"
            << "pre { page-break-inside: avoid; }"
            << "</style></head><body>" << html_content << "</body></html>";
    }

    std::error_code ec;
    std::filesystem::create_directories(output_path.parent_path(), ec);

    // Build command.
    std::string cmd = "wkhtmltopdf --quiet"
                      " --page-size " +
                      options.page_size + " --margin-top " + options.margin + " --margin-bottom " +
                      options.margin + " --margin-left " + options.margin + " --margin-right " +
                      options.margin;
    if (options.landscape)
        cmd += " --orientation Landscape";
    cmd += " " + temp_html.string() + " " + output_path.string() + " 2>&1";

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    {
        std::filesystem::remove(temp_html);
        return std::unexpected("Failed to run wkhtmltopdf");
    }

    std::array<char, 256> buf{};
    std::string output;
    while (fgets(buf.data(), static_cast<int>(buf.size()), pipe))
        output += buf.data();
    int exit_code = pclose(pipe);

    std::filesystem::remove(temp_html);

    if (exit_code != 0)
    {
        return std::unexpected("wkhtmltopdf failed: " + output);
    }

    return {};
}

auto PdfExporter::generate_via_pandoc(const std::string& markdown_source,
                                      const std::filesystem::path& output_path,
                                      const ExportOptions& options)
    -> std::expected<void, std::string>
{
    auto temp_md = std::filesystem::temp_directory_path() / "markamp_pdf_input.md";
    {
        std::ofstream out(temp_md);
        if (!out)
            return std::unexpected("Cannot create temp file");
        out << markdown_source;
    }

    auto pandoc_path = options.pandoc_path.empty() ? "pandoc" : options.pandoc_path.string();

    std::string cmd =
        pandoc_path + " -f markdown -o " + output_path.string() + " " + temp_md.string();
    if (!options.pandoc_extra_args.empty())
        cmd += " " + options.pandoc_extra_args;
    cmd += " 2>&1";

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    {
        std::filesystem::remove(temp_md);
        return std::unexpected("Failed to run Pandoc");
    }

    std::array<char, 256> buf{};
    std::string output;
    while (fgets(buf.data(), static_cast<int>(buf.size()), pipe))
        output += buf.data();
    int exit_code = pclose(pipe);

    std::filesystem::remove(temp_md);

    if (exit_code != 0)
    {
        return std::unexpected("Pandoc PDF failed: " + output);
    }

    return {};
}

} // namespace markamp::core
