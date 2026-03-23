/// @file V27ThemeAudit.h
/// @brief V27 Phase 17 — Theme parity, token normalization, cross-surface correctness.
#pragma once
#include <string>
#include <vector>
namespace markamp::core
{
enum class V27ThemeAuditSeverity { kPass, kWarning, kFail };
struct V27ThemeAuditEntry {
    std::string surface;
    std::string token;
    V27ThemeAuditSeverity severity{V27ThemeAuditSeverity::kPass};
    std::string description;
    bool is_hardcoded{false};
};
struct V27ThemeAuditSummary {
    int total_checks{0};
    int pass_count{0};
    int warning_count{0};
    int fail_count{0};
    int hardcoded_colors{0};
    [[nodiscard]] auto pass_rate_pct() const noexcept -> int {
        return total_checks > 0 ? (pass_count * 100) / total_checks : 100;
    }
};
class V27ThemeAudit {
public:
    V27ThemeAudit() = default;
    void record(const V27ThemeAuditEntry& entry);
    [[nodiscard]] auto entries() const -> const std::vector<V27ThemeAuditEntry>& { return entries_; }
    [[nodiscard]] auto entry_count() const noexcept -> int { return static_cast<int>(entries_.size()); }
    [[nodiscard]] auto summary() const -> V27ThemeAuditSummary;
    [[nodiscard]] auto entries_for_surface(const std::string& surface) const -> std::vector<V27ThemeAuditEntry>;
    [[nodiscard]] auto hardcoded_entries() const -> std::vector<V27ThemeAuditEntry>;
    void run_full_audit();
private:
    std::vector<V27ThemeAuditEntry> entries_;
};
} // namespace markamp::core
