#pragma once

/// @file CompilerDetector.h
/// @brief Phase 37 Task 2 — Auto-detect installed C++ compilers and cache tools.

#include "CompilerConfig.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Cache tool statistics.
struct CacheStats
{
    std::string tool_name;  ///< "ccache" or "sccache"
    std::string cache_size; ///< e.g., "2.3G"
    std::string max_size;   ///< e.g., "10G"
    double hit_rate{0.0};   ///< percentage, e.g. 78.5
    bool available{false};
};

/// Scans the system for installed C++ compilers and cache tools.
class CompilerDetector
{
public:
    CompilerDetector() = default;

    /// Detect all available C++ compilers on the system.
    [[nodiscard]] auto detect_compilers() const -> std::vector<DetectedCompiler>;

    /// Detect a specific compiler at a given path.
    [[nodiscard]] auto detect_compiler_at(const std::string& path) const -> DetectedCompiler;

    /// Extract version from compiler --version output.
    [[nodiscard]] static auto extract_version(const std::string& version_output) -> std::string;

    /// Derive supported standards from version and compiler name.
    [[nodiscard]] static auto derive_standards(const std::string& compiler_name,
                                               const std::string& version)
        -> std::vector<std::string>;

    /// Detect installed cache tools (ccache, sccache).
    [[nodiscard]] auto detect_cache_tools() const -> std::vector<CacheStats>;

    /// Query cache stats for a specific tool.
    [[nodiscard]] auto query_cache_stats(CacheTool tool) const -> CacheStats;

    /// Find a binary in PATH.
    [[nodiscard]] static auto find_in_path(const std::string& binary_name) -> std::string;

    /// Default search paths for compilers on the current platform.
    [[nodiscard]] static auto default_search_paths() -> std::vector<std::string>;
};

} // namespace markamp::core
