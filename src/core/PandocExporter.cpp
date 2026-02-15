// ============================================================================
// File: src/core/PandocExporter.cpp
// Phase 39: Multi-Format Export — PandocBaseExporter implementation
// ============================================================================

#include "PandocExporter.h"

#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace markamp::core
{

PandocBaseExporter::PandocBaseExporter(ExportFormat target_format)
    : target_format_(target_format)
{
}

auto PandocBaseExporter::descriptor() const -> ExportFormatDescriptor
{
    ExportFormatDescriptor desc;
    desc.format = target_format_;
    desc.requires_pandoc = true;
    desc.supports_assets = true;
    desc.supports_toc = true;
    desc.extension = file_extension();

    switch (target_format_)
    {
        case ExportFormat::DOCX:
            desc.name = "Microsoft Word";
            desc.mime_type =
                "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
            desc.description = "Word document via Pandoc";
            break;
        case ExportFormat::EPUB:
            desc.name = "EPUB";
            desc.mime_type = "application/epub+zip";
            desc.description = "Electronic publication format";
            break;
        case ExportFormat::LaTeX:
            desc.name = "LaTeX";
            desc.mime_type = "application/x-latex";
            desc.description = "LaTeX document";
            break;
        case ExportFormat::RTF:
            desc.name = "Rich Text Format";
            desc.mime_type = "application/rtf";
            desc.description = "RTF document";
            break;
        case ExportFormat::ODT:
            desc.name = "OpenDocument Text";
            desc.mime_type = "application/vnd.oasis.opendocument.text";
            desc.description = "LibreOffice/OpenOffice document";
            break;
        case ExportFormat::PPTX:
            desc.name = "PowerPoint";
            desc.mime_type =
                "application/vnd.openxmlformats-officedocument.presentationml.presentation";
            desc.description = "PowerPoint presentation via Pandoc";
            break;
        case ExportFormat::ConfluentWiki:
            desc.name = "Confluence Wiki";
            desc.mime_type = "text/plain";
            desc.description = "Atlassian Confluence wiki markup";
            break;
        case ExportFormat::MediaWiki:
            desc.name = "MediaWiki";
            desc.mime_type = "text/plain";
            desc.description = "MediaWiki markup";
            break;
        case ExportFormat::Textile:
            desc.name = "Textile";
            desc.mime_type = "text/plain";
            desc.description = "Textile markup";
            break;
        case ExportFormat::AsciiDoc:
            desc.name = "AsciiDoc";
            desc.mime_type = "text/plain";
            desc.description = "AsciiDoc format";
            break;
        case ExportFormat::ReStructuredText:
            desc.name = "reStructuredText";
            desc.mime_type = "text/x-rst";
            desc.description = "Sphinx/reStructuredText";
            break;
        default:
            desc.name = "Unknown";
            break;
    }

    return desc;
}

auto PandocBaseExporter::export_content(const std::string& markdown_source,
                                        const ExportOptions& options) const
    -> std::expected<std::string, std::string>
{
    // Write source to temp file.
    auto temp_dir = std::filesystem::temp_directory_path();
    auto input_file = temp_dir / "markamp_export_input.md";
    auto output_file = temp_dir / ("markamp_export_output" + file_extension());

    {
        std::ofstream out(input_file);
        if (!out)
            return std::unexpected("Cannot create temp input file");
        out << markdown_source;
    }

    // Determine pandoc path.
    auto pandoc_path_str = options.pandoc_path.empty() ? "pandoc" : options.pandoc_path.string();

    // Run Pandoc.
    auto result = run_pandoc(input_file.string(),
                             output_file.string(),
                             "markdown",
                             pandoc_format_name(),
                             pandoc_path_str,
                             options.pandoc_extra_args);

    // Clean up input.
    std::filesystem::remove(input_file);

    if (!result)
    {
        return std::unexpected(result.error());
    }

    // For text-based formats, read and return the output.
    // For binary formats (DOCX, EPUB, etc.), the file is at output_file.
    std::ifstream out_file(output_file, std::ios::binary);
    if (!out_file)
        return std::unexpected("Pandoc output file not found");

    std::ostringstream ss;
    ss << out_file.rdbuf();

    // If the output path is set, move the file.
    if (!options.output_path.empty())
    {
        std::error_code ec;
        std::filesystem::create_directories(options.output_path.parent_path(), ec);
        std::filesystem::copy_file(output_file,
                                   options.output_path,
                                   std::filesystem::copy_options::overwrite_existing,
                                   ec);
    }

    std::filesystem::remove(output_file);
    return ss.str();
}

auto PandocBaseExporter::check_dependencies() const -> std::expected<void, std::string>
{
    return find_pandoc("pandoc").transform([](const std::string&) {});
}

auto PandocBaseExporter::pandoc_format_name() const -> std::string
{
    switch (target_format_)
    {
        case ExportFormat::DOCX:
            return "docx";
        case ExportFormat::EPUB:
            return "epub";
        case ExportFormat::LaTeX:
            return "latex";
        case ExportFormat::RTF:
            return "rtf";
        case ExportFormat::ODT:
            return "odt";
        case ExportFormat::PPTX:
            return "pptx";
        case ExportFormat::ConfluentWiki:
            return "jira";
        case ExportFormat::MediaWiki:
            return "mediawiki";
        case ExportFormat::Textile:
            return "textile";
        case ExportFormat::AsciiDoc:
            return "asciidoc";
        case ExportFormat::ReStructuredText:
            return "rst";
        default:
            return "plain";
    }
}

auto PandocBaseExporter::file_extension() const -> std::string
{
    switch (target_format_)
    {
        case ExportFormat::DOCX:
            return ".docx";
        case ExportFormat::EPUB:
            return ".epub";
        case ExportFormat::LaTeX:
            return ".tex";
        case ExportFormat::RTF:
            return ".rtf";
        case ExportFormat::ODT:
            return ".odt";
        case ExportFormat::PPTX:
            return ".pptx";
        case ExportFormat::AsciiDoc:
            return ".adoc";
        case ExportFormat::ReStructuredText:
            return ".rst";
        default:
            return ".txt";
    }
}

auto PandocBaseExporter::run_pandoc(const std::string& input_file,
                                    const std::string& output_file,
                                    const std::string& from_format,
                                    const std::string& to_format,
                                    const std::string& pandoc_path,
                                    const std::string& extra_args)
    -> std::expected<void, std::string>
{
    std::string cmd = pandoc_path + " -f " + from_format + " -t " + to_format + " -o " +
                      output_file + " " + input_file;
    if (!extra_args.empty())
        cmd += " " + extra_args;

    // Execute Pandoc.
    std::array<char, 256> buffer{};
    std::string output;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    {
        return std::unexpected("Failed to run Pandoc: " + cmd);
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe))
    {
        output += buffer.data();
    }
    int exit_code = pclose(pipe);
    if (exit_code != 0)
    {
        return std::unexpected("Pandoc failed (exit " + std::to_string(exit_code) + "): " + output);
    }

    return {};
}

auto PandocBaseExporter::find_pandoc(const std::string& hint_path)
    -> std::expected<std::string, std::string>
{
    // Check if pandoc is available via `which`.
    std::string cmd = "which " + hint_path + " 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
        return std::unexpected("Cannot check for Pandoc");

    std::array<char, 256> buffer{};
    std::string result;
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe))
    {
        result += buffer.data();
    }
    int exit_code = pclose(pipe);

    if (exit_code != 0 || result.empty())
    {
        return std::unexpected("Pandoc not found. Install from https://pandoc.org");
    }

    // Trim newline.
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
        result.pop_back();

    return result;
}

} // namespace markamp::core
