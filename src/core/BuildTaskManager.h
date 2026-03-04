#pragma once

/// @file BuildTaskManager.h
/// @brief Phase 38 Task 6 — Custom build task management.

#include <string>
#include <vector>

namespace markamp::core
{

/// A custom build task definition.
struct BuildTask
{
    std::string label;             ///< Task display name
    std::string command;           ///< Shell command to execute
    std::vector<std::string> args; ///< Command arguments
    std::string working_dir;       ///< Working directory (supports ${workspaceFolder})
    std::string group;             ///< "build", "test", "clean", "custom"
    bool is_default{false};        ///< Default task for its group
    bool run_in_terminal{true};    ///< Show in terminal vs background
    std::string problem_matcher;   ///< Error parser to use: "gcc", "msvc"
};

/// Manages custom build tasks, persisted to .markamp/tasks.json.
class BuildTaskManager
{
public:
    BuildTaskManager() = default;

    // ── CRUD ──

    /// Add a task. Replaces if label already exists.
    void add_task(BuildTask task);

    /// Remove a task by label.
    auto remove_task(const std::string& label) -> bool;

    /// Find a task by label.
    [[nodiscard]] auto find_task(const std::string& label) const -> const BuildTask*;

    /// Get all tasks.
    [[nodiscard]] auto all_tasks() const -> const std::vector<BuildTask>&;

    /// Get tasks for a specific group.
    [[nodiscard]] auto tasks_for_group(const std::string& group) const -> std::vector<BuildTask>;

    /// Get the default task for a group.
    [[nodiscard]] auto default_task(const std::string& group) const -> const BuildTask*;

    /// Task count.
    [[nodiscard]] auto task_count() const -> std::size_t;

    // ── Variable substitution ──

    /// Expand variables in a command string.
    /// Supports: ${workspaceFolder}, ${file}, ${fileBasename}, ${fileDirname}
    [[nodiscard]] auto expand_variables(const std::string& input,
                                        const std::string& workspace_folder,
                                        const std::string& current_file = "") const -> std::string;

    // ── Persistence ──

    /// Load tasks from a tasks.json file.
    void load_from_file(const std::string& path);

    /// Save tasks to a tasks.json file.
    void save_to_file(const std::string& path) const;

private:
    std::vector<BuildTask> tasks_;
};

} // namespace markamp::core
