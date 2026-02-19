#include "ExtensionBrowserModel.h"

#include <algorithm>

namespace markamp::ui
{

void ExtensionBrowserModel::set_extensions(std::vector<ExtensionInfo> extensions)
{
    extensions_ = std::move(extensions);
}

auto ExtensionBrowserModel::all() const -> const std::vector<ExtensionInfo>&
{
    return extensions_;
}

void ExtensionBrowserModel::set_filter(ExtensionFilter filter)
{
    filter_ = filter;
}
auto ExtensionBrowserModel::filter() const -> ExtensionFilter
{
    return filter_;
}

auto ExtensionBrowserModel::filtered() const -> std::vector<ExtensionInfo>
{
    if (filter_ == ExtensionFilter::kAll)
    {
        return extensions_;
    }

    std::vector<ExtensionInfo> result;
    for (const auto& ext : extensions_)
    {
        bool include = false;
        switch (filter_)
        {
            case ExtensionFilter::kInstalled:
                include = (ext.state != ExtensionState::kNotInstalled);
                break;
            case ExtensionFilter::kEnabled:
                include = (ext.state == ExtensionState::kEnabled);
                break;
            case ExtensionFilter::kDisabled:
                include = (ext.state == ExtensionState::kDisabled);
                break;
            case ExtensionFilter::kUpdatesAvailable:
                include = (ext.state == ExtensionState::kUpdateAvailable);
                break;
            default:
                include = true;
                break;
        }
        if (include)
        {
            result.push_back(ext);
        }
    }
    return result;
}

auto ExtensionBrowserModel::actions_for_state(ExtensionState state) -> std::vector<std::string>
{
    switch (state)
    {
        case ExtensionState::kNotInstalled:
            return {"Install"};
        case ExtensionState::kInstalled:
        case ExtensionState::kEnabled:
            return {"Disable", "Uninstall"};
        case ExtensionState::kDisabled:
            return {"Enable", "Uninstall"};
        case ExtensionState::kUpdateAvailable:
            return {"Update", "Disable", "Uninstall"};
        case ExtensionState::kInstalling:
        case ExtensionState::kUpdating:
        case ExtensionState::kUninstalling:
            return {}; // In progress — no actions
        case ExtensionState::kError:
            return {"Retry", "Uninstall"};
    }
    return {};
}

void ExtensionBrowserModel::toggle_selection(const std::string& extension_id)
{
    for (auto& ext : extensions_)
    {
        if (ext.extension_id == extension_id)
        {
            ext.is_selected = !ext.is_selected;
            return;
        }
    }
}

void ExtensionBrowserModel::select_all_visible()
{
    const auto visible = filtered();
    for (auto& ext : extensions_)
    {
        ext.is_selected = std::any_of(visible.begin(),
                                      visible.end(),
                                      [&](const ExtensionInfo& vis)
                                      { return vis.extension_id == ext.extension_id; });
    }
}

void ExtensionBrowserModel::clear_selection()
{
    for (auto& ext : extensions_)
    {
        ext.is_selected = false;
    }
}

auto ExtensionBrowserModel::selected_count() const -> int
{
    int count = 0;
    for (const auto& ext : extensions_)
    {
        if (ext.is_selected)
        {
            ++count;
        }
    }
    return count;
}

auto ExtensionBrowserModel::selected_ids() const -> std::vector<std::string>
{
    std::vector<std::string> ids;
    for (const auto& ext : extensions_)
    {
        if (ext.is_selected)
        {
            ids.push_back(ext.extension_id);
        }
    }
    return ids;
}

auto ExtensionBrowserModel::count_by_state(ExtensionState state) const -> int
{
    int count = 0;
    for (const auto& ext : extensions_)
    {
        if (ext.state == state)
        {
            ++count;
        }
    }
    return count;
}

} // namespace markamp::ui
