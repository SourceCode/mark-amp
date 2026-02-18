/// FaultDomain.h — V7 Phase 38: Subsystem fault domains
///
/// Health transitions: Healthy → Degraded → Faulted → Isolated
/// 8 pre-registered domains for subsystem isolation.

#pragma once

#include "Result.h"

#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Health States
// ══════════════════════════════════════════════════════════════════════════════

/// Health state for a fault domain.
enum class DomainHealth : uint8_t
{
    kHealthy = 0,
    kDegraded = 1,
    kFaulted = 2,
    kIsolated = 3,
};

/// Get display name for a health state.
[[nodiscard]] auto health_name(DomainHealth health) -> std::string;

// ══════════════════════════════════════════════════════════════════════════════
// Fault Domain
// ══════════════════════════════════════════════════════════════════════════════

/// Represents an isolated failure domain in the application.
struct FaultDomain
{
    std::string name;
    DomainHealth health{DomainHealth::kHealthy};
    size_t error_count{0};
    int64_t last_error_ms{0};
    std::string last_error_message;
};

// ══════════════════════════════════════════════════════════════════════════════
// Fault Domain Registry
// ══════════════════════════════════════════════════════════════════════════════

/// Registry for all fault domains, with health transitions and isolation.
class FaultDomainRegistry
{
public:
    FaultDomainRegistry();

    /// Register a new fault domain.
    void register_domain(const std::string& name);

    /// Record an error in a domain, potentially transitioning health state.
    void record_error(const std::string& domain_name, const std::string& error_message);

    /// Get the health of a domain.
    [[nodiscard]] auto health(const std::string& domain_name) const -> DomainHealth;

    /// Manually set the health of a domain.
    void set_health(const std::string& domain_name, DomainHealth health_state);

    /// Isolate a domain (set to Isolated, preventing operations).
    void isolate(const std::string& domain_name);

    /// Recover a domain (set to Healthy, reset error count).
    void recover(const std::string& domain_name);

    /// Check if a domain is operational (Healthy or Degraded).
    [[nodiscard]] auto is_operational(const std::string& domain_name) const -> bool;

    /// Get all domain names.
    [[nodiscard]] auto domain_names() const -> std::vector<std::string>;

    /// Get a snapshot of all domains.
    [[nodiscard]] auto all_domains() const -> std::vector<FaultDomain>;

    /// Get the number of registered domains.
    [[nodiscard]] auto domain_count() const -> size_t;

    /// Set the error threshold for degraded and faulted transitions.
    void set_thresholds(size_t degraded_threshold, size_t faulted_threshold);

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, FaultDomain> domains_;
    size_t degraded_threshold_{3};
    size_t faulted_threshold_{10};
};

} // namespace markamp::core
