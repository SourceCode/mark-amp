/// @file ShellVisualSystem.cpp
/// @brief V20 P09-T01/T03: Shell visual system implementation.

#include "ShellVisualSystem.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

ShellVisualSystem::ShellVisualSystem(EventBus& bus)
    : event_bus_(bus)
{
    register_defaults();
}

void ShellVisualSystem::register_chrome(const ShellChromeToken& token)
{
    chrome_[token.name] = token;

    events::ShellChromeRegisteredEvent evt;
    evt.token_name = token.name;
    evt.layer = static_cast<int>(token.layer);
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Shell chrome registered: {} (layer={})", token.name, static_cast<int>(token.layer));
}

void ShellVisualSystem::register_state(const ShellStateToken& token)
{
    const auto key = token.surface + "." + token.state;
    states_[key] = token;
    MARKAMP_LOG_DEBUG("Shell state registered: {}", key);
}

auto ShellVisualSystem::chrome(const std::string& name) const -> const ShellChromeToken*
{
    auto it = chrome_.find(name);
    return it != chrome_.end() ? &it->second : nullptr;
}

auto ShellVisualSystem::state(const std::string& surface, const std::string& state_name) const
    -> const ShellStateToken*
{
    const auto key = surface + "." + state_name;
    auto it = states_.find(key);
    return it != states_.end() ? &it->second : nullptr;
}

auto ShellVisualSystem::all_chrome() const -> std::vector<ShellChromeToken>
{
    std::vector<ShellChromeToken> result;
    result.reserve(chrome_.size());
    for (const auto& [name, token] : chrome_)
    {
        result.push_back(token);
    }
    return result;
}

auto ShellVisualSystem::all_states() const -> std::vector<ShellStateToken>
{
    std::vector<ShellStateToken> result;
    result.reserve(states_.size());
    for (const auto& [key, token] : states_)
    {
        result.push_back(token);
    }
    return result;
}

void ShellVisualSystem::register_defaults()
{
    // Shell chrome defaults (dark theme base)
    register_chrome({"shell.titlebar", ShellLayer::kTitleBar, "#1e1e1e", "#333333", "#cccccc", 0, 0, 1.0});
    register_chrome({"shell.sidebar", ShellLayer::kSidebar, "#252526", "#333333", "#cccccc", 1, 0, 1.0});
    register_chrome({"shell.editor", ShellLayer::kEditor, "#1e1e1e", "", "#d4d4d4", 0, 0, 1.0});
    register_chrome({"shell.panel", ShellLayer::kPanel, "#1e1e1e", "#333333", "#cccccc", 1, 0, 1.0});
    register_chrome({"shell.statusbar", ShellLayer::kStatusBar, "#007acc", "", "#ffffff", 0, 0, 1.0});
    register_chrome({"shell.dialog", ShellLayer::kDialog, "#252526", "#454545", "#cccccc", 1, 4, 1.0});
    register_chrome({"shell.popover", ShellLayer::kPopover, "#252526", "#454545", "#cccccc", 1, 3, 1.0});
    register_chrome({"shell.toast", ShellLayer::kToast, "#333333", "#454545", "#ffffff", 1, 2, 0.95});

    // Shell state defaults
    register_state({"tab", "hover", "#2a2d2e", "#ffffff", "", 1.0});
    register_state({"tab", "active", "#1e1e1e", "#ffffff", "", 1.0});
    register_state({"tab", "dirty", "", "#e8a838", "", 1.0});
    register_state({"tree-row", "hover", "#2a2d2e", "#ffffff", "", 1.0});
    register_state({"tree-row", "selected", "#094771", "#ffffff", "", 1.0});
    register_state({"toolbar-button", "hover", "#5a5d5e40", "#ffffff", "", 1.0});
    register_state({"toolbar-button", "active", "#5a5d5e80", "#ffffff", "", 1.0});
    register_state({"toolbar-button", "disabled", "", "#cccccc60", "", 0.5});

    MARKAMP_LOG_DEBUG("Shell visual system defaults registered: {} tokens", total_tokens());
}

} // namespace markamp::core
