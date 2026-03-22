#pragma once

#include "IFileTreeDecorationProvider.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/GitCommandRunner.h"
#include "core/ThemeEngine.h"

#include <wx/event.h>
#include <wx/timer.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace markamp::ui
{

class GitStatusProvider : public wxEvtHandler, public IFileTreeDecorationProvider
{
public:
    GitStatusProvider();
    ~GitStatusProvider() override;

    // Non-copyable, non-movable
    GitStatusProvider(const GitStatusProvider&) = delete;
    auto operator=(const GitStatusProvider&) -> GitStatusProvider& = delete;
    GitStatusProvider(GitStatusProvider&&) = delete;
    auto operator=(GitStatusProvider&&) -> GitStatusProvider& = delete;

    void SetWorkspaceRoot(const std::string& root_path);

    // Set the event bus for publishing global Git events
    void SetEventBus(core::EventBus* event_bus);

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

    /// Stop the background worker thread and wait for it.
    void StopWorker();

    [[nodiscard]] std::string BuildAbsolutePath(const std::string& relative_path) const;
    static core::GitChangeStatus ParseStatusCode(char code);

    std::string workspace_root_;

    // Synchronization for concurrent access by UI / Timer threads
    mutable std::mutex status_mutex_;
    std::vector<core::GitChangeEntry> cached_changes_;
    // Fast path for directory lookup / file lookup
    std::unordered_map<std::string, core::GitChangeStatus> file_status_map_;

    std::function<void()> on_status_changed_;
    core::EventBus* event_bus_{nullptr};

    wxTimer refresh_timer_;

    // Joinable worker thread with proper lifecycle
    std::thread worker_thread_;
    std::mutex worker_mutex_;
    std::condition_variable worker_cv_;
    std::atomic<bool> refresh_requested_{false};
    std::atomic<bool> stop_requested_{false};
    std::atomic<bool> is_running_{false};

    /// Background worker loop — waits for refresh requests.
    void WorkerLoop();

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
