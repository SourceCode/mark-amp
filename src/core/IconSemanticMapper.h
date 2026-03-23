/// @file IconSemanticMapper.h
/// @brief V20 P09-T02 + V27-P01-T02: Icon semantic mapping and normalization.
///
/// Completes icon migration by mapping semantic roles to icon identifiers,
/// enforcing sizing, labels, and theming consistency across all surfaces.
/// V27 adds placement rules, per-role usage constraints, and expanded
/// surface coverage.
#pragma once

#include "EventBus.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Icon size category.
enum class IconSize
{
    kSmall,     ///< 12-14px (tree items, breadcrumbs)
    kMedium,    ///< 16px (standard)
    kLarge,     ///< 20-24px (activity bar, large toolbars)
    kXLarge     ///< 32px+ (empty states, welcome)
};

/// V27: Icon placement position within a control or row.
enum class IconPlacement
{
    kLeading,     ///< Before text label (most common)
    kTrailing,    ///< After text label (e.g. expand/collapse arrows)
    kStandalone,  ///< Icon-only button, no adjacent text
    kBadge,       ///< Overlay badge (e.g. notification count)
    kInline,      ///< Inline within text flow (e.g. severity markers)
};

/// Total number of icon placement types.
[[nodiscard]] constexpr auto icon_placement_count() noexcept -> int { return 5; }

/// V27: Per-role icon usage constraint.
struct IconUsageRule
{
    IconSize min_size{IconSize::kMedium};
    IconSize max_size{IconSize::kLarge};
    IconPlacement default_placement{IconPlacement::kLeading};
    bool requires_label{true};         ///< Must have an accessibility label
    bool requires_theme_aware{true};   ///< Must adapt to light/dark theme
    bool allows_standalone{false};     ///< Can be used without text
    int hit_target_min_px{24};         ///< Minimum hit target size
};

/// Semantic icon mapping entry.
struct SemanticIcon
{
    std::string action_id;            ///< e.g. "file.new", "editor.save"
    std::string icon_name;            ///< Resolved icon name (e.g. "codicon-new-file")
    std::string accessibility_label;  ///< Screen reader label
    IconSize default_size{IconSize::kMedium};
    bool is_theme_aware{true};        ///< Icons adapt to light/dark theme
    bool has_interactive_states{true}; ///< Hover/active/disabled states
    // V27 additions
    IconPlacement placement{IconPlacement::kLeading}; ///< V27: default placement
    std::string surface_hint;         ///< V27: primary surface (e.g. "toolbar", "settings")

    [[nodiscard]] auto has_label() const noexcept -> bool { return !accessibility_label.empty(); }
};

/// Icon residue report entry.
struct IconResidueEntry
{
    std::string surface;      ///< Where the residue was found
    std::string old_icon;     ///< Legacy icon identifier
    std::string description;  ///< Description of the residue
};

/// Manages semantic icon mappings and residue tracking.
class IconSemanticMapper
{
public:
    explicit IconSemanticMapper(EventBus& bus);

    /// Register a semantic icon mapping.
    void register_icon(const SemanticIcon& icon);

    /// Look up an icon by action ID.
    [[nodiscard]] auto icon(const std::string& action_id) const -> const SemanticIcon*;

    /// All registered icons.
    [[nodiscard]] auto all_icons() const -> std::vector<SemanticIcon>;

    /// Report an icon residue (legacy/placeholder).
    void report_residue(const IconResidueEntry& entry);

    /// All residue entries.
    [[nodiscard]] auto all_residue() const -> const std::vector<IconResidueEntry>&
    {
        return residue_;
    }

    /// Icons missing accessibility labels.
    [[nodiscard]] auto unlabeled_icons() const -> std::vector<SemanticIcon>;

    /// Total registered icons.
    [[nodiscard]] auto icon_count() const noexcept -> int
    {
        return static_cast<int>(icons_.size());
    }

    /// Total residue entries.
    [[nodiscard]] auto residue_count() const noexcept -> int
    {
        return static_cast<int>(residue_.size());
    }

    /// Populate default icon mappings.
    void register_defaults();

    // ── V27 additions ─────────────────────────────────────────────────────

    /// V27: Get usage rules for a specific icon size category.
    [[nodiscard]] auto v27_usage_rules(IconSize size) const -> IconUsageRule;

    /// V27: Register expanded V27 semantic mappings covering all surfaces.
    void register_v27_defaults();

    /// V27: Icons with standalone placement (icon-only buttons).
    [[nodiscard]] auto standalone_icons() const -> std::vector<SemanticIcon>;

    /// V27: Icons by surface hint.
    [[nodiscard]] auto icons_for_surface(const std::string& surface) const -> std::vector<SemanticIcon>;

private:
    EventBus& event_bus_;
    std::unordered_map<std::string, SemanticIcon> icons_;
    std::vector<IconResidueEntry> residue_;
};

} // namespace markamp::core
