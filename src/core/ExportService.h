// ============================================================================
// File: src/core/ExportService.h
// Phase 39: Multi-Format Export — Export orchestration service
// ============================================================================
#pragma once

#include "ExportTypes.h"
#include "IExportFormat.h"

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

// Progress callback: (current_item, percent).
using ExportProgressCallback = std::function<void(const std::string&, int)>;

// ExportService — manages format registrations and orchestrates exports.
class ExportService
{
public:
    ExportService(EventBus& event_bus, Config& config);

    // Register an export format.
    auto register_format(std::unique_ptr<IExportFormat> format) -> void;

    // Get all registered format descriptors.
    [[nodiscard]] auto available_formats() const -> std::vector<ExportFormatDescriptor>;

    // Check if a specific format is available.
    [[nodiscard]] auto is_format_available(ExportFormat format) const -> bool;

    // Export content using the specified options.
    [[nodiscard]] auto export_content(const std::string& markdown_source,
                                      const ExportOptions& options,
                                      ExportProgressCallback progress = nullptr) -> ExportResult;

    // Export a document by ID.
    [[nodiscard]] auto export_document(const std::string& doc_id,
                                       const ExportOptions& options,
                                       ExportProgressCallback progress = nullptr) -> ExportResult;

    // Export a notebook (all documents).
    [[nodiscard]] auto export_notebook(const std::string& notebook_id,
                                       const ExportOptions& options,
                                       ExportProgressCallback progress = nullptr) -> ExportResult;

    // Get the default output filename for a format.
    [[nodiscard]] static auto default_filename(const std::string& base_name, ExportFormat format)
        -> std::string;

    // Register all built-in format exporters.
    auto register_builtins() -> void;

    /// (#191) Return the number of registered export formats.
    [[nodiscard]] auto format_count() const -> std::size_t;

    /// (#192) Check if an exporter is registered and its dependencies are met.
    [[nodiscard]] auto has_exporter(ExportFormat format) const -> bool;

private:
    [[maybe_unused]] EventBus& event_bus_;
    [[maybe_unused]] Config& config_;
    mutable std::mutex mutex_;

    // Map of format -> exporter.
    std::unordered_map<uint8_t, std::unique_ptr<IExportFormat>> formats_;

    // Find the exporter for a format.
    [[nodiscard]] auto find_exporter(ExportFormat format) const -> IExportFormat*;

    // Copy assets referenced in the content to the output directory.
    auto copy_assets(const std::string& content,
                     const std::filesystem::path& output_dir,
                     ExportResult& result) -> std::string;

    // Embed assets as base64 in the content.
    auto embed_assets(const std::string& content) -> std::string;

    // Generate a table of contents from headings.
    [[nodiscard]] static auto generate_toc(const std::string& markdown_source) -> std::string;
};

} // namespace markamp::core
