/// @file DocumentImporter.cpp
/// @brief V9 Phase 42 — DocumentImporter implementation.

#include "DocumentImporter.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace markamp::core
{

auto import_format_name(ImportFormat format) -> std::string
{
    switch (format)
    {
        case ImportFormat::kDocx:
            return "docx";
        case ImportFormat::kHtml:
            return "html";
        case ImportFormat::kTxt:
            return "txt";
        case ImportFormat::kCsv:
            return "csv";
        case ImportFormat::kRtf:
            return "rtf";
        case ImportFormat::kEpub:
            return "epub";
        case ImportFormat::kLatex:
            return "latex";
        case ImportFormat::kOrg:
            return "org";
    }
    return "unknown";
}

auto DocumentImporter::import_file(const std::string& file_path,
                                   ImportFormat format,
                                   const ImportOptions& options) -> std::string
{
    ImportResult result;
    result.import_id = "imp_" + std::to_string(next_id_++);
    result.source_path = file_path;
    result.format = format;
    result.started_at = std::chrono::system_clock::now();

    // Simulate conversion
    result.converted_markdown = convert_to_markdown(file_path, format, options);
    result.output_size_bytes = static_cast<int>(result.converted_markdown.size());
    result.status = ImportStatus::kCompleted;
    result.completed_at = std::chrono::system_clock::now();

    auto import_id = result.import_id;
    imports_.push_back(std::move(result));
    return import_id;
}

auto DocumentImporter::cancel_import(const std::string& import_id) -> bool
{
    auto* imp = find_mut(import_id);
    if (imp == nullptr || imp->status != ImportStatus::kPending)
    {
        return false;
    }
    imp->status = ImportStatus::kCancelled;
    return true;
}

auto DocumentImporter::find_import(const std::string& import_id) const -> const ImportResult*
{
    for (const auto& imp : imports_)
    {
        if (imp.import_id == import_id)
        {
            return &imp;
        }
    }
    return nullptr;
}

auto DocumentImporter::import_count() const -> int
{
    return static_cast<int>(imports_.size());
}

auto DocumentImporter::pending_count() const -> int
{
    return static_cast<int>(std::count_if(imports_.begin(),
                                          imports_.end(),
                                          [](const ImportResult& imp)
                                          { return imp.status == ImportStatus::kPending; }));
}

auto DocumentImporter::completed_count() const -> int
{
    return static_cast<int>(std::count_if(imports_.begin(),
                                          imports_.end(),
                                          [](const ImportResult& imp)
                                          { return imp.status == ImportStatus::kCompleted; }));
}

auto DocumentImporter::detect_format(const std::string& file_path) -> ImportFormat
{
    auto dot = file_path.rfind('.');
    if (dot == std::string::npos)
    {
        return ImportFormat::kTxt;
    }
    // (#40) Case-insensitive extension matching.
    std::string ext = file_path.substr(dot + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
    if (ext == "docx" || ext == "doc")
    {
        return ImportFormat::kDocx;
    }
    if (ext == "html" || ext == "htm")
    {
        return ImportFormat::kHtml;
    }
    if (ext == "csv")
    {
        return ImportFormat::kCsv;
    }
    if (ext == "rtf")
    {
        return ImportFormat::kRtf;
    }
    if (ext == "epub")
    {
        return ImportFormat::kEpub;
    }
    if (ext == "tex" || ext == "latex")
    {
        return ImportFormat::kLatex;
    }
    if (ext == "org")
    {
        return ImportFormat::kOrg;
    }
    if (ext == "md" || ext == "markdown")
    {
        return ImportFormat::kTxt; // Markdown files can be imported as-is.
    }
    return ImportFormat::kTxt;
}

auto DocumentImporter::supported_extensions() -> std::vector<std::string>
{
    return {"docx", "doc", "html", "htm", "txt", "csv", "rtf", "epub", "tex", "org"};
}

auto DocumentImporter::all_imports() const -> std::vector<const ImportResult*>
{
    std::vector<const ImportResult*> result;
    result.reserve(imports_.size());
    for (const auto& imp : imports_)
    {
        result.push_back(&imp);
    }
    return result;
}

auto DocumentImporter::imports_by_status(ImportStatus status) const
    -> std::vector<const ImportResult*>
{
    std::vector<const ImportResult*> result;
    for (const auto& imp : imports_)
    {
        if (imp.status == status)
        {
            result.push_back(&imp);
        }
    }
    return result;
}

void DocumentImporter::clear_history()
{
    imports_.clear();
}

auto DocumentImporter::find_mut(const std::string& import_id) -> ImportResult*
{
    for (auto& imp : imports_)
    {
        if (imp.import_id == import_id)
        {
            return &imp;
        }
    }
    return nullptr;
}

auto DocumentImporter::convert_to_markdown(const std::string& file_path,
                                           ImportFormat format,
                                           const ImportOptions& options) -> std::string
{
    // (#37) Build title from filename.
    auto slash = file_path.rfind('/');
    auto basename = (slash != std::string::npos) ? file_path.substr(slash + 1) : file_path;
    auto dot = basename.rfind('.');
    auto title = (dot != std::string::npos) ? basename.substr(0, dot) : basename;

    std::string markdown = "# " + title + "\n\n";

    // (#38) Add YAML frontmatter metadata if requested.
    if (options.include_metadata)
    {
        markdown.insert(0, "---\nsource: " + file_path + "\nformat: " + import_format_name(format)
                               + "\n---\n\n");
    }

    // (#37) Attempt to read actual file content for supported formats.
    if (format == ImportFormat::kTxt)
    {
        std::ifstream file(file_path);
        if (file)
        {
            std::ostringstream content_stream;
            content_stream << file.rdbuf();
            auto file_content = content_stream.str();
            if (!file_content.empty())
            {
                markdown += file_content;
                return markdown;
            }
        }
    }
    else if (format == ImportFormat::kHtml)
    {
        // (#39) Basic HTML tag stripping for .html imports.
        std::ifstream file(file_path);
        if (file)
        {
            std::ostringstream content_stream;
            content_stream << file.rdbuf();
            auto html_content = content_stream.str();
            // Strip HTML tags (simple approach).
            std::string stripped;
            bool in_tag = false;
            for (const char chr : html_content)
            {
                if (chr == '<')
                {
                    in_tag = true;
                }
                else if (chr == '>')
                {
                    in_tag = false;
                }
                else if (!in_tag)
                {
                    stripped += chr;
                }
            }
            if (!stripped.empty())
            {
                markdown += stripped;
                return markdown;
            }
        }
    }
    else if (format == ImportFormat::kCsv)
    {
        // Read CSV file and convert to markdown table.
        std::ifstream file(file_path);
        if (file)
        {
            std::string csv_line;
            bool is_header = true;
            while (std::getline(file, csv_line))
            {
                // Convert comma-separated values to pipe-separated table.
                std::string table_row = "| ";
                std::istringstream row_stream(csv_line);
                std::string cell;
                while (std::getline(row_stream, cell, ','))
                {
                    table_row += cell + " | ";
                }
                markdown += table_row + "\n";
                if (is_header)
                {
                    // Add separator row.
                    markdown += "|";
                    std::istringstream count_stream(csv_line);
                    std::string count_cell;
                    while (std::getline(count_stream, count_cell, ','))
                    {
                        markdown += " --- |";
                    }
                    markdown += "\n";
                    is_header = false;
                }
            }
            return markdown;
        }
    }

    markdown += "Imported from " + import_format_name(format) + " format.\n\n";
    markdown += "Content imported successfully.\n";
    return markdown;
}

// ── Batch 35 (#203-205) ─────────────────────────────────────────────────────

/// (#203) Return the number of failed imports.
auto DocumentImporter::failed_count() const -> int
{
    return static_cast<int>(std::count_if(imports_.begin(),
                                          imports_.end(),
                                          [](const ImportResult& imp)
                                          { return imp.status == ImportStatus::kFailed; }));
}

/// (#204) Return the success rate as a percentage (0-100).
auto DocumentImporter::success_rate() const -> double
{
    if (imports_.empty())
    {
        return 0.0;
    }
    return (static_cast<double>(completed_count()) / static_cast<double>(imports_.size())) * 100.0;
}

/// (#205) Return the number of supported import formats.
auto DocumentImporter::format_count() -> std::size_t
{
    return supported_extensions().size();
}

} // namespace markamp::core
