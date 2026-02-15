// ============================================================================
// File: src/core/ExportDialog.cpp
// Phase 39: Multi-Format Export — ExportDialog implementation
// ============================================================================

#include "ExportDialog.h"

#include "ExportService.h"

#include <filesystem>

namespace markamp::core
{

ExportDialog::ExportDialog(ExportService& export_service)
    : export_service_(export_service)
{
}

auto ExportDialog::show(const std::string& document_title) -> std::optional<ExportOptions>
{
    // Stub: In production, this opens a wxWidgets dialog.
    // For now, return default Markdown options.
    return default_options(ExportFormat::Markdown, document_title);
}

auto ExportDialog::available_formats() const -> std::vector<ExportFormatDescriptor>
{
    return export_service_.available_formats();
}

auto ExportDialog::validate(const ExportOptions& options) const -> std::vector<std::string>
{
    std::vector<std::string> errors;

    if (options.output_path.empty())
    {
        errors.push_back("Output path is required");
    }

    if (!export_service_.is_format_available(options.format))
    {
        errors.push_back("Selected format is not available");
    }

    // Check if output directory is writable.
    if (!options.output_path.empty())
    {
        auto parent = options.output_path.parent_path();
        if (!parent.empty() && !std::filesystem::exists(parent))
        {
            errors.push_back("Output directory does not exist");
        }
    }

    return errors;
}

auto ExportDialog::default_options(ExportFormat format, const std::string& doc_title) const
    -> ExportOptions
{
    ExportOptions opts;
    opts.format = format;
    opts.scope = ExportScope::CurrentDocument;
    opts.output_path =
        std::filesystem::current_path() /
        ExportService::default_filename(doc_title.empty() ? "untitled" : doc_title, format);
    opts.include_frontmatter = true;
    opts.include_toc = (format == ExportFormat::HTML || format == ExportFormat::PDF ||
                        format == ExportFormat::EPUB);
    opts.include_assets = true;
    opts.embed_assets = (format == ExportFormat::HTML);

    return opts;
}

} // namespace markamp::core
