/// @file PrivacyCommandProvider.cpp
/// @brief V9 Phase 44 — PrivacyCommandProvider implementation.

#include "PrivacyCommandProvider.h"

namespace markamp::core
{

void PrivacyCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto PrivacyCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "privacy.classify",
        "privacy.setPolicy",
        "privacy.redact",
        "privacy.scan",
        "privacy.stripMetadata",
        "privacy.anonymize",
        "privacy.viewClassifications",
        "privacy.clearRules",
    };
}

auto PrivacyCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto PrivacyCommandProvider::command_count() -> int
{
    return 8;
}

auto PrivacyCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "privacy.classify";
        cmd.title = "Classify Document";
        cmd.category = "Privacy";
        cmd.description = "Set privacy classification for the document";
        cmd.icon = "tag";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "privacy.setPolicy";
        cmd.title = "Set Privacy Policy";
        cmd.category = "Privacy";
        cmd.description = "Configure global privacy policy settings";
        cmd.icon = "shield";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "privacy.redact";
        cmd.title = "Redact Content";
        cmd.category = "Privacy";
        cmd.description = "Apply PII redaction rules to content";
        cmd.icon = "eraser";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "privacy.scan";
        cmd.title = "Scan for PII";
        cmd.category = "Privacy";
        cmd.description = "Scan content for personally identifiable information";
        cmd.icon = "search";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "privacy.stripMetadata";
        cmd.title = "Strip Metadata";
        cmd.category = "Privacy";
        cmd.description = "Remove metadata from exported content";
        cmd.icon = "scissors";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "privacy.anonymize";
        cmd.title = "Anonymize Export";
        cmd.category = "Privacy";
        cmd.description = "Export with anonymized usernames and dates";
        cmd.icon = "incognito";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "privacy.viewClassifications";
        cmd.title = "View Classifications";
        cmd.category = "Privacy";
        cmd.description = "View all document privacy classifications";
        cmd.icon = "list";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "privacy.clearRules";
        cmd.title = "Clear Redaction Rules";
        cmd.category = "Privacy";
        cmd.description = "Remove all custom redaction rules";
        cmd.icon = "trash";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
