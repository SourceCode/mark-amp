/// @file StructuredDataCompletionAuditor.h
/// @brief V23 Phase 15 — Structured data, AV, graph, task, and knowledge workflow completion auditor.
#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace markamp::core {
enum class StructuredDataArea : uint8_t {
    kAVTableEditor, kKnowledgeGraph, kDeckFlashcard,
    kSearchIndexing, kBacklinks, kActivityTimeline,
    kTaskCalendar, kDataMigration,
};
[[nodiscard]] constexpr auto structured_data_label(StructuredDataArea a) -> const char* {
    switch (a) {
    case StructuredDataArea::kAVTableEditor:    return "AVTableEditor";
    case StructuredDataArea::kKnowledgeGraph:   return "KnowledgeGraph";
    case StructuredDataArea::kDeckFlashcard:    return "DeckFlashcard";
    case StructuredDataArea::kSearchIndexing:   return "SearchIndexing";
    case StructuredDataArea::kBacklinks:        return "Backlinks";
    case StructuredDataArea::kActivityTimeline: return "ActivityTimeline";
    case StructuredDataArea::kTaskCalendar:     return "TaskCalendar";
    case StructuredDataArea::kDataMigration:    return "DataMigration";
    } return "Unknown";
}
struct StructuredDataItem {
    StructuredDataArea area{StructuredDataArea::kAVTableEditor};
    std::string feature_name;
    bool is_real_data_flow{false}; bool has_error_handling{false};
    std::string evidence_file; int evidence_line{0};
    [[nodiscard]] auto is_complete() const noexcept -> bool { return is_real_data_flow && has_error_handling; }
};
struct StructuredDataReport {
    std::size_t total{0}; std::size_t complete{0}; std::size_t incomplete{0};
    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    { return total > 0 ? (static_cast<double>(complete)/static_cast<double>(total))*100.0 : 100.0; }
};
class StructuredDataCompletionAuditor {
public:
    void add_item(StructuredDataItem item);
    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(StructuredDataArea a) const -> std::vector<const StructuredDataItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const StructuredDataItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const StructuredDataItem*>;
    [[nodiscard]] auto report() const -> StructuredDataReport;
    void clear();
    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;
private:
    std::vector<StructuredDataItem> items_;
};
} // namespace markamp::core
