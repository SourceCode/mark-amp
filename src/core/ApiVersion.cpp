/// ApiVersion.cpp — V7 Phase 24: Extension API contract versioning

#include "ApiVersion.h"

#include <charconv>

namespace markamp::core
{

// Current host API version — bump when breaking changes are made.
static constexpr ApiVersion kCurrentApiVersion{2, 0, 0};

auto ApiVersion::to_string() const -> std::string
{
    return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
}

auto ApiVersion::parse(std::string_view version_str) -> Result<ApiVersion>
{
    if (version_str.empty())
    {
        return std::unexpected(
            make_validation_error("Version string is empty", ErrorCode::StringTooShort));
    }

    // Find first dot
    auto dot1 = version_str.find('.');
    if (dot1 == std::string_view::npos)
    {
        return std::unexpected(make_validation_error("Invalid version format: missing first dot",
                                                     ErrorCode::ParseError));
    }

    // Find second dot
    auto dot2 = version_str.find('.', dot1 + 1);
    if (dot2 == std::string_view::npos)
    {
        return std::unexpected(make_validation_error("Invalid version format: missing second dot",
                                                     ErrorCode::ParseError));
    }

    // Check no more dots
    if (version_str.find('.', dot2 + 1) != std::string_view::npos)
    {
        return std::unexpected(
            make_validation_error("Invalid version format: too many dots", ErrorCode::ParseError));
    }

    auto major_str = version_str.substr(0, dot1);
    auto minor_str = version_str.substr(dot1 + 1, dot2 - dot1 - 1);
    auto patch_str = version_str.substr(dot2 + 1);

    ApiVersion result{};

    auto [major_ptr, major_ec] =
        std::from_chars(major_str.data(), major_str.data() + major_str.size(), result.major);
    if (major_ec != std::errc{})
    {
        return std::unexpected(
            make_validation_error("Invalid major version number", ErrorCode::ParseError));
    }

    auto [minor_ptr, minor_ec] =
        std::from_chars(minor_str.data(), minor_str.data() + minor_str.size(), result.minor);
    if (minor_ec != std::errc{})
    {
        return std::unexpected(
            make_validation_error("Invalid minor version number", ErrorCode::ParseError));
    }

    auto [patch_ptr, patch_ec] =
        std::from_chars(patch_str.data(), patch_str.data() + patch_str.size(), result.patch);
    if (patch_ec != std::errc{})
    {
        return std::unexpected(
            make_validation_error("Invalid patch version number", ErrorCode::ParseError));
    }

    return result;
}

auto ApiVersion::current() noexcept -> ApiVersion
{
    return kCurrentApiVersion;
}

auto ApiVersion::is_compatible_with(const ApiVersion& required) const noexcept -> bool
{
    // Same major version required
    if (major != required.major)
    {
        return false;
    }
    // Host version must be >= required
    return *this >= required;
}

auto ApiVersion::compare(const ApiVersion& other) const noexcept -> int
{
    if (major != other.major)
    {
        return major < other.major ? -1 : 1;
    }
    if (minor != other.minor)
    {
        return minor < other.minor ? -1 : 1;
    }
    if (patch != other.patch)
    {
        return patch < other.patch ? -1 : 1;
    }
    return 0;
}

auto check_api_compatibility(const ApiVersion& required) -> Result<void>
{
    auto host = ApiVersion::current();
    if (!host.is_compatible_with(required))
    {
        return std::unexpected(make_error(ErrorCode::PluginApiVersionMismatch,
                                          SubsystemId::ExtensionHost,
                                          "Extension requires API v" + required.to_string() +
                                              ", host provides v" + host.to_string()));
    }
    return {};
}

} // namespace markamp::core
