// ============================================================================
// File: src/core/IExportFormat.h
// Phase 39: Multi-Format Export — Exporter interface
// ============================================================================
#pragma once

#include "ExportTypes.h"

#include <expected>
#include <string>
#include <vector>

namespace markamp::core
{

// IExportFormat — abstract interface for format exporters.
class IExportFormat
{
public:
    virtual ~IExportFormat() = default;

    // Return the format descriptor for this exporter.
    [[nodiscard]] virtual auto descriptor() const -> ExportFormatDescriptor = 0;

    // Export content (as Markdown source) to the target format.
    // Returns the formatted output as a string, or writes to a file.
    [[nodiscard]] virtual auto export_content(const std::string& markdown_source,
                                              const ExportOptions& options) const
        -> std::expected<std::string, std::string> = 0;

    // Check if this exporter's dependencies are available.
    [[nodiscard]] virtual auto check_dependencies() const -> std::expected<void, std::string> = 0;
};

} // namespace markamp::core
