#pragma once

/// @file CMakeIntegration.h
/// @brief Phase 38 Tasks 2-4 — CMake integration for project detection, configure, and build.

#include <string>
#include <vector>

namespace markamp::core
{

/// A CMake preset from CMakePresets.json.
struct CMakePreset
{
    std::string name;
    std::string display_name;
    std::string description;
    std::string generator;
    std::string build_dir;
    std::string cmake_build_type;
    bool hidden{false};
};

/// A CMake target discovered from the query API or cmake --build --target list.
struct CMakeTarget
{
    std::string name;
    std::string type; ///< "EXECUTABLE", "STATIC_LIBRARY", "SHARED_LIBRARY", "UTILITY"
    std::string source_dir;
};

/// Integrates with CMake for project management.
class CMakeIntegration
{
public:
    CMakeIntegration() = default;

    // ── Detection ──

    /// Check if the workspace has a CMake project.
    [[nodiscard]] auto detect_project(const std::string& workspace_path) const -> bool;

    /// Check if CMakePresets.json exists.
    [[nodiscard]] auto has_presets(const std::string& workspace_path) const -> bool;

    /// Parse CMakePresets.json and return available presets.
    [[nodiscard]] auto parse_presets(const std::string& workspace_path) const
        -> std::vector<CMakePreset>;

    // ── Configure ──

    /// Generate the cmake configure command.
    [[nodiscard]] auto configure_command(const std::string& workspace_path,
                                         const std::string& build_dir = "build",
                                         const std::string& build_type = "Debug") const
        -> std::string;

    /// Generate configure command using a preset.
    [[nodiscard]] auto configure_with_preset(const std::string& workspace_path,
                                             const std::string& preset_name) const -> std::string;

    // ── Build ──

    /// Generate the cmake build command.
    [[nodiscard]] auto build_command(const std::string& build_dir = "build",
                                     const std::string& target = "",
                                     int parallel_jobs = 0) const -> std::string;

    /// Generate the cmake clean command.
    [[nodiscard]] auto clean_command(const std::string& build_dir = "build") const -> std::string;

    // ── Targets ──

    /// Discover available targets from the build directory.
    [[nodiscard]] auto discover_targets(const std::string& build_dir) const
        -> std::vector<CMakeTarget>;

    /// Find cmake binary path.
    [[nodiscard]] static auto find_cmake() -> std::string;

    /// Get CMake version.
    [[nodiscard]] static auto cmake_version() -> std::string;
};

} // namespace markamp::core
