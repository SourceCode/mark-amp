// ============================================================================
// File: src/core/PandocImporter.h
// Phase 40: Import — Pandoc-based multi-format importer
// ============================================================================
#pragma once

#include "IImportFormat.h"

namespace markamp::core
{

// PandocImporter — delegates to Pandoc for DOCX, EPUB, LaTeX, RTF.
class PandocImporter : public IImportFormat
{
public:
    explicit PandocImporter(ImportFormat target_format);

    [[nodiscard]] auto descriptor() const -> ImportFormatDescriptor override;
    [[nodiscard]] auto import_file(const std::filesystem::path& source_path,
                                   const ImportOptions& options) const
        -> std::expected<std::string, std::string> override;
    [[nodiscard]] auto check_dependencies() const -> std::expected<void, std::string> override;
    [[nodiscard]] auto can_import(const std::filesystem::path& file_path) const -> bool override;

private:
    ImportFormat target_format_;
    [[nodiscard]] auto pandoc_input_format() const -> std::string;
    [[nodiscard]] auto file_extension() const -> std::string;
};

// OPMLImporter — imports OPML outlines as nested Markdown headings.
class OPMLImporter : public IImportFormat
{
public:
    [[nodiscard]] auto descriptor() const -> ImportFormatDescriptor override;
    [[nodiscard]] auto import_file(const std::filesystem::path& source_path,
                                   const ImportOptions& options) const
        -> std::expected<std::string, std::string> override;
    [[nodiscard]] auto check_dependencies() const -> std::expected<void, std::string> override;
    [[nodiscard]] auto can_import(const std::filesystem::path& file_path) const -> bool override;
};

// CSVImporter — imports CSV data as Markdown tables.
class CSVImporter : public IImportFormat
{
public:
    [[nodiscard]] auto descriptor() const -> ImportFormatDescriptor override;
    [[nodiscard]] auto import_file(const std::filesystem::path& source_path,
                                   const ImportOptions& options) const
        -> std::expected<std::string, std::string> override;
    [[nodiscard]] auto check_dependencies() const -> std::expected<void, std::string> override;
    [[nodiscard]] auto can_import(const std::filesystem::path& file_path) const -> bool override;
};

} // namespace markamp::core
