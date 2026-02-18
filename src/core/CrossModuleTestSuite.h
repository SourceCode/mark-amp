/// @file CrossModuleTestSuite.h
/// @brief V9 Phase 49 — Cross-module dependency and compatibility validation.
#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

/// A dependency between two modules.
struct ModuleDependency
{
    std::string from_module;
    std::string to_module;
    bool is_satisfied{true};
};

/// Result of a compatibility check.
struct CompatibilityResult
{
    std::string module_name;
    bool compatible{true};
    std::string message;
};

/// Validates cross-module dependencies and API compatibility.
class CrossModuleTestSuite
{
public:
    CrossModuleTestSuite() = default;

    // ── Module registry ───────────────────────────────────────────────
    void register_module(const std::string& module_name);
    [[nodiscard]] auto has_module(const std::string& module_name) const -> bool;

    // ── Dependencies ──────────────────────────────────────────────────
    void add_dependency(const std::string& from, const std::string& to_module);
    [[nodiscard]] auto check_dependencies() const -> std::vector<ModuleDependency>;

    // ── Compatibility ─────────────────────────────────────────────────
    [[nodiscard]] auto run_compatibility_check() const -> std::vector<CompatibilityResult>;

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto module_count() const -> int;
    [[nodiscard]] auto dependency_count() const -> int;
    void clear();

private:
    std::unordered_set<std::string> modules_;
    std::vector<ModuleDependency> dependencies_;
};

} // namespace markamp::core
