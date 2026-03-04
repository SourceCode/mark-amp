#pragma once

/// @file MakeIntegration.h
/// @brief Phase 38 Task 5 — Makefile detection and target parsing.

#include <string>
#include <vector>

namespace markamp::core
{

/// A target parsed from a Makefile.
struct MakeTarget
{
    std::string name;
    bool is_phony{false};   ///< True if target is .PHONY
    bool is_default{false}; ///< True if target is the first non-phony target
};

/// Integrates with Make for Makefile-based projects.
class MakeIntegration
{
public:
    MakeIntegration() = default;

    /// Check if a Makefile exists in the workspace.
    [[nodiscard]] auto detect_makefile(const std::string& workspace_path) const -> bool;

    /// Parse targets from a Makefile.
    [[nodiscard]] auto parse_targets(const std::string& makefile_path) const
        -> std::vector<MakeTarget>;

    /// Generate the make build command.
    [[nodiscard]] auto build_command(const std::string& target = "",
                                     const std::string& makefile_dir = ".",
                                     int parallel_jobs = 0) const -> std::string;

    /// Generate the make clean command.
    [[nodiscard]] auto clean_command(const std::string& makefile_dir = ".") const -> std::string;

    /// Find make binary path.
    [[nodiscard]] static auto find_make() -> std::string;
};

} // namespace markamp::core
