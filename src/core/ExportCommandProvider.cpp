/// @file ExportCommandProvider.cpp
/// @brief V9 Phase 24 – Command palette integration for export & publishing.

#include "ExportCommandProvider.h"

#include <algorithm>

namespace markamp::core
{

ExportCommandProvider::ExportCommandProvider()
{
    init_commands();
}

auto ExportCommandProvider::init_commands() -> void
{
    commands_ = {
        {"export.document",
         "Export: Export Document",
         "Export the current document using configured settings",
         "Export",
         "Cmd+Shift+E",
         true},

        {"export.as_pdf",
         "Export: Export as PDF",
         "Export the current document as a PDF file",
         "Export",
         std::nullopt,
         true},

        {"export.as_html",
         "Export: Export as HTML",
         "Export the current document as a standalone HTML file",
         "Export",
         std::nullopt,
         true},

        {"export.quick",
         "Export: Quick Export (Last Profile)",
         "Re-run the most recently used publishing profile",
         "Export",
         "Cmd+Alt+E",
         true},

        {"export.batch",
         "Export: Batch Export",
         "Export multiple documents at once",
         "Export",
         std::nullopt,
         true},

        {"export.print_preview",
         "Export: Print Preview",
         "Show a print preview of the current document",
         "Export",
         "Cmd+P",
         true},

        {"export.manage_profiles",
         "Export: Manage Publishing Profiles",
         "Create, edit, or delete saved publishing profiles",
         "Export",
         std::nullopt,
         true},

        {"export.validate",
         "Export: Validate for Export",
         "Run pre-export validation checks on the current document",
         "Export",
         std::nullopt,
         true},
    };
}

auto ExportCommandProvider::get_commands() const -> const std::vector<ExportCommandInfo>&
{
    return commands_;
}

auto ExportCommandProvider::is_enabled(const std::string& command_id) const -> bool
{
    auto iter =
        std::find_if(commands_.begin(),
                     commands_.end(),
                     [&](const ExportCommandInfo& cmd) { return cmd.cmd_id == command_id; });
    return iter != commands_.end() && iter->enabled;
}

auto ExportCommandProvider::get_command(const std::string& command_id) const
    -> std::optional<ExportCommandInfo>
{
    auto iter =
        std::find_if(commands_.begin(),
                     commands_.end(),
                     [&](const ExportCommandInfo& cmd) { return cmd.cmd_id == command_id; });
    if (iter == commands_.end())
    {
        return std::nullopt;
    }
    return *iter;
}

auto ExportCommandProvider::get_keybinding(const std::string& command_id) const
    -> std::optional<std::string>
{
    auto cmd = get_command(command_id);
    if (!cmd.has_value())
    {
        return std::nullopt;
    }
    return cmd->keybinding;
}

} // namespace markamp::core
