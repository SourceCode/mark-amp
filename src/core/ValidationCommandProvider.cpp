/// @file ValidationCommandProvider.cpp
/// @brief V9 Phase 49 — ValidationCommandProvider implementation.

#include "ValidationCommandProvider.h"

namespace markamp::core
{

void ValidationCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto ValidationCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "validate.runRegression",
        "validate.addBaseline",
        "validate.viewRegressions",
        "validate.runCompatibility",
        "validate.systemHealth",
        "validate.moduleStatus",
        "validate.exportReport",
        "validate.clearBaselines",
    };
}

auto ValidationCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto ValidationCommandProvider::command_count() -> int
{
    return 8;
}

auto ValidationCommandProvider::build_entries() -> std::vector<CommandEntry>
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
        cmd.category = "Validation";
        cmd.description = desc;
        cmd.icon = icon;
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    };

    make("validate.runRegression",
         "Run Regression",
         "Run regression tests against baselines",
         "trending-down");
    make("validate.addBaseline", "Add Baseline", "Add a new regression baseline", "bookmark");
    make("validate.viewRegressions",
         "View Regressions",
         "View regression test results",
         "bar-chart");
    make("validate.runCompatibility",
         "Run Compatibility",
         "Run cross-module compatibility check",
         "check-square");
    make("validate.systemHealth", "System Health", "View system health status", "activity");
    make("validate.moduleStatus", "Module Status", "View module dependency status", "layers");
    make("validate.exportReport", "Export Report", "Export validation report", "file-text");
    make(
        "validate.clearBaselines", "Clear Baselines", "Clear all regression baselines", "x-circle");

    return entries;
}

} // namespace markamp::core
