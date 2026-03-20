/// @file SettingsCatalogBootstrap.cpp
/// @brief P06-T02: Registers built-in settings catalog at startup.

#include "SettingsCatalogBootstrap.h"

#include "Config.h"
#include "EventBus.h"
#include "Logger.h"

#include <algorithm>

namespace markamp::core
{

SettingsCatalogBootstrap::SettingsCatalogBootstrap(EventBus& bus, Config& cfg)
    : event_bus_(bus)
    , config_(cfg)
{
    register_built_in_settings();
}

void SettingsCatalogBootstrap::register_setting(const SettingDefinition& def)
{
    catalog_[def.key] = def;
}

auto SettingsCatalogBootstrap::get_definition(const std::string& key) const
    -> const SettingDefinition*
{
    auto iter = catalog_.find(key);
    if (iter != catalog_.end())
    {
        return &iter->second;
    }
    return nullptr;
}

auto SettingsCatalogBootstrap::by_category(const std::string& category) const
    -> std::vector<const SettingDefinition*>
{
    std::vector<const SettingDefinition*> results;
    for (const auto& [key, def] : catalog_)
    {
        if (def.category == category)
        {
            results.push_back(&def);
        }
    }
    return results;
}

auto SettingsCatalogBootstrap::search(const std::string& query) const
    -> std::vector<const SettingDefinition*>
{
    std::vector<const SettingDefinition*> results;
    const auto lower_query = [&]()
    {
        std::string result = query;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char chr) { return std::tolower(chr); });
        return result;
    }();

    for (const auto& [key, def] : catalog_)
    {
        auto lower_key = def.key;
        std::transform(lower_key.begin(), lower_key.end(), lower_key.begin(),
                       [](unsigned char chr) { return std::tolower(chr); });
        auto lower_label = def.label;
        std::transform(lower_label.begin(), lower_label.end(), lower_label.begin(),
                       [](unsigned char chr) { return std::tolower(chr); });

        if (lower_key.find(lower_query) != std::string::npos ||
            lower_label.find(lower_query) != std::string::npos)
        {
            results.push_back(&def);
        }
    }
    return results;
}

auto SettingsCatalogBootstrap::validate(const std::string& key,
                                        const std::string& value) const -> bool
{
    const auto* def = get_definition(key);
    if (def == nullptr)
    {
        return true; // Unknown keys pass validation (extensibility)
    }

    if (def->value_type == "enum")
    {
        return std::find(def->enum_values.begin(), def->enum_values.end(), value) !=
               def->enum_values.end();
    }

    return true; // Basic types: accept any string representation
}

void SettingsCatalogBootstrap::register_built_in_settings()
{
    // Editor settings
    register_setting({"editor.wordWrap", "Word Wrap", "Controls how lines should wrap",
                      "Editor", "off", "enum", {"off", "on", "wordWrapColumn", "bounded"}, false});
    register_setting({"editor.fontSize", "Font Size", "Editor font size in pixels",
                      "Editor", "14", "int", {}, false});
    register_setting({"editor.tabSize", "Tab Size", "Number of spaces per tab",
                      "Editor", "4", "int", {}, false});
    register_setting({"editor.lineNumbers", "Line Numbers", "Controls line number display",
                      "Editor", "on", "enum", {"off", "on", "relative"}, false});

    // Theme settings
    register_setting({"workbench.colorTheme", "Color Theme", "Active color theme",
                      "Appearance", "Neon Genesis", "string", {}, false});
    register_setting({"workbench.densityProfile", "Density Profile", "UI density level",
                      "Appearance", "comfortable", "enum", {"compact", "comfortable", "spacious"}, false});

    // Preview settings
    register_setting({"preview.autoRefresh", "Auto Refresh", "Auto-refresh preview on save",
                      "Preview", "true", "bool", {}, false});

    // AI settings
    register_setting({"ai.provider", "AI Provider", "Active AI provider",
                      "AI", "none", "enum", {"none", "openai", "anthropic", "local"}, true});

    MARKAMP_LOG_INFO("Settings catalog: {} built-in settings registered", catalog_.size());
}

} // namespace markamp::core
