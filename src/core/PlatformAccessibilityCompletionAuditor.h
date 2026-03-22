/// @file PlatformAccessibilityCompletionAuditor.h
/// @brief V23 Phase 16 — Platform, accessibility, and native service completion auditor.
#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace markamp::core {
enum class PlatformAccessibilityArea : uint8_t {
    kPlatformStubs, kAccessibilityBridge, kFileWatchUpdate,
    kPromptHost, kCapabilityReporting, kScreenReader,
    kShellIntegration, kCompletionGates,
};
[[nodiscard]] constexpr auto platform_accessibility_label(PlatformAccessibilityArea a) -> const char* {
    switch (a) {
    case PlatformAccessibilityArea::kPlatformStubs:       return "PlatformStubs";
    case PlatformAccessibilityArea::kAccessibilityBridge: return "AccessibilityBridge";
    case PlatformAccessibilityArea::kFileWatchUpdate:     return "FileWatchUpdate";
    case PlatformAccessibilityArea::kPromptHost:          return "PromptHost";
    case PlatformAccessibilityArea::kCapabilityReporting: return "CapabilityReporting";
    case PlatformAccessibilityArea::kScreenReader:        return "ScreenReader";
    case PlatformAccessibilityArea::kShellIntegration:    return "ShellIntegration";
    case PlatformAccessibilityArea::kCompletionGates:     return "CompletionGates";
    } return "Unknown";
}
struct PlatformAccessibilityItem {
    PlatformAccessibilityArea area{PlatformAccessibilityArea::kPlatformStubs};
    std::string feature_name;
    bool is_real_platform{false}; bool has_error_handling{false};
    std::string evidence_file; int evidence_line{0};
    [[nodiscard]] auto is_complete() const noexcept -> bool { return is_real_platform && has_error_handling; }
};
struct PlatformAccessibilityReport {
    std::size_t total{0}; std::size_t complete{0}; std::size_t incomplete{0};
    [[nodiscard]] auto has_gaps() const noexcept -> bool { return incomplete > 0; }
    [[nodiscard]] auto coverage_pct() const noexcept -> double
    { return total > 0 ? (static_cast<double>(complete)/static_cast<double>(total))*100.0 : 100.0; }
};
class PlatformAccessibilityCompletionAuditor {
public:
    void add_item(PlatformAccessibilityItem item);
    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(PlatformAccessibilityArea a) const -> std::vector<const PlatformAccessibilityItem*>;
    [[nodiscard]] auto complete_items() const -> std::vector<const PlatformAccessibilityItem*>;
    [[nodiscard]] auto incomplete_items() const -> std::vector<const PlatformAccessibilityItem*>;
    [[nodiscard]] auto report() const -> PlatformAccessibilityReport;
    void clear();
    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;
private:
    std::vector<PlatformAccessibilityItem> items_;
};
} // namespace markamp::core
