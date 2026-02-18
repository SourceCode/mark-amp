/// @file LocalizationCommandProvider.cpp
/// @brief V9 Phase 47 — LocalizationCommandProvider implementation.

#include "LocalizationCommandProvider.h"

namespace markamp::core
{

void LocalizationCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto LocalizationCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "locale.change",
        "locale.info",
        "locale.list",
        "locale.add",
        "locale.setDirection",
        "locale.toggleRTL",
        "locale.missingTranslations",
        "locale.reloadTranslations",
    };
}

auto LocalizationCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
{
    for (auto& entry : build_entries())
    {
        if (entry.id == command_id)
        {
            return entry;
        }
    }
    return {};
}

auto LocalizationCommandProvider::command_count() -> int
{
    return 8;
}

auto LocalizationCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    auto make = [&](const std::string& cmd_id,
                    const std::string& cmd_title,
                    const std::string& desc,
                    const std::string& icon)
    {
        CommandEntry cmd;
        cmd.id = cmd_id;
        cmd.title = cmd_title;
        cmd.category = "Localization";
        cmd.description = desc;
        cmd.icon = icon;
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    };

    make("locale.change", "Change Locale", "Switch application language", "globe");
    make("locale.info", "Show Locale Info", "Display current locale details", "info");
    make("locale.list", "List Locales", "Show all available locales", "list");
    make("locale.add", "Add Locale", "Register a new locale", "plus");
    make("locale.setDirection",
         "Set Text Direction",
         "Change text direction (LTR/RTL)",
         "arrow-right");
    make("locale.toggleRTL", "Toggle RTL", "Toggle right-to-left mode", "toggle-right");
    make("locale.missingTranslations",
         "Show Missing Translations",
         "List untranslated keys",
         "alert-triangle");
    make("locale.reloadTranslations",
         "Reload Translations",
         "Reload translation catalog",
         "refresh-cw");

    return entries;
}

} // namespace markamp::core
