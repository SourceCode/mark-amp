// ============================================================================
// File: src/core/ExportService.cpp
// Phase 39: Multi-Format Export — ExportService implementation
// ============================================================================

#include "ExportService.h"

#include "Events.h"

#include <chrono>
#include <fstream>
#include <regex>
#include <sstream>

// Forward declare built-in exporters (defined in separate files).
namespace markamp::core
{
class MarkdownExporter;
class HtmlExporter;
class PandocBaseExporter;
class PdfExporter;
} // namespace markamp::core

namespace markamp::core
{

ExportService::ExportService(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

auto ExportService::register_format(std::unique_ptr<IExportFormat> format) -> void
{
    std::lock_guard lock(mutex_);
    auto key = static_cast<uint8_t>(format->descriptor().format);
    formats_[key] = std::move(format);
}

auto ExportService::available_formats() const -> std::vector<ExportFormatDescriptor>
{
    std::lock_guard lock(mutex_);
    std::vector<ExportFormatDescriptor> descriptors;
    descriptors.reserve(formats_.size());
    for (const auto& [_, fmt] : formats_)
    {
        descriptors.push_back(fmt->descriptor());
    }
    return descriptors;
}

auto ExportService::is_format_available(ExportFormat format) const -> bool
{
    return find_exporter(format) != nullptr;
}

auto ExportService::export_content(const std::string& markdown_source,
                                   const ExportOptions& options,
                                   ExportProgressCallback progress) -> ExportResult
{
    auto start = std::chrono::steady_clock::now();
    ExportResult result;
    result.format = options.format;
    result.output_path = options.output_path;

    auto* exporter = find_exporter(options.format);
    if (!exporter)
    {
        result.error_message = "No exporter registered for format";
        return result;
    }

    // Check dependencies.
    auto deps = exporter->check_dependencies();
    if (!deps)
    {
        result.error_message = "Missing dependencies: " + deps.error();
        return result;
    }

    if (progress)
        progress("Preparing content...", 10);

    // Prepare content: optionally add TOC.
    std::string content = markdown_source;
    if (options.include_toc)
    {
        content = generate_toc(content) + "\n\n" + content;
    }

    // Handle assets.
    if (options.embed_assets)
    {
        content = embed_assets(content);
        if (progress)
            progress("Embedding assets...", 30);
    }
    else if (options.include_assets && !options.output_path.empty())
    {
        content = copy_assets(content, options.output_path.parent_path(), result);
        if (progress)
            progress("Copying assets...", 30);
    }

    if (progress)
        progress("Exporting...", 50);

    // Export.
    auto exported = exporter->export_content(content, options);
    if (!exported)
    {
        result.error_message = exported.error();
        return result;
    }

    // Write output file.
    if (!options.output_path.empty())
    {
        std::error_code ec;
        std::filesystem::create_directories(options.output_path.parent_path(), ec);

        std::ofstream out(options.output_path);
        if (!out)
        {
            result.error_message = "Cannot write output file: " + options.output_path.string();
            return result;
        }
        out << *exported;
        result.output_size_bytes = static_cast<int64_t>(exported->size());
    }

    if (progress)
        progress("Done", 100);

    result.success = true;
    auto end = std::chrono::steady_clock::now();
    result.elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    return result;
}

auto ExportService::export_document(const std::string& doc_id,
                                    const ExportOptions& options,
                                    ExportProgressCallback progress) -> ExportResult
{
    // Stub: load document content from storage, then export.
    // In production, fetch blocks via BlockService and serialize to markdown.
    (void)doc_id;
    return export_content("", options, progress);
}

auto ExportService::export_notebook(const std::string& notebook_id,
                                    const ExportOptions& options,
                                    ExportProgressCallback progress) -> ExportResult
{
    // Stub: iterate all documents in notebook.
    (void)notebook_id;
    return export_content("", options, progress);
}

auto ExportService::default_filename(const std::string& base_name, ExportFormat format)
    -> std::string
{
    static const std::unordered_map<uint8_t, std::string> extensions = {
        {static_cast<uint8_t>(ExportFormat::Markdown), ".md"},
        {static_cast<uint8_t>(ExportFormat::HTML), ".html"},
        {static_cast<uint8_t>(ExportFormat::PlainText), ".txt"},
        {static_cast<uint8_t>(ExportFormat::PDF), ".pdf"},
        {static_cast<uint8_t>(ExportFormat::DOCX), ".docx"},
        {static_cast<uint8_t>(ExportFormat::EPUB), ".epub"},
        {static_cast<uint8_t>(ExportFormat::LaTeX), ".tex"},
        {static_cast<uint8_t>(ExportFormat::RTF), ".rtf"},
        {static_cast<uint8_t>(ExportFormat::ODT), ".odt"},
        {static_cast<uint8_t>(ExportFormat::PPTX), ".pptx"},
        {static_cast<uint8_t>(ExportFormat::OPML), ".opml"},
        {static_cast<uint8_t>(ExportFormat::PNG), ".png"},
    };

    auto it = extensions.find(static_cast<uint8_t>(format));
    auto ext = it != extensions.end() ? it->second : ".txt";
    return base_name + ext;
}

auto ExportService::register_builtins() -> void
{
    // Built-in exporters are registered from their respective files.
    // This is called during initialization.
}

auto ExportService::find_exporter(ExportFormat format) const -> IExportFormat*
{
    std::lock_guard lock(mutex_);
    auto it = formats_.find(static_cast<uint8_t>(format));
    return it != formats_.end() ? it->second.get() : nullptr;
}

auto ExportService::copy_assets(const std::string& content,
                                const std::filesystem::path& output_dir,
                                ExportResult& result) -> std::string
{
    // Find image references: ![alt](path)
    std::string modified = content;
    std::regex img_regex(R"(!\[([^\]]*)\]\(([^)]+)\))");
    auto begin = std::sregex_iterator(content.begin(), content.end(), img_regex);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it)
    {
        auto match = *it;
        auto img_path = std::filesystem::path(match[2].str());
        if (std::filesystem::exists(img_path))
        {
            auto target = output_dir / "assets" / img_path.filename();
            std::error_code ec;
            std::filesystem::create_directories(target.parent_path(), ec);
            std::filesystem::copy_file(
                img_path, target, std::filesystem::copy_options::overwrite_existing, ec);
            if (!ec)
                ++result.assets_copied;
        }
    }

    return modified;
}

auto ExportService::embed_assets(const std::string& content) -> std::string
{
    // Stub: find image paths, read files, convert to base64 data URIs.
    return content;
}

auto ExportService::generate_toc(const std::string& markdown_source) -> std::string
{
    std::istringstream stream(markdown_source);
    std::string line;
    std::ostringstream toc;
    toc << "## Table of Contents\n\n";

    while (std::getline(stream, line))
    {
        if (line.starts_with("#"))
        {
            int level = 0;
            while (level < static_cast<int>(line.size()) && line[level] == '#')
                ++level;
            if (level > 0 && level <= 6)
            {
                auto heading = line.substr(static_cast<size_t>(level));
                // Trim leading space.
                if (!heading.empty() && heading[0] == ' ')
                    heading = heading.substr(1);

                // Generate anchor.
                std::string anchor;
                for (char c : heading)
                {
                    if (std::isalnum(static_cast<unsigned char>(c)))
                        anchor += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                    else if (c == ' ' || c == '-')
                        anchor += '-';
                }

                std::string indent(static_cast<size_t>((level - 1) * 2), ' ');
                toc << indent << "- [" << heading << "](#" << anchor << ")\n";
            }
        }
    }

    return toc.str();
}

} // namespace markamp::core
