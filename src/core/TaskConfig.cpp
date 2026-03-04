#include "TaskConfig.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

void TaskConfig::load_from_json(const std::string& json_text)
{
    // Minimal JSON parsing for tasks.json structure:
    // { "version": "2.0.0", "tasks": [ { "label": "...", "command": "...", ... } ] }
    // For now, parse basic key-value pairs within task objects.
    // Full JSON library integration (nlohmann::json) would be used in production.

    tasks_.clear();

    // Simple line-based parser for task definitions.
    std::istringstream stream(json_text);
    std::string line;
    TaskDefinition current_task;
    bool in_task = false;
    int brace_depth = 0;

    while (std::getline(stream, line))
    {
        // Trim whitespace
        auto trim_start = line.find_first_not_of(" \t\r\n");
        if (trim_start == std::string::npos)
        {
            continue;
        }
        auto trimmed = line.substr(trim_start);

        if (trimmed.find('{') != std::string::npos)
        {
            brace_depth++;
            if (brace_depth == 2) // Task object level
            {
                in_task = true;
                current_task = TaskDefinition{};
            }
        }

        if (in_task)
        {
            // Parse "key": "value" pairs
            auto parse_string_field = [&](const std::string& key, std::string& target)
            {
                auto key_pos = trimmed.find("\"" + key + "\"");
                if (key_pos != std::string::npos)
                {
                    auto colon_pos = trimmed.find(':', key_pos);
                    if (colon_pos != std::string::npos)
                    {
                        auto val_start = trimmed.find('"', colon_pos + 1);
                        if (val_start != std::string::npos)
                        {
                            auto val_end = trimmed.find('"', val_start + 1);
                            if (val_end != std::string::npos)
                            {
                                target = trimmed.substr(val_start + 1, val_end - val_start - 1);
                            }
                        }
                    }
                }
            };

            parse_string_field("label", current_task.label);
            parse_string_field("command", current_task.command);

            std::string group_str;
            parse_string_field("group", group_str);
            if (!group_str.empty())
            {
                current_task.group = task_group_from_string(group_str);
            }

            std::string type_str;
            parse_string_field("type", type_str);
            if (type_str == "process")
            {
                current_task.type = TaskType::kProcess;
            }

            parse_string_field("problemMatcher", current_task.problem_matcher);
        }

        if (trimmed.find('}') != std::string::npos)
        {
            if (in_task && brace_depth == 2)
            {
                in_task = false;
                if (current_task.name.empty())
                {
                    current_task.name = current_task.label;
                }
                if (!current_task.command.empty())
                {
                    tasks_.push_back(std::move(current_task));
                }
            }
            brace_depth--;
        }
    }
}

auto TaskConfig::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"version\": \"2.0.0\",\n";
    oss << "  \"tasks\": [\n";

    for (std::size_t idx = 0; idx < tasks_.size(); ++idx)
    {
        const auto& task = tasks_[idx];
        oss << "    {\n";
        oss << "      \"label\": \"" << task.label << "\",\n";
        oss << "      \"command\": \"" << task.command << "\",\n";
        oss << "      \"type\": \"" << (task.type == TaskType::kProcess ? "process" : "shell")
            << "\",\n";
        oss << "      \"group\": \"" << task_group_name(task.group) << "\"";
        if (!task.problem_matcher.empty())
        {
            oss << ",\n      \"problemMatcher\": \"" << task.problem_matcher << "\"";
        }
        oss << "\n    }";
        if (idx + 1 < tasks_.size())
        {
            oss << ",";
        }
        oss << "\n";
    }

    oss << "  ]\n";
    oss << "}\n";
    return oss.str();
}

auto TaskConfig::tasks() const -> const std::vector<TaskDefinition>&
{
    return tasks_;
}

auto TaskConfig::find_task(const std::string& task_name) const -> const TaskDefinition*
{
    for (const auto& task : tasks_)
    {
        if (task.name == task_name || task.label == task_name)
        {
            return &task;
        }
    }
    return nullptr;
}

auto TaskConfig::tasks_by_group(TaskGroup group) const -> std::vector<TaskDefinition>
{
    std::vector<TaskDefinition> result;
    for (const auto& task : tasks_)
    {
        if (task.group == group)
        {
            result.push_back(task);
        }
    }
    return result;
}

auto TaskConfig::default_task(TaskGroup group) const -> const TaskDefinition*
{
    for (const auto& task : tasks_)
    {
        if (task.group == group && task.is_default)
        {
            return &task;
        }
    }
    return nullptr;
}

void TaskConfig::add_task(TaskDefinition task)
{
    if (task.name.empty())
    {
        task.name = task.label;
    }
    tasks_.push_back(std::move(task));
}

void TaskConfig::remove_task(const std::string& task_name)
{
    tasks_.erase(std::remove_if(tasks_.begin(),
                                tasks_.end(),
                                [&task_name](const TaskDefinition& task)
                                { return task.name == task_name; }),
                 tasks_.end());
}

auto TaskConfig::cmake_build_template(const std::string& build_dir, const std::string& config)
    -> TaskDefinition
{
    TaskDefinition task;
    task.name = "cmake-build";
    task.label = "CMake: Build";
    task.command = "cmake";
    task.args = {"--build", build_dir, "--config", config};
    task.group = TaskGroup::kBuild;
    task.type = TaskType::kShell;
    task.problem_matcher = "$gcc";
    task.is_default = true;
    return task;
}

auto TaskConfig::make_build_template(const std::string& target) -> TaskDefinition
{
    TaskDefinition task;
    task.name = "make-build";
    task.label = "Make: Build";
    task.command = "make";
    task.args = {target, "-j$(nproc)"};
    task.group = TaskGroup::kBuild;
    task.type = TaskType::kShell;
    task.problem_matcher = "$gcc";
    return task;
}

auto TaskConfig::ctest_template(const std::string& build_dir) -> TaskDefinition
{
    TaskDefinition task;
    task.name = "ctest";
    task.label = "CTest: Run Tests";
    task.command = "ctest";
    task.args = {"--test-dir", build_dir, "--output-on-failure"};
    task.group = TaskGroup::kTest;
    task.type = TaskType::kShell;
    task.is_default = true;
    return task;
}

auto TaskConfig::custom_command_template(const std::string& name,
                                         const std::string& command,
                                         const std::string& working_dir,
                                         TaskGroup group) -> TaskDefinition
{
    TaskDefinition task;
    task.name = name;
    task.label = name;
    task.command = command;
    task.working_directory = working_dir;
    task.group = group;
    task.type = TaskType::kShell;
    task.presentation.reveal = true;
    task.presentation.clear_before = true;
    return task;
}

} // namespace markamp::core
