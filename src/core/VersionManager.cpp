/// @file VersionManager.cpp
/// @brief V9 Phase 50 — VersionManager implementation.

#include "VersionManager.h"

#include <sstream>

namespace markamp::core
{

auto SemanticVersion::to_string() const -> std::string
{
    std::string result =
        std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    if (!pre_release.empty())
    {
        result += "-" + pre_release;
    }
    return result;
}

auto SemanticVersion::operator==(const SemanticVersion& other) const -> bool
{
    return major == other.major && minor == other.minor && patch == other.patch &&
           pre_release == other.pre_release;
}

auto SemanticVersion::operator<(const SemanticVersion& other) const -> bool
{
    if (major != other.major)
        return major < other.major;
    if (minor != other.minor)
        return minor < other.minor;
    return patch < other.patch;
}

void VersionManager::set_version(SemanticVersion version)
{
    history_.push_back(current_);
    current_ = std::move(version);
}

auto VersionManager::get_version() const -> const SemanticVersion&
{
    return current_;
}

void VersionManager::bump_major()
{
    history_.push_back(current_);
    current_.major++;
    current_.minor = 0;
    current_.patch = 0;
    current_.pre_release.clear();
}

void VersionManager::bump_minor()
{
    history_.push_back(current_);
    current_.minor++;
    current_.patch = 0;
    current_.pre_release.clear();
}

void VersionManager::bump_patch()
{
    history_.push_back(current_);
    current_.patch++;
    current_.pre_release.clear();
}

auto VersionManager::parse(const std::string& version_string) -> SemanticVersion
{
    SemanticVersion ver;
    std::string input = version_string;

    // Strip leading 'v' if present
    if (!input.empty() && input[0] == 'v')
    {
        input = input.substr(1);
    }

    // Split on '-' for pre-release
    auto dash_pos = input.find('-');
    std::string core_part = input;
    if (dash_pos != std::string::npos)
    {
        ver.pre_release = input.substr(dash_pos + 1);
        core_part = input.substr(0, dash_pos);
    }

    // Parse major.minor.patch
    std::istringstream stream(core_part);
    char dot = '.';
    stream >> ver.major >> dot >> ver.minor >> dot >> ver.patch;

    return ver;
}

auto VersionManager::compare(const SemanticVersion& ver_a, const SemanticVersion& ver_b) -> int
{
    if (ver_a.major != ver_b.major)
        return ver_a.major < ver_b.major ? -1 : 1;
    if (ver_a.minor != ver_b.minor)
        return ver_a.minor < ver_b.minor ? -1 : 1;
    if (ver_a.patch != ver_b.patch)
        return ver_a.patch < ver_b.patch ? -1 : 1;
    return 0;
}

auto VersionManager::is_compatible(const SemanticVersion& ver_a, const SemanticVersion& ver_b)
    -> bool
{
    // Compatible if same major version (semver compatibility rule)
    return ver_a.major == ver_b.major;
}

auto VersionManager::version_history() const -> const std::vector<SemanticVersion>&
{
    return history_;
}

void VersionManager::clear_history()
{
    history_.clear();
}

} // namespace markamp::core
