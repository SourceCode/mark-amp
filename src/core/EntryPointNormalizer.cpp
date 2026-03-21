/// @file EntryPointNormalizer.cpp
/// @brief V20 P06-T03/T05: Entry-point normalizer implementation.

#include "EntryPointNormalizer.h"

#include "Logger.h"

namespace markamp::core
{

void EntryPointNormalizer::register_entry_point(const EntryPointMapping& mapping)
{
    mappings_.push_back(mapping);
    MARKAMP_LOG_DEBUG("Entry point registered: {} -> {}", mapping.label, mapping.command_id);
}

auto EntryPointNormalizer::mappings_for_surface(EntryPointSurface surface) const
    -> std::vector<EntryPointMapping>
{
    std::vector<EntryPointMapping> result;
    for (const auto& m : mappings_)
    {
        if (m.surface == surface)
        {
            result.push_back(m);
        }
    }
    return result;
}

auto EntryPointNormalizer::resolve_command(EntryPointSurface surface,
                                            const std::string& action) const -> std::string
{
    for (const auto& m : mappings_)
    {
        if (m.surface == surface && m.is_active)
        {
            if (action.empty() || m.label == action)
            {
                return m.command_id;
            }
        }
    }
    return {};
}

void EntryPointNormalizer::register_panel_rule(const PanelActivationRule& rule)
{
    panel_rules_.push_back(rule);
}

auto EntryPointNormalizer::rules_for_trigger(const std::string& trigger) const
    -> std::vector<PanelActivationRule>
{
    std::vector<PanelActivationRule> result;
    for (const auto& r : panel_rules_)
    {
        if (r.trigger == trigger)
        {
            result.push_back(r);
        }
    }
    return result;
}

auto EntryPointNormalizer::focus_return_for(const std::string& panel_id) const -> std::string
{
    for (const auto& r : panel_rules_)
    {
        if (r.panel_id == panel_id && !r.focus_return_target.empty())
        {
            return r.focus_return_target;
        }
    }
    return "active_editor";
}

} // namespace markamp::core
