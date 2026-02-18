/// @file AutomationRule.cpp
/// @brief V9 Phase 40 — AutomationRuleManager implementation.

#include "AutomationRule.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

auto automation_trigger_name(AutomationTrigger trigger) -> std::string
{
    switch (trigger)
    {
        case AutomationTrigger::kOnFileSave:
            return "on_file_save";
        case AutomationTrigger::kOnFileCreate:
            return "on_file_create";
        case AutomationTrigger::kOnFileDelete:
            return "on_file_delete";
        case AutomationTrigger::kOnWorkspaceOpen:
            return "on_workspace_open";
        case AutomationTrigger::kOnWorkspaceClose:
            return "on_workspace_close";
        case AutomationTrigger::kOnTimer:
            return "on_timer";
        case AutomationTrigger::kOnManual:
            return "on_manual";
        case AutomationTrigger::kOnGitCommit:
            return "on_git_commit";
    }
    return "unknown";
}

auto automation_action_name(AutomationAction action) -> std::string
{
    switch (action)
    {
        case AutomationAction::kRunCommand:
            return "run_command";
        case AutomationAction::kRunScript:
            return "run_script";
        case AutomationAction::kShowNotification:
            return "show_notification";
        case AutomationAction::kExportFile:
            return "export_file";
        case AutomationAction::kRefreshIndex:
            return "refresh_index";
        case AutomationAction::kCustomCallback:
            return "custom_callback";
    }
    return "unknown";
}

void AutomationRuleManager::add_rule(AutomationRuleEntry rule)
{
    if (rule.rule_id.empty())
    {
        rule.rule_id = "auto_" + std::to_string(rules_.size());
    }
    rules_.push_back(std::move(rule));
}

auto AutomationRuleManager::remove_rule(const std::string& rule_id) -> bool
{
    auto iter =
        std::remove_if(rules_.begin(),
                       rules_.end(),
                       [&](const AutomationRuleEntry& rule) { return rule.rule_id == rule_id; });
    if (iter == rules_.end())
    {
        return false;
    }
    rules_.erase(iter, rules_.end());
    return true;
}

auto AutomationRuleManager::find_rule(const std::string& rule_id) const
    -> const AutomationRuleEntry*
{
    for (const auto& rule : rules_)
    {
        if (rule.rule_id == rule_id)
        {
            return &rule;
        }
    }
    return nullptr;
}

auto AutomationRuleManager::all_rules() const -> const std::vector<AutomationRuleEntry>&
{
    return rules_;
}

auto AutomationRuleManager::rule_count() const -> int
{
    return static_cast<int>(rules_.size());
}

auto AutomationRuleManager::enable_rule(const std::string& rule_id) -> bool
{
    auto* rule = find_mut(rule_id);
    if (rule == nullptr)
    {
        return false;
    }
    rule->enabled = true;
    return true;
}

auto AutomationRuleManager::disable_rule(const std::string& rule_id) -> bool
{
    auto* rule = find_mut(rule_id);
    if (rule == nullptr)
    {
        return false;
    }
    rule->enabled = false;
    return true;
}

auto AutomationRuleManager::rules_for_trigger(AutomationTrigger trigger) const
    -> std::vector<const AutomationRuleEntry*>
{
    std::vector<const AutomationRuleEntry*> result;
    for (const auto& rule : rules_)
    {
        if (rule.trigger == trigger && rule.enabled)
        {
            result.push_back(&rule);
        }
    }
    return result;
}

auto AutomationRuleManager::matching_rules(AutomationTrigger trigger,
                                           const std::string& file_path) const
    -> std::vector<const AutomationRuleEntry*>
{
    std::vector<const AutomationRuleEntry*> result;
    for (const auto& rule : rules_)
    {
        if (rule.trigger != trigger || !rule.enabled)
        {
            continue;
        }
        // If no file pattern, matches everything
        if (rule.file_pattern.empty() || matches_pattern(file_path, rule.file_pattern))
        {
            result.push_back(&rule);
        }
    }
    return result;
}

auto AutomationRuleManager::mark_executed(const std::string& rule_id) -> bool
{
    auto* rule = find_mut(rule_id);
    if (rule == nullptr)
    {
        return false;
    }
    ++rule->execution_count;
    rule->last_executed = std::chrono::system_clock::now();
    return true;
}

auto AutomationRuleManager::export_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\n  \"automation_rules\": [\n";

    bool first = true;
    for (const auto& rule : rules_)
    {
        if (!first)
        {
            oss << ",\n";
        }
        first = false;
        oss << "    {\"id\": \"" << rule.rule_id << "\", \"name\": \"" << rule.name
            << "\", \"trigger\": \"" << automation_trigger_name(rule.trigger)
            << "\", \"action\": \"" << automation_action_name(rule.action)
            << "\", \"enabled\": " << (rule.enabled ? "true" : "false")
            << ", \"executions\": " << rule.execution_count << "}";
    }

    oss << "\n  ]\n}\n";
    return oss.str();
}

void AutomationRuleManager::load_defaults()
{
    rules_.clear();

    // Default: refresh index on file save
    AutomationRuleEntry refresh;
    refresh.rule_id = "default_refresh_on_save";
    refresh.name = "Refresh Index on Save";
    refresh.description = "Rebuild search index when a Markdown file is saved";
    refresh.trigger = AutomationTrigger::kOnFileSave;
    refresh.action = AutomationAction::kRefreshIndex;
    refresh.file_pattern = "*.md";
    rules_.push_back(std::move(refresh));

    // Default: notify on workspace open
    AutomationRuleEntry notify_open;
    notify_open.rule_id = "default_notify_open";
    notify_open.name = "Welcome Notification";
    notify_open.description = "Show welcome message when workspace opens";
    notify_open.trigger = AutomationTrigger::kOnWorkspaceOpen;
    notify_open.action = AutomationAction::kShowNotification;
    notify_open.notification_message = "Workspace loaded successfully";
    rules_.push_back(std::move(notify_open));
}

void AutomationRuleManager::clear_all()
{
    rules_.clear();
}

auto AutomationRuleManager::find_mut(const std::string& rule_id) -> AutomationRuleEntry*
{
    for (auto& rule : rules_)
    {
        if (rule.rule_id == rule_id)
        {
            return &rule;
        }
    }
    return nullptr;
}

auto AutomationRuleManager::matches_pattern(const std::string& path, const std::string& pattern)
    -> bool
{
    // Simple suffix matching — e.g., "*.md" matches "notes/todo.md"
    if (pattern.size() >= 2 && pattern[0] == '*' && pattern[1] == '.')
    {
        auto extension = pattern.substr(1); // ".md"
        if (path.size() >= extension.size())
        {
            return path.compare(path.size() - extension.size(), extension.size(), extension) == 0;
        }
        return false;
    }
    // Substring match fallback
    return path.find(pattern) != std::string::npos;
}

} // namespace markamp::core
