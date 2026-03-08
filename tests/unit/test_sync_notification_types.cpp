// test_sync_notification_types.cpp — 10 tests for Cloud Sync notification and share types
#include "core/CloudSyncTypes.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SyncNotification defaults", "[sync][notification]")
{
    SyncNotification notification;
    CHECK(notification.type == SyncNotificationType::kSyncComplete);
    CHECK(notification.message.empty());
    CHECK(notification.file_count == 0);
    CHECK_FALSE(notification.has_retry_action);
}

TEST_CASE("SyncNotification error type", "[sync][notification]")
{
    SyncNotification notification;
    notification.type = SyncNotificationType::kSyncError;
    notification.message = "Connection timeout";
    notification.has_retry_action = true;
    CHECK(notification.type == SyncNotificationType::kSyncError);
    CHECK(notification.has_retry_action);
}

TEST_CASE("SyncNotificationType enum values distinct", "[sync][notification]")
{
    CHECK(SyncNotificationType::kSyncComplete != SyncNotificationType::kSyncError);
    CHECK(SyncNotificationType::kConflictDetected != SyncNotificationType::kOfflineQueued);
    CHECK(SyncNotificationType::kShareActivity != SyncNotificationType::kSyncComplete);
}

TEST_CASE("WorkspaceShareInfo defaults", "[sync][share]")
{
    WorkspaceShareInfo info;
    CHECK(info.share_id.empty());
    CHECK(info.permission == SharePermission::kReadOnly);
    CHECK(info.created_at == 0);
    CHECK(info.is_active);
}

TEST_CASE("WorkspaceShareInfo with collaborators", "[sync][share]")
{
    WorkspaceShareInfo info;
    info.share_id = "share-001";
    info.workspace_path = "/projects/notes";
    info.permission = SharePermission::kReadWrite;
    info.shared_by = "alice";
    info.shared_with = {"bob", "charlie"};
    CHECK(info.shared_with.size() == 2);
    CHECK(info.permission == SharePermission::kReadWrite);
}

TEST_CASE("SharePermission enum values", "[sync][share]")
{
    CHECK(SharePermission::kReadOnly != SharePermission::kReadWrite);
}

TEST_CASE("SyncHistoryEntry defaults", "[sync][history]")
{
    SyncHistoryEntry entry;
    CHECK(entry.entry_id.empty());
    CHECK(entry.timestamp == 0);
    CHECK(entry.files_uploaded == 0);
    CHECK(entry.files_downloaded == 0);
    CHECK(entry.conflicts == 0);
    CHECK(entry.status == CloudSyncStatus::Completed);
}

TEST_CASE("SyncHistoryEntry with data", "[sync][history]")
{
    SyncHistoryEntry entry;
    entry.entry_id = "sync-42";
    entry.operation = "sync";
    entry.files_uploaded = 5;
    entry.files_downloaded = 3;
    entry.elapsed_ms = 1200;
    entry.device_name = "MacBook Pro";
    CHECK(entry.files_uploaded == 5);
    CHECK(entry.device_name == "MacBook Pro");
}

TEST_CASE("SyncScheduleInterval enum values", "[sync][schedule]")
{
    CHECK(SyncScheduleInterval::kManual != SyncScheduleInterval::k5Min);
    CHECK(SyncScheduleInterval::k15Min != SyncScheduleInterval::k30Min);
    CHECK(SyncScheduleInterval::k60Min != SyncScheduleInterval::kManual);
}

TEST_CASE("CloudSyncStatus enum covers all states", "[sync][status]")
{
    CHECK(CloudSyncStatus::Idle != CloudSyncStatus::Connecting);
    CHECK(CloudSyncStatus::Uploading != CloudSyncStatus::Downloading);
    CHECK(CloudSyncStatus::Encrypting != CloudSyncStatus::Decrypting);
    CHECK(CloudSyncStatus::Completed != CloudSyncStatus::Failed);
}
