/// @file ReleaseGateCompletionAuditor.h
/// @brief V23 Phase 20 — Final placeholder eradication, dead path removal, and release gate completion auditor.
#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace markamp::core {
enum class ReleaseGateArea : uint8_t {
    kPlaceholderEradication, kDeadPathRemoval, kCompletionGates,
    kScoreboard, kFinalSweep, kLegacyDeletion,
    kWaiverProcess, kCertification,
};
[[nodiscard]] constexpr auto release_gate_label(ReleaseGateArea a) -> const char* {
    switch (a) {
    case ReleaseGateArea::kPlaceholderEradication: return "PlaceholderEradication";
    case ReleaseGateArea::kDeadPathRemoval:        return "DeadPathRemoval";
    case ReleaseGateArea::kCompletionGates:        return "CompletionGates";
    case ReleaseGateArea::kScoreboard:             return "Scoreboard";
    case ReleaseGateArea::kFinalSweep:             return "FinalSweep";
    case ReleaseGateArea::kLegacyDeletion:         return "LegacyDeletion";
    case ReleaseGateArea::kWaiverProcess:          return "WaiverProcess";
    case ReleaseGateArea::kCertification:          return "Certification";
    } return "Unknown";
}
struct ReleaseGateItem {
    ReleaseGateArea area{ReleaseGateArea::kPlaceholderEradication};
    std::string feature_name;
    bool is_resolved{false}; bool has_error_handling{false};
    std::string evidence_file; int evidence_line{0};
    [[nodiscard]] auto is_complete() const noexcept -> bool { return is_resolved && has_error_handling; }
};
struct ReleaseGateReport {
    std::size_t total{0}; std::size_t complete{0}; std::size_t incomplete{0};
    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    { return total > 0 ? (static_cast<double>(complete)/static_cast<double>(total))*100.0 : 100.0; }
};
class ReleaseGateCompletionAuditor {
public:
    void add_item(ReleaseGateItem item);
    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(ReleaseGateArea a) const -> std::vector<const ReleaseGateItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const ReleaseGateItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const ReleaseGateItem*>;
    [[nodiscard]] auto report() const -> ReleaseGateReport;
    void clear();
    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;
private:
    std::vector<ReleaseGateItem> items_;
};
} // namespace markamp::core
