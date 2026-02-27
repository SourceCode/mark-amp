#include "ui/GitStatusProvider.h"

#include <wx/app.h>
#include <wx/log.h>

#include <filesystem>
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
    // Auto-refresh every 5 seconds for status polling.
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
    cached_changes_.clear();
    file_status_map_.clear();

    if (!workspace_root_.empty())
    {
        Refresh();
    }
}

core::GitChangeStatus GitStatusProvider::GetFileStatus(const std::string& absolute_path) const
{
    std::lock_guard<std::mutex> lock(status_mutex_);
    auto it = file_status_map_.find(absolute_path);
    return it != file_status_map_.end() ? it->second : core::GitChangeStatus::None;
}

std::vector<core::GitChangeEntry> GitStatusProvider::GetChanges() const
{
    std::lock_guard<std::mutex> lock(status_mutex_);
    return cached_changes_;
}

auto GitStatusProvider::ProvideDecoration(const core::FileNode& node) const
    -> std::optional<FileTreeDecoration>
{
    core::GitChangeStatus status = GetFileStatus(node.id);
    if (status == core::GitChangeStatus::None)
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

    if (status == core::GitChangeStatus::Untracked || status == core::GitChangeStatus::Added)
    {
        dec.badge_text = "U";
        dec.badge_color = wxColour(115, 201, 145);
        dec.text_color = wxColour(115, 201, 145);
    }
    else if (status == core::GitChangeStatus::Deleted)
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
    for (const auto& [path, status] : file_status_map_)
    {
        if (status != core::GitChangeStatus::None)
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

std::string GitStatusProvider::BuildAbsolutePath(const std::string& relative_path) const
{
    auto path = fs::path(workspace_root_) / relative_path;
    return path.string();
}

core::GitChangeStatus GitStatusProvider::ParseStatusCode(char code)
{
    switch (code)
    {
        case 'M':
            return core::GitChangeStatus::Modified;
        case 'A':
            return core::GitChangeStatus::Added;
        case 'D':
            return core::GitChangeStatus::Deleted;
        case 'R':
            return core::GitChangeStatus::Renamed;
        case 'C':
            return core::GitChangeStatus::Copied;
        case 'U':
            return core::GitChangeStatus::Unmerged;
        case '?':
            return core::GitChangeStatus::Untracked;
        default:
            return core::GitChangeStatus::None;
    }
}

void GitStatusProvider::RunGitStatus()
{
    core::GitCommandRunner runner{workspace_root_};

    // v2 porcelain with -z emits machine readable null-terminated strings
    auto result = runner.RunSync("git status --porcelain=v2 -z");
    if (!result.success() || result.stdout_text.empty())
    {
        return;
    }

    std::vector<core::GitChangeEntry> new_entries;
    std::unordered_map<std::string, core::GitChangeStatus> new_map;

    const std::string& output = result.stdout_text;
    size_t pos = 0;

    // v2 porcelain lines start with '1' (ordinary), '2' (renames), 'u' (unmerged), '?' (untracked)
    // Detailed docs: https://git-scm.com/docs/git-status#_porcelain_format_version_2

    while (pos < output.size())
    {
        size_t next_null = output.find('\0', pos);
        if (next_null == std::string::npos)
            break;

        std::string line = output.substr(pos, next_null - pos);
        if (line.empty())
        {
            pos = next_null + 1;
            continue;
        }

        std::istringstream stream(line);
        std::string type, codes;
        stream >> type;

        core::GitChangeEntry entry;
        std::string rel_path;

        if (type == "1") // Ordinary (1 <XY> <sub> <mH> <mI> <mW> <hH> <hI> <path>)
        {
            stream >> codes; // XY
            entry.index_status = ParseStatusCode(codes[0]);
            entry.working_status = ParseStatusCode(codes[1]);

            // Extract path (remainder of the string after fixed metadata width)
            // Example: 1 .M N... 100644 100644 100644 cccd... cccd... src/ui/TabBar.cpp

            // Fast leap to the paths by jumping past 8 spaces
            int spaces_seen = 0;
            size_t token_cursor = 0;
            while (spaces_seen < 8 && token_cursor < line.length())
            {
                if (line[token_cursor] == ' ')
                    spaces_seen++;
                token_cursor++;
            }
            rel_path = line.substr(token_cursor);
        }
        else if (type == "2") // Rename (2 <XY> <sub> <mH> <mI> <mW> <hH> <hI> <X><score>
                              // <path><sep><origPath>)
        {
            stream >> codes; // XY
            entry.index_status = ParseStatusCode(codes[0]);
            entry.working_status = ParseStatusCode(codes[1]);

            int spaces_seen = 0;
            size_t token_cursor = 0;
            while (spaces_seen < 9 && token_cursor < line.length())
            {
                if (line[token_cursor] == ' ')
                    spaces_seen++;
                token_cursor++;
            }
            rel_path = line.substr(token_cursor); // Extract to end

            // v2 with -z outputs the <path> null terminated, followed by <original_path> null
            // terminated
            pos = next_null + 1; // move past path
            size_t orig_null = output.find('\0', pos);
            if (orig_null != std::string::npos)
            {
                entry.original_path = BuildAbsolutePath(output.substr(pos, orig_null - pos));
                next_null = orig_null; // Advance outer loop's horizon
            }
        }
        else if (type == "u") // Unmerged
        {
            stream >> codes;
            entry.index_status = ParseStatusCode(codes[0]);
            entry.working_status = ParseStatusCode(codes[1]);
            // Format: u <xy> <sub> <m1> <m2> <m3> <mW> <h1> <h2> <h3> <path>
            int spaces_seen = 0;
            size_t token_cursor = 0;
            while (spaces_seen < 10 && token_cursor < line.length())
            {
                if (line[token_cursor] == ' ')
                    spaces_seen++;
                token_cursor++;
            }
            rel_path = line.substr(token_cursor);
        }
        else if (type == "?") // Untracked (? <path>)
        {
            entry.working_status = core::GitChangeStatus::Untracked;
            rel_path = line.substr(2); // Skip "? "
        }
        else
        {
            // Ignore (e.g., '!')
            pos = next_null + 1;
            continue;
        }

        entry.path = BuildAbsolutePath(rel_path);
        new_entries.push_back(entry);

        // Overall logic mapped to simpler file_status_map_ enum for fast UI decoration matching
        core::GitChangeStatus overall = entry.working_status != core::GitChangeStatus::None
                                            ? entry.working_status
                                            : entry.index_status;

        new_map[entry.path] = overall;

        pos = next_null + 1;
    }

    // Now fetch diff stats
    auto parse_numstat = [&](const std::string& cmd, bool is_staged)
    {
        auto stat_res = runner.RunSync(cmd);
        if (stat_res.success() && !stat_res.stdout_text.empty())
        {
            std::istringstream stream(stat_res.stdout_text);
            std::string line;
            while (std::getline(stream, line))
            {
                if (line.empty())
                    continue;
                std::istringstream ls(line);
                std::string add_str, del_str, rel_path;
                ls >> add_str >> del_str;
                std::getline(ls, rel_path);

                // Trim leading whitespace
                size_t first = rel_path.find_first_not_of(" \t");
                if (first != std::string::npos)
                    rel_path = rel_path.substr(first);

                // Handle binary files which report '-'
                int adds = (add_str != "-") ? std::stoi(add_str) : 0;
                int dels = (del_str != "-") ? std::stoi(del_str) : 0;
                std::string full_path = BuildAbsolutePath(rel_path);

                for (auto& entry : new_entries)
                {
                    if (entry.path == full_path)
                    {
                        if (is_staged)
                        {
                            entry.staged_additions = adds;
                            entry.staged_deletions = dels;
                        }
                        else
                        {
                            entry.unstaged_additions = adds;
                            entry.unstaged_deletions = dels;
                        }
                        break;
                    }
                }
            }
        }
    };

    parse_numstat("git diff --numstat", false);
    parse_numstat("git diff --cached --numstat", true);

    {
        std::lock_guard<std::mutex> lock(status_mutex_);
        cached_changes_ = std::move(new_entries);
        file_status_map_ = std::move(new_map);
    }

    if (on_status_changed_)
    {
        // Safe UI thread callback
        if (wxTheApp)
        {
            wxTheApp->CallAfter(on_status_changed_);
        }
    }
}

} // namespace markamp::ui
