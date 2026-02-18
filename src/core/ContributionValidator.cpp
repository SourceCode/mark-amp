#include "ContributionValidator.h"

#include "Logger.h"

#include <unordered_set>

namespace markamp::core
{

auto ContributionValidator::validate_extension(const std::string& extension_id,
                                               const ExtensionContributions& contributions)
    -> std::vector<ContributionDiagnostic>
{
    std::vector<ContributionDiagnostic> diagnostics;
    has_errors_ = false;

    check_commands(extension_id, contributions.commands, diagnostics);
    check_keybindings(extension_id, contributions.keybindings, diagnostics);
    check_views(extension_id, contributions.views, diagnostics);
    check_themes(extension_id, contributions.themes, diagnostics);

    // Check for errors
    for (const auto& diag : diagnostics)
    {
        if (diag.severity == ContributionDiagnostic::Severity::kError)
        {
            has_errors_ = true;
            break;
        }
    }

    if (!diagnostics.empty())
    {
        MARKAMP_LOG_DEBUG("Contribution validation for '{}': {} diagnostics ({} errors)",
                          extension_id,
                          diagnostics.size(),
                          has_errors_ ? "has" : "no");
    }

    return diagnostics;
}

auto ContributionValidator::validate_plugin(const std::string& plugin_id,
                                            const PluginManifest::ContributionPoints& contributions)
    -> std::vector<ContributionDiagnostic>
{
    std::vector<ContributionDiagnostic> diagnostics;
    has_errors_ = false;

    // Validate commands: check for empty command IDs
    for (const auto& cmd : contributions.commands)
    {
        if (cmd.id.empty())
        {
            diagnostics.push_back({ContributionDiagnostic::Severity::kError,
                                   "command",
                                   "id",
                                   "Command contribution missing required 'id' field",
                                   plugin_id});
        }
        if (cmd.title.empty())
        {
            diagnostics.push_back({ContributionDiagnostic::Severity::kWarning,
                                   "command",
                                   "title",
                                   "Command contribution missing 'title' field",
                                   plugin_id});
        }
    }

    // Validate keybindings
    for (const auto& binding : contributions.keybindings)
    {
        if (binding.command_id.empty())
        {
            diagnostics.push_back({ContributionDiagnostic::Severity::kError,
                                   "keybinding",
                                   "command_id",
                                   "Keybinding contribution missing required 'command_id' field",
                                   plugin_id});
        }
    }

    // Check for errors
    for (const auto& diag : diagnostics)
    {
        if (diag.severity == ContributionDiagnostic::Severity::kError)
        {
            has_errors_ = true;
            break;
        }
    }

    return diagnostics;
}

void ContributionValidator::reset()
{
    has_errors_ = false;
}

void ContributionValidator::check_commands(const std::string& ext_id,
                                           const std::vector<ExtensionCommand>& commands,
                                           std::vector<ContributionDiagnostic>& diagnostics)
{
    std::unordered_set<std::string> seen_ids;
    for (const auto& cmd : commands)
    {
        // Required: command ID
        if (cmd.command.empty())
        {
            diagnostics.push_back({ContributionDiagnostic::Severity::kError,
                                   "command",
                                   "command",
                                   "Command contribution missing required 'command' field",
                                   ext_id});
            continue;
        }

        // Duplicate check
        if (seen_ids.contains(cmd.command))
        {
            diagnostics.push_back({ContributionDiagnostic::Severity::kError,
                                   "command",
                                   "command",
                                   "Duplicate command ID: " + cmd.command,
                                   ext_id});
        }
        seen_ids.insert(cmd.command);

        // Optional: title
        if (cmd.title.empty())
        {
            diagnostics.push_back({ContributionDiagnostic::Severity::kWarning,
                                   "command",
                                   "title",
                                   "Command '" + cmd.command + "' missing 'title' field",
                                   ext_id});
        }
    }
}

void ContributionValidator::check_keybindings(const std::string& ext_id,
                                              const std::vector<ExtensionKeybinding>& keybindings,
                                              std::vector<ContributionDiagnostic>& diagnostics)
{
    for (const auto& binding : keybindings)
    {
        if (binding.command.empty())
        {
            diagnostics.push_back({ContributionDiagnostic::Severity::kError,
                                   "keybinding",
                                   "command",
                                   "Keybinding missing required 'command' field",
                                   ext_id});
        }
        if (binding.key.empty() && binding.mac.empty())
        {
            diagnostics.push_back({ContributionDiagnostic::Severity::kWarning,
                                   "keybinding",
                                   "key",
                                   "Keybinding for '" + binding.command + "' has no key binding",
                                   ext_id});
        }
    }
}

void ContributionValidator::check_views(const std::string& ext_id,
                                        const std::vector<ExtensionView>& views,
                                        std::vector<ContributionDiagnostic>& diagnostics)
{
    std::unordered_set<std::string> seen_ids;
    for (const auto& view : views)
    {
        if (view.view_id.empty())
        {
            diagnostics.push_back({ContributionDiagnostic::Severity::kError,
                                   "view",
                                   "view_id",
                                   "View contribution missing required 'view_id' field",
                                   ext_id});
            continue;
        }
        if (seen_ids.contains(view.view_id))
        {
            diagnostics.push_back({ContributionDiagnostic::Severity::kError,
                                   "view",
                                   "view_id",
                                   "Duplicate view ID: " + view.view_id,
                                   ext_id});
        }
        seen_ids.insert(view.view_id);

        if (view.name.empty())
        {
            diagnostics.push_back({ContributionDiagnostic::Severity::kWarning,
                                   "view",
                                   "name",
                                   "View '" + view.view_id + "' missing 'name' field",
                                   ext_id});
        }
    }
}

void ContributionValidator::check_themes(const std::string& ext_id,
                                         const std::vector<ExtensionTheme>& themes,
                                         std::vector<ContributionDiagnostic>& diagnostics)
{
    for (const auto& theme : themes)
    {
        if (theme.theme_id.empty())
        {
            diagnostics.push_back({ContributionDiagnostic::Severity::kError,
                                   "theme",
                                   "theme_id",
                                   "Theme contribution missing required 'theme_id' field",
                                   ext_id});
        }
        if (theme.path.empty())
        {
            diagnostics.push_back({ContributionDiagnostic::Severity::kWarning,
                                   "theme",
                                   "path",
                                   "Theme '" + theme.theme_id + "' missing 'path' field",
                                   ext_id});
        }
    }
}

} // namespace markamp::core
