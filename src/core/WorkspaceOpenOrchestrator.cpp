/// @file WorkspaceOpenOrchestrator.cpp
/// @brief P03-T05: Single entry point for all workspace-open paths.

#include "WorkspaceOpenOrchestrator.h"

#include "Config.h"
#include "EventBus.h"
#include "Events.h"
#include "Logger.h"
#include "RecentWorkspaces.h"
#include "WorkspaceSessionRestore.h"

#include <filesystem>

namespace markamp::core
{

WorkspaceOpenOrchestrator::WorkspaceOpenOrchestrator(EventBus& bus,
                                                     Config& cfg,
                                                     RecentWorkspaces& recents,
                                                     WorkspaceSessionRestore& session_restore)
    : event_bus_(bus)
    , config_(cfg)
    , recents_(recents)
    , session_restore_(session_restore)
{
}

auto WorkspaceOpenOrchestrator::open_workspace(const std::string& folder_path) -> bool
{
    if (opening_)
    {
        MARKAMP_LOG_WARN("Workspace open already in progress, ignoring: {}", folder_path);
        event_bus_.publish(events::NotificationEvent{
            "A workspace is already being opened.",
            events::NotificationLevel::Warning,
            2000,
        });
        return false;
    }

    // 1. Validate path
    std::error_code err_code;
    if (!std::filesystem::is_directory(folder_path, err_code))
    {
        MARKAMP_LOG_WARN("Invalid workspace path: {} ({})", folder_path, err_code.message());
        event_bus_.publish(events::NotificationEvent{
            "Cannot open workspace: path is not a valid directory.",
            events::NotificationLevel::Error,
            3000,
        });
        return false;
    }

    opening_ = true;
    current_workspace_ = folder_path;

    MARKAMP_LOG_INFO("Opening workspace: {}", folder_path);

    // 2. Publish loading notification
    event_bus_.publish(events::NotificationEvent{
        "Opening workspace\u2026",
        events::NotificationLevel::Info,
        1500,
    });

    // 3. Update recents
    recents_.add(folder_path);

    // 4. Save workspace path to config
    config_.set("last_workspace", folder_path);

    // 5. Publish workspace open event for MainFrame/LayoutManager to handle
    events::WorkspaceRefreshRequestEvent open_evt;
    event_bus_.publish(open_evt);

    // 6. Check for session restore
    const auto* latest = session_restore_.latest_snapshot(folder_path);
    if (latest != nullptr && session_restore_.get_policy().auto_restore)
    {
        MARKAMP_LOG_INFO("Restoring session for workspace: {} (snapshot: {})",
                         folder_path,
                         latest->snapshot_id);
        session_restore_.restore_snapshot(latest->snapshot_id);

        // Publish events to restore open files
        for (const auto& file : latest->open_files)
        {
            events::ActiveFileChangedEvent file_evt;
            file_evt.file_id = file;
            event_bus_.publish(file_evt);
        }

        // Restore active file last
        if (!latest->active_file.empty())
        {
            events::ActiveFileChangedEvent active_evt;
            active_evt.file_id = latest->active_file;
            event_bus_.publish(active_evt);
        }
    }

    opening_ = false;

    event_bus_.publish(events::NotificationEvent{
        "Workspace opened successfully.",
        events::NotificationLevel::Success,
        2000,
    });

    MARKAMP_LOG_INFO("Workspace opened: {}", folder_path);
    return true;
}

} // namespace markamp::core
