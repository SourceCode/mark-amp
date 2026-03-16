#include "CanvasIntegrationService.h"

#include "core/Events.h"
#include "core/Logger.h"

namespace markamp::core
{

CanvasIntegrationService::CanvasIntegrationService(EventBus& event_bus, ExtensionSandbox& sandbox)
    : event_bus_(event_bus)
    , sandbox_(sandbox)
{
}

auto CanvasIntegrationService::register_app(const std::string& extension_id,
                                            const CanvasAppManifest& manifest) -> std::string
{
    auto app_id = "canvas_app_" + std::to_string(next_app_id_++);

    RegisteredCanvasApp app;
    app.app_id = app_id;
    app.extension_id = extension_id;
    app.manifest = manifest;
    app.is_active = true;

    apps_[app_id] = std::move(app);

    events::CanvasAppRegisteredEvent evt;
    evt.app_id = app_id;
    // Derive a display name from the manifest's contribution labels.
    std::string resolved_name = app_id;
    if (!manifest.widgets.empty() && !manifest.widgets.front().label.empty())
    {
        resolved_name = manifest.widgets.front().label;
    }
    else if (!manifest.tools.empty() && !manifest.tools.front().label.empty())
    {
        resolved_name = manifest.tools.front().label;
    }
    evt.app_name = resolved_name;
    evt.extension_id = extension_id;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Registered canvas app '{}' from extension '{}'", app_id, extension_id);

    return app_id;
}

auto CanvasIntegrationService::unregister_app(const std::string& app_id) -> void
{
    apps_.erase(app_id);
    MARKAMP_LOG_INFO("Unregistered canvas app '{}'", app_id);
}

auto CanvasIntegrationService::find_app(const std::string& app_id) const
    -> const RegisteredCanvasApp*
{
    auto found = apps_.find(app_id);
    if (found == apps_.end())
    {
        return nullptr;
    }
    return &found->second;
}

auto CanvasIntegrationService::registered_apps() const
    -> const std::unordered_map<std::string, RegisteredCanvasApp>&
{
    return apps_;
}

auto CanvasIntegrationService::sync_widget(const std::string& widget_id,
                                           const std::string& provider_id) -> std::string
{
    auto job_id = "sync_job_" + std::to_string(next_job_id_++);

    SyncJob job;
    job.job_id = job_id;
    job.widget_id = widget_id;
    job.provider_id = provider_id;
    job.status = SyncJobStatus::kRunning;

    sync_jobs_[job_id] = std::move(job);

    events::CanvasWidgetSyncStartedEvent evt;
    evt.widget_id = widget_id;
    evt.provider_id = provider_id;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Started sync job '{}' for widget '{}'", job_id, widget_id);

    return job_id;
}

auto CanvasIntegrationService::sync_all() -> std::vector<std::string>
{
    // In a real implementation, iterate active widgets and start sync jobs.
    // For now, return empty — widgets would be queried from the Board.
    return {};
}

auto CanvasIntegrationService::get_sync_job(const std::string& job_id) const -> const SyncJob*
{
    auto found = sync_jobs_.find(job_id);
    if (found == sync_jobs_.end())
    {
        return nullptr;
    }
    return &found->second;
}

auto CanvasIntegrationService::complete_sync(const std::string& job_id, int items_synced) -> void
{
    auto found = sync_jobs_.find(job_id);
    if (found == sync_jobs_.end())
    {
        return;
    }

    found->second.status = SyncJobStatus::kCompleted;
    found->second.items_synced = items_synced;

    events::CanvasWidgetSyncCompletedEvent evt;
    evt.widget_id = found->second.widget_id;
    evt.provider_id = found->second.provider_id;
    evt.items_synced = items_synced;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Sync job '{}' completed ({} items)", job_id, items_synced);
}

auto CanvasIntegrationService::fail_sync(const std::string& job_id,
                                         const std::string& error_message) -> void
{
    auto found = sync_jobs_.find(job_id);
    if (found == sync_jobs_.end())
    {
        return;
    }

    found->second.status = SyncJobStatus::kFailed;
    found->second.error_message = error_message;

    events::CanvasWidgetSyncFailedEvent evt;
    evt.widget_id = found->second.widget_id;
    evt.provider_id = found->second.provider_id;
    evt.error_message = error_message;
    event_bus_.publish(evt);

    MARKAMP_LOG_WARN("Sync job '{}' failed: {}", job_id, error_message);
}

auto CanvasIntegrationService::check_permission(const std::string& extension_id,
                                                ExtensionPermission scope) const -> bool
{
    return sandbox_.has_permission(extension_id, scope);
}

auto CanvasIntegrationService::app_count() const -> size_t
{
    return apps_.size();
}

auto CanvasIntegrationService::active_sync_count() const -> size_t
{
    size_t count = 0;
    for (const auto& [job_id, job] : sync_jobs_)
    {
        if (job.status == SyncJobStatus::kRunning || job.status == SyncJobStatus::kRetrying)
        {
            ++count;
        }
    }
    return count;
}

} // namespace markamp::core
