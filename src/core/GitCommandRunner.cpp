#include "GitCommandRunner.h"

#include <wx/log.h>
#include <wx/process.h>
#include <wx/string.h>
#include <wx/utils.h>

#include <memory>
#include <sstream>

namespace markamp::core
{

GitCommandRunner::GitCommandRunner(std::string workspace_root)
    : workspace_root_{std::move(workspace_root)}
{
}

auto GitCommandRunner::RunSync(const std::string& command) -> CommandResult
{
    CommandResult result;

    wxString wx_command = wxString::FromUTF8(command);
    wxArrayString stdout_arr;
    wxArrayString stderr_arr;

    long wx_result = wxExecute(wx_command, stdout_arr, stderr_arr, wxEXEC_SYNC | wxEXEC_NODISABLE);

    result.exit_code = static_cast<int>(wx_result);

    std::ostringstream out_stream;
    for (const auto& line : stdout_arr)
    {
        out_stream << line.utf8_string() << '\n';
    }
    result.stdout_text = out_stream.str();

    std::ostringstream err_stream;
    for (const auto& line : stderr_arr)
    {
        err_stream << line.utf8_string() << '\n';
    }
    result.stderr_text = err_stream.str();

    return result;
}

void GitCommandRunner::RunAsync(const std::string& command,
                                std::function<void(CommandResult)> callback)
{
    // A detached thread handles the blocking sys call and dispatches the callback
    // To be fully robust in wxWidgets, the callback must be marshaled back to the main GUI thread.
    // For now, invoking synchronously to test shell stability before integrating CallAfter hooks.

    std::thread(
        [this, command, callback = std::move(callback)]()
        {
            CommandResult result = this->RunSync(command);
            if (callback)
            {
                callback(result);
            }
        })
        .detach();
}

auto GitCommandRunner::GetStatus() -> std::vector<GitChangeEntry>
{
    std::vector<GitChangeEntry> changes;

    // git status --porcelain=v2 provides a stable machine-readable output
    std::string cmd = "git -C \"" + workspace_root_ + "\" status --porcelain=v2";
    auto result = RunSync(cmd);

    if (!result.success())
    {
        wxLogWarning("GetStatus failed: %s", result.stderr_text);
        return changes;
    }

    auto map_status = [](char c) -> GitChangeStatus
    {
        switch (c)
        {
            case 'M':
                return GitChangeStatus::Modified;
            case 'A':
                return GitChangeStatus::Added;
            case 'D':
                return GitChangeStatus::Deleted;
            case 'R':
                return GitChangeStatus::Renamed;
            case 'C':
                return GitChangeStatus::Copied;
            case 'U':
                return GitChangeStatus::Unmerged;
            case '?':
                return GitChangeStatus::Untracked;
            default:
                return GitChangeStatus::None;
        }
    };

    std::istringstream stream(result.stdout_text);
    std::string line;
    while (std::getline(stream, line))
    {
        if (line.empty())
        {
            continue;
        }

        const char type_char = line[0];
        if (type_char == '1') // Normal tracked
        {
            std::istringstream line_stream(line);
            std::string type_str, status_str, sub_str, mode_head, mode_index, mode_work, hash_head,
                hash_index, path_str;
            line_stream >> type_str >> status_str >> sub_str >> mode_head >> mode_index >>
                mode_work >> hash_head >> hash_index;
            std::getline(line_stream >> std::ws, path_str);

            if (status_str.length() >= 2)
            {
                GitChangeEntry entry;
                entry.index_status = map_status(status_str[0]);
                entry.working_status = map_status(status_str[1]);
                entry.path = path_str;
                changes.push_back(entry);
            }
        }
        else if (type_char == '2') // Renamed or copied
        {
            std::istringstream line_stream(line);
            std::string type_str, status_str, sub_str, mode_head, mode_index, mode_work, hash_head,
                hash_index, score_str, paths_str;
            line_stream >> type_str >> status_str >> sub_str >> mode_head >> mode_index >>
                mode_work >> hash_head >> hash_index >> score_str;
            std::getline(line_stream >> std::ws, paths_str);

            const size_t tab_pos = paths_str.find('\t');
            if (tab_pos != std::string::npos)
            {
                GitChangeEntry entry;
                if (status_str.length() >= 2)
                {
                    entry.index_status = map_status(status_str[0]);
                    entry.working_status = map_status(status_str[1]);
                }
                entry.path = paths_str.substr(0, tab_pos);
                entry.original_path = paths_str.substr(tab_pos + 1);
                changes.push_back(entry);
            }
        }
        else if (type_char == 'u') // Unmerged
        {
            std::istringstream line_stream(line);
            std::string type_str, status_str, sub_str, mode_1, mode_2, mode_3, mode_work, hash_1,
                hash_2, hash_3, path_str;
            line_stream >> type_str >> status_str >> sub_str >> mode_1 >> mode_2 >> mode_3 >>
                mode_work >> hash_1 >> hash_2 >> hash_3;
            std::getline(line_stream >> std::ws, path_str);

            GitChangeEntry entry;
            entry.index_status = GitChangeStatus::Unmerged;
            entry.working_status = GitChangeStatus::Unmerged;
            entry.path = path_str;
            changes.push_back(entry);
        }
        else if (type_char == '?') // Untracked
        {
            const std::string path_str = line.substr(2);
            GitChangeEntry entry;
            entry.index_status = GitChangeStatus::Untracked;
            entry.working_status = GitChangeStatus::Untracked;
            entry.path = path_str;
            changes.push_back(entry);
        }
    }

    return changes;
}

auto GitCommandRunner::GetBranch() -> std::string
{
    std::string cmd = "git -C \"" + workspace_root_ + "\" branch --show-current";
    auto result = RunSync(cmd);

    if (result.success() && !result.stdout_text.empty())
    {
        wxString branch = wxString::FromUTF8(result.stdout_text);
        branch.Trim(true);
        branch.Trim(false);
        return branch.utf8_string();
    }
    return "";
}

auto GitCommandRunner::GetBranches() -> std::vector<std::string>
{
    std::vector<std::string> branches;
    std::string cmd = "git -C \"" + workspace_root_ + "\" branch --format=\"%(refname:short)\"";
    auto result = RunSync(cmd);

    if (result.success())
    {
        std::istringstream stream(result.stdout_text);
        std::string line;
        while (std::getline(stream, line))
        {
            if (!line.empty())
            {
                branches.push_back(line);
            }
        }
    }
    return branches;
}

auto GitCommandRunner::GetLog(const std::string& file, int count, bool with_graph)
    -> std::vector<GitLogEntry>
{
    std::vector<GitLogEntry> log_entries;
    const std::string limit = count > 0 ? " -n " + std::to_string(count) : "";
    const std::string target = file.empty() ? "" : " -- " + file;

    // Use null bytes to separate fields. If with_graph is true, the graph comes before the first
    // null byte.
    const std::string graph_flag = with_graph ? "--graph " : "";
    const std::string format_str =
        with_graph ? "%x00%h%x00%an%x00%cr%x00%s" : "%h%x00%an%x00%cr%x00%s";
    const std::string cmd = "git -C \"" + workspace_root_ + "\" log " + graph_flag + "--format=\"" +
                            format_str + "\"" + limit + target;
    auto result = RunSync(cmd);

    if (result.success() && !result.stdout_text.empty())
    {
        std::istringstream stream(result.stdout_text);
        std::string line;
        while (std::getline(stream, line))
        {
            if (line.empty())
            {
                continue;
            }

            GitLogEntry entry;
            size_t pos_zero = 0;
            if (with_graph)
            {
                pos_zero = line.find('\0');
                if (pos_zero != std::string::npos)
                {
                    entry.graph = line.substr(0, pos_zero);
                    pos_zero++; // Move past the null byte
                }
            }

            const size_t pos_one = line.find('\0', pos_zero);
            if (pos_one != std::string::npos)
            {
                entry.hash = line.substr(pos_zero, pos_one - pos_zero);
                const size_t pos_two = line.find('\0', pos_one + 1);
                if (pos_two != std::string::npos)
                {
                    entry.author = line.substr(pos_one + 1, pos_two - pos_one - 1);
                    const size_t pos_three = line.find('\0', pos_two + 1);
                    if (pos_three != std::string::npos)
                    {
                        entry.date = line.substr(pos_two + 1, pos_three - pos_two - 1);
                        entry.message = line.substr(pos_three + 1);
                    }
                }
            }
            // Add if we got a hash, or if we got a graph fragment without a commit (e.g. padding
            // lines in complex merges)
            if (!entry.hash.empty() || (!entry.graph.empty() && with_graph))
            {
                log_entries.push_back(entry);
            }
        }
    }

    return log_entries;
}

auto GitCommandRunner::GetBlame(const std::string& file) -> std::vector<BlameLine>
{
    std::vector<BlameLine> blame;
    std::string cmd = "git -C \"" + workspace_root_ + "\" blame --porcelain -- " + file;
    auto result = RunSync(cmd);
    // Parsing deferred
    return blame;
}

auto GitCommandRunner::GetDiff(const std::string& file, bool staged) -> std::string
{
    std::string cmd =
        "git -C \"" + workspace_root_ + "\" diff" + (staged ? " --cached" : "") + " -- " + file;
    auto result = RunSync(cmd);
    return result.stdout_text;
}

auto GitCommandRunner::GetFileContentAtHEAD(const std::string& file) -> std::string
{
    std::string cmd = "git -C \"" + workspace_root_ + "\" show HEAD:\"" + file + "\"";
    auto result = RunSync(cmd);
    if (!result.success())
        return "";
    return result.stdout_text;
}

auto GitCommandRunner::GetFileContentFromIndex(const std::string& file) -> std::string
{
    std::string cmd = "git -C \"" + workspace_root_ + "\" show :\"" + file + "\"";
    auto result = RunSync(cmd);
    if (!result.success())
        return "";
    return result.stdout_text;
}

void GitCommandRunner::Stage(const std::string& path)
{
    RunSync("git -C \"" + workspace_root_ + "\" add \"" + path + "\"");
}

void GitCommandRunner::Unstage(const std::string& path)
{
    RunSync("git -C \"" + workspace_root_ + "\" restore --staged \"" + path + "\"");
}

void GitCommandRunner::Commit(const std::string& message)
{
    RunSync("git -C \"" + workspace_root_ + "\" commit -m \"" + message + "\"");
}

void GitCommandRunner::Push()
{
    // Async push would be preferable
    RunSync("git -C \"" + workspace_root_ + "\" push");
}

void GitCommandRunner::Pull()
{
    RunSync("git -C \"" + workspace_root_ + "\" pull");
}

void GitCommandRunner::Fetch()
{
    RunSync("git -C \"" + workspace_root_ + "\" fetch");
}

void GitCommandRunner::SwitchBranch(const std::string& branch)
{
    RunSync("git -C \"" + workspace_root_ + "\" checkout " + branch);
}

void GitCommandRunner::CreateBranch(const std::string& name)
{
    RunSync("git -C \"" + workspace_root_ + "\" checkout -b " + name);
}

void GitCommandRunner::Stash(const std::string& message)
{
    RunSync("git -C \"" + workspace_root_ + "\" stash push -m \"" + message + "\"");
}

void GitCommandRunner::StashPop()
{
    RunSync("git -C \"" + workspace_root_ + "\" stash pop");
}

void GitCommandRunner::Discard(const std::string& path)
{
    // Discard unstaged changes in working directory
    RunSync("git -C \"" + workspace_root_ + "\" restore -- \"" + path + "\"");
}

void GitCommandRunner::ResolveConflict(const std::string& path, bool accept_current)
{
    std::string checkout_flag = accept_current ? "--ours" : "--theirs";
    RunSync("git -C \"" + workspace_root_ + "\" checkout " + checkout_flag + " -- \"" + path +
            "\"");
    // After resolving, stage the file to mark conflict as resolved
    Stage(path);
}

} // namespace markamp::core
