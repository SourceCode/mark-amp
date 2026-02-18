/// SafePath.h — V7 Phase 08: Filesystem canonicalization wrapper
///
/// Prevents directory traversal, detects symlink escapes, and ensures
/// safe filesystem operations using std::error_code exclusively.

#pragma once

#include "Result.h"

#include <filesystem>
#include <string>
#include <string_view>

namespace markamp::core
{

/// Safe filesystem path wrapper with security checks.
class SafePath
{
public:
    /// Resolve an absolute path safely, rejecting traversal attempts.
    [[nodiscard]] static auto resolve(const std::filesystem::path& base, std::string_view relative)
        -> Result<std::filesystem::path>;

    /// Resolve with additional symlink escape detection.
    [[nodiscard]] static auto resolve_with_symlink_check(const std::filesystem::path& base,
                                                         std::string_view relative)
        -> Result<std::filesystem::path>;

    /// Join two paths safely, ensuring the result stays within the base.
    [[nodiscard]] static auto join(const std::filesystem::path& base, std::string_view child)
        -> Result<std::filesystem::path>;

    /// Validate that a path is within a given root directory.
    [[nodiscard]] static auto is_within(const std::filesystem::path& path,
                                        const std::filesystem::path& root) -> bool;
};

} // namespace markamp::core
