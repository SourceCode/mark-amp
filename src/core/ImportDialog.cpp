// ============================================================================
// File: src/core/ImportDialog.cpp
// Phase 40: Import & AI Integration — ImportDialog implementation
// ============================================================================

#include "ImportDialog.h"

#include "ImportService.h"

namespace markamp::core
{

ImportDialog::ImportDialog(ImportService& import_service)
    : import_service_(import_service)
{
}

auto ImportDialog::show() -> std::optional<ImportOptions>
{
    // Stub: In production, opens a wxWidgets file dialog.
    // Returns nullopt when cancelled.
    return std::nullopt;
}

auto ImportDialog::available_formats() const -> std::vector<ImportFormatDescriptor>
{
    return import_service_.available_formats();
}

auto ImportDialog::validate(const ImportOptions& options) const -> std::vector<std::string>
{
    std::vector<std::string> errors;

    if (options.source_path.empty())
    {
        errors.push_back("Source path is required");
    }
    else if (!std::filesystem::exists(options.source_path))
    {
        errors.push_back("Source path does not exist: " + options.source_path.string());
    }

    return errors;
}

auto ImportDialog::default_options(const std::filesystem::path& source_path) const -> ImportOptions
{
    ImportOptions opts;
    opts.source_path = source_path;
    opts.format = import_service_.detect_format(source_path);
    opts.conflict_handling = ImportConflict::Rename;
    opts.import_assets = true;
    opts.preserve_structure = true;
    opts.convert_links = true;
    opts.extract_frontmatter = true;
    return opts;
}

} // namespace markamp::core
