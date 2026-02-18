/// @file SlideExporter.h
/// @brief V9 Phase 43 — Export presentations to PDF, HTML, and images.
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Export output format.
enum class SlideExportFormat : uint8_t
{
    kPdf = 0,
    kHtml = 1,
    kPng = 2,
    kSvg = 3,
    kPptx = 4,
};

/// Export status.
enum class ExportStatus : uint8_t
{
    kPending = 0,
    kInProgress = 1,
    kCompleted = 2,
    kFailed = 3,
};

/// Options for slide export.
struct SlideExportOptions
{
    SlideExportFormat format{SlideExportFormat::kPdf};
    std::string output_path;
    bool include_speaker_notes{false};
    bool include_hidden_slides{false};
    int image_dpi{150};
    std::string page_size; ///< e.g. "16:9", "4:3", "A4"
};

/// Result of an export operation.
struct SlideExportResult
{
    std::string export_id;
    std::string presentation_id;
    SlideExportFormat format{SlideExportFormat::kPdf};
    ExportStatus status{ExportStatus::kPending};
    std::string output_path;
    int slides_exported{0};
    int file_size_bytes{0};
    std::string error_message;
    std::chrono::system_clock::time_point completed_at;
};

/// Convert format to display name.
[[nodiscard]] auto slide_export_format_name(SlideExportFormat format) -> std::string;

/// Service for exporting slide presentations.
class SlideExporter
{
public:
    SlideExporter() = default;

    // ── Export operations ─────────────────────────────────────────────
    auto export_presentation(const std::string& presentation_id, const SlideExportOptions& options)
        -> std::string;
    auto cancel_export(const std::string& export_id) -> bool;

    // ── Status ────────────────────────────────────────────────────────
    [[nodiscard]] auto find_export(const std::string& export_id) const -> const SlideExportResult*;
    [[nodiscard]] auto export_count() const -> int;

    // ── History ───────────────────────────────────────────────────────
    [[nodiscard]] auto all_exports() const -> std::vector<const SlideExportResult*>;
    [[nodiscard]] auto exports_by_format(SlideExportFormat format) const
        -> std::vector<const SlideExportResult*>;
    void clear_history();

private:
    std::vector<SlideExportResult> exports_;
    int next_id_{1};
};

} // namespace markamp::core
