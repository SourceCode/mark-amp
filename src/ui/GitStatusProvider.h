#pragma once

#include "IFileTreeDecorationProvider.h"
#include "core/Config.h"
#include "core/GitCommandRunner.h"
#include "core/ThemeEngine.h"

#include <wx/event.h>
#include <wx/timer.h>

#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::ui
{

class GitStatusProvider : public wxEvtHandler, public IFileTreeDecorationProvider
{
public:
    GitStatusProvider();
    ~GitStatusProvider() override;

    void SetWorkspaceRoot(const std::string& root_path);

    // Legacy API mappings
    [[nodiscard]] core::GitChangeStatus GetFileStatus(const std::string& absolute_path) const;
    [[nodiscard]] size_t GetModifiedCountInDirectory(const std::string& absolute_dir_path) const;

    // Rich API for source control
    [[nodiscard]] std::vector<core::GitChangeEntry> GetChanges() const;

    // Run a git status check
    void RunGitStatus();

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

    [[nodiscard]] std::string BuildAbsolutePath(const std::string& relative_path) const;
    static core::GitChangeStatus ParseStatusCode(char code);

    std::string workspace_root_;

    // Synchronization for concurrent access by UI / Timer threads
    mutable std::mutex status_mutex_;
    std::vector<core::GitChangeEntry> cached_changes_;
    // Fast path for directory lookup / file lookup
    std::unordered_map<std::string, core::GitChangeStatus> file_status_map_;

    std::function<void()> on_status_changed_;

    wxTimer refresh_timer_;
    std::atomic<bool> is_running_{false};

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
