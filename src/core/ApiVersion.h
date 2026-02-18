/// ApiVersion.h — V7 Phase 24: Extension API contract versioning
///
/// Semantic version comparison and compatibility checking for the
/// extension API. Extensions declare a minimum API version; the host
/// checks compatibility before activation.

#pragma once

#include "Result.h"

#include <cstdint>
#include <string>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// ApiVersion
// ══════════════════════════════════════════════════════════════════════════════

/// Semantic version for API contract tracking.
struct ApiVersion
{
    uint16_t major{0};
    uint16_t minor{0};
    uint16_t patch{0};

    /// Format as "major.minor.patch".
    [[nodiscard]] auto to_string() const -> std::string;

    /// Parse "major.minor.patch" string.
    [[nodiscard]] static auto parse(std::string_view version_str) -> Result<ApiVersion>;

    /// Get the current host API version.
    [[nodiscard]] static auto current() noexcept -> ApiVersion;

    /// Check if this version is compatible with a required minimum version.
    /// Compatible if same major version and this >= required.
    [[nodiscard]] auto is_compatible_with(const ApiVersion& required) const noexcept -> bool;

    /// Three-way comparison: <0 if this < other, 0 if equal, >0 if this > other.
    [[nodiscard]] auto compare(const ApiVersion& other) const noexcept -> int;

    auto operator==(const ApiVersion& other) const noexcept -> bool = default;
    auto operator<=>(const ApiVersion& other) const noexcept = default;
};

/// Check if a plugin's required API version is supported by the host.
[[nodiscard]] auto check_api_compatibility(const ApiVersion& required) -> Result<void>;

} // namespace markamp::core
