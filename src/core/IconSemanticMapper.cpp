/// @file IconSemanticMapper.cpp
/// @brief V20 P09-T02: Icon semantic mapper implementation.

#include "IconSemanticMapper.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

IconSemanticMapper::IconSemanticMapper(EventBus& bus)
    : event_bus_(bus)
{
    register_defaults();
}

void IconSemanticMapper::register_icon(const SemanticIcon& icon)
{
    icons_[icon.action_id] = icon;

    events::IconMappingRegisteredEvent evt;
    evt.action_id = icon.action_id;
    evt.icon_name = icon.icon_name;
    evt.has_label = icon.has_label();
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Icon mapping: {} -> {}", icon.action_id, icon.icon_name);
}

auto IconSemanticMapper::icon(const std::string& action_id) const -> const SemanticIcon*
{
    auto it = icons_.find(action_id);
    return it != icons_.end() ? &it->second : nullptr;
}

auto IconSemanticMapper::all_icons() const -> std::vector<SemanticIcon>
{
    std::vector<SemanticIcon> result;
    result.reserve(icons_.size());
    for (const auto& [id, ic] : icons_)
    {
        result.push_back(ic);
    }
    return result;
}

void IconSemanticMapper::report_residue(const IconResidueEntry& entry)
{
    residue_.push_back(entry);

    events::IconResidueReportedEvent evt;
    evt.surface = entry.surface;
    evt.old_icon = entry.old_icon;
    event_bus_.publish(evt);

    MARKAMP_LOG_WARN("Icon residue: {} in {} ({})", entry.old_icon, entry.surface, entry.description);
}

auto IconSemanticMapper::unlabeled_icons() const -> std::vector<SemanticIcon>
{
    std::vector<SemanticIcon> result;
    for (const auto& [id, ic] : icons_)
    {
        if (!ic.has_label())
        {
            result.push_back(ic);
        }
    }
    return result;
}

void IconSemanticMapper::register_defaults()
{
    // File actions
    register_icon({"file.new", "codicon-new-file", "New File", IconSize::kMedium, true, true, IconPlacement::kLeading, "toolbar"});
    register_icon({"file.open", "codicon-folder-opened", "Open File", IconSize::kMedium, true, true, IconPlacement::kLeading, "toolbar"});
    register_icon({"file.save", "codicon-save", "Save File", IconSize::kMedium, true, true, IconPlacement::kLeading, "toolbar"});
    register_icon({"file.saveAll", "codicon-save-all", "Save All", IconSize::kMedium, true, true, IconPlacement::kLeading, "toolbar"});

    // Notebook actions
    register_icon({"notebook.new", "codicon-notebook", "New Notebook", IconSize::kMedium, true, true, IconPlacement::kLeading, "notebook"});
    register_icon({"notebook.run", "codicon-play", "Run Cell", IconSize::kMedium, true, true, IconPlacement::kLeading, "notebook"});
    register_icon({"notebook.stop", "codicon-debug-stop", "Stop Execution", IconSize::kMedium, true, true, IconPlacement::kLeading, "notebook"});

    // Canvas actions
    register_icon({"canvas.new", "codicon-whitespace", "New Canvas", IconSize::kMedium, true, true, IconPlacement::kLeading, "canvas"});

    // Edit actions
    register_icon({"edit.undo", "codicon-discard", "Undo", IconSize::kMedium, true, true, IconPlacement::kLeading, "toolbar"});
    register_icon({"edit.redo", "codicon-redo", "Redo", IconSize::kMedium, true, true, IconPlacement::kLeading, "toolbar"});
    register_icon({"edit.cut", "codicon-clippy", "Cut", IconSize::kMedium, true, true, IconPlacement::kLeading, "contextmenu"});
    register_icon({"edit.copy", "codicon-copy", "Copy", IconSize::kMedium, true, true, IconPlacement::kLeading, "contextmenu"});
    register_icon({"edit.paste", "codicon-paste", "Paste", IconSize::kMedium, true, true, IconPlacement::kLeading, "contextmenu"});

    // View actions
    register_icon({"view.explorer", "codicon-files", "Explorer", IconSize::kLarge, true, true, IconPlacement::kStandalone, "activitybar"});
    register_icon({"view.search", "codicon-search", "Search", IconSize::kLarge, true, true, IconPlacement::kStandalone, "activitybar"});
    register_icon({"view.extensions", "codicon-extensions", "Extensions", IconSize::kLarge, true, true, IconPlacement::kStandalone, "activitybar"});
    register_icon({"view.settings", "codicon-settings-gear", "Settings", IconSize::kMedium, true, true, IconPlacement::kStandalone, "activitybar"});

    MARKAMP_LOG_DEBUG("Default icon mappings registered: {} icons", icon_count());
}

} // namespace markamp::core
