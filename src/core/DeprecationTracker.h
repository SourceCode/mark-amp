/// @file DeprecationTracker.h
/// @brief V9 Phase 50 — Tracks deprecated features and their replacements.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

/// A deprecation record.
struct DeprecationEntry
{
    std::string feature_name;
    std::string deprecated_in; ///< Version deprecated
    std::string removed_in;    ///< Version removed (empty if still present)
    std::string replacement;   ///< Recommended replacement
    std::string reason;
};

/// Tracks deprecated and removed features.
class DeprecationTracker
{
public:
    DeprecationTracker() = default;

    // ── Management ────────────────────────────────────────────────────
    void add_deprecation(DeprecationEntry entry);
    [[nodiscard]] auto is_deprecated(const std::string& feature) const -> bool;
    [[nodiscard]] auto get_replacement(const std::string& feature) const -> std::string;
    [[nodiscard]] auto get_entry(const std::string& feature) const -> const DeprecationEntry*;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto active_deprecations() const -> std::vector<const DeprecationEntry*>;
    [[nodiscard]] auto removed_features() const -> std::vector<const DeprecationEntry*>;

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto deprecation_count() const -> int;
    void clear();

private:
    std::vector<DeprecationEntry> entries_;
};

} // namespace markamp::core
