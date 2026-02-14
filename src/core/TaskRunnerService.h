#pragma once

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Represents a defined task.
/// P4 stub — task running is N/A for a Markdown editor.
struct TaskDefinition
{
    std::string name;    ///< Task name
    std::string type;    ///< Task type (e.g. "shell", "process")
    std::string command; ///< Command to execute
    std::string group;   ///< Task group (e.g. "build", "test")
};

/// Stub service for task running.
///
/// P4: Not applicable for a Markdown editor. This stub exists so the
/// extension API surface is complete. Extensions contributing task
/// providers will get well-defined "not available" responses.
class TaskRunnerService
{
public:
    TaskRunnerService() = default;

    /// Register a task provider.
    /// Stub: always returns false.
    auto register_task_provider(const std::string& type,
                                std::function<std::vector<TaskDefinition>()> provider) -> bool;

    /// Execute a task by name.
    /// Stub: always returns false.
    auto execute_task(const std::string& task_name) -> bool;

    /// Get all available tasks.
    [[nodiscard]] auto get_tasks() const -> std::vector<TaskDefinition>;

private:
    std::vector<TaskDefinition> tasks_;
};

} // namespace markamp::core
