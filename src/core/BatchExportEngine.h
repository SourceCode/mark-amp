// ============================================================================
// File: src/core/BatchExportEngine.h
// Phase 24: Export & Publishing — Multi-document batch export engine
// ============================================================================
#pragma once

#include "ExportTypes.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class ExportService;

// ============================================================================
// Data structures
// ============================================================================

/// A single document entry for batch export.
struct BatchDocumentEntry
{
    std::string document_id;     ///< Internal document identifier
    std::string title;           ///< Display title (used for file naming)
    std::string markdown_source; ///< Raw markdown content
};

/// File naming pattern tokens: {title}, {date}, {index}, {format}.
enum class NamingToken : uint8_t
{
    kTitle, ///< Document title (slugified)
    kDate,  ///< Export date (YYYY-MM-DD)
    kIndex, ///< Sequential 1-based index
    kFormat ///< Format extension
};

/// Configuration for a batch export job.
struct BatchExportJob
{
    std::vector<BatchDocumentEntry> documents; ///< Documents to export
    ExportOptions options;                     ///< Shared export options
    std::string output_directory;              ///< Target directory
    std::string naming_pattern{"{title}"};     ///< File naming pattern
    bool generate_index_page{true};            ///< Create HTML index
    bool stop_on_error{false};                 ///< Abort on first error
};

/// Per-document result within a batch.
struct BatchItemResult
{
    std::string document_id;
    std::string title;
    std::string output_filename;
    bool success{false};
    std::string error_message;
    int64_t output_size_bytes{0};
};

/// Overall batch export result.
struct BatchExportResult
{
    bool completed{false};              ///< True if job ran to completion
    bool cancelled{false};              ///< True if cancelled mid-run
    int32_t total{0};                   ///< Total documents attempted
    int32_t succeeded{0};               ///< Documents exported successfully
    int32_t failed{0};                  ///< Documents that failed
    int64_t total_size_bytes{0};        ///< Aggregate output size
    int64_t elapsed_ms{0};              ///< Wall-clock time
    std::string index_path;             ///< Path to generated index page
    std::vector<BatchItemResult> items; ///< Per-document results
};

/// Progress callback: (current_index, total, document_title).
using BatchProgressCallback =
    std::function<void(int32_t current, int32_t total, const std::string& title)>;

// ============================================================================
// BatchExportEngine
// ============================================================================

/// BatchExportEngine — exports multiple documents in one operation with
/// progress tracking, error isolation, and optional HTML index generation.
class BatchExportEngine
{
public:
    explicit BatchExportEngine(EventBus& event_bus);

    /// Set the ExportService to delegate individual exports to.
    auto set_export_service(ExportService& svc) -> void;

    /// Execute a batch export job.  Returns the aggregate result.
    [[nodiscard]] auto execute(const BatchExportJob& job, BatchProgressCallback progress = nullptr)
        -> BatchExportResult;

    /// Request cancellation of the currently running batch.
    auto cancel() -> void;

    /// Check whether cancellation has been requested.
    [[nodiscard]] auto is_cancelled() const -> bool;

    // ----- Helpers ----------------------------------------------------------

    /// Resolve a naming pattern for a specific document.
    /// Supported tokens: {title}, {date}, {index}, {format}.
    [[nodiscard]] static auto resolve_filename(const std::string& pattern,
                                               const std::string& title,
                                               int index,
                                               const std::string& extension,
                                               const std::string& date) -> std::string;

    /// Slugify a title for use as a filename (lowercase, hyphens, no specials).
    [[nodiscard]] static auto slugify(const std::string& title) -> std::string;

    /// (#102) Return the number of documents in a batch export job.
    [[nodiscard]] static auto document_count(const BatchExportJob& job) -> std::size_t;

    /// Generate an HTML index page linking to all exported files.
    [[nodiscard]] static auto generate_index(const std::string& batch_title,
                                             const std::vector<BatchItemResult>& items)
        -> std::string;

private:
    EventBus& event_bus_;
    ExportService* export_service_{nullptr};
    bool cancelled_{false};
};

} // namespace markamp::core
