#pragma once

/**
 * @file TelemetryPrivacyController.h
 * @brief Phase 38 Task 4: Privacy tiers, opt-in/out, retention policy.
 *
 * Manages telemetry privacy settings including tiers (off/anonymous/detailed),
 * data retention limits, and metric event schema validation.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Privacy tier for telemetry.
enum class PrivacyTier : uint8_t
{
    kOff,       ///< No telemetry
    kAnonymous, ///< Aggregated counts only, no identifiers
    kDetailed,  ///< Full events with context
};

/// Data retention policy.
struct RetentionPolicy
{
    int max_events{10000};
    int max_age_hours{720}; ///< 30 days default

    /// Check if the policy is valid.
    [[nodiscard]] auto is_valid() const -> bool;
};

/// Schema for a metric event field.
struct MetricFieldSchema
{
    std::string field_name;
    bool required{true};
    bool pii_sensitive{false}; ///< Contains personally identifiable info
};

/**
 * @brief Manages telemetry privacy settings.
 */
class TelemetryPrivacyController
{
public:
    TelemetryPrivacyController() = default;

    // ── Privacy tier ───────────────────────────────────────────────

    /// Set the privacy tier.
    void set_tier(PrivacyTier tier);

    /// Get the current privacy tier.
    [[nodiscard]] auto tier() const -> PrivacyTier;

    /// Get tier as string.
    [[nodiscard]] auto tier_name() const -> std::string;

    /// Check if telemetry is active (not off).
    [[nodiscard]] auto is_active() const -> bool;

    /// Check if detailed context is allowed.
    [[nodiscard]] auto allows_context() const -> bool;

    // ── Opt-in/out ─────────────────────────────────────────────────

    /// Set explicit user consent.
    void set_user_consent(bool consented);

    /// Check if user has consented.
    [[nodiscard]] auto has_user_consent() const -> bool;

    // ── Retention ──────────────────────────────────────────────────

    /// Set the retention policy.
    void set_retention(const RetentionPolicy& policy);

    /// Get the retention policy.
    [[nodiscard]] auto retention() const -> const RetentionPolicy&;

    // ── Schema validation ──────────────────────────────────────────

    /// Register a field schema.
    void register_field(const MetricFieldSchema& field);

    /// Get registered fields.
    [[nodiscard]] auto fields() const -> const std::vector<MetricFieldSchema>&;

    /// Check if a field is PII-sensitive.
    [[nodiscard]] auto is_pii_field(const std::string& field_name) const -> bool;

    /// Get PII-sensitive field count.
    [[nodiscard]] auto pii_field_count() const -> int;

    /// Check if PII fields are allowed at the current tier.
    [[nodiscard]] auto pii_allowed() const -> bool;

private:
    PrivacyTier tier_{PrivacyTier::kOff};
    bool user_consent_{false};
    RetentionPolicy retention_;
    std::vector<MetricFieldSchema> fields_;
};

} // namespace markamp::ui
