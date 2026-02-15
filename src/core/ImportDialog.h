// ============================================================================
// File: src/core/ImportDialog.h
// Phase 40: Import & AI Integration — Import dialog UI
// ============================================================================
#pragma once

#include "ImportTypes.h"

#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class ImportService;

// ImportDialog — user-facing import configuration dialog.
class ImportDialog
{
public:
    explicit ImportDialog(ImportService& import_service);

    // Show the import dialog and return selected options.
    [[nodiscard]] auto show() -> std::optional<ImportOptions>;

    // Get available format descriptors.
    [[nodiscard]] auto available_formats() const -> std::vector<ImportFormatDescriptor>;

    // Validate import options.
    [[nodiscard]] auto validate(const ImportOptions& options) const -> std::vector<std::string>;

    // Build default options for a file.
    [[nodiscard]] auto default_options(const std::filesystem::path& source_path) const
        -> ImportOptions;

private:
    ImportService& import_service_;
};

} // namespace markamp::core
