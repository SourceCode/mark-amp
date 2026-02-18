/// @file VersionManager.h
/// @brief V9 Phase 50 — Semantic versioning management.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

/// Semantic version representation.
struct SemanticVersion
{
    int major{0};
    int minor{0};
    int patch{0};
    std::string pre_release;

    [[nodiscard]] auto to_string() const -> std::string;
    auto operator==(const SemanticVersion& other) const -> bool;
    auto operator<(const SemanticVersion& other) const -> bool;
};

/// Manages application versioning with semver.
class VersionManager
{
public:
    VersionManager() = default;

    // ── Version control ───────────────────────────────────────────────
    void set_version(SemanticVersion version);
    [[nodiscard]] auto get_version() const -> const SemanticVersion&;

    // ── Bumping ───────────────────────────────────────────────────────
    void bump_major();
    void bump_minor();
    void bump_patch();

    // ── Parsing & comparison ──────────────────────────────────────────
    [[nodiscard]] static auto parse(const std::string& version_string) -> SemanticVersion;
    [[nodiscard]] static auto compare(const SemanticVersion& ver_a, const SemanticVersion& ver_b)
        -> int;
    [[nodiscard]] static auto is_compatible(const SemanticVersion& ver_a,
                                            const SemanticVersion& ver_b) -> bool;

    // ── History ───────────────────────────────────────────────────────
    [[nodiscard]] auto version_history() const -> const std::vector<SemanticVersion>&;
    void clear_history();

private:
    SemanticVersion current_{1, 0, 0};
    std::vector<SemanticVersion> history_;
};

} // namespace markamp::core
