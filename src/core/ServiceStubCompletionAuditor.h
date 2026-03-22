/// @file ServiceStubCompletionAuditor.h
/// @brief V23 Phase 17 — Service stub replacement and interface adoption completion auditor.
#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace markamp::core {
enum class ServiceStubArea : uint8_t {
    kDefaultReturns, kInterfaceAdoption, kUnsupportedState,
    kHealthReporting, kCapabilityContracts, kMigrationParity,
    kServiceReadiness, kRegressionCoverage,
};
[[nodiscard]] constexpr auto service_stub_label(ServiceStubArea a) -> const char* {
    switch (a) {
    case ServiceStubArea::kDefaultReturns:     return "DefaultReturns";
    case ServiceStubArea::kInterfaceAdoption:  return "InterfaceAdoption";
    case ServiceStubArea::kUnsupportedState:   return "UnsupportedState";
    case ServiceStubArea::kHealthReporting:    return "HealthReporting";
    case ServiceStubArea::kCapabilityContracts:return "CapabilityContracts";
    case ServiceStubArea::kMigrationParity:    return "MigrationParity";
    case ServiceStubArea::kServiceReadiness:   return "ServiceReadiness";
    case ServiceStubArea::kRegressionCoverage: return "RegressionCoverage";
    } return "Unknown";
}
struct ServiceStubItem {
    ServiceStubArea area{ServiceStubArea::kDefaultReturns};
    std::string feature_name;
    bool is_real_service{false}; bool has_error_handling{false};
    std::string evidence_file; int evidence_line{0};
    [[nodiscard]] auto is_complete() const noexcept -> bool { return is_real_service && has_error_handling; }
};
struct ServiceStubReport {
    std::size_t total{0}; std::size_t complete{0}; std::size_t incomplete{0};
    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    { return total > 0 ? (static_cast<double>(complete)/static_cast<double>(total))*100.0 : 100.0; }
};
class ServiceStubCompletionAuditor {
public:
    void add_item(ServiceStubItem item);
    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(ServiceStubArea a) const -> std::vector<const ServiceStubItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const ServiceStubItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const ServiceStubItem*>;
    [[nodiscard]] auto report() const -> ServiceStubReport;
    void clear();
    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;
private:
    std::vector<ServiceStubItem> items_;
};
} // namespace markamp::core
