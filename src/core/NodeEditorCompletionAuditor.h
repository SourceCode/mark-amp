/// @file NodeEditorCompletionAuditor.h
/// @brief V23 Phase 14 — Node editor, domain runtime, and advanced workflow completion auditor.
#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace markamp::core {
enum class NodeEditorArea : uint8_t {
    kDomainRuntime, kInteractionFidelity, kTransformEvaluation,
    kPersistenceExport, kHitTesting, kPreviewSystem,
    kGraphSerialization, kCompletionGates,
};
[[nodiscard]] constexpr auto node_editor_area_label(NodeEditorArea a) -> const char* {
    switch (a) {
    case NodeEditorArea::kDomainRuntime:       return "DomainRuntime";
    case NodeEditorArea::kInteractionFidelity: return "InteractionFidelity";
    case NodeEditorArea::kTransformEvaluation: return "TransformEvaluation";
    case NodeEditorArea::kPersistenceExport:   return "PersistenceExport";
    case NodeEditorArea::kHitTesting:          return "HitTesting";
    case NodeEditorArea::kPreviewSystem:       return "PreviewSystem";
    case NodeEditorArea::kGraphSerialization:  return "GraphSerialization";
    case NodeEditorArea::kCompletionGates:     return "CompletionGates";
    } return "Unknown";
}
struct NodeEditorCompletionItem {
    NodeEditorArea area{NodeEditorArea::kDomainRuntime};
    std::string feature_name;
    bool is_real_execution{false}; bool has_error_handling{false};
    std::string evidence_file; int evidence_line{0};
    [[nodiscard]] auto is_complete() const noexcept -> bool { return is_real_execution && has_error_handling; }
};
struct NodeEditorCompletionReport {
    std::size_t total{0}; std::size_t complete{0}; std::size_t incomplete{0};
    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    { return total > 0 ? (static_cast<double>(complete)/static_cast<double>(total))*100.0 : 100.0; }
};
class NodeEditorCompletionAuditor {
public:
    void add_item(NodeEditorCompletionItem item);
    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(NodeEditorArea a) const -> std::vector<const NodeEditorCompletionItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const NodeEditorCompletionItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const NodeEditorCompletionItem*>;
    [[nodiscard]] auto report() const -> NodeEditorCompletionReport;
    void clear();
    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;
private:
    std::vector<NodeEditorCompletionItem> items_;
};
} // namespace markamp::core
