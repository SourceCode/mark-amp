/// @file SlideExporter.cpp
/// @brief V9 Phase 43 — SlideExporter implementation.

#include "SlideExporter.h"

#include <algorithm>

namespace markamp::core
{

auto slide_export_format_name(SlideExportFormat format) -> std::string
{
    switch (format)
    {
        case SlideExportFormat::kPdf:
            return "pdf";
        case SlideExportFormat::kHtml:
            return "html";
        case SlideExportFormat::kPng:
            return "png";
        case SlideExportFormat::kSvg:
            return "svg";
        case SlideExportFormat::kPptx:
            return "pptx";
    }
    return "unknown";
}

auto SlideExporter::export_presentation(const std::string& presentation_id,
                                        const SlideExportOptions& options) -> std::string
{
    SlideExportResult result;
    result.export_id = "exp_" + std::to_string(next_id_++);
    result.presentation_id = presentation_id;
    result.format = options.format;
    result.output_path = options.output_path;

    // Simulate export
    result.slides_exported = 10;
    result.file_size_bytes = 1024 * 50; // 50 KB
    result.status = ExportStatus::kCompleted;
    result.completed_at = std::chrono::system_clock::now();

    auto exp_id = result.export_id;
    exports_.push_back(std::move(result));
    return exp_id;
}

auto SlideExporter::cancel_export(const std::string& export_id) -> bool
{
    for (auto& exp : exports_)
    {
        if (exp.export_id == export_id && exp.status == ExportStatus::kPending)
        {
            exp.status = ExportStatus::kFailed;
            exp.error_message = "Cancelled by user";
            return true;
        }
    }
    return false;
}

auto SlideExporter::find_export(const std::string& export_id) const -> const SlideExportResult*
{
    for (const auto& exp : exports_)
    {
        if (exp.export_id == export_id)
        {
            return &exp;
        }
    }
    return nullptr;
}

auto SlideExporter::export_count() const -> int
{
    return static_cast<int>(exports_.size());
}

auto SlideExporter::all_exports() const -> std::vector<const SlideExportResult*>
{
    std::vector<const SlideExportResult*> result;
    result.reserve(exports_.size());
    for (const auto& exp : exports_)
    {
        result.push_back(&exp);
    }
    return result;
}

auto SlideExporter::exports_by_format(SlideExportFormat format) const
    -> std::vector<const SlideExportResult*>
{
    std::vector<const SlideExportResult*> result;
    for (const auto& exp : exports_)
    {
        if (exp.format == format)
        {
            result.push_back(&exp);
        }
    }
    return result;
}

void SlideExporter::clear_history()
{
    exports_.clear();
}

} // namespace markamp::core
