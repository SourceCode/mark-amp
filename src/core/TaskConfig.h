#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

/// Task group classification.
enum class TaskGroup
{
    kBuild,
    kTest,
    kDeploy,
    kOther,
};

/// Task type.
enum class TaskType
{
    kShell,
    kProcess,
};

/// Presentation options for task output.
struct TaskPresentation
{
    bool reveal{true};        ///< Auto-reveal output panel
    bool focus{false};        ///< Focus on the panel
    bool clear_before{false}; ///< Clear output before running
    bool show_rerun{true};    ///< Show rerun button
};

/// A single task definition, parsed from tasks.json or created as a template.
struct TaskDefinition
{
    std::string name;
    std::string label; ///< Display label (defaults to name)
    std::string command;
    std::vector<std::string> args;
    std::string working_directory;
    TaskGroup group{TaskGroup::kOther};
    TaskType type{TaskType::kShell};
    TaskPresentation presentation;
    std::string problem_matcher; ///< Compiler output matcher ID
    bool is_default{false};      ///< Default task for the group
    bool is_background{false};   ///< Long-running background task

    [[nodiscard]] auto full_command() const -> std::string
    {
        std::string result = command;
        for (const auto& arg : args)
        {
            result += " " + arg;
        }
        return result;
    }
};

/// Manages task configuration — loading/saving tasks.json, templates, and CRUD.
/// Phase 25 Task 3.
class TaskConfig
{
public:
    TaskConfig() = default;

    /// Load tasks from a JSON string (tasks.json content).
    void load_from_json(const std::string& json_text);

    /// Serialize current tasks to JSON string.
    [[nodiscard]] auto to_json() const -> std::string;

    /// All task definitions.
    [[nodiscard]] auto tasks() const -> const std::vector<TaskDefinition>&;

    /// Find a task by name.
    [[nodiscard]] auto find_task(const std::string& task_name) const -> const TaskDefinition*;

    /// Get tasks filtered by group.
    [[nodiscard]] auto tasks_by_group(TaskGroup group) const -> std::vector<TaskDefinition>;

    /// Get the default task for a group (if any).
    [[nodiscard]] auto default_task(TaskGroup group) const -> const TaskDefinition*;

    /// Add a task definition.
    void add_task(TaskDefinition task);

    /// Remove a task by name.
    void remove_task(const std::string& task_name);

    // ── Task Templates ──

    /// Create a CMake build task template.
    [[nodiscard]] static auto cmake_build_template(const std::string& build_dir = "build",
                                                   const std::string& config = "Debug")
        -> TaskDefinition;

    /// Create a Make build task template.
    [[nodiscard]] static auto make_build_template(const std::string& target = "all")
        -> TaskDefinition;

    /// Create a CTest task template.
    [[nodiscard]] static auto ctest_template(const std::string& build_dir = "build")
        -> TaskDefinition;

    /// Create a custom build command task from user input.
    [[nodiscard]] static auto custom_command_template(const std::string& name,
                                                      const std::string& command,
                                                      const std::string& working_dir = ".",
                                                      TaskGroup group = TaskGroup::kBuild)
        -> TaskDefinition;

private:
    std::vector<TaskDefinition> tasks_;
};

/// Maps TaskGroup enum to string.
[[nodiscard]] inline auto task_group_name(TaskGroup group) -> const char*
{
    switch (group)
    {
        case TaskGroup::kBuild:
            return "build";
        case TaskGroup::kTest:
            return "test";
        case TaskGroup::kDeploy:
            return "deploy";
        case TaskGroup::kOther:
            return "other";
    }
    return "other";
}

/// Maps string to TaskGroup enum.
[[nodiscard]] inline auto task_group_from_string(const std::string& str) -> TaskGroup
{
    if (str == "build")
        return TaskGroup::kBuild;
    if (str == "test")
        return TaskGroup::kTest;
    if (str == "deploy")
        return TaskGroup::kDeploy;
    return TaskGroup::kOther;
}

} // namespace markamp::core
