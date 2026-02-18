/// @file IntegrationTestCommandProvider.cpp
/// @brief V9 Phase 49 — IntegrationTestCommandProvider implementation.

#include "IntegrationTestCommandProvider.h"

namespace markamp::core
{

void IntegrationTestCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto IntegrationTestCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "integration.runAll",
        "integration.runSuite",
        "integration.createSuite",
        "integration.viewResults",
        "integration.healthCheck",
        "integration.checkDeps",
        "integration.exportResults",
        "integration.clearResults",
    };
}

auto IntegrationTestCommandProvider::get_command(const std::string& command_id) const
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

auto IntegrationTestCommandProvider::command_count() -> int
{
    return 8;
}

auto IntegrationTestCommandProvider::build_entries() -> std::vector<CommandEntry>
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
        cmd.category = "Testing";
        cmd.description = desc;
        cmd.icon = icon;
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    };

    make("integration.runAll", "Run All Tests", "Run all integration test suites", "play-circle");
    make("integration.runSuite", "Run Suite", "Run a specific test suite", "play");
    make("integration.createSuite", "Create Suite", "Create new test suite", "plus-circle");
    make("integration.viewResults", "View Results", "View test results", "list");
    make("integration.healthCheck", "Health Check", "Run system health check", "heart-pulse");
    make("integration.checkDeps", "Check Dependencies", "Verify module dependencies", "git-branch");
    make("integration.exportResults", "Export Results", "Export test results to file", "download");
    make("integration.clearResults", "Clear Results", "Clear all test results", "trash-2");

    return entries;
}

} // namespace markamp::core
