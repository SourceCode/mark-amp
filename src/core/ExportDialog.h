// ============================================================================
// File: src/core/ExportDialog.h
// Phase 39: Multi-Format Export — Export dialog UI
// ============================================================================
#pragma once

#include "ExportTypes.h"

#include <string>
#include <vector>

namespace markamp::core
{

class ExportService;

// ExportDialog — user-facing export configuration dialog.
// Provides format selection, options, and preview.
class ExportDialog
{
public:
    explicit ExportDialog(ExportService& export_service);

    // Show the export dialog and return selected options.
    // Returns nullopt if cancelled.
    [[nodiscard]] auto show(const std::string& document_title) -> std::optional<ExportOptions>;

    // Get list of available formats for the dropdown.
    [[nodiscard]] auto available_formats() const -> std::vector<ExportFormatDescriptor>;

    // Validate the current options.
    [[nodiscard]] auto validate(const ExportOptions& options) const -> std::vector<std::string>;

    // Build default options for a format.
    [[nodiscard]] auto default_options(ExportFormat format, const std::string& doc_title) const
        -> ExportOptions;

private:
    ExportService& export_service_;
};

} // namespace markamp::core
