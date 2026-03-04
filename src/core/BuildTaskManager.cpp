/// @file BuildTaskManager.cpp
/// @brief Phase 38 Task 6 — Custom build task management implementation.

#include "core/BuildTaskManager.h"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

namespace markamp::core
{

void BuildTaskManager::add_task(BuildTask task)
{
    for (auto& t : tasks_)
    {
        if (t.label == task.label)
        {
            t = std::move(task);
            return;
        }
    }
    tasks_.push_back(std::move(task));
}

auto BuildTaskManager::remove_task(const std::string& label) -> bool
{
    for (auto it = tasks_.begin(); it != tasks_.end(); ++it)
    {
        if (it->label == label)
        {
            tasks_.erase(it);
            return true;
        }
    }
    return false;
}

auto BuildTaskManager::find_task(const std::string& label) const -> const BuildTask*
{
    for (const auto& t : tasks_)
    {
        if (t.label == label)
            return &t;
    }
    return nullptr;
}

auto BuildTaskManager::all_tasks() const -> const std::vector<BuildTask>&
{
    return tasks_;
}

auto BuildTaskManager::tasks_for_group(const std::string& group) const -> std::vector<BuildTask>
{
    std::vector<BuildTask> result;
    for (const auto& t : tasks_)
    {
        if (t.group == group)
            result.push_back(t);
    }
    return result;
}

auto BuildTaskManager::default_task(const std::string& group) const -> const BuildTask*
{
    for (const auto& t : tasks_)
    {
        if (t.group == group && t.is_default)
            return &t;
    }
    return nullptr;
}

auto BuildTaskManager::task_count() const -> std::size_t
{
    return tasks_.size();
}

auto BuildTaskManager::expand_variables(const std::string& input,
                                        const std::string& workspace_folder,
                                        const std::string& current_file) const -> std::string
{
    std::string result = input;

    // ${workspaceFolder}
    auto pos = result.find("${workspaceFolder}");
    while (pos != std::string::npos)
    {
        result.replace(pos, 18, workspace_folder);
        pos = result.find("${workspaceFolder}", pos + workspace_folder.size());
    }

    // ${file}
    pos = result.find("${file}");
    while (pos != std::string::npos)
    {
        result.replace(pos, 7, current_file);
        pos = result.find("${file}", pos + current_file.size());
    }

    // ${fileBasename}
    if (!current_file.empty())
    {
        auto basename = std::filesystem::path(current_file).filename().string();
        pos = result.find("${fileBasename}");
        while (pos != std::string::npos)
        {
            result.replace(pos, 15, basename);
            pos = result.find("${fileBasename}", pos + basename.size());
        }

        // ${fileDirname}
        auto dirname = std::filesystem::path(current_file).parent_path().string();
        pos = result.find("${fileDirname}");
        while (pos != std::string::npos)
        {
            result.replace(pos, 14, dirname);
            pos = result.find("${fileDirname}", pos + dirname.size());
        }
    }

    return result;
}

void BuildTaskManager::load_from_file(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return;

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    auto j = nlohmann::json::parse(content, nullptr, false);
    if (j.is_discarded() || !j.contains("tasks"))
        return;

    tasks_.clear();
    for (const auto& task_json : j["tasks"])
    {
        BuildTask task;
        task.label = task_json.value("label", "");
        task.command = task_json.value("command", "");
        task.working_dir = task_json.value("cwd", "");
        task.group = task_json.value("group", "custom");
        task.is_default = task_json.value("isDefault", false);
        task.run_in_terminal = task_json.value("runInTerminal", true);
        task.problem_matcher = task_json.value("problemMatcher", "");

        if (task_json.contains("args"))
            task.args = task_json["args"].get<std::vector<std::string>>();

        if (!task.label.empty())
            tasks_.push_back(std::move(task));
    }
}

void BuildTaskManager::save_to_file(const std::string& path) const
{
    nlohmann::json j;
    j["version"] = "2.0.0";

    auto& tasks_json = j["tasks"];
    tasks_json = nlohmann::json::array();

    for (const auto& task : tasks_)
    {
        nlohmann::json tj;
        tj["label"] = task.label;
        tj["command"] = task.command;
        tj["args"] = task.args;
        tj["cwd"] = task.working_dir;
        tj["group"] = task.group;
        tj["isDefault"] = task.is_default;
        tj["runInTerminal"] = task.run_in_terminal;
        if (!task.problem_matcher.empty())
            tj["problemMatcher"] = task.problem_matcher;
        tasks_json.push_back(std::move(tj));
    }

    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    std::ofstream file(path);
    if (file.is_open())
    {
        file << j.dump(2);
    }
}

} // namespace markamp::core
