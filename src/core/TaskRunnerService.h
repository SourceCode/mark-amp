#pragma once

#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Represents a defined task.
struct TaskDefinition
{
    std::string name;    ///< Task name
    std::string type;    ///< Task type (e.g. "shell", "process")
    std::string command; ///< Command to execute
    std::string group;   ///< Task group (e.g. "build", "test")
    std::string label;   ///< Human-readable label (Phase 36)
};

/// Status of a running task.
enum class TaskStatus
{
    kIdle,      // Task not running
    kRunning,   // Task currently executing
    kCompleted, // Task finished successfully
    kFailed     // Task finished with error
};

/// Runtime state for a task instance.
struct TaskState
{
    std::string task_name;
    TaskStatus status{TaskStatus::kIdle};
    std::chrono::steady_clock::time_point start_time;
    std::string output; // Captured output
    std::string error;  // Error message if failed
    int exit_code{0};
};

/// Service for defining, executing, and tracking tasks.
///
/// Upgraded from P4 stub to support:
/// - Task definitions from `.markamp/tasks.json`
/// - Built-in tasks: lint, export, reindex
/// - Task status tracking
/// - Task output capture
class TaskRunnerService
{
public:
    TaskRunnerService() = default;

    /// Register a task provider.
    auto register_task_provider(const std::string& type,
                                std::function<std::vector<TaskDefinition>()> provider) -> bool;

    /// Execute a task by name. Returns true if task started.
    auto execute_task(const std::string& task_name) -> bool;

    /// Cancel a running task.
    auto cancel_task(const std::string& task_name) -> bool;

    /// Get all available tasks.
    [[nodiscard]] auto get_tasks() const -> std::vector<TaskDefinition>;

    /// Get the status of a specific task.
    [[nodiscard]] auto get_task_status(const std::string& task_name) const -> TaskStatus;

    /// Get the full state of a specific task.
    [[nodiscard]] auto get_task_state(const std::string& task_name) const
        -> std::optional<TaskState>;

    // ── Phase 36: JSON loading and built-in tasks ──

    /// Load task definitions from a JSON file (e.g. `.markamp/tasks.json`).
    auto load_tasks_from_json(const std::string& path) -> bool;

    /// Register the 3 built-in tasks: lint, export, reindex.
    void register_built_in_tasks();

    /// Get the number of registered tasks.
    [[nodiscard]] auto task_count() const -> std::size_t;

    /// Check if a specific task is currently running.
    [[nodiscard]] auto is_running(const std::string& task_name) const -> bool;

private:
    std::vector<TaskDefinition> tasks_;
    std::unordered_map<std::string, TaskState> task_states_;
    std::unordered_map<std::string, std::function<std::vector<TaskDefinition>()>> providers_;
};

} // namespace markamp::core
