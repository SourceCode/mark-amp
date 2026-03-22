/// @file PluginMarketplaceCompletionAuditor.h
/// @brief V23 Phase 11 — Plugin, extension, theme, marketplace, and contribution completion auditor.
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core {

enum class PluginCapabilityArea : uint8_t {
    kContributionFulfillment, kMarketplaceListings, kThemeSharing,
    kActivationLifecycle, kQuarantineRecovery, kTelemetry,
    kServiceSurfaces, kCompletionGates,
};

[[nodiscard]] constexpr auto plugin_capability_label(PluginCapabilityArea a) -> const char* {
    switch (a) {
    case PluginCapabilityArea::kContributionFulfillment: return "ContributionFulfillment";
    case PluginCapabilityArea::kMarketplaceListings:     return "MarketplaceListings";
    case PluginCapabilityArea::kThemeSharing:            return "ThemeSharing";
    case PluginCapabilityArea::kActivationLifecycle:     return "ActivationLifecycle";
    case PluginCapabilityArea::kQuarantineRecovery:      return "QuarantineRecovery";
    case PluginCapabilityArea::kTelemetry:               return "Telemetry";
    case PluginCapabilityArea::kServiceSurfaces:         return "ServiceSurfaces";
    case PluginCapabilityArea::kCompletionGates:         return "CompletionGates";
    } return "Unknown";
}

struct PluginCompletionItem {
    PluginCapabilityArea area{PluginCapabilityArea::kContributionFulfillment};
    std::string feature_name;
    bool is_fulfilled{false};
    bool has_error_handling{false};
    std::string evidence_file;
    int evidence_line{0};
    [[nodiscard]] auto is_complete() const noexcept -> bool { return is_fulfilled && has_error_handling; }
};

struct PluginCompletionReport {
    std::size_t total{0}; std::size_t complete{0}; std::size_t incomplete{0};
    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    { return total > 0 ? (static_cast<double>(complete) / static_cast<double>(total)) * 100.0 : 100.0; }
};

class PluginMarketplaceCompletionAuditor {
public:
    void add_item(PluginCompletionItem item);
    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(PluginCapabilityArea a) const -> std::vector<const PluginCompletionItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const PluginCompletionItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const PluginCompletionItem*>;
    [[nodiscard]] auto report() const -> PluginCompletionReport;
    void clear();
    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;
private:
    std::vector<PluginCompletionItem> items_;
};
} // namespace markamp::core
