// ============================================================================
// File: src/core/MarkdownImporter.h
// Phase 40: Import — Markdown & PlainText importers
// ============================================================================
#pragma once

#include "IImportFormat.h"

namespace markamp::core
{

// MarkdownImporter — pass-through with optional frontmatter extraction.
class MarkdownImporter : public IImportFormat
{
public:
    [[nodiscard]] auto descriptor() const -> ImportFormatDescriptor override;
    [[nodiscard]] auto import_file(const std::filesystem::path& source_path,
                                   const ImportOptions& options) const
        -> std::expected<std::string, std::string> override;
    [[nodiscard]] auto check_dependencies() const -> std::expected<void, std::string> override;
    [[nodiscard]] auto can_import(const std::filesystem::path& file_path) const -> bool override;
};

// PlainTextImporter — wraps plain text as Markdown.
class PlainTextImporter : public IImportFormat
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
