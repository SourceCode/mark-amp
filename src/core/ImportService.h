// ============================================================================
// File: src/core/ImportService.h
// Phase 40: Import & AI Integration — Import orchestration service
// ============================================================================
#pragma once

#include "IImportFormat.h"
#include "ImportTypes.h"

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

// Progress callback: (filename, percent).
using ImportProgressCallback = std::function<void(const std::string&, int)>;

// ImportService — manages format registrations and orchestrates imports.
class ImportService
{
public:
    ImportService(EventBus& event_bus, Config& config);

    // Register an import format.
    auto register_format(std::unique_ptr<IImportFormat> format) -> void;

    // Get all registered format descriptors.
    [[nodiscard]] auto available_formats() const -> std::vector<ImportFormatDescriptor>;

    // Auto-detect the format of a file.
    [[nodiscard]] auto detect_format(const std::filesystem::path& file_path) const -> ImportFormat;

    // Import a single file.
    [[nodiscard]] auto import_file(const std::filesystem::path& source_path,
                                   const ImportOptions& options,
                                   ImportProgressCallback progress = nullptr) -> ImportResult;

    // Import a directory (recursively).
    [[nodiscard]] auto import_directory(const std::filesystem::path& source_dir,
                                        const ImportOptions& options,
                                        ImportProgressCallback progress = nullptr) -> ImportResult;

    // Register all built-in importers.
    auto register_builtins() -> void;

private:
    EventBus& event_bus_;
    Config& config_;
    mutable std::mutex mutex_;
    std::unordered_map<uint8_t, std::unique_ptr<IImportFormat>> formats_;

    // Find importer for a format.
    [[nodiscard]] auto find_importer(ImportFormat format) const -> IImportFormat*;

    // Process the converted Markdown into blocks and store.
    [[nodiscard]] auto process_markdown(const std::string& markdown_content,
                                        const std::string& doc_title,
                                        const ImportOptions& options)
        -> std::expected<std::string, std::string>;
};

} // namespace markamp::core
