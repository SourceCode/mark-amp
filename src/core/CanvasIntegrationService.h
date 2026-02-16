#pragma once

/// @file CanvasIntegrationService.h
/// @brief V8 Phase 8: Canvas integration orchestration.
///
/// Manages provider connections, sync jobs, rate limits, retry strategy,
/// and status emission to UI via EventBus.

#include "core/CanvasAppManifest.h"
#include "core/EventBus.h"
#include "core/ExtensionSandbox.h"

#include <chrono>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Registered canvas app descriptor.
struct RegisteredCanvasApp
{
    std::string app_id;
    std::string extension_id;
    CanvasAppManifest manifest;
    bool is_active{true};
};

/// Status of a widget sync job.
enum class SyncJobStatus
{
    kPending,
    kRunning,
    kCompleted,
    kFailed,
    kRetrying
};

/// Describes an active or completed sync job.
struct SyncJob
{
    std::string job_id;
    std::string widget_id;
    std::string provider_id;
    SyncJobStatus status{SyncJobStatus::kPending};
    std::string error_message;
    int retry_count{0};
    int max_retries{3};
    int items_synced{0};
};

/// Orchestrates canvas app registration, widget sync, and permission checks.
class CanvasIntegrationService
{
public:
    explicit CanvasIntegrationService(EventBus& event_bus, ExtensionSandbox& sandbox);

    // ── App registration ──────────────────────────────────────────

    /// Register a canvas app from its manifest.
    auto register_app(const std::string& extension_id, const CanvasAppManifest& manifest)
        -> std::string;

    /// Unregister an app by ID.
    auto unregister_app(const std::string& app_id) -> void;

    /// Find a registered app by ID.
    [[nodiscard]] auto find_app(const std::string& app_id) const -> const RegisteredCanvasApp*;

    /// List all registered apps.
    [[nodiscard]] auto registered_apps() const
        -> const std::unordered_map<std::string, RegisteredCanvasApp>&;

    // ── Widget sync ───────────────────────────────────────────────

    /// Start a sync job for a specific widget.
    auto sync_widget(const std::string& widget_id, const std::string& provider_id) -> std::string;

    /// Sync all active widgets.
    auto sync_all() -> std::vector<std::string>;

    /// Get the status of a sync job.
    [[nodiscard]] auto get_sync_job(const std::string& job_id) const -> const SyncJob*;

    /// Mark a sync job as completed with results.
    auto complete_sync(const std::string& job_id, int items_synced) -> void;

    /// Mark a sync job as failed.
    auto fail_sync(const std::string& job_id, const std::string& error_message) -> void;

    // ── Permission checks ─────────────────────────────────────────

    /// Check if an extension has the required canvas permissions.
    [[nodiscard]] auto check_permission(const std::string& extension_id,
                                        ExtensionPermission scope) const -> bool;

    // ── Queries ───────────────────────────────────────────────────

    [[nodiscard]] auto app_count() const -> size_t;
    [[nodiscard]] auto active_sync_count() const -> size_t;

private:
    EventBus& event_bus_;
    ExtensionSandbox& sandbox_;
    std::unordered_map<std::string, RegisteredCanvasApp> apps_;
    std::unordered_map<std::string, SyncJob> sync_jobs_;
    int next_app_id_{1};
    int next_job_id_{1};
};

} // namespace markamp::core
