/// @file ComputationCommandProvider.cpp
/// @brief V9 Phase 48 — ComputationCommandProvider implementation.

#include "ComputationCommandProvider.h"

namespace markamp::core
{

void ComputationCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto ComputationCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "compute.evaluateFormula",
        "compute.setVariable",
        "compute.createChart",
        "compute.addTransform",
        "compute.runPipeline",
        "compute.validateFormula",
        "compute.clearVariables",
        "compute.generateChart",
    };
}

auto ComputationCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto ComputationCommandProvider::command_count() -> int
{
    return 8;
}

auto ComputationCommandProvider::build_entries() -> std::vector<CommandEntry>
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
        cmd.category = "Computation";
        cmd.description = desc;
        cmd.icon = icon;
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    };

    make("compute.evaluateFormula", "Evaluate Formula", "Evaluate an expression", "calculator");
    make("compute.setVariable", "Set Variable", "Define a formula variable", "variable");
    make("compute.createChart", "Create Chart", "Create a chart dataset", "bar-chart-2");
    make("compute.addTransform", "Add Transform", "Add a pipeline transform step", "git-branch");
    make("compute.runPipeline", "Run Pipeline", "Execute transform pipeline", "play");
    make("compute.validateFormula", "Validate Formula", "Check formula syntax", "check-circle");
    make("compute.clearVariables", "Clear Variables", "Remove all formula variables", "x-circle");
    make("compute.generateChart", "Generate Chart", "Auto-generate chart from table", "bar-chart");

    return entries;
}

} // namespace markamp::core
