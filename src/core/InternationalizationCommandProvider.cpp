/// @file InternationalizationCommandProvider.cpp
/// @brief V9 Phase 47 — InternationalizationCommandProvider implementation.

#include "InternationalizationCommandProvider.h"

namespace markamp::core
{

void InternationalizationCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto InternationalizationCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "i18n.formatNumber",
        "i18n.formatDate",
        "i18n.pluralize",
        "i18n.formatTemplate",
        "i18n.addTranslation",
        "i18n.removeTranslation",
        "i18n.exportTranslations",
        "i18n.importTranslations",
    };
}

auto InternationalizationCommandProvider::get_command(const std::string& command_id) const
    -> CommandEntry
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

auto InternationalizationCommandProvider::command_count() -> int
{
    return 8;
}

auto InternationalizationCommandProvider::build_entries() -> std::vector<CommandEntry>
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
        cmd.category = "Internationalization";
        cmd.description = desc;
        cmd.icon = icon;
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    };

    make("i18n.formatNumber", "Format Number", "Format number for current locale", "hash");
    make("i18n.formatDate", "Format Date", "Format date for current locale", "calendar");
    make("i18n.pluralize", "Pluralize", "Apply pluralization rules", "type");
    make("i18n.formatTemplate", "Format Template", "Apply positional substitution", "code");
    make("i18n.addTranslation", "Add Translation", "Add a translation entry", "plus");
    make("i18n.removeTranslation", "Remove Translation", "Remove a translation entry", "minus");
    make("i18n.exportTranslations",
         "Export Translations",
         "Export translations to file",
         "download");
    make("i18n.importTranslations",
         "Import Translations",
         "Import translations from file",
         "upload");

    return entries;
}

} // namespace markamp::core
