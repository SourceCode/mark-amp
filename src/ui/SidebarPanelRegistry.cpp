#include "ui/SidebarPanelRegistry.h"

#include "core/Logger.h"

#include <algorithm>

namespace markamp::ui
{

void SidebarPanelRegistry::Register(SidebarMode mode,
                                    const std::string& label,
                                    const std::string& icon_char,
                                    SidebarPanelFactory factory)
{
    // Overwrite if already registered
    if (auto* entry = FindEntry(mode); entry != nullptr)
    {
        entry->label = label;
        entry->icon_char = icon_char;
        entry->factory = std::move(factory);
        MARKAMP_LOG_DEBUG("SidebarPanelRegistry: re-registered mode {}", label);
        return;
    }

    entries_.push_back({mode, label, icon_char, std::move(factory), nullptr});
    MARKAMP_LOG_DEBUG("SidebarPanelRegistry: registered mode {}", label);
}

auto SidebarPanelRegistry::GetOrCreate(SidebarMode mode, wxWindow* parent) -> wxPanel*
{
    auto* entry = FindEntry(mode);
    if (entry == nullptr)
    {
        MARKAMP_LOG_WARN("SidebarPanelRegistry: no entry for requested mode");
        return nullptr;
    }

    if (entry->panel == nullptr && entry->factory && parent != nullptr)
    {
        entry->panel = entry->factory(parent);
        MARKAMP_LOG_INFO("SidebarPanelRegistry: lazily created panel for mode {}", entry->label);
    }

    return entry->panel;
}

auto SidebarPanelRegistry::GetLabel(SidebarMode mode) const -> std::string
{
    const auto* entry = FindEntry(mode);
    return (entry != nullptr) ? entry->label : std::string{};
}

auto SidebarPanelRegistry::GetIconChar(SidebarMode mode) const -> std::string
{
    const auto* entry = FindEntry(mode);
    return (entry != nullptr) ? entry->icon_char : std::string{};
}

auto SidebarPanelRegistry::IsRegistered(SidebarMode mode) const -> bool
{
    return FindEntry(mode) != nullptr;
}

auto SidebarPanelRegistry::AllModes() const -> std::vector<SidebarMode>
{
    std::vector<SidebarMode> modes;
    modes.reserve(entries_.size());
    for (const auto& entry : entries_)
    {
        modes.push_back(entry.mode);
    }
    return modes;
}

auto SidebarPanelRegistry::Count() const -> size_t
{
    return entries_.size();
}

auto SidebarPanelRegistry::FindEntry(SidebarMode mode) -> SidebarPanelEntry*
{
    auto iter = std::find_if(entries_.begin(),
                             entries_.end(),
                             [mode](const SidebarPanelEntry& entry) { return entry.mode == mode; });
    return (iter != entries_.end()) ? &(*iter) : nullptr;
}

auto SidebarPanelRegistry::FindEntry(SidebarMode mode) const -> const SidebarPanelEntry*
{
    auto iter = std::find_if(entries_.begin(),
                             entries_.end(),
                             [mode](const SidebarPanelEntry& entry) { return entry.mode == mode; });
    return (iter != entries_.end()) ? &(*iter) : nullptr;
}

} // namespace markamp::ui
