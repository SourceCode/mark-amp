#pragma once

/// @file BuildService.h
/// @brief Build & Task Runner service — build orchestration, task execution, error parsing.
///
/// Features:
///   - Build lifecycle management (build/clean/rebuild/cancel)
///   - Compiler selection (GCC/Clang/MSVC) and target selector
///   - CMake deep integration — target discovery, configure
///   - Make integration — target discovery from Makefiles
///   - Ninja build progress tracking ([N/M])
///   - Auto-build on save, build-before-run integration
///   - Error/warning count summary and build time display
///   - CompilerErrorParser integration for diagnostics
///   - Task templates, task groups, custom build commands
///   - Status bar build indicator integration
///
/// @see CompilerErrorParser, TaskConfig, TaskRunner, BuildPanel

#include "CompilerErrorParser.h"
#include "DiagnosticsService.h"
#include "EventBus.h"
#include "OutputChannelService.h"
#include "TaskConfig.h"

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Build system type.
enum class BuildSystem : uint8_t
{
    kCMake,
    kMake,
    kCustom,
};

/// A discovered build target.
struct BuildTarget
{
    std::string name;
    std::string description;
    bool is_default{false};
};

/// Build configuration (e.g., Debug, Release).
struct BuildConfiguration
{
    std::string name;
    std::string cmake_preset; ///< CMake preset name (optional)
};

/// Current build status.
enum class BuildStatus : uint8_t
{
    kIdle,
    kConfiguring,
    kBuilding,
    kFinished,
    kFailed,
};

/// Summary of a completed build.
struct BuildResult
{
    bool success{false};
    int error_count{0};
    int warning_count{0};
    float duration_seconds{0.0F};
    std::string output;
};

/// Orchestrator for build operations — manages CMake/Make/custom builds,
/// compiler detection, target discovery, error parsing, and progress tracking.
/// Phase 25 Task 5.
class BuildService
{
public:
    BuildService() = default;

    // ── Build operations ──

    /// Start a build. Returns false if a build is already running.
    auto start_build(const std::string& target = "", const std::string& config = "Debug") -> bool;

    /// Stop the current build.
    void stop_build();

    /// Clean build artifacts.
    auto clean(const std::string& config = "Debug") -> bool;

    /// Configure the project (cmake -B build).
    auto configure(const std::string& config = "Debug") -> bool;

    /// Build-before-run: ensures a successful build before launching.
    /// Returns true if build succeeded (or was already up-to-date).
    auto ensure_built(const std::string& target = "", const std::string& config = "Debug") -> bool;

    /// Enable/disable auto build-before-run.
    void set_build_before_run(bool enabled);
    [[nodiscard]] auto build_before_run() const -> bool;

    // ── Build system detection ──

    /// Detect the build system from workspace files.
    void detect_build_system(const std::string& workspace_path);

    [[nodiscard]] auto build_system() const -> BuildSystem;
    void set_build_system(BuildSystem system);

    // ── Target management ──

    /// Discover available targets.
    void discover_targets(const std::string& build_dir = "build");
    [[nodiscard]] auto targets() const -> const std::vector<BuildTarget>&;

    /// CMake-specific: discover targets from CMake cache/API.
    void discover_cmake_targets(const std::string& build_dir = "build");

    /// Make-specific: discover targets from Makefile.
    void discover_make_targets(const std::string& makefile_dir = ".");

    /// Set the active target.
    void set_active_target(const std::string& target_name);
    [[nodiscard]] auto active_target() const -> const std::string&;

    // ── Configuration management ──

    [[nodiscard]] auto configurations() const -> const std::vector<BuildConfiguration>&;
    void set_active_configuration(const std::string& config_name);
    [[nodiscard]] auto active_configuration() const -> const std::string&;

    // ── Status ──

    [[nodiscard]] auto status() const -> BuildStatus;
    [[nodiscard]] auto last_result() const -> const BuildResult&;
    [[nodiscard]] auto is_building() const -> bool;

    // ── Build progress ──

    /// Parse Ninja/Make progress markers (e.g., "[42/100]").
    struct BuildProgress
    {
        int completed{0};
        int total{0};
        float percentage{0.0F};
        std::string current_file;
    };

    [[nodiscard]] auto progress() const -> const BuildProgress&;

    /// Feed build output line for progress and error parsing.
    void process_output_line(const std::string& line);

    // ── Error parser ──

    [[nodiscard]] auto error_parser() -> CompilerErrorParser&;
    [[nodiscard]] auto parsed_errors() const -> const std::vector<ParsedCompilerError>&;

    // ── Service wiring ──

    void set_diagnostics_service(DiagnosticsService* service);
    void set_output_service(OutputChannelService* service);

private:
    BuildSystem build_system_{BuildSystem::kCMake};
    BuildStatus status_{BuildStatus::kIdle};
    BuildResult last_result_;
    BuildProgress progress_;
    CompilerErrorParser error_parser_;
    std::vector<ParsedCompilerError> errors_;

    std::string active_target_;
    std::string active_config_{"Debug"};
    std::vector<BuildTarget> targets_;
    std::vector<BuildConfiguration> configurations_{
        {.name = "Debug"},
        {.name = "Release"},
        {.name = "RelWithDebInfo"},
        {.name = "MinSizeRel"},
    };

    std::string workspace_path_;
    DiagnosticsService* diagnostics_service_{nullptr};
    OutputChannelService* output_service_{nullptr};

    std::chrono::steady_clock::time_point build_start_time_;
    bool build_before_run_{true};

    void parse_progress(const std::string& line);
    void publish_errors_to_diagnostics();
};

} // namespace markamp::core
