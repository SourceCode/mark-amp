/// @file CanvasCompletionAuditor.h
/// @brief V23 Phase 07 — Canvas board, tool, import/export, and collaboration completion auditor.
///
/// Audits that canvas subsystem behaviors — board lifecycle, tools, import/export,
/// metadata, PDF objects, widgets, and collaboration — have real implementations.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

enum class CanvasCapabilityArea : uint8_t
{
    kBoardLifecycle,       ///< Create, open, rename, save, restore
    kToolIntegration,      ///< Draw, select, pan, shape recognition
    kObjectSerialization,  ///< Object to_json/from_json roundtrip
    kImportExport,         ///< Board import/export to image, PDF, SVG
    kCollaboration,        ///< Real-time collaboration, cursors, sync
    kWidgets,              ///< App widgets, embedded content
    kMetadata,             ///< Metadata scraping, link previews
    kPDFObjects,           ///< PDF page objects on canvas
};

[[nodiscard]] constexpr auto canvas_capability_label(CanvasCapabilityArea area) -> const char*
{
    switch (area)
    {
    case CanvasCapabilityArea::kBoardLifecycle:      return "BoardLifecycle";
    case CanvasCapabilityArea::kToolIntegration:     return "ToolIntegration";
    case CanvasCapabilityArea::kObjectSerialization: return "ObjectSerialization";
    case CanvasCapabilityArea::kImportExport:        return "ImportExport";
    case CanvasCapabilityArea::kCollaboration:       return "Collaboration";
    case CanvasCapabilityArea::kWidgets:             return "Widgets";
    case CanvasCapabilityArea::kMetadata:            return "Metadata";
    case CanvasCapabilityArea::kPDFObjects:          return "PDFObjects";
    }
    return "Unknown";
}

struct CanvasCompletionItem
{
    CanvasCapabilityArea area{CanvasCapabilityArea::kBoardLifecycle};
    std::string feature_name;
    bool is_implemented{false};
    bool has_roundtrip{false};       ///< Serialization roundtrips correctly
    bool has_error_handling{false};
    std::string evidence_file;
    int evidence_line{0};

    [[nodiscard]] auto is_complete() const noexcept -> bool
    { return is_implemented && has_error_handling; }
};

struct CanvasCompletionReport
{
    std::size_t total{0};
    std::size_t complete{0};
    std::size_t incomplete{0};

    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    { return total > 0 ? (static_cast<double>(complete) / static_cast<double>(total)) * 100.0 : 100.0; }
};

class CanvasCompletionAuditor
{
public:
    CanvasCompletionAuditor() = default;

    void add_item(CanvasCompletionItem item);
    void add_items(std::vector<CanvasCompletionItem> items);

    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(CanvasCapabilityArea area) const
        -> std::vector<const CanvasCompletionItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const CanvasCompletionItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const CanvasCompletionItem*>;

    [[nodiscard]] auto report() const -> CanvasCompletionReport;
    void clear();

    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;

private:
    std::vector<CanvasCompletionItem> items_;
};

} // namespace markamp::core
