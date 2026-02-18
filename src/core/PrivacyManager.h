/// @file PrivacyManager.h
/// @brief V9 Phase 44 — Document privacy classification and policy management.
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Privacy classification level.
enum class PrivacyLevel : uint8_t
{
    kPublic = 0,
    kInternal = 1,
    kConfidential = 2,
    kSecret = 3,
};

/// Global privacy policy.
struct PrivacyPolicy
{
    PrivacyLevel default_level{PrivacyLevel::kInternal};
    bool redact_on_export{false};
    bool strip_metadata{false};
    bool anonymize_usernames{false};
    int retention_days{365};
};

/// Classification of a single document.
struct PrivacyClassification
{
    std::string document_id;
    PrivacyLevel level{PrivacyLevel::kInternal};
    std::vector<std::string> tags;
    std::chrono::system_clock::time_point classified_at;
};

/// Convert level to display name.
[[nodiscard]] auto privacy_level_name(PrivacyLevel level) -> std::string;

/// Manages document privacy levels and policies.
class PrivacyManager
{
public:
    PrivacyManager() = default;

    // ── Classification ────────────────────────────────────────────────
    auto classify_document(const std::string& doc_id, PrivacyLevel level) -> bool;
    [[nodiscard]] auto get_classification(const std::string& doc_id) const
        -> const PrivacyClassification*;
    auto remove_classification(const std::string& doc_id) -> bool;

    // ── Policy ────────────────────────────────────────────────────────
    void set_policy(PrivacyPolicy policy);
    [[nodiscard]] auto get_policy() const -> PrivacyPolicy;
    void apply_policy_defaults();

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto documents_at_level(PrivacyLevel level) const
        -> std::vector<const PrivacyClassification*>;
    [[nodiscard]] auto classification_count() const -> int;
    void clear_all();

private:
    std::vector<PrivacyClassification> classifications_;
    PrivacyPolicy policy_;
};

} // namespace markamp::core
