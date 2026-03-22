/// @file FeatureFlagMigrationCompletionAuditor.h
/// @brief V23 Phase 18 — Feature flag, gated capability, and migration seam completion auditor.
#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace markamp::core {
enum class FeatureFlagArea : uint8_t {
    kFlagInventory, kDualPathMigration, kGatedPanels,
    kMigrationReporting, kClosureTests, kShadowMigration,
    kExperimentalFlags, kRetirementTracking,
};
[[nodiscard]] constexpr auto feature_flag_label(FeatureFlagArea a) -> const char* {
    switch (a) {
    case FeatureFlagArea::kFlagInventory:      return "FlagInventory";
    case FeatureFlagArea::kDualPathMigration:  return "DualPathMigration";
    case FeatureFlagArea::kGatedPanels:        return "GatedPanels";
    case FeatureFlagArea::kMigrationReporting: return "MigrationReporting";
    case FeatureFlagArea::kClosureTests:       return "ClosureTests";
    case FeatureFlagArea::kShadowMigration:    return "ShadowMigration";
    case FeatureFlagArea::kExperimentalFlags:  return "ExperimentalFlags";
    case FeatureFlagArea::kRetirementTracking: return "RetirementTracking";
    } return "Unknown";
}
struct FeatureFlagItem {
    FeatureFlagArea area{FeatureFlagArea::kFlagInventory};
    std::string feature_name;
    bool is_resolved{false}; bool has_error_handling{false};
    std::string evidence_file; int evidence_line{0};
    [[nodiscard]] auto is_complete() const noexcept -> bool { return is_resolved && has_error_handling; }
};
struct FeatureFlagReport {
    std::size_t total{0}; std::size_t complete{0}; std::size_t incomplete{0};
    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    { return total > 0 ? (static_cast<double>(complete)/static_cast<double>(total))*100.0 : 100.0; }
};
class FeatureFlagMigrationCompletionAuditor {
public:
    void add_item(FeatureFlagItem item);
    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(FeatureFlagArea a) const -> std::vector<const FeatureFlagItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const FeatureFlagItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const FeatureFlagItem*>;
    [[nodiscard]] auto report() const -> FeatureFlagReport;
    void clear();
    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;
private:
    std::vector<FeatureFlagItem> items_;
};
} // namespace markamp::core
