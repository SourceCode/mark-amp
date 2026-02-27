#pragma once

#include "IFileTreeDecorationProvider.h"

#include <wx/event.h>
#include <wx/timer.h>

#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>

namespace markamp::ui
{

enum class GitFileStatus
{
    None,
    Modified,
    Added,
    Deleted,
    Untracked,
    Renamed,
    Copied,
    Ignored
};

class GitStatusProvider : public wxEvtHandler, public IFileTreeDecorationProvider
{
public:
    GitStatusProvider();
    ~GitStatusProvider() override;

    void SetWorkspaceRoot(const std::string& root_path);
    [[nodiscard]] GitFileStatus GetFileStatus(const std::string& absolute_path) const;
    [[nodiscard]] size_t GetModifiedCountInDirectory(const std::string& absolute_dir_path) const;

    void Refresh();

    // IFileTreeDecorationProvider
    [[nodiscard]] auto ProvideDecoration(const core::FileNode& node) const
        -> std::optional<FileTreeDecoration> override;

    void SetRefreshCallback(std::function<void()> cb)
    {
        on_status_changed_ = std::move(cb);
    }

private:
    void OnTimer(wxTimerEvent& event);
    void RunGitStatus();

    [[nodiscard]] std::string ProcessGitPath(const std::string& relative_path) const;

    std::string workspace_root_;
    mutable std::mutex status_mutex_;
    std::unordered_map<std::string, GitFileStatus> file_statuses_;

    std::function<void()> on_status_changed_;

    wxTimer refresh_timer_;
    std::atomic<bool> is_running_{false};

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
