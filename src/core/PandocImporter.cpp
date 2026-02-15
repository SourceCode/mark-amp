// ============================================================================
// File: src/core/PandocImporter.cpp
// Phase 40: Import — PandocImporter, OPMLImporter, CSVImporter
// ============================================================================

#include "PandocImporter.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <sstream>

namespace markamp::core
{

// ── PandocImporter ──────────────────────────────────────────────────────────

PandocImporter::PandocImporter(ImportFormat target_format)
    : target_format_(target_format)
{
}

auto PandocImporter::descriptor() const -> ImportFormatDescriptor
{
    ImportFormatDescriptor desc;
    desc.format = target_format_;
    desc.requires_pandoc = true;
    desc.extension = file_extension();

    switch (target_format_)
    {
        case ImportFormat::DOCX:
            desc.name = "Microsoft Word";
            desc.all_extensions = {".docx"};
            desc.description = "Word document via Pandoc";
            break;
        case ImportFormat::EPUB:
            desc.name = "EPUB";
            desc.all_extensions = {".epub"};
            desc.description = "EPUB ebook";
            break;
        case ImportFormat::LaTeX:
            desc.name = "LaTeX";
            desc.all_extensions = {".tex", ".latex"};
            desc.description = "LaTeX document";
            break;
        case ImportFormat::RTF:
            desc.name = "Rich Text Format";
            desc.all_extensions = {".rtf"};
            desc.description = "RTF document";
            break;
        default:
            desc.name = "Unknown";
            break;
    }

    return desc;
}

auto PandocImporter::import_file(const std::filesystem::path& source_path,
                                 const ImportOptions& options) const
    -> std::expected<std::string, std::string>
{
    auto pandoc_path = options.pandoc_path.empty() ? "pandoc" : options.pandoc_path.string();

    auto temp_output = std::filesystem::temp_directory_path() / "markamp_import_output.md";

    std::string cmd = pandoc_path + " -f " + pandoc_input_format() + " -t markdown" + " -o " +
                      temp_output.string() + " " + source_path.string();
    if (!options.pandoc_extra_args.empty())
        cmd += " " + options.pandoc_extra_args;
    cmd += " 2>&1";

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
        return std::unexpected("Failed to run Pandoc");

    std::array<char, 256> buf{};
    std::string output;
    while (fgets(buf.data(), static_cast<int>(buf.size()), pipe))
        output += buf.data();
    int exit_code = pclose(pipe);

    if (exit_code != 0)
    {
        return std::unexpected("Pandoc failed: " + output);
    }

    std::ifstream result_file(temp_output);
    if (!result_file)
        return std::unexpected("Cannot read Pandoc output");

    std::ostringstream ss;
    ss << result_file.rdbuf();
    std::filesystem::remove(temp_output);

    return ss.str();
}

auto PandocImporter::check_dependencies() const -> std::expected<void, std::string>
{
    FILE* pipe = popen("which pandoc 2>/dev/null", "r");
    if (!pipe)
        return std::unexpected("Cannot search for Pandoc");
    std::array<char, 256> buf{};
    std::string result;
    while (fgets(buf.data(), static_cast<int>(buf.size()), pipe))
        result += buf.data();
    if (pclose(pipe) != 0 || result.empty())
    {
        return std::unexpected("Pandoc not found");
    }
    return {};
}

auto PandocImporter::can_import(const std::filesystem::path& file_path) const -> bool
{
    auto ext = file_path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == file_extension();
}

auto PandocImporter::pandoc_input_format() const -> std::string
{
    switch (target_format_)
    {
        case ImportFormat::DOCX:
            return "docx";
        case ImportFormat::EPUB:
            return "epub";
        case ImportFormat::LaTeX:
            return "latex";
        case ImportFormat::RTF:
            return "rtf";
        default:
            return "markdown";
    }
}

auto PandocImporter::file_extension() const -> std::string
{
    switch (target_format_)
    {
        case ImportFormat::DOCX:
            return ".docx";
        case ImportFormat::EPUB:
            return ".epub";
        case ImportFormat::LaTeX:
            return ".tex";
        case ImportFormat::RTF:
            return ".rtf";
        default:
            return ".txt";
    }
}

// ── OPMLImporter ────────────────────────────────────────────────────────────

auto OPMLImporter::descriptor() const -> ImportFormatDescriptor
{
    return {.format = ImportFormat::OPML,
            .name = "OPML",
            .extension = ".opml",
            .all_extensions = {".opml"},
            .description = "Outline Processor Markup Language",
            .requires_pandoc = false};
}

auto OPMLImporter::import_file(const std::filesystem::path& source_path,
                               const ImportOptions& /*options*/) const
    -> std::expected<std::string, std::string>
{
    std::ifstream file(source_path);
    if (!file)
        return std::unexpected("Cannot open: " + source_path.string());

    // Basic OPML→Markdown: extract <outline text="..."> as headings.
    std::ostringstream md;
    std::string line;
    int depth = 0;

    while (std::getline(file, line))
    {
        // Count indentation for heading level.
        auto trimmed = line;
        size_t indent = 0;
        while (indent < trimmed.size() && (trimmed[indent] == ' ' || trimmed[indent] == '\t'))
            ++indent;
        trimmed = trimmed.substr(indent);

        // Look for <outline text="..." ...>
        auto text_pos = trimmed.find("text=\"");
        if (text_pos != std::string::npos)
        {
            text_pos += 6;
            auto end_pos = trimmed.find('"', text_pos);
            if (end_pos != std::string::npos)
            {
                auto text = trimmed.substr(text_pos, end_pos - text_pos);
                int level = std::min(static_cast<int>(indent / 2) + 1, 6);
                md << std::string(static_cast<size_t>(level), '#') << " " << text << "\n\n";
            }
        }

        // Track body/outline depth.
        if (trimmed.starts_with("<body"))
            ++depth;
        if (trimmed.starts_with("</body"))
            --depth;
    }

    return md.str();
}

auto OPMLImporter::check_dependencies() const -> std::expected<void, std::string>
{
    return {};
}

auto OPMLImporter::can_import(const std::filesystem::path& file_path) const -> bool
{
    auto ext = file_path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".opml";
}

// ── CSVImporter ─────────────────────────────────────────────────────────────

auto CSVImporter::descriptor() const -> ImportFormatDescriptor
{
    return {.format = ImportFormat::CSV,
            .name = "CSV",
            .extension = ".csv",
            .all_extensions = {".csv", ".tsv"},
            .description = "CSV/TSV data as Markdown table",
            .requires_pandoc = false};
}

auto CSVImporter::import_file(const std::filesystem::path& source_path,
                              const ImportOptions& /*options*/) const
    -> std::expected<std::string, std::string>
{
    std::ifstream file(source_path);
    if (!file)
        return std::unexpected("Cannot open: " + source_path.string());

    auto title = source_path.stem().string();
    char delimiter = source_path.extension() == ".tsv" ? '\t' : ',';

    std::ostringstream md;
    md << "# " << title << "\n\n";

    std::string line;
    bool is_header = true;

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        // Parse CSV row (simple, no quotes handling).
        std::vector<std::string> cells;
        std::istringstream row(line);
        std::string cell;
        while (std::getline(row, cell, delimiter))
        {
            // Trim whitespace.
            auto f = cell.find_first_not_of(" \t");
            auto l = cell.find_last_not_of(" \t");
            cells.push_back(f == std::string::npos ? "" : cell.substr(f, l - f + 1));
        }

        // Write table row.
        md << "| ";
        for (const auto& c : cells)
            md << c << " | ";
        md << "\n";

        // Add separator after header.
        if (is_header)
        {
            md << "| ";
            for (size_t i = 0; i < cells.size(); ++i)
                md << "--- | ";
            md << "\n";
            is_header = false;
        }
    }

    return md.str();
}

auto CSVImporter::check_dependencies() const -> std::expected<void, std::string>
{
    return {};
}

auto CSVImporter::can_import(const std::filesystem::path& file_path) const -> bool
{
    auto ext = file_path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".csv" || ext == ".tsv";
}

} // namespace markamp::core
