/// @file PdfCommandProvider.cpp
/// @brief V9 Phase 42 — PdfCommandProvider implementation.

#include "PdfCommandProvider.h"

namespace markamp::core
{

void PdfCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto PdfCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "pdf.openFile",
        "pdf.closeFile",
        "pdf.nextPage",
        "pdf.previousPage",
        "pdf.goToPage",
        "pdf.zoomIn",
        "pdf.zoomOut",
        "pdf.extractText",
    };
}

auto PdfCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto PdfCommandProvider::command_count() -> int
{
    return 8;
}

auto PdfCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "pdf.openFile";
        cmd.title = "Open PDF File";
        cmd.category = "PDF";
        cmd.description = "Open a PDF document for viewing";
        cmd.icon = "file-pdf";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "pdf.closeFile";
        cmd.title = "Close PDF";
        cmd.category = "PDF";
        cmd.description = "Close the current PDF document";
        cmd.icon = "close";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "pdf.nextPage";
        cmd.title = "Next PDF Page";
        cmd.category = "PDF";
        cmd.description = "Go to the next page";
        cmd.shortcut = "Cmd+Right";
        cmd.icon = "arrow-right";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "pdf.previousPage";
        cmd.title = "Previous PDF Page";
        cmd.category = "PDF";
        cmd.description = "Go to the previous page";
        cmd.shortcut = "Cmd+Left";
        cmd.icon = "arrow-left";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "pdf.goToPage";
        cmd.title = "Go to PDF Page";
        cmd.category = "PDF";
        cmd.description = "Navigate to a specific page number";
        cmd.icon = "bookmark";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "pdf.zoomIn";
        cmd.title = "Zoom In PDF";
        cmd.category = "PDF";
        cmd.description = "Increase PDF zoom level";
        cmd.shortcut = "Cmd+=";
        cmd.icon = "zoom-in";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "pdf.zoomOut";
        cmd.title = "Zoom Out PDF";
        cmd.category = "PDF";
        cmd.description = "Decrease PDF zoom level";
        cmd.shortcut = "Cmd+-";
        cmd.icon = "zoom-out";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "pdf.extractText";
        cmd.title = "Extract PDF Text";
        cmd.category = "PDF";
        cmd.description = "Extract text from the current PDF";
        cmd.icon = "text";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
