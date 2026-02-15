// ============================================================================
// File: src/core/ImportService.cpp
// Phase 40: Import & AI Integration — ImportService implementation
// ============================================================================

#include "ImportService.h"

#include "Events.h"

#include <algorithm>
#include <chrono>
#include <filesystem>

namespace markamp::core
{

ImportService::ImportService(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

auto ImportService::register_format(std::unique_ptr<IImportFormat> format) -> void
{
    std::lock_guard lock(mutex_);
    auto key = static_cast<uint8_t>(format->descriptor().format);
    formats_[key] = std::move(format);
}

auto ImportService::available_formats() const -> std::vector<ImportFormatDescriptor>
{
    std::lock_guard lock(mutex_);
    std::vector<ImportFormatDescriptor> descriptors;
    descriptors.reserve(formats_.size());
    for (const auto& [_, fmt] : formats_)
    {
        descriptors.push_back(fmt->descriptor());
    }
    return descriptors;
}

auto ImportService::detect_format(const std::filesystem::path& file_path) const -> ImportFormat
{
    auto ext = file_path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".md" || ext == ".markdown")
        return ImportFormat::Markdown;
    if (ext == ".html" || ext == ".htm")
        return ImportFormat::HTML;
    if (ext == ".docx")
        return ImportFormat::DOCX;
    if (ext == ".epub")
        return ImportFormat::EPUB;
    if (ext == ".opml")
        return ImportFormat::OPML;
    if (ext == ".csv" || ext == ".tsv")
        return ImportFormat::CSV;
    if (ext == ".txt")
        return ImportFormat::PlainText;
    if (ext == ".tex")
        return ImportFormat::LaTeX;
    if (ext == ".rtf")
        return ImportFormat::RTF;
    if (ext == ".sy.zip" || ext == ".siyuan")
        return ImportFormat::SiYuanPackage;

    return ImportFormat::PlainText;
}

auto ImportService::import_file(const std::filesystem::path& source_path,
                                const ImportOptions& options,
                                ImportProgressCallback progress) -> ImportResult
{
    auto start = std::chrono::steady_clock::now();
    ImportResult result;
    result.format = options.format;

    if (!std::filesystem::exists(source_path))
    {
        result.errors.push_back("File not found: " + source_path.string());
        return result;
    }

    auto* importer = find_importer(options.format);
    if (!importer)
    {
        result.errors.push_back("No importer for format");
        return result;
    }

    // Check dependencies.
    auto deps = importer->check_dependencies();
    if (!deps)
    {
        result.errors.push_back("Missing dependencies: " + deps.error());
        return result;
    }

    if (progress)
        progress(source_path.filename().string(), 20);

    // Convert to Markdown.
    auto md_result = importer->import_file(source_path, options);
    if (!md_result)
    {
        result.errors.push_back(md_result.error());
        return result;
    }

    if (progress)
        progress(source_path.filename().string(), 60);

    // Process Markdown into blocks.
    auto doc_title = source_path.stem().string();
    auto doc_result = process_markdown(*md_result, doc_title, options);
    if (doc_result)
    {
        result.imported_doc_ids.push_back(*doc_result);
        ++result.documents_imported;
    }
    else
    {
        result.errors.push_back(doc_result.error());
    }

    if (progress)
        progress(source_path.filename().string(), 100);

    auto end = std::chrono::steady_clock::now();
    result.elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    result.success = result.errors.empty();
    result.errors_count = static_cast<int32_t>(result.errors.size());

    return result;
}

auto ImportService::import_directory(const std::filesystem::path& source_dir,
                                     const ImportOptions& options,
                                     ImportProgressCallback progress) -> ImportResult
{
    auto start = std::chrono::steady_clock::now();
    ImportResult result;

    if (!std::filesystem::is_directory(source_dir))
    {
        result.errors.push_back("Not a directory: " + source_dir.string());
        return result;
    }

    // Collect files.
    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(source_dir))
    {
        if (entry.is_regular_file())
        {
            files.push_back(entry.path());
        }
    }

    int total = static_cast<int>(files.size());
    for (int i = 0; i < total; ++i)
    {
        auto file_options = options;
        file_options.source_path = files[static_cast<size_t>(i)];
        file_options.format = detect_format(files[static_cast<size_t>(i)]);

        if (progress)
        {
            progress(files[static_cast<size_t>(i)].filename().string(),
                     total > 0 ? (i * 100 / total) : 100);
        }

        auto file_result = import_file(files[static_cast<size_t>(i)], file_options, nullptr);

        result.documents_imported += file_result.documents_imported;
        result.blocks_created += file_result.blocks_created;
        result.assets_imported += file_result.assets_imported;
        result.skipped += file_result.skipped;
        for (const auto& err : file_result.errors)
            result.errors.push_back(err);
        for (const auto& id : file_result.imported_doc_ids)
            result.imported_doc_ids.push_back(id);
    }

    auto end = std::chrono::steady_clock::now();
    result.elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    result.success = result.errors.empty();
    result.errors_count = static_cast<int32_t>(result.errors.size());

    return result;
}

auto ImportService::register_builtins() -> void
{
    // Built-in importers registered from their respective files.
}

auto ImportService::find_importer(ImportFormat format) const -> IImportFormat*
{
    std::lock_guard lock(mutex_);
    auto it = formats_.find(static_cast<uint8_t>(format));
    return it != formats_.end() ? it->second.get() : nullptr;
}

auto ImportService::process_markdown(const std::string& markdown_content,
                                     const std::string& doc_title,
                                     const ImportOptions& options)
    -> std::expected<std::string, std::string>
{
    // Stub: In production, parse markdown into blocks, create document.
    // For now, return a generated doc ID.
    (void)markdown_content;
    (void)doc_title;
    (void)options;
    auto doc_id = "imported-" + doc_title;
    return doc_id;
}

} // namespace markamp::core
