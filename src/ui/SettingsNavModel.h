#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// A node in the settings category hierarchy (Phase 11 Task 1).
struct SettingsCategoryNode
{
    std::string group;      ///< Top-level group (e.g., "Editor", "Appearance")
    std::string subgroup;   ///< Subgroup within the group (e.g., "Font", "Cursor")
    bool is_expanded{true}; ///< Whether this category node is expanded in the tree
    int setting_count{0};   ///< Number of settings in this category
};

/// A match snippet from settings search (Phase 11 Task 2).
struct SettingsSearchMatch
{
    int setting_index{0};      ///< Index into the flat settings list
    std::string setting_id;    ///< Setting ID (e.g., "editor.fontSize")
    std::string label;         ///< Display label
    std::string match_context; ///< Snippet showing where the match occurred
    int score{0};              ///< Ranking score
};

/// Breadcrumb path for deep-linking into settings (Phase 11 Task 3).
struct SettingsBreadcrumb
{
    std::string group;
    std::string subgroup;
    std::string setting_id; ///< Empty if viewing a category, non-empty if viewing a setting
};

/// Metadata badges for settings (Phase 11 Task 4).
enum class SettingBadge : uint8_t
{
    kNone,
    kRestartRequired, ///< Changing this setting requires restart
    kExperimental,    ///< Feature is experimental/unstable
    kDeprecated,      ///< Setting is deprecated
};

/// Testable model for Settings Navigation (Phase 11).
///
/// Encapsulates:
/// - Hierarchical category tree building from flat settings
/// - Search with match highlighting and scope filtering
/// - Deep-link breadcrumb navigation
/// - Metadata badge rules (restart-required, experimental, deprecated)
class SettingsNavModel
{
public:
    /// Simple setting descriptor for model purposes.
    struct SettingEntry
    {
        std::string setting_id;
        std::string label;
        std::string description;
        std::string group;
        std::string subgroup;
        bool restart_required{false};
        bool experimental{false};
        bool deprecated{false};
        std::vector<std::string> keywords;
    };

    /// Load settings entries.
    void set_entries(std::vector<SettingEntry> entries);

    /// Get all entries.
    [[nodiscard]] auto entries() const -> const std::vector<SettingEntry>&;

    /// Build hierarchical category tree (grouped by group → subgroup).
    [[nodiscard]] auto category_tree() const -> std::vector<SettingsCategoryNode>;

    // ── Search ──────────────────────────────────────────────────────

    /// Search settings by query. Matches against label, description, setting_id, keywords.
    [[nodiscard]] auto search(const std::string& query) const -> std::vector<SettingsSearchMatch>;

    // ── Deep-link ───────────────────────────────────────────────────

    /// Build breadcrumb for a given setting ID.
    [[nodiscard]] auto breadcrumb_for(const std::string& setting_id) const
        -> std::optional<SettingsBreadcrumb>;

    // ── Badges ──────────────────────────────────────────────────────

    /// Get the badge type for a setting.
    [[nodiscard]] auto badge_for(const std::string& setting_id) const -> SettingBadge;

    /// Get tooltip text for a badge.
    [[nodiscard]] static auto badge_tooltip(SettingBadge badge) -> std::string;

private:
    std::vector<SettingEntry> entries_;

    [[nodiscard]] static auto match_score(const std::string& query, const std::string& text) -> int;
};

} // namespace markamp::ui
