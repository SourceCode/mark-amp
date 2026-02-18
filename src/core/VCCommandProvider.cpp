/// @file VCCommandProvider.cpp
/// @brief Phase 25: Version Control Integration — VC command provider.

#include "core/VCCommandProvider.h"

#include <algorithm>
#include <unordered_set>

namespace markamp::core
{

VCCommandProvider::VCCommandProvider()
{
    register_commands();
}

auto VCCommandProvider::commands() const -> const std::vector<VCCommandInfo>&
{
    return commands_;
}

auto VCCommandProvider::find_command(const std::string& command_id) const -> const VCCommandInfo*
{
    auto cmd_iter = std::ranges::find_if(
        commands_, [&](const VCCommandInfo& cmd) { return cmd.id == command_id; });
    return cmd_iter != commands_.end() ? &(*cmd_iter) : nullptr;
}

auto VCCommandProvider::commands_for_category(const std::string& category) const
    -> std::vector<VCCommandInfo>
{
    std::vector<VCCommandInfo> result;
    for (const auto& cmd : commands_)
    {
        if (cmd.category == category)
        {
            result.push_back(cmd);
        }
    }
    return result;
}

auto VCCommandProvider::categories() const -> std::vector<std::string>
{
    std::unordered_set<std::string> unique;
    for (const auto& cmd : commands_)
    {
        unique.insert(cmd.category);
    }

    std::vector<std::string> result(unique.begin(), unique.end());
    std::ranges::sort(result);
    return result;
}

auto VCCommandProvider::command_count() const -> int
{
    return static_cast<int>(commands_.size());
}

auto VCCommandProvider::register_commands() -> void
{
    commands_ = {
        {
            "git.init",
            "Git: Initialize Repository",
            "Git: Repository",
            "",
            "Initialize a new Git repository in the workspace",
        },
        {
            "git.stage",
            "Git: Stage File",
            "Git: Staging",
            "Cmd+Shift+A",
            "Stage the current file for commit",
        },
        {
            "git.unstage",
            "Git: Unstage File",
            "Git: Staging",
            "Cmd+Shift+U",
            "Unstage the current file",
        },
        {
            "git.commit",
            "Git: Commit",
            "Git: Commit",
            "Cmd+Shift+K",
            "Commit staged changes with a message",
        },
        {
            "git.stash",
            "Git: Stash Changes",
            "Git: Stash",
            "Cmd+K Cmd+S",
            "Stash current working tree changes",
        },
        {
            "git.stash_pop",
            "Git: Stash Pop",
            "Git: Stash",
            "Cmd+K Cmd+P",
            "Pop the top stash entry and apply it",
        },
        {
            "git.push",
            "Git: Push",
            "Git: Remote",
            "Cmd+Shift+P",
            "Push local commits to the remote repository",
        },
        {
            "git.pull",
            "Git: Pull",
            "Git: Remote",
            "Cmd+Shift+L",
            "Pull and merge remote changes",
        },
        {
            "git.blame",
            "Git: Blame File",
            "Git: Tools",
            "Cmd+Shift+B",
            "Show blame annotations for the current file",
        },
        {
            "git.graph",
            "Git: Show Commit Graph",
            "Git: Tools",
            "Cmd+Shift+G",
            "Open the commit graph visualization",
        },
    };
}

} // namespace markamp::core
