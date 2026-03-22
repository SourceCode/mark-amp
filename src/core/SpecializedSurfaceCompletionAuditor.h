/// @file SpecializedSurfaceCompletionAuditor.h
/// @brief V23 Phase 08 — Panel, shell, PDF, history, diff, and specialized surface completion auditor.
///
/// Audits that specialized panels and secondary surfaces have real implementations
/// rather than phase-labeled stubs or placeholder-backed utilities.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

enum class SurfaceCapabilityArea : uint8_t
{
    kPDFViewer,           ///< PDF page rendering, thumbnails, annotations
    kHistoryTimeline,     ///< File/workspace history timeline
    kDiffViewer,          ///< Diff view, merge conflict
    kTerminalPanel,       ///< Terminal integration
    kOutputPanel,         ///< Build/task output
    kDebugPanel,          ///< Debug surfaces
    kProblemsPanel,       ///< Diagnostics / problems list
    kBreadcrumb,          ///< Navigation breadcrumb
};

[[nodiscard]] constexpr auto surface_capability_label(SurfaceCapabilityArea area) -> const char*
{
    switch (area)
    {
    case SurfaceCapabilityArea::kPDFViewer:       return "PDFViewer";
    case SurfaceCapabilityArea::kHistoryTimeline: return "HistoryTimeline";
    case SurfaceCapabilityArea::kDiffViewer:      return "DiffViewer";
    case SurfaceCapabilityArea::kTerminalPanel:   return "TerminalPanel";
    case SurfaceCapabilityArea::kOutputPanel:     return "OutputPanel";
    case SurfaceCapabilityArea::kDebugPanel:      return "DebugPanel";
    case SurfaceCapabilityArea::kProblemsPanel:   return "ProblemsPanel";
    case SurfaceCapabilityArea::kBreadcrumb:      return "Breadcrumb";
    }
    return "Unknown";
}

struct SurfaceCompletionItem
{
    SurfaceCapabilityArea area{SurfaceCapabilityArea::kPDFViewer};
    std::string feature_name;
    bool is_implemented{false};
    bool has_persistence{false};     ///< State persists across sessions
    bool has_error_handling{false};
    std::string evidence_file;
    int evidence_line{0};

    [[nodiscard]] auto is_complete() const noexcept -> bool
    { return is_implemented && has_error_handling; }
};

struct SurfaceCompletionReport
{
    std::size_t total{0};
    std::size_t complete{0};
    std::size_t incomplete{0};

    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    { return total > 0 ? (static_cast<double>(complete) / static_cast<double>(total)) * 100.0 : 100.0; }
};

class SpecializedSurfaceCompletionAuditor
{
public:
    SpecializedSurfaceCompletionAuditor() = default;

    void add_item(SurfaceCompletionItem item);
    void add_items(std::vector<SurfaceCompletionItem> items);

    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(SurfaceCapabilityArea area) const
        -> std::vector<const SurfaceCompletionItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const SurfaceCompletionItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const SurfaceCompletionItem*>;

    [[nodiscard]] auto report() const -> SurfaceCompletionReport;
    void clear();

    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;

private:
    std::vector<SurfaceCompletionItem> items_;
};

} // namespace markamp::core
