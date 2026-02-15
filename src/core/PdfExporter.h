// ============================================================================
// File: src/core/PdfExporter.h
// Phase 39: Multi-Format Export — PDF exporter
// ============================================================================
#pragma once

#include "IExportFormat.h"

namespace markamp::core
{

// PdfExporter — generates PDF via HTML→wkhtmltopdf or Pandoc.
class PdfExporter : public IExportFormat
{
public:
    [[nodiscard]] auto descriptor() const -> ExportFormatDescriptor override;
    [[nodiscard]] auto export_content(const std::string& markdown_source,
                                      const ExportOptions& options) const
        -> std::expected<std::string, std::string> override;
    [[nodiscard]] auto check_dependencies() const -> std::expected<void, std::string> override;

private:
    // Generate PDF using wkhtmltopdf from HTML.
    [[nodiscard]] static auto generate_via_wkhtmltopdf(const std::string& html_content,
                                                       const std::filesystem::path& output_path,
                                                       const ExportOptions& options)
        -> std::expected<void, std::string>;

    // Generate PDF using Pandoc.
    [[nodiscard]] static auto generate_via_pandoc(const std::string& markdown_source,
                                                  const std::filesystem::path& output_path,
                                                  const ExportOptions& options)
        -> std::expected<void, std::string>;
};

} // namespace markamp::core
