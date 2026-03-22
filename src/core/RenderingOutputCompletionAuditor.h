/// @file RenderingOutputCompletionAuditor.h
/// @brief V23 Phase 13 — Rendering, markdown, mermaid, media, PDF, and print completion auditor.
#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace markamp::core {
enum class RenderingArea : uint8_t {
    kDiagramRenderer, kPDFExtraction, kExportTemplate, kPrintPrep,
    kTokenizerPipeline, kMediaFallback, kContentFidelity, kRegressionGates,
};
[[nodiscard]] constexpr auto rendering_area_label(RenderingArea a) -> const char* {
    switch (a) {
    case RenderingArea::kDiagramRenderer:   return "DiagramRenderer";
    case RenderingArea::kPDFExtraction:     return "PDFExtraction";
    case RenderingArea::kExportTemplate:    return "ExportTemplate";
    case RenderingArea::kPrintPrep:         return "PrintPrep";
    case RenderingArea::kTokenizerPipeline: return "TokenizerPipeline";
    case RenderingArea::kMediaFallback:     return "MediaFallback";
    case RenderingArea::kContentFidelity:   return "ContentFidelity";
    case RenderingArea::kRegressionGates:   return "RegressionGates";
    } return "Unknown";
}
struct RenderingCompletionItem {
    RenderingArea area{RenderingArea::kDiagramRenderer};
    std::string feature_name;
    bool is_real_renderer{false}; bool has_error_handling{false};
    std::string evidence_file; int evidence_line{0};
    [[nodiscard]] auto is_complete() const noexcept -> bool { return is_real_renderer && has_error_handling; }
};
struct RenderingCompletionReport {
    std::size_t total{0}; std::size_t complete{0}; std::size_t incomplete{0};
    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    { return total > 0 ? (static_cast<double>(complete)/static_cast<double>(total))*100.0 : 100.0; }
};
class RenderingOutputCompletionAuditor {
public:
    void add_item(RenderingCompletionItem item);
    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(RenderingArea a) const -> std::vector<const RenderingCompletionItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const RenderingCompletionItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const RenderingCompletionItem*>;
    [[nodiscard]] auto report() const -> RenderingCompletionReport;
    void clear();
    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;
private:
    std::vector<RenderingCompletionItem> items_;
};
} // namespace markamp::core
