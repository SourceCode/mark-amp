/// @file AutomationRule.h
/// @brief V9 Phase 40 — Trigger/action automation rules for workspace automation.
#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Trigger types for automation rules.
enum class AutomationTrigger : uint8_t
{
    kOnFileSave = 0,       ///< When a file is saved
    kOnFileCreate = 1,     ///< When a file is created
    kOnFileDelete = 2,     ///< When a file is deleted
    kOnWorkspaceOpen = 3,  ///< When workspace opens
    kOnWorkspaceClose = 4, ///< When workspace closes
    kOnTimer = 5,          ///< On a timed interval
    kOnManual = 6,         ///< Manually triggered
    kOnGitCommit = 7,      ///< After a git commit
};

/// Action types for automation rules.
enum class AutomationAction : uint8_t
{
    kRunCommand = 0,       ///< Execute a registered command
    kRunScript = 1,        ///< Run an external script
    kShowNotification = 2, ///< Show a notification
    kExportFile = 3,       ///< Export/transform a file
    kRefreshIndex = 4,     ///< Rebuild search index
    kCustomCallback = 5,   ///< Call a custom function
};

/// Convert trigger type to display name.
[[nodiscard]] auto automation_trigger_name(AutomationTrigger trigger) -> std::string;

/// Convert action type to display name.
[[nodiscard]] auto automation_action_name(AutomationAction action) -> std::string;

/// A single automation rule with trigger, condition, and action.
struct AutomationRuleEntry
{
    std::string rule_id;     ///< Unique identifier
    std::string name;        ///< Human-readable name
    std::string description; ///< Detailed description
    AutomationTrigger trigger{AutomationTrigger::kOnManual};
    AutomationAction action{AutomationAction::kRunCommand};

    std::string file_pattern;         ///< Glob pattern for file triggers
    std::string command_id;           ///< Command to execute (for kRunCommand)
    std::string script_path;          ///< Script path (for kRunScript)
    std::string notification_message; ///< Message (for kShowNotification)
    int interval_seconds{0};          ///< Interval (for kOnTimer)

    bool enabled{true};
    int execution_count{0}; ///< How many times this rule has fired
    std::chrono::system_clock::time_point last_executed;
};

/// Manages a collection of automation rules.
///
/// Rules define trigger/action pairs that automate workspace tasks.
/// Supports file-pattern matching, JSON export/import for sharing,
/// and execution tracking.
class AutomationRuleManager
{
public:
    AutomationRuleManager() = default;

    // ── Rule management ───────────────────────────────────────────────
    void add_rule(AutomationRuleEntry rule);
    auto remove_rule(const std::string& rule_id) -> bool;
    [[nodiscard]] auto find_rule(const std::string& rule_id) const -> const AutomationRuleEntry*;
    [[nodiscard]] auto all_rules() const -> const std::vector<AutomationRuleEntry>&;
    [[nodiscard]] auto rule_count() const -> int;

    // ── State ─────────────────────────────────────────────────────────
    auto enable_rule(const std::string& rule_id) -> bool;
    auto disable_rule(const std::string& rule_id) -> bool;

    // ── Matching ──────────────────────────────────────────────────────
    [[nodiscard]] auto rules_for_trigger(AutomationTrigger trigger) const
        -> std::vector<const AutomationRuleEntry*>;
    [[nodiscard]] auto matching_rules(AutomationTrigger trigger, const std::string& file_path) const
        -> std::vector<const AutomationRuleEntry*>;

    // ── Execution tracking ────────────────────────────────────────────
    auto mark_executed(const std::string& rule_id) -> bool;

    // ── Import/Export ─────────────────────────────────────────────────
    [[nodiscard]] auto export_json() const -> std::string;

    // ── Built-in rules ────────────────────────────────────────────────
    void load_defaults();
    void clear_all();

private:
    std::vector<AutomationRuleEntry> rules_;

    auto find_mut(const std::string& rule_id) -> AutomationRuleEntry*;
    [[nodiscard]] static auto matches_pattern(const std::string& path, const std::string& pattern)
        -> bool;
};

} // namespace markamp::core
