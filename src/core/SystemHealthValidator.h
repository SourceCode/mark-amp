/// @file SystemHealthValidator.h
/// @brief V9 Phase 49 — System health checks and validation.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Health check category.
enum class HealthCategory : uint8_t
{
    kCore = 0,
    kUI = 1,
    kExtension = 2,
    kData = 3,
    kSecurity = 4,
};

/// A single health check result.
struct HealthCheck
{
    std::string name;
    HealthCategory category{HealthCategory::kCore};
    bool passed{false};
    std::string message;
};

/// Runs and aggregates system health checks.
class SystemHealthValidator
{
public:
    SystemHealthValidator() = default;

    // ── Check management ──────────────────────────────────────────────
    void add_check(HealthCheck check);
    void load_default_checks();

    // ── Execution ─────────────────────────────────────────────────────
    void run_all_checks();
    [[nodiscard]] auto get_results() const -> const std::vector<HealthCheck>&;

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto passed_count() const -> int;
    [[nodiscard]] auto failed_count() const -> int;
    [[nodiscard]] auto check_count() const -> int;
    [[nodiscard]] auto is_healthy() const -> bool;
    void clear();

private:
    std::vector<HealthCheck> checks_;
};

} // namespace markamp::core
