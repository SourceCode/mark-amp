/// @file DataRedactionEngine.h
/// @brief V9 Phase 44 — Pattern-based PII detection and redaction.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Type of redaction rule.
enum class RedactionRuleType : uint8_t
{
    kEmail = 0,
    kPhone = 1,
    kSsn = 2,
    kCreditCard = 3,
    kIpAddress = 4,
    kCustomRegex = 5,
};

/// A single redaction rule.
struct RedactionRule
{
    std::string rule_id;
    std::string pattern;     ///< Regex or simple pattern
    std::string replacement; ///< e.g. "[REDACTED]"
    RedactionRuleType rule_type{RedactionRuleType::kCustomRegex};
    bool is_active{true};
};

/// Result of a redaction operation.
struct RedactionResult
{
    std::string redacted_text;
    int original_length{0};
    int redacted_length{0};
    int redactions_applied{0};
    int rules_matched{0};
};

/// A PII match found during scanning.
struct PiiMatch
{
    std::string matched_text;
    RedactionRuleType rule_type{RedactionRuleType::kCustomRegex};
    int position{0};
    int length{0};
};

/// Convert rule type to display name.
[[nodiscard]] auto redaction_rule_type_name(RedactionRuleType rule_type) -> std::string;

/// Pattern-based PII detection and redaction engine.
class DataRedactionEngine
{
public:
    DataRedactionEngine() = default;

    // ── Rule management ───────────────────────────────────────────────
    void add_rule(RedactionRule rule);
    auto remove_rule(const std::string& rule_id) -> bool;
    [[nodiscard]] auto find_rule(const std::string& rule_id) const -> const RedactionRule*;
    void load_defaults();

    // ── Redaction ─────────────────────────────────────────────────────
    [[nodiscard]] auto redact(const std::string& content) const -> RedactionResult;
    [[nodiscard]] auto scan(const std::string& content) const -> std::vector<PiiMatch>;

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto rule_count() const -> int;
    [[nodiscard]] auto active_rule_count() const -> int;
    void clear_rules();

private:
    std::vector<RedactionRule> rules_;
    int next_id_{1};

    /// Simple substring-based redaction (avoids regex dependency for testability).
    [[nodiscard]] auto apply_rule(const std::string& content, const RedactionRule& rule) const
        -> std::pair<std::string, int>;
};

} // namespace markamp::core
