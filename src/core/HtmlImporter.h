// ============================================================================
// File: src/core/HtmlImporter.h
// Phase 40: Import — HTML importer
// ============================================================================
#pragma once

#include "IImportFormat.h"

namespace markamp::core
{

// HtmlImporter — converts HTML to Markdown.
class HtmlImporter : public IImportFormat
{
public:
    [[nodiscard]] auto descriptor() const -> ImportFormatDescriptor override;
    [[nodiscard]] auto import_file(const std::filesystem::path& source_path,
                                   const ImportOptions& options) const
        -> std::expected<std::string, std::string> override;
    [[nodiscard]] auto check_dependencies() const -> std::expected<void, std::string> override;
    [[nodiscard]] auto can_import(const std::filesystem::path& file_path) const -> bool override;

private:
    // Basic HTML to Markdown converter.
    [[nodiscard]] static auto html_to_markdown(const std::string& html) -> std::string;
};

} // namespace markamp::core
