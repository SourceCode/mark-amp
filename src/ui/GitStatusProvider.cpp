#include "ui/GitStatusProvider.h"

#include <array>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <sstream>
#include <thread>

namespace fs = std::filesystem;

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(GitStatusProvider, wxEvtHandler) EVT_TIMER(wxID_ANY, GitStatusProvider::OnTimer)
    wxEND_EVENT_TABLE()

        GitStatusProvider::GitStatusProvider()
    : refresh_timer_(this)
{
    // Check every 5 seconds
    refresh_timer_.Start(5000);
}

GitStatusProvider::~GitStatusProvider()
{
    refresh_timer_.Stop();
}

void GitStatusProvider::SetWorkspaceRoot(const std::string& root_path)
{
    std::lock_guard<std::mutex> lock(status_mutex_);
    workspace_root_ = root_path;
    file_statuses_.clear();

    if (!workspace_root_.empty())
    {
        Refresh();
    }
}

GitFileStatus GitStatusProvider::GetFileStatus(const std::string& absolute_path) const
{
    std::lock_guard<std::mutex> lock(status_mutex_);
    auto it = file_statuses_.find(absolute_path);
    return it != file_statuses_.end() ? it->second : GitFileStatus::None;
}

auto GitStatusProvider::ProvideDecoration(const core::FileNode& node) const
    -> std::optional<FileTreeDecoration>
{
    GitFileStatus status = GetFileStatus(node.id);
    if (status == GitFileStatus::None || status == GitFileStatus::Ignored)
    {
        if (node.is_folder())
        {
            size_t count = GetModifiedCountInDirectory(node.id);
            if (count > 0)
            {
                FileTreeDecoration dec;
                dec.badge_text = std::to_string(count);
                dec.badge_color = wxColour(226, 192, 141);
                dec.priority = 10;
                return dec;
            }
        }
        return std::nullopt;
    }

    FileTreeDecoration dec;
    dec.priority = 10;

    if (status == GitFileStatus::Untracked || status == GitFileStatus::Added)
    {
        dec.badge_text = "U";
        dec.badge_color = wxColour(115, 201, 145);
        dec.text_color = wxColour(115, 201, 145);
    }
    else if (status == GitFileStatus::Deleted)
    {
        dec.badge_text = "D";
        dec.badge_color = wxColour(238, 83, 83);
        dec.text_color = wxColour(238, 83, 83);
    }
    else
    {
        dec.badge_text = "M";
        dec.badge_color = wxColour(226, 192, 141);
        dec.text_color = wxColour(226, 192, 141);
    }
    return dec;
}

size_t GitStatusProvider::GetModifiedCountInDirectory(const std::string& absolute_dir_path) const
{
    std::lock_guard<std::mutex> lock(status_mutex_);

#ifdef _WIN32
    char sep = '\\';
#else
    char sep = '/';
#endif
    std::string prefix = absolute_dir_path;
    if (!prefix.empty() && prefix.back() != sep)
    {
        prefix += sep;
    }

    size_t count = 0;
    for (const auto& [path, status] : file_statuses_)
    {
        if (status != GitFileStatus::None && status != GitFileStatus::Ignored)
        {
            if (path.starts_with(prefix))
            {
                count++;
            }
        }
    }
    return count;
}

void GitStatusProvider::Refresh()
{
    if (workspace_root_.empty())
        return;

    if (is_running_.exchange(true))
    {
        // Already running
        return;
    }

    std::thread(
        [this]()
        {
            RunGitStatus();
            is_running_ = false;
        })
        .detach();
}

void GitStatusProvider::OnTimer(wxTimerEvent&)
{
    Refresh();
}

std::string GitStatusProvider::ProcessGitPath(const std::string& relative_path) const
{
    auto path = fs::path(workspace_root_) / relative_path;
    return path.string();
}

void GitStatusProvider::RunGitStatus()
{
    std::string cmd = "git -C \"" + workspace_root_ + "\" status --porcelain -z 2>/dev/null";

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
        return;

    std::vector<char> buffer(4096);
    std::string output;

    size_t bytes_read = 0;
    while ((bytes_read = fread(buffer.data(), 1, buffer.size(), pipe.get())) > 0)
    {
        output.append(buffer.data(), bytes_read);
    }

    std::unordered_map<std::string, GitFileStatus> new_statuses;

    size_t pos = 0;
    while (pos < output.size())
    {
        if (pos + 2 >= output.size())
            break;

        char x = output[pos];
        char y = output[pos + 1];

        size_t path_start = pos + 3;
        size_t path_end = output.find('\0', path_start);

        if (path_end == std::string::npos)
            break;

        std::string rel_path = output.substr(path_start, path_end - path_start);
        std::string abs_path = ProcessGitPath(rel_path);

        GitFileStatus status = GitFileStatus::None;

        if (x == '?' && y == '?')
            status = GitFileStatus::Untracked;
        else if (x == '!' && y == '!')
            status = GitFileStatus::Ignored;
        else if (x == 'M' || y == 'M')
            status = GitFileStatus::Modified;
        else if (x == 'A' || y == 'A')
            status = GitFileStatus::Added;
        else if (x == 'D' || y == 'D')
            status = GitFileStatus::Deleted;
        else if (x == 'R' || y == 'R')
            status = GitFileStatus::Renamed;
        else if (x == 'C' || y == 'C')
            status = GitFileStatus::Copied;
        else
            status = GitFileStatus::Modified;

        new_statuses[abs_path] = status;

        pos = path_end + 1; // Move past null byte
    }

    {
        std::lock_guard<std::mutex> lock(status_mutex_);
        file_statuses_ = std::move(new_statuses);
    }

    if (on_status_changed_)
    {
        // Must be called on main thread, but RunGitStatus is background thread.
        // We'll use CallAfter or a custom event to notify the UI.
        this->CallAfter(on_status_changed_);
    }
}

} // namespace markamp::ui
