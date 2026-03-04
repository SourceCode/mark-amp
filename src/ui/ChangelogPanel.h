// ============================================================================
// File: src/ui/ChangelogPanel.h
// Phase 48: Welcome and Onboarding — Changelog panel model
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Category badge for a changelog entry.
enum class ChangelogCategory : uint8_t
{
    Feature,
    Fix,
    Improvement,
    Breaking,
    Deprecated
};

/// A single changelog entry.
struct ChangelogEntry
{
    std::string description;
    ChangelogCategory category{ChangelogCategory::Feature};
};

/// A version section in the changelog.
struct ChangelogVersion
{
    std::string version;
    std::string date;
    std::vector<ChangelogEntry> entries;
    bool is_collapsed{false};
};

/// Model for the changelog panel.
class ChangelogPanelModel
{
public:
    ChangelogPanelModel() = default;

    /// Parse a CHANGELOG.md string.
    void parse(const std::string& changelog_text);

    /// Get parsed versions.
    [[nodiscard]] auto versions() const -> const std::vector<ChangelogVersion>&
    {
        return versions_;
    }

    /// Get version count.
    [[nodiscard]] auto version_count() const -> int
    {
        return static_cast<int>(versions_.size());
    }

    /// Toggle collapse for a version.
    void toggle_version(size_t index);

private:
    std::vector<ChangelogVersion> versions_;

    [[nodiscard]] static auto detect_category(const std::string& text) -> ChangelogCategory;
};

} // namespace markamp::ui
