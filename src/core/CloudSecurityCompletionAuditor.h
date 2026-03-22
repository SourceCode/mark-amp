/// @file CloudSecurityCompletionAuditor.h
/// @brief V23 Phase 12 — Cloud sync, security, vault, and network transport completion auditor.
#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace markamp::core {
enum class CloudSecurityArea : uint8_t {
    kEncryption, kSyncTransport, kVaultWatching, kTelemetryExport,
    kRetryRecovery, kOfflineQueue, kCredentialManagement, kSecurityHarness,
};
[[nodiscard]] constexpr auto cloud_security_label(CloudSecurityArea a) -> const char* {
    switch (a) {
    case CloudSecurityArea::kEncryption:           return "Encryption";
    case CloudSecurityArea::kSyncTransport:        return "SyncTransport";
    case CloudSecurityArea::kVaultWatching:        return "VaultWatching";
    case CloudSecurityArea::kTelemetryExport:      return "TelemetryExport";
    case CloudSecurityArea::kRetryRecovery:        return "RetryRecovery";
    case CloudSecurityArea::kOfflineQueue:         return "OfflineQueue";
    case CloudSecurityArea::kCredentialManagement: return "CredentialManagement";
    case CloudSecurityArea::kSecurityHarness:      return "SecurityHarness";
    } return "Unknown";
}
struct CloudSecurityItem {
    CloudSecurityArea area{CloudSecurityArea::kEncryption};
    std::string feature_name;
    bool is_real_crypto{false}; bool has_error_handling{false};
    std::string evidence_file; int evidence_line{0};
    [[nodiscard]] auto is_complete() const noexcept -> bool { return is_real_crypto && has_error_handling; }
};
struct CloudSecurityReport {
    std::size_t total{0}; std::size_t complete{0}; std::size_t incomplete{0};
    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    { return total > 0 ? (static_cast<double>(complete)/static_cast<double>(total))*100.0 : 100.0; }
};
class CloudSecurityCompletionAuditor {
public:
    void add_item(CloudSecurityItem item);
    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(CloudSecurityArea a) const -> std::vector<const CloudSecurityItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const CloudSecurityItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const CloudSecurityItem*>;
    [[nodiscard]] auto report() const -> CloudSecurityReport;
    void clear();
    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;
private:
    std::vector<CloudSecurityItem> items_;
};
} // namespace markamp::core
