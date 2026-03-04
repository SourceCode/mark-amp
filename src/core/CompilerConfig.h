#pragma once

/// @file CompilerConfig.h
/// @brief Phase 37 — Compiler configuration data model, serialization, and validation.
///
/// Stores all compilation parameters: compiler path, standard, flags, paths,
/// optimization, sanitizers, diagnostics, PCH, LTO, cache, and sysroot.

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Detected compiler information.
struct DetectedCompiler
{
    std::string path;                             ///< Absolute path to compiler binary
    std::string name;                             ///< Short name, e.g. "clang++"
    std::string version;                          ///< Version string, e.g. "17.0.0"
    std::vector<std::string> supported_standards; ///< e.g. {"c++14","c++17","c++20","c++23"}
};

/// LTO mode options.
enum class LtoMode : uint8_t
{
    kOff,
    kFull,
    kThin,
};

/// Compiler cache tool selection.
enum class CacheTool : uint8_t
{
    kNone,
    kCcache,
    kSccache,
};

/// Diagnostic output color mode.
enum class DiagnosticColorMode : uint8_t
{
    kAuto,
    kAlways,
    kNever,
};

/// Diagnostic output format.
enum class DiagnosticFormat : uint8_t
{
    kText,
    kSarif,
    kJson,
};

/// Complete compiler configuration for a workspace.
struct CompilerConfig
{
    // ── Compiler selection ──
    std::string compiler_path;    ///< e.g., "/usr/bin/clang++"
    std::string compiler_name;    ///< "clang++", "g++", "cl.exe"
    std::string compiler_version; ///< "17.0.0"

    // ── Language standard ──
    std::string cpp_standard{"c++17"}; ///< "c++14", "c++17", "c++20", "c++23", "c++26"

    // ── Optimization & warnings ──
    std::string optimization_level{"O0"}; ///< "O0", "O1", "O2", "O3", "Os", "Oz"
    std::string warning_level{"default"}; ///< "none", "default", "all", "extra", "pedantic"

    // ── Paths ──
    std::vector<std::string> include_paths;
    std::vector<std::string> library_paths;
    std::vector<std::string> defines;      ///< e.g., "DEBUG=1", "NDEBUG"
    std::vector<std::string> custom_flags; ///< e.g., "-fno-exceptions"
    std::vector<std::string> linker_flags;

    // ── Sanitizers ──
    bool enable_asan{false};
    bool enable_ubsan{false};
    bool enable_tsan{false};
    bool enable_msan{false};
    bool enable_coverage{false};
    bool position_independent{false};

    // ── Output ──
    std::string output_type{"executable"}; ///< "executable", "static_lib", "shared_lib", "object"
    std::string target_arch{"native"};     ///< "native", "x86_64", "arm64", "armv7", "wasm32"
    std::string toolchain_file;            ///< CMake toolchain file path

    // ── Diagnostics (Task 26) ──
    DiagnosticColorMode diagnostic_color_mode{DiagnosticColorMode::kAuto};
    DiagnosticFormat diagnostic_format{DiagnosticFormat::kText};
    int template_backtrace_limit{10};
    int diagnostic_column_limit{0}; ///< 0 = unlimited
    bool show_source_ranges{true};
    bool show_fix_its{true};

    // ── Precompiled Headers (Task 27) ──
    std::string pch_header_path;
    std::string pch_output_path;
    bool enable_pch{false};
    bool force_include_pch{true};

    // ── LTO (Task 28) ──
    LtoMode lto_mode{LtoMode::kOff};
    int lto_jobs{0}; ///< 0 = auto
    std::string lto_cache_dir;

    // ── Compiler cache (Task 29) ──
    CacheTool cache_tool{CacheTool::kNone};
    std::string cache_tool_path;
    std::string cache_max_size{"10G"};
    std::string cache_dir;

    // ── Sysroot (Task 30) ──
    std::string sysroot_path;
    std::string sysroot_target_triple;

    // ── Serialization ──

    /// Serialize to JSON string.
    [[nodiscard]] auto to_json() const -> std::string;

    /// Deserialize from JSON string. Returns false on parse failure.
    [[nodiscard]] static auto from_json(const std::string& json_str) -> CompilerConfig;

    // ── Command generation ──

    /// Build the full compiler command-line from current settings.
    [[nodiscard]] auto build_command_line() const -> std::string;

    /// Build just the compiler flags (no compiler path).
    [[nodiscard]] auto build_flags() const -> std::vector<std::string>;

    // ── Validation helpers ──

    /// Check if this is a Clang compiler.
    [[nodiscard]] auto is_clang() const -> bool;

    /// Check if this is a GCC compiler.
    [[nodiscard]] auto is_gcc() const -> bool;

    /// Check if cross-compilation is active.
    [[nodiscard]] auto is_cross_compiling() const -> bool;

    /// Return a default Debug configuration.
    [[nodiscard]] static auto default_debug() -> CompilerConfig;

    /// Return a default Release configuration.
    [[nodiscard]] static auto default_release() -> CompilerConfig;
};

} // namespace markamp::core
