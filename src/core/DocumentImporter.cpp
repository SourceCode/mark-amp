/// @file DocumentImporter.cpp
/// @brief V9 Phase 42 — DocumentImporter implementation.

#include "DocumentImporter.h"

#include <algorithm>

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
    auto ext = file_path.substr(dot + 1);
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
    // Simulated conversion — extracts title from filename
    auto slash = file_path.rfind('/');
    auto basename = (slash != std::string::npos) ? file_path.substr(slash + 1) : file_path;
    auto dot = basename.rfind('.');
    auto title = (dot != std::string::npos) ? basename.substr(0, dot) : basename;

    std::string markdown = "# " + title + "\n\n";
    markdown += "Imported from " + import_format_name(format) + " format.\n\n";

    if (options.include_metadata)
    {
        markdown += "---\n";
        markdown += "source: " + file_path + "\n";
        markdown += "format: " + import_format_name(format) + "\n";
        markdown += "---\n\n";
    }

    markdown += "Content imported successfully.\n";
    return markdown;
}

} // namespace markamp::core
