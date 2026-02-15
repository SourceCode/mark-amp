// ============================================================================
// File: src/core/HtmlExporter.h
// Phase 39: Multi-Format Export — HTML exporter
// ============================================================================
#pragma once

#include "IExportFormat.h"

namespace markamp::core
{

// HtmlExporter — converts Markdown to standalone HTML with CSS.
class HtmlExporter : public IExportFormat
{
public:
    [[nodiscard]] auto descriptor() const -> ExportFormatDescriptor override;
    [[nodiscard]] auto export_content(const std::string& markdown_source,
                                      const ExportOptions& options) const
        -> std::expected<std::string, std::string> override;
    [[nodiscard]] auto check_dependencies() const -> std::expected<void, std::string> override;

private:
    // Convert Markdown to HTML body (basic converter).
    [[nodiscard]] static auto md_to_html(const std::string& markdown) -> std::string;

    // Default CSS for standalone HTML export.
    [[nodiscard]] static auto default_css() -> std::string;
};

} // namespace markamp::core
