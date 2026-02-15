// ============================================================================
// File: src/core/PandocExporter.h
// Phase 39: Multi-Format Export — Pandoc-based multi-format exporter
// ============================================================================
#pragma once

#include "IExportFormat.h"

namespace markamp::core
{

// PandocBaseExporter — delegates to Pandoc for DOCX, EPUB, LaTeX, etc.
// One instance is created per target format.
class PandocBaseExporter : public IExportFormat
{
public:
    explicit PandocBaseExporter(ExportFormat target_format);

    [[nodiscard]] auto descriptor() const -> ExportFormatDescriptor override;
    [[nodiscard]] auto export_content(const std::string& markdown_source,
                                      const ExportOptions& options) const
        -> std::expected<std::string, std::string> override;
    [[nodiscard]] auto check_dependencies() const -> std::expected<void, std::string> override;

private:
    ExportFormat target_format_;

    // Get the Pandoc output format string.
    [[nodiscard]] auto pandoc_format_name() const -> std::string;

    // Get file extension for the target format.
    [[nodiscard]] auto file_extension() const -> std::string;

    // Run Pandoc as a subprocess.
    [[nodiscard]] static auto run_pandoc(const std::string& input_file,
                                         const std::string& output_file,
                                         const std::string& from_format,
                                         const std::string& to_format,
                                         const std::string& pandoc_path,
                                         const std::string& extra_args)
        -> std::expected<void, std::string>;

    // Check if Pandoc is installed and accessible.
    [[nodiscard]] static auto find_pandoc(const std::string& hint_path)
        -> std::expected<std::string, std::string>;
};

} // namespace markamp::core
