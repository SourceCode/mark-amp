// ============================================================================
// File: tests/unit/test_phase27_cloud_sync.cpp
// Phase 27: Cloud Sync & Collaboration — Comprehensive test suite
// ============================================================================

#include "core/CloudSyncTypes.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/SelectiveSyncFilter.h"
#include "core/SyncCommandProvider.h"
#include "core/SyncConflictManager.h"
#include "core/SyncEngine.h"
#include "core/SyncHistoryLogger.h"
#include "core/SyncScheduler.h"
#include "core/SyncTypes.h"
#include "core/WorkspaceSharing.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// CloudSyncTypes — Extended types tests
// ============================================================================

TEST_CASE("CloudSyncTypes: SyncScheduleInterval values", "[phase27][types]")
{
    REQUIRE(static_cast<int>(SyncScheduleInterval::kManual) == 0);
    REQUIRE(static_cast<int>(SyncScheduleInterval::k5Min) == 1);
    REQUIRE(static_cast<int>(SyncScheduleInterval::k15Min) == 2);
    REQUIRE(static_cast<int>(SyncScheduleInterval::k30Min) == 3);
    REQUIRE(static_cast<int>(SyncScheduleInterval::k60Min) == 4);
}

TEST_CASE("CloudSyncTypes: SyncIgnorePatterns default_patterns", "[phase27][types]")
{
    auto defaults = SyncIgnorePatterns::default_patterns();
    REQUIRE(defaults.size() >= 8);

    // Check some expected defaults.
    bool has_ds_store = false;
    bool has_tmp = false;
    bool has_git = false;
    for (const auto& pattern : defaults)
    {
        if (pattern == ".DS_Store")
            has_ds_store = true;
        if (pattern == "*.tmp")
            has_tmp = true;
        if (pattern == ".git/")
            has_git = true;
    }
    REQUIRE(has_ds_store);
    REQUIRE(has_tmp);
    REQUIRE(has_git);
}

TEST_CASE("CloudSyncTypes: DeviceInfo defaults", "[phase27][types]")
{
    DeviceInfo info;
    REQUIRE(info.device_id.empty());
    REQUIRE(info.device_name.empty());
    REQUIRE(info.last_sync_at == 0);
    REQUIRE_FALSE(info.is_current);
    REQUIRE(info.platform.empty());
}

TEST_CASE("CloudSyncTypes: BandwidthConfig defaults", "[phase27][types]")
{
    BandwidthConfig config;
    REQUIRE(config.upload_limit_kbps == 0);
    REQUIRE(config.download_limit_kbps == 0);
    REQUIRE_FALSE(config.pause_on_metered);
    REQUIRE(config.prioritize_small_files);
}

TEST_CASE("CloudSyncTypes: SyncNotification defaults", "[phase27][types]")
{
    SyncNotification notif;
    REQUIRE(notif.type == SyncNotificationType::kSyncComplete);
    REQUIRE(notif.message.empty());
    REQUIRE(notif.file_count == 0);
    REQUIRE_FALSE(notif.has_retry_action);
}

TEST_CASE("CloudSyncTypes: WorkspaceShareInfo defaults", "[phase27][types]")
{
    WorkspaceShareInfo info;
    REQUIRE(info.share_id.empty());
    REQUIRE(info.permission == SharePermission::kReadOnly);
    REQUIRE(info.shared_with.empty());
    REQUIRE(info.is_active);
}

TEST_CASE("CloudSyncTypes: SyncHistoryEntry defaults", "[phase27][types]")
{
    SyncHistoryEntry entry;
    REQUIRE(entry.entry_id.empty());
    REQUIRE(entry.timestamp == 0);
    REQUIRE(entry.operation.empty());
    REQUIRE(entry.files_uploaded == 0);
    REQUIRE(entry.files_downloaded == 0);
    REQUIRE(entry.status == CloudSyncStatus::Completed);
}

// ============================================================================
// SyncScheduler tests
// ============================================================================

TEST_CASE("SyncScheduler: construction with default interval", "[phase27][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);

    REQUIRE(scheduler.interval() == SyncScheduleInterval::k30Min);
    REQUIRE(scheduler.interval_minutes() == 30);
    REQUIRE_FALSE(scheduler.is_paused());
    REQUIRE(scheduler.is_online());
}

TEST_CASE("SyncScheduler: set interval", "[phase27][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);

    scheduler.set_interval(SyncScheduleInterval::k5Min);
    REQUIRE(scheduler.interval() == SyncScheduleInterval::k5Min);
    REQUIRE(scheduler.interval_minutes() == 5);

    scheduler.set_interval(SyncScheduleInterval::k15Min);
    REQUIRE(scheduler.interval_minutes() == 15);

    scheduler.set_interval(SyncScheduleInterval::k60Min);
    REQUIRE(scheduler.interval_minutes() == 60);

    scheduler.set_interval(SyncScheduleInterval::kManual);
    REQUIRE(scheduler.interval_minutes() == 0);
}

TEST_CASE("SyncScheduler: pause and resume", "[phase27][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);

    REQUIRE_FALSE(scheduler.is_paused());

    scheduler.pause();
    REQUIRE(scheduler.is_paused());

    scheduler.resume();
    REQUIRE_FALSE(scheduler.is_paused());
}

TEST_CASE("SyncScheduler: online/offline state", "[phase27][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);

    REQUIRE(scheduler.is_online());

    scheduler.set_online(false);
    REQUIRE_FALSE(scheduler.is_online());

    scheduler.set_online(true);
    REQUIRE(scheduler.is_online());
}

TEST_CASE("SyncScheduler: offline queue management", "[phase27][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);

    scheduler.set_online(false);

    REQUIRE(scheduler.pending_count() == 0);

    scheduler.queue_offline_change("docs/readme.md");
    REQUIRE(scheduler.pending_count() == 1);

    scheduler.queue_offline_change("docs/notes.md");
    REQUIRE(scheduler.pending_count() == 2);

    // Duplicate should not increase count.
    scheduler.queue_offline_change("docs/readme.md");
    REQUIRE(scheduler.pending_count() == 2);

    auto files = scheduler.pending_files();
    REQUIRE(files.size() == 2);

    scheduler.clear_offline_queue();
    REQUIRE(scheduler.pending_count() == 0);
}

TEST_CASE("SyncScheduler: should_sync timing", "[phase27][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);

    // Not syncing without cloud config.
    REQUIRE_FALSE(scheduler.should_sync(1000));

    // Set up cloud config.
    CloudSyncConfig cloud_config;
    scheduler.set_cloud_config(cloud_config);
    REQUIRE(scheduler.has_cloud_config());

    // Should not sync when paused.
    scheduler.pause();
    REQUIRE_FALSE(scheduler.should_sync(1000));
    scheduler.resume();

    // Should not sync when manual.
    scheduler.set_interval(SyncScheduleInterval::kManual);
    REQUIRE_FALSE(scheduler.should_sync(1000));

    // Set interval and last sync time.
    scheduler.set_interval(SyncScheduleInterval::k5Min);
    scheduler.set_last_sync_time(100);

    // Before next sync time (100 + 300 = 400).
    REQUIRE_FALSE(scheduler.should_sync(200));

    // At or after next sync time.
    REQUIRE(scheduler.should_sync(400));
    REQUIRE(scheduler.should_sync(500));
}

TEST_CASE("SyncScheduler: next_sync_time calculation", "[phase27][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);

    scheduler.set_last_sync_time(1000);
    scheduler.set_interval(SyncScheduleInterval::k30Min);
    REQUIRE(scheduler.next_sync_time() == 1000 + 30 * 60);

    scheduler.set_interval(SyncScheduleInterval::kManual);
    REQUIRE(scheduler.next_sync_time() == 0);

    scheduler.set_interval(SyncScheduleInterval::k5Min);
    scheduler.pause();
    REQUIRE(scheduler.next_sync_time() == 0);
}

TEST_CASE("SyncScheduler: save debounce config", "[phase27][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);

    REQUIRE(scheduler.save_debounce_ms() == 30000);

    scheduler.set_save_debounce_ms(5000);
    REQUIRE(scheduler.save_debounce_ms() == 5000);
}

TEST_CASE("SyncScheduler: sync_on_close config", "[phase27][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);

    REQUIRE(scheduler.sync_on_close_enabled());

    scheduler.set_sync_on_close_enabled(false);
    REQUIRE_FALSE(scheduler.sync_on_close_enabled());
}

TEST_CASE("SyncScheduler: bandwidth config", "[phase27][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);

    BandwidthConfig bw;
    bw.upload_limit_kbps = 1024;
    bw.download_limit_kbps = 2048;
    bw.pause_on_metered = true;

    scheduler.set_bandwidth_config(bw);
    REQUIRE(scheduler.bandwidth_config().upload_limit_kbps == 1024);
    REQUIRE(scheduler.bandwidth_config().download_limit_kbps == 2048);
    REQUIRE(scheduler.bandwidth_config().pause_on_metered);
}

// ============================================================================
// SyncConflictManager tests
// ============================================================================

TEST_CASE("SyncConflictManager: initial state", "[phase27][conflicts]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager mgr(bus, engine);

    REQUIRE_FALSE(mgr.has_conflicts());
    REQUIRE(mgr.pending_count() == 0);
    REQUIRE(mgr.resolution_count() == 0);
}

TEST_CASE("SyncConflictManager: set and query conflicts", "[phase27][conflicts]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager mgr(bus, engine);

    SyncConflict conflict1;
    conflict1.relative_path = "docs/readme.md";
    conflict1.local_change = FileChangeType::Modified;
    conflict1.remote_change = FileChangeType::Modified;

    SyncConflict conflict2;
    conflict2.relative_path = "docs/notes.md";

    mgr.set_pending_conflicts({conflict1, conflict2});

    REQUIRE(mgr.has_conflicts());
    REQUIRE(mgr.pending_count() == 2);

    auto found = mgr.find_conflict("docs/readme.md");
    REQUIRE(found.has_value());
    REQUIRE(found->relative_path == "docs/readme.md");

    auto not_found = mgr.find_conflict("nonexistent.md");
    REQUIRE_FALSE(not_found.has_value());
}

TEST_CASE("SyncConflictManager: resolve single conflict", "[phase27][conflicts]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager mgr(bus, engine);

    SyncConflict conflict;
    conflict.relative_path = "docs/readme.md";
    mgr.set_pending_conflicts({conflict});

    REQUIRE(mgr.resolve_conflict("docs/readme.md", SyncConflictResolution::KeepLocal));
    REQUIRE(mgr.pending_count() == 0);
    REQUIRE(mgr.resolution_count() == 1);

    auto history = mgr.resolution_history();
    REQUIRE(history.size() == 1);
    REQUIRE(history[0].file_path == "docs/readme.md");
    REQUIRE(history[0].kept_version == "local");
}

TEST_CASE("SyncConflictManager: resolve all conflicts", "[phase27][conflicts]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager mgr(bus, engine);

    SyncConflict c1, c2, c3;
    c1.relative_path = "a.md";
    c2.relative_path = "b.md";
    c3.relative_path = "c.md";

    mgr.set_pending_conflicts({c1, c2, c3});
    auto resolved = mgr.resolve_all(SyncConflictResolution::KeepRemote);

    REQUIRE(resolved == 3);
    REQUIRE(mgr.pending_count() == 0);
    REQUIRE(mgr.resolution_count() == 3);
}

TEST_CASE("SyncConflictManager: auto_resolve with default strategy", "[phase27][conflicts]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager mgr(bus, engine);

    REQUIRE(mgr.default_strategy() == SyncConflictResolution::KeepNewer);

    mgr.set_default_strategy(SyncConflictResolution::KeepBoth);
    REQUIRE(mgr.default_strategy() == SyncConflictResolution::KeepBoth);

    SyncConflict conflict;
    conflict.relative_path = "file.md";
    mgr.set_pending_conflicts({conflict});

    auto resolved = mgr.auto_resolve();
    REQUIRE(resolved == 1);

    REQUIRE(mgr.resolution_history()[0].kept_version == "both");
}

TEST_CASE("SyncConflictManager: resolve nonexistent conflict returns false", "[phase27][conflicts]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager mgr(bus, engine);

    REQUIRE_FALSE(mgr.resolve_conflict("nonexistent.md", SyncConflictResolution::KeepLocal));
}

TEST_CASE("SyncConflictManager: clear pending and history", "[phase27][conflicts]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager mgr(bus, engine);

    SyncConflict conflict;
    conflict.relative_path = "file.md";
    mgr.set_pending_conflicts({conflict});
    mgr.resolve_conflict("file.md", SyncConflictResolution::KeepLocal);

    mgr.clear_pending();
    REQUIRE(mgr.pending_count() == 0);

    REQUIRE(mgr.resolution_count() == 1);
    mgr.clear_history();
    REQUIRE(mgr.resolution_count() == 0);
}

TEST_CASE("SyncConflictManager: resolution history kept version strings", "[phase27][conflicts]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager mgr(bus, engine);

    // Test all resolution strategy strings.
    SyncConflict c1, c2, c3, c4;
    c1.relative_path = "a.md";
    c2.relative_path = "b.md";
    c3.relative_path = "c.md";
    c4.relative_path = "d.md";

    mgr.set_pending_conflicts({c1, c2, c3, c4});

    mgr.resolve_conflict("a.md", SyncConflictResolution::KeepLocal);
    mgr.resolve_conflict("b.md", SyncConflictResolution::KeepRemote);
    mgr.resolve_conflict("c.md", SyncConflictResolution::KeepBoth);
    mgr.resolve_conflict("d.md", SyncConflictResolution::KeepNewer);

    auto history = mgr.resolution_history();
    REQUIRE(history[0].kept_version == "local");
    REQUIRE(history[1].kept_version == "remote");
    REQUIRE(history[2].kept_version == "both");
    REQUIRE(history[3].kept_version == "newer");
}

// ============================================================================
// SelectiveSyncFilter tests
// ============================================================================

TEST_CASE("SelectiveSyncFilter: initial state", "[phase27][filter]")
{
    EventBus bus;
    Config config;
    SelectiveSyncFilter filter(config);

    REQUIRE(filter.pattern_count() == 0);
    REQUIRE(filter.use_defaults());
    REQUIRE(filter.excluded_folders().empty());
    REQUIRE(filter.cloud_only_files().empty());
}

TEST_CASE("SelectiveSyncFilter: add and remove patterns", "[phase27][filter]")
{
    EventBus bus;
    Config config;
    SelectiveSyncFilter filter(config);

    filter.add_pattern("*.bak");
    REQUIRE(filter.pattern_count() == 1);

    filter.add_pattern("*.log");
    REQUIRE(filter.pattern_count() == 2);

    // Duplicate should not increase.
    filter.add_pattern("*.bak");
    REQUIRE(filter.pattern_count() == 2);

    REQUIRE(filter.remove_pattern("*.bak"));
    REQUIRE(filter.pattern_count() == 1);

    REQUIRE_FALSE(filter.remove_pattern("nonexistent"));
}

TEST_CASE("SelectiveSyncFilter: should_sync with extension patterns", "[phase27][filter]")
{
    EventBus bus;
    Config config;
    SelectiveSyncFilter filter(config);
    filter.set_use_defaults(false);

    filter.add_pattern("*.tmp");
    filter.add_pattern("*.bak");

    REQUIRE_FALSE(filter.should_sync("docs/file.tmp"));
    REQUIRE_FALSE(filter.should_sync("file.bak"));
    REQUIRE(filter.should_sync("docs/readme.md"));
    REQUIRE(filter.should_sync("src/main.cpp"));
}

TEST_CASE("SelectiveSyncFilter: should_sync with exact filename patterns", "[phase27][filter]")
{
    EventBus bus;
    Config config;
    SelectiveSyncFilter filter(config);
    filter.set_use_defaults(false);

    filter.add_pattern(".DS_Store");

    REQUIRE_FALSE(filter.should_sync(".DS_Store"));
    REQUIRE_FALSE(filter.should_sync("docs/.DS_Store"));
    REQUIRE(filter.should_sync("readme.md"));
}

TEST_CASE("SelectiveSyncFilter: should_sync with directory patterns", "[phase27][filter]")
{
    EventBus bus;
    Config config;
    SelectiveSyncFilter filter(config);
    filter.set_use_defaults(false);

    filter.add_pattern("node_modules/");

    REQUIRE_FALSE(filter.should_sync("node_modules/package.json"));
    REQUIRE(filter.should_sync("src/main.cpp"));
}

TEST_CASE("SelectiveSyncFilter: negation patterns", "[phase27][filter]")
{
    EventBus bus;
    Config config;
    SelectiveSyncFilter filter(config);
    filter.set_use_defaults(false);

    filter.add_pattern("*.log");
    filter.add_pattern("!important.log");

    REQUIRE_FALSE(filter.should_sync("debug.log"));
    REQUIRE(filter.should_sync("important.log"));
}

TEST_CASE("SelectiveSyncFilter: folder exclusions", "[phase27][filter]")
{
    EventBus bus;
    Config config;
    SelectiveSyncFilter filter(config);
    filter.set_use_defaults(false);

    filter.exclude_folder("archive/");

    REQUIRE(filter.is_folder_excluded("archive/"));
    REQUIRE_FALSE(filter.should_sync("archive/old-notes.md"));
    REQUIRE(filter.should_sync("docs/readme.md"));

    filter.include_folder("archive/");
    REQUIRE_FALSE(filter.is_folder_excluded("archive/"));
}

TEST_CASE("SelectiveSyncFilter: cloud-only files", "[phase27][filter]")
{
    EventBus bus;
    Config config;
    SelectiveSyncFilter filter(config);

    filter.add_cloud_only_file("large-video.mp4");
    REQUIRE(filter.is_cloud_only("large-video.mp4"));
    REQUIRE_FALSE(filter.is_cloud_only("small-file.md"));

    REQUIRE(filter.remove_cloud_only_file("large-video.mp4"));
    REQUIRE_FALSE(filter.is_cloud_only("large-video.mp4"));
}

TEST_CASE("SelectiveSyncFilter: effective patterns with defaults", "[phase27][filter]")
{
    EventBus bus;
    Config config;
    SelectiveSyncFilter filter(config);

    filter.add_pattern("custom.pattern");

    auto effective = filter.effective_patterns();
    // Should include defaults + user patterns.
    auto defaults = SyncIgnorePatterns::default_patterns();
    REQUIRE(effective.size() == defaults.size() + 1);

    filter.set_use_defaults(false);
    effective = filter.effective_patterns();
    REQUIRE(effective.size() == 1);
}

TEST_CASE("SelectiveSyncFilter: load from syncignore content", "[phase27][filter]")
{
    EventBus bus;
    Config config;
    SelectiveSyncFilter filter(config);

    filter.load_patterns("# Comment line\n*.bak\n\n*.tmp\n  *.swp  \n");
    REQUIRE(filter.pattern_count() == 3);

    auto patterns = filter.patterns();
    REQUIRE(patterns[0] == "*.bak");
    REQUIRE(patterns[1] == "*.tmp");
    REQUIRE(patterns[2] == "*.swp");
}

TEST_CASE("SelectiveSyncFilter: serialize to syncignore format", "[phase27][filter]")
{
    EventBus bus;
    Config config;
    SelectiveSyncFilter filter(config);

    filter.add_pattern("*.bak");
    filter.add_pattern("*.tmp");

    auto serialized = filter.serialize();
    REQUIRE(serialized.find("*.bak") != std::string::npos);
    REQUIRE(serialized.find("*.tmp") != std::string::npos);
    REQUIRE(serialized.find("# .markamp-syncignore") != std::string::npos);
}

// ============================================================================
// SyncHistoryLogger tests
// ============================================================================

TEST_CASE("SyncHistoryLogger: initial state", "[phase27][history]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);

    REQUIRE(logger.is_empty());
    REQUIRE(logger.entry_count() == 0);
    REQUIRE(logger.max_entries() == 200);
    REQUIRE_FALSE(logger.latest().has_value());
}

TEST_CASE("SyncHistoryLogger: log sync result", "[phase27][history]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);

    CloudSyncResult result;
    result.files_uploaded = 5;
    result.files_downloaded = 3;
    result.elapsed_ms = 1500;

    logger.log_sync(result, "sync", "MacBook");

    REQUIRE(logger.entry_count() == 1);
    REQUIRE_FALSE(logger.is_empty());

    auto latest = logger.latest();
    REQUIRE(latest.has_value());
    REQUIRE(latest->operation == "sync");
    REQUIRE(latest->device_name == "MacBook");
    REQUIRE(latest->files_uploaded == 5);
    REQUIRE(latest->files_downloaded == 3);
}

TEST_CASE("SyncHistoryLogger: log custom entry", "[phase27][history]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);

    SyncHistoryEntry entry;
    entry.entry_id = "custom_1";
    entry.operation = "push";
    entry.files_uploaded = 10;

    logger.log_entry(entry);
    REQUIRE(logger.entry_count() == 1);
    REQUIRE(logger.latest()->entry_id == "custom_1");
}

TEST_CASE("SyncHistoryLogger: search by operation", "[phase27][history]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);

    CloudSyncResult sync_result;
    sync_result.files_uploaded = 1;

    CloudSyncResult push_result;
    push_result.files_uploaded = 2;

    logger.log_sync(sync_result, "sync");
    logger.log_sync(push_result, "push");
    logger.log_sync(sync_result, "sync");

    auto syncs = logger.search_by_operation("sync");
    REQUIRE(syncs.size() == 2);

    auto pushes = logger.search_by_operation("push");
    REQUIRE(pushes.size() == 1);
}

TEST_CASE("SyncHistoryLogger: error entries", "[phase27][history]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);

    CloudSyncResult good_result;
    good_result.files_uploaded = 1;

    CloudSyncResult bad_result;
    bad_result.errors = {"Connection timeout", "Auth failed"};

    logger.log_sync(good_result, "sync");
    logger.log_sync(bad_result, "sync");

    auto errors = logger.error_entries();
    REQUIRE(errors.size() == 1);
    REQUIRE(errors[0].errors.size() == 2);
}

TEST_CASE("SyncHistoryLogger: device entries", "[phase27][history]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);

    CloudSyncResult result;
    logger.log_sync(result, "sync", "MacBook");
    logger.log_sync(result, "sync", "iPad");
    logger.log_sync(result, "sync", "MacBook");

    auto mac_entries = logger.device_entries("MacBook");
    REQUIRE(mac_entries.size() == 2);

    auto ipad_entries = logger.device_entries("iPad");
    REQUIRE(ipad_entries.size() == 1);
}

TEST_CASE("SyncHistoryLogger: total_files_synced", "[phase27][history]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);

    CloudSyncResult r1;
    r1.files_uploaded = 5;
    r1.files_downloaded = 3;

    CloudSyncResult r2;
    r2.files_uploaded = 2;
    r2.files_downloaded = 1;

    logger.log_sync(r1, "sync");
    logger.log_sync(r2, "sync");

    REQUIRE(logger.total_files_synced() == 11); // 5+3+2+1
}

TEST_CASE("SyncHistoryLogger: max entries trimming", "[phase27][history]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);
    logger.set_max_entries(5);

    CloudSyncResult result;
    for (int i = 0; i < 10; ++i)
    {
        result.files_uploaded = i;
        logger.log_sync(result, "sync");
    }

    REQUIRE(logger.entry_count() == 5);
    // Should have the latest 5 entries.
    REQUIRE(logger.latest()->files_uploaded == 9);
}

TEST_CASE("SyncHistoryLogger: export JSON", "[phase27][history]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);

    CloudSyncResult result;
    result.files_uploaded = 1;
    logger.log_sync(result, "sync", "MacBook");

    auto json = logger.export_json();
    REQUIRE(json.find("MacBook") != std::string::npos);
    REQUIRE(json.find("sync") != std::string::npos);
    REQUIRE(json.find("files_uploaded") != std::string::npos);
}

TEST_CASE("SyncHistoryLogger: clear", "[phase27][history]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);

    CloudSyncResult result;
    logger.log_sync(result, "sync");
    logger.log_sync(result, "push");

    REQUIRE(logger.entry_count() == 2);
    logger.clear();
    REQUIRE(logger.is_empty());
}

// ============================================================================
// WorkspaceSharing tests
// ============================================================================

TEST_CASE("WorkspaceSharing: initial state", "[phase27][sharing]")
{
    EventBus bus;
    WorkspaceSharing sharing(bus);

    REQUIRE(sharing.share_count() == 0);
    REQUIRE(sharing.list_shares().empty());
}

TEST_CASE("WorkspaceSharing: create and get share", "[phase27][sharing]")
{
    EventBus bus;
    WorkspaceSharing sharing(bus);

    auto id = sharing.create_share("/workspace/notes", SharePermission::kReadOnly);
    REQUIRE_FALSE(id.empty());

    auto share = sharing.get_share(id);
    REQUIRE(share.has_value());
    REQUIRE(share->workspace_path == "/workspace/notes");
    REQUIRE(share->permission == SharePermission::kReadOnly);
    REQUIRE(share->is_active);
}

TEST_CASE("WorkspaceSharing: revoke share", "[phase27][sharing]")
{
    EventBus bus;
    WorkspaceSharing sharing(bus);

    auto id = sharing.create_share("/workspace/notes", SharePermission::kReadWrite);
    REQUIRE(sharing.share_count() == 1);

    REQUIRE(sharing.revoke_share(id));
    REQUIRE(sharing.share_count() == 0);

    REQUIRE_FALSE(sharing.revoke_share("nonexistent"));
}

TEST_CASE("WorkspaceSharing: update permission", "[phase27][sharing]")
{
    EventBus bus;
    WorkspaceSharing sharing(bus);

    auto id = sharing.create_share("/workspace/notes", SharePermission::kReadOnly);

    REQUIRE(sharing.update_permission(id, SharePermission::kReadWrite));
    auto share = sharing.get_share(id);
    REQUIRE(share->permission == SharePermission::kReadWrite);

    REQUIRE_FALSE(sharing.update_permission("nonexistent", SharePermission::kReadOnly));
}

TEST_CASE("WorkspaceSharing: add and remove participants", "[phase27][sharing]")
{
    EventBus bus;
    WorkspaceSharing sharing(bus);

    auto id = sharing.create_share("/workspace/notes", SharePermission::kReadWrite);

    REQUIRE(sharing.add_participant(id, "alice@example.com"));
    REQUIRE(sharing.add_participant(id, "bob@example.com"));

    auto share = sharing.get_share(id);
    REQUIRE(share->shared_with.size() == 2);

    // Duplicate participant should not increase count.
    sharing.add_participant(id, "alice@example.com");
    share = sharing.get_share(id);
    REQUIRE(share->shared_with.size() == 2);

    REQUIRE(sharing.remove_participant(id, "alice@example.com"));
    share = sharing.get_share(id);
    REQUIRE(share->shared_with.size() == 1);
}

TEST_CASE("WorkspaceSharing: is_shared check", "[phase27][sharing]")
{
    EventBus bus;
    WorkspaceSharing sharing(bus);

    REQUIRE_FALSE(sharing.is_shared("/workspace/notes"));

    sharing.create_share("/workspace/notes", SharePermission::kReadOnly);
    REQUIRE(sharing.is_shared("/workspace/notes"));
    REQUIRE_FALSE(sharing.is_shared("/workspace/other"));
}

TEST_CASE("WorkspaceSharing: shares_for_workspace", "[phase27][sharing]")
{
    EventBus bus;
    WorkspaceSharing sharing(bus);

    sharing.create_share("/workspace/notes", SharePermission::kReadOnly);
    sharing.create_share("/workspace/notes", SharePermission::kReadWrite);
    sharing.create_share("/workspace/other", SharePermission::kReadOnly);

    auto note_shares = sharing.shares_for_workspace("/workspace/notes");
    REQUIRE(note_shares.size() == 2);

    auto other_shares = sharing.shares_for_workspace("/workspace/other");
    REQUIRE(other_shares.size() == 1);
}

TEST_CASE("WorkspaceSharing: activity tracking", "[phase27][sharing]")
{
    EventBus bus;
    WorkspaceSharing sharing(bus);

    auto id = sharing.create_share("/workspace/notes", SharePermission::kReadWrite);

    ShareActivity activity;
    activity.participant = "alice@example.com";
    activity.action = "modified";
    activity.file_path = "docs/readme.md";
    activity.timestamp = 1000;

    sharing.record_activity(id, activity);

    auto activities = sharing.get_activities(id);
    REQUIRE(activities.size() == 1);

    auto recent = sharing.recent_activities(10);
    REQUIRE(recent.size() == 1);
    REQUIRE(recent[0].participant == "alice@example.com");
}

// ============================================================================
// SyncCommandProvider tests
// ============================================================================

TEST_CASE("SyncCommandProvider: has 10 commands", "[phase27][commands]")
{
    SyncCommandProvider provider;
    REQUIRE(provider.command_count() == 10);
}

TEST_CASE("SyncCommandProvider: find command by ID", "[phase27][commands]")
{
    SyncCommandProvider provider;

    auto cmd = provider.find_command("sync.sync_now");
    REQUIRE(cmd.has_value());
    REQUIRE(cmd->title == "Sync: Sync Now");
    REQUIRE(cmd->keybinding == "Cmd+Shift+S");
    REQUIRE(cmd->requires_sync_configured);

    auto not_found = provider.find_command("nonexistent");
    REQUIRE_FALSE(not_found.has_value());
}

TEST_CASE("SyncCommandProvider: categories", "[phase27][commands]")
{
    SyncCommandProvider provider;

    auto categories = provider.categories();
    REQUIRE(categories.size() == 2);

    // Check both categories.
    bool has_sync = false;
    bool has_mgmt = false;
    for (const auto& cat : categories)
    {
        if (cat == "Sync")
            has_sync = true;
        if (cat == "Sync Management")
            has_mgmt = true;
    }
    REQUIRE(has_sync);
    REQUIRE(has_mgmt);
}

TEST_CASE("SyncCommandProvider: commands in category", "[phase27][commands]")
{
    SyncCommandProvider provider;

    auto sync_cmds = provider.commands_in_category("Sync");
    REQUIRE(sync_cmds.size() == 5);

    auto mgmt_cmds = provider.commands_in_category("Sync Management");
    REQUIRE(mgmt_cmds.size() == 5);
}

TEST_CASE("SyncCommandProvider: available commands filtering", "[phase27][commands]")
{
    SyncCommandProvider provider;

    // Without sync configured, only non-requiring commands are available.
    auto unconfigured = provider.available_commands(false);
    for (const auto& cmd : unconfigured)
    {
        REQUIRE_FALSE(cmd.requires_sync_configured);
    }
    REQUIRE(unconfigured.size() < provider.command_count());

    // With sync configured, all commands available.
    auto configured = provider.available_commands(true);
    REQUIRE(configured.size() == provider.command_count());
}

TEST_CASE("SyncCommandProvider: configure always available", "[phase27][commands]")
{
    SyncCommandProvider provider;

    auto cmd = provider.find_command("sync.configure");
    REQUIRE(cmd.has_value());
    REQUIRE_FALSE(cmd->requires_sync_configured);
}

// ============================================================================
// Phase 27 Events tests
// ============================================================================

TEST_CASE("Events: SyncCompletedEvent", "[phase27][events]")
{
    events::SyncCompletedEvent evt;
    evt.files_uploaded = 5;
    evt.files_downloaded = 3;
    evt.elapsed_ms = 1500;

    REQUIRE(evt.files_uploaded == 5);
    REQUIRE(evt.files_downloaded == 3);
    REQUIRE(evt.elapsed_ms == 1500);
}

TEST_CASE("Events: SyncConflictDetectedEvent", "[phase27][events]")
{
    events::SyncConflictDetectedEvent evt;
    evt.file_path = "docs/readme.md";
    evt.conflict_count = 3;

    REQUIRE(evt.file_path == "docs/readme.md");
    REQUIRE(evt.conflict_count == 3);
}

TEST_CASE("Events: SyncConflictResolvedEvent", "[phase27][events]")
{
    events::SyncConflictResolvedEvent evt;
    evt.file_path = "docs/readme.md";
    evt.resolution_strategy = 1;

    REQUIRE(evt.file_path == "docs/readme.md");
    REQUIRE(evt.resolution_strategy == 1);
}

TEST_CASE("Events: SyncScheduleChangedEvent", "[phase27][events]")
{
    events::SyncScheduleChangedEvent evt;
    evt.interval_minutes = 15;
    evt.is_paused = true;

    REQUIRE(evt.interval_minutes == 15);
    REQUIRE(evt.is_paused);
}

TEST_CASE("Events: WorkspaceShareCreatedEvent", "[phase27][events]")
{
    events::WorkspaceShareCreatedEvent evt;
    evt.share_id = "share_1";
    evt.permission = 1;

    REQUIRE(evt.share_id == "share_1");
    REQUIRE(evt.permission == 1);
}

TEST_CASE("Events: SyncOfflineQueuedEvent", "[phase27][events]")
{
    events::SyncOfflineQueuedEvent evt;
    evt.queued_count = 5;
    evt.total_bytes = 102400;

    REQUIRE(evt.queued_count == 5);
    REQUIRE(evt.total_bytes == 102400);
}
