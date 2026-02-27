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

    // Parsing logic for v2 porcelain output deferred per Task 2
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

auto GitCommandRunner::GetLog(const std::string& file, int count) -> std::vector<GitLogEntry>
{
    std::vector<GitLogEntry> log_entries;
    std::string limit = count > 0 ? " -n " + std::to_string(count) : "";
    std::string target = file.empty() ? "" : " -- " + file;

    std::string cmd = "git -C \"" + workspace_root_ + "\" log --oneline" + limit + target;
    auto result = RunSync(cmd);
    // Parsing deferred
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

} // namespace markamp::core
