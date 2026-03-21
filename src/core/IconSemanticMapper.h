/// @file IconSemanticMapper.h
/// @brief V20 P09-T02: Icon semantic mapping and normalization.
///
/// Completes icon migration by mapping semantic roles to icon identifiers,
/// enforcing sizing, labels, and theming consistency across all surfaces.
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

/// Semantic icon mapping entry.
struct SemanticIcon
{
    std::string action_id;            ///< e.g. "file.new", "editor.save"
    std::string icon_name;            ///< Resolved icon name (e.g. "codicon-new-file")
    std::string accessibility_label;  ///< Screen reader label
    IconSize default_size{IconSize::kMedium};
    bool is_theme_aware{true};        ///< Icons adapt to light/dark theme
    bool has_interactive_states{true}; ///< Hover/active/disabled states

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

private:
    EventBus& event_bus_;
    std::unordered_map<std::string, SemanticIcon> icons_;
    std::vector<IconResidueEntry> residue_;
};

} // namespace markamp::core
