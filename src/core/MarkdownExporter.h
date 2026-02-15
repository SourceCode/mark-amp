// ============================================================================
// File: src/core/MarkdownExporter.h
// Phase 39: Multi-Format Export — Markdown exporter
// ============================================================================
#pragma once

#include "IExportFormat.h"

namespace markamp::core
{

// MarkdownExporter — pass-through with optional formatting adjustments.
class MarkdownExporter : public IExportFormat
{
public:
    [[nodiscard]] auto descriptor() const -> ExportFormatDescriptor override;
    [[nodiscard]] auto export_content(const std::string& markdown_source,
                                      const ExportOptions& options) const
        -> std::expected<std::string, std::string> override;
    [[nodiscard]] auto check_dependencies() const -> std::expected<void, std::string> override;
};

// PlainTextExporter — strips all Markdown formatting.
class PlainTextExporter : public IExportFormat
{
public:
    [[nodiscard]] auto descriptor() const -> ExportFormatDescriptor override;
    [[nodiscard]] auto export_content(const std::string& markdown_source,
                                      const ExportOptions& options) const
        -> std::expected<std::string, std::string> override;
    [[nodiscard]] auto check_dependencies() const -> std::expected<void, std::string> override;
};

} // namespace markamp::core
