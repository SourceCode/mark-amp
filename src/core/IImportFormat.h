// ============================================================================
// File: src/core/IImportFormat.h
// Phase 40: Import & AI Integration — Importer interface
// ============================================================================
#pragma once

#include "ImportTypes.h"

#include <expected>
#include <string>

namespace markamp::core
{

// IImportFormat — abstract interface for format importers.
// Each importer converts its source format to intermediate Markdown.
class IImportFormat
{
public:
    virtual ~IImportFormat() = default;

    // Return the format descriptor.
    [[nodiscard]] virtual auto descriptor() const -> ImportFormatDescriptor = 0;

    // Import a file and produce Markdown.
    // Returns the converted Markdown content.
    [[nodiscard]] virtual auto import_file(const std::filesystem::path& source_path,
                                           const ImportOptions& options) const
        -> std::expected<std::string, std::string> = 0;

    // Check if this importer's dependencies are available.
    [[nodiscard]] virtual auto check_dependencies() const -> std::expected<void, std::string> = 0;

    // Check if a file can be imported by this importer.
    [[nodiscard]] virtual auto can_import(const std::filesystem::path& file_path) const -> bool = 0;
};

} // namespace markamp::core
