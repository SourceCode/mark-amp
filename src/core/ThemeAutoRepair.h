/// @file ThemeAutoRepair.h
/// @brief V9 Phase 3 — Automatic theme repair for missing tokens and contrast violations.

#pragma once

#include "Theme.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Record of a single repair action performed.
struct RepairAction
{
    std::string token_name;  ///< Which token was repaired
    std::string repair_type; ///< "missing_fill", "contrast_fix", "fallback"
    std::string old_value;   ///< Previous hex value (or "none")
    std::string new_value;   ///< New hex value applied
};

/// Automatically repair themes with missing tokens or contrast violations.
class ThemeAutoRepair
{
public:
    /// Repair a theme in-place. Fills missing tokens from defaults,
    /// fixes contrast violations using luminance adjustments.
    /// Returns the list of repairs performed.
    [[nodiscard]] static auto repair(Theme& theme) -> std::vector<RepairAction>;

    /// Check if a theme needs repair (has missing or problematic tokens).
    [[nodiscard]] static auto needs_repair(const Theme& theme) -> bool;

private:
    /// Fill missing tokens (values at 0,0,0) from sensible defaults.
    static auto fill_missing_tokens(Theme& theme) -> std::vector<RepairAction>;

    /// Fix contrast violations between fg/bg pairs.
    static auto fix_contrast_violations(Theme& theme) -> std::vector<RepairAction>;
};

} // namespace markamp::core
