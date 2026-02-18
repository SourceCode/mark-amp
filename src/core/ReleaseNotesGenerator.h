/// @file ReleaseNotesGenerator.h
/// @brief V9 Phase 50 — Release notes generation and management.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Category for a release entry.
enum class ReleaseCategory : uint8_t
{
    kFeature = 0,
    kFix = 1,
    kBreaking = 2,
    kImprovement = 3,
};

/// A single entry in a release.
struct ReleaseEntry
{
    std::string title;
    std::string description;
    ReleaseCategory category{ReleaseCategory::kFeature};
    std::string author;
};

/// A complete set of release notes.
struct ReleaseNotes
{
    std::string release_id;
    std::string version;
    std::string date;
    std::string summary;
    std::vector<ReleaseEntry> entries;
};

/// Manages releases and generates markdown release notes.
class ReleaseNotesGenerator
{
public:
    ReleaseNotesGenerator() = default;

    // ── Release lifecycle ─────────────────────────────────────────────
    auto create_release(const std::string& version, const std::string& date) -> std::string;
    auto add_entry(const std::string& release_id, ReleaseEntry entry) -> bool;
    [[nodiscard]] auto get_release(const std::string& release_id) const -> const ReleaseNotes*;

    // ── Generation ────────────────────────────────────────────────────
    [[nodiscard]] auto generate_markdown(const std::string& release_id) const -> std::string;
    [[nodiscard]] auto list_releases() const -> std::vector<const ReleaseNotes*>;

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto release_count() const -> int;
    void clear();

private:
    std::vector<ReleaseNotes> releases_;
    int next_id_{1};

    [[nodiscard]] static auto category_label(ReleaseCategory cat) -> std::string;
};

} // namespace markamp::core
