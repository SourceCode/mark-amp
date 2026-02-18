# Phase 27: Cloud Sync and Collaboration

## Overview
CloudSyncService (434 lines), SyncEngine, S3Client, WebDavClient, and SyncTypes exist in core with encryption and conflict resolution infrastructure. However, cloud sync is not wired to any user-facing functionality. Users cannot sync workspaces to cloud storage or collaborate via shared workspaces. This phase builds the cloud sync experience.

## Prerequisites
- Phase 20 (File management and workspace)
- Phase 25 (Version control for conflict awareness)
- Phase 02 (Config for sync settings)

## Tasks

### Task 1: Wire CloudSyncService Provider Configuration
**Files:** `src/core/CloudSyncService.cpp`, `src/core/CloudSyncService.h`, `src/core/CloudSyncTypes.h`
**Description:** CloudSyncService exists. Wire provider configuration: S3-compatible storage, WebDAV, and local folder sync. Credentials stored securely.
**Acceptance Criteria:**
- Provider settings in preferences: type, endpoint, credentials
- S3 provider: bucket, region, access key (in keychain)
- WebDAV provider: server URL, username, password (in keychain)
- Local folder sync: target directory path
- Connection test: "Sync: Test Connection" command
- `SyncProviderConfiguredEvent` emitted

### Task 2: Wire SyncEngine for Bidirectional Sync
**Files:** `src/core/SyncEngine.cpp`, `src/core/SyncEngine.h`
**Description:** SyncEngine exists. Wire bidirectional sync: detect local and remote changes, merge non-conflicting changes, flag conflicts.
**Acceptance Criteria:**
- Sync detects: new local files, modified local, deleted local
- Sync detects: new remote files, modified remote, deleted remote
- Non-conflicting changes merged automatically
- Conflicting changes flagged for manual resolution
- `SyncCompletedEvent` emitted with statistics
- `SyncConflictEvent` emitted per conflict

### Task 3: Wire S3Client for Cloud Storage
**Files:** `src/core/S3Client.cpp`, `src/core/S3Client.h`
**Description:** S3Client exists. Wire it for file upload/download/delete/list operations against S3-compatible storage.
**Acceptance Criteria:**
- Upload files to S3 bucket with path prefix
- Download files from S3 bucket
- Delete files from S3 bucket
- List files with metadata (size, modified date, etag)
- Multipart upload for large files (>5MB)
- Error handling: auth failure, network error, bucket not found

### Task 4: Wire WebDavClient for Server Sync
**Files:** `src/core/WebDavClient.cpp`, `src/core/WebDavClient.h`
**Description:** WebDavClient exists. Wire it for WebDAV operations: PUT, GET, DELETE, PROPFIND, MKCOL. Support Nextcloud and ownCloud.
**Acceptance Criteria:**
- Upload files via PUT
- Download files via GET
- Delete files via DELETE
- List directory via PROPFIND
- Create directory via MKCOL
- Chunked upload for large files

### Task 5: Wire Sync Status UI
**Files:** `src/ui/StatusBarPanel.cpp`, `src/core/CloudSyncService.cpp`
**Description:** Status bar shows sync status: idle, syncing (with progress), error (with details). Click opens sync panel.
**Acceptance Criteria:**
- Sync icon in status bar: idle (cloud), syncing (spinning), error (warning)
- Progress: file count (3/15 files synced)
- Click opens sync details panel
- Error: tooltip shows error message
- Last sync time shown

### Task 6: Wire Auto-Sync Scheduling
**Files:** `src/core/CloudSyncService.cpp`, `src/core/Config.h`
**Description:** Auto-sync at configurable intervals. Also sync on file save (debounced) and on workspace close.
**Acceptance Criteria:**
- Auto-sync interval: 5, 15, 30, 60 minutes (configurable)
- Sync on save: debounced 30 seconds
- Sync on workspace close
- Manual sync: "Sync: Sync Now" command
- Pause/resume sync
- Sync only when online

### Task 7: Wire Conflict Resolution UI
**Files:** `src/ui/DiffPanel.cpp`, `src/core/SyncEngine.cpp`
**Description:** When sync conflicts occur, show resolution UI: side-by-side diff of local vs remote, with options to keep local, keep remote, or merge.
**Acceptance Criteria:**
- Conflict notification with file count
- Click opens conflict resolution panel
- Side-by-side diff: local (left) vs remote (right)
- Actions: Keep Local, Keep Remote, Merge (open editor)
- "Keep Both" renames one copy
- Resolved conflicts continue sync

### Task 8: Wire Encryption for Sync
**Files:** `src/core/CloudSyncService.cpp`, `src/core/EncryptionService.cpp`
**Description:** Optional end-to-end encryption for synced files. Files encrypted before upload, decrypted after download.
**Acceptance Criteria:**
- Encryption toggle in sync settings
- AES-256-GCM encryption
- Encryption key derived from user password
- File names optionally encrypted
- Decrypt on download (transparent to user)
- Key management: change password re-encrypts all files

### Task 9: Wire Selective Sync
**Files:** `src/core/CloudSyncService.cpp`, `src/core/Config.h`
**Description:** Selective sync: include/exclude folders and file patterns from sync. Useful for large workspaces.
**Acceptance Criteria:**
- Include/exclude patterns in sync settings
- Folder-level toggle in file explorer
- Pattern-based: `*.md`, `!*.tmp`, `docs/**`
- Excluded files shown as "cloud-only" in explorer
- "Download on demand" for excluded files
- Sync config saved per workspace

### Task 10: Wire Sync History and Audit Log
**Files:** `src/core/CloudSyncService.cpp`, `src/core/OutputChannelService.cpp`
**Description:** Track sync history: every sync operation logged with files synced, conflicts, errors, timing.
**Acceptance Criteria:**
- Sync log in "Sync" output channel
- Each entry: timestamp, operation, file, result
- Conflicts logged with resolution
- Errors logged with details
- "Sync: Show History" command
- Sync history exportable

### Task 11: Wire Workspace Sharing
**Files:** `src/core/CloudSyncService.cpp`
**Description:** Share workspace with other users via sync provider. Generate share link, manage permissions (read, read-write).
**Acceptance Criteria:**
- "Sync: Share Workspace" generates share link
- Permissions: read-only, read-write
- Revoke share access
- Shared workspace indicator in status bar
- Notification when collaborator makes changes
- Conflict resolution for simultaneous edits

### Task 12: Wire Offline Mode
**Files:** `src/core/CloudSyncService.cpp`, `src/core/SyncEngine.cpp`
**Description:** Full offline support: all edits queued when offline, synced when connectivity restored. Offline indicator in status bar.
**Acceptance Criteria:**
- Offline detection (network connectivity check)
- All edits continue working offline
- Changes queued in `.markamp/sync-queue/`
- Sync resumes automatically when online
- Queue size shown in status bar
- Large queue warning notification

### Task 13: Wire Sync Bandwidth Management
**Files:** `src/core/CloudSyncService.cpp`
**Description:** Bandwidth management: upload/download rate limiting, pause on metered connection, prioritize small files.
**Acceptance Criteria:**
- Upload rate limit configurable (KB/s)
- Download rate limit configurable (KB/s)
- "Pause on metered connection" toggle
- Small files synced first (<100KB)
- Large file progress shown individually
- Bandwidth usage statistics

### Task 14: Wire Sync Ignore Patterns
**Files:** `src/core/CloudSyncService.cpp`
**Description:** `.markamp-syncignore` file (like .gitignore) for excluding files from sync. Default patterns for temp files, large binaries.
**Acceptance Criteria:**
- `.markamp-syncignore` file in workspace root
- Same syntax as .gitignore
- Default patterns: `*.tmp`, `.DS_Store`, `node_modules/`
- Patterns apply to upload and download
- Settings UI for editing ignore patterns

### Task 15: Wire Sync Notifications
**Files:** `src/core/NotificationService.cpp`, `src/core/CloudSyncService.cpp`
**Description:** Sync notifications: sync complete, sync error, conflict detected, share activity.
**Acceptance Criteria:**
- Sync complete: "Synced X files" notification
- Sync error: "Sync failed: [reason]" with retry action
- Conflict: "X conflicts detected" with resolve action
- Notifications configurable: enable/disable per type
- Toast notifications for important events

### Task 16: Wire Sync Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register sync commands: "Sync: Sync Now", "Sync: Configure", "Sync: Pause", "Sync: Resume", "Sync: Show History", "Sync: Resolve Conflicts", "Sync: Share Workspace".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "Sync:" prefix
- Commands disabled when no sync configured
- "Sync Now" with keyboard shortcut
- "Configure" opens sync settings

### Task 17: Wire Sync Settings UI
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/Config.h`
**Description:** Sync settings section in preferences: provider, credentials, auto-sync interval, encryption, selective sync, bandwidth limits.
**Acceptance Criteria:**
- Settings section: "Cloud Sync"
- Provider selector with configuration per provider
- Auto-sync interval selector
- Encryption toggle and password management
- Selective sync pattern editor
- Bandwidth limit sliders

### Task 18: Wire Multi-Device Sync State
**Files:** `src/core/SyncEngine.cpp`
**Description:** Track sync state per device: last sync timestamp, device name, active status. Show which devices have latest changes.
**Acceptance Criteria:**
- Device registration on first sync
- Device list in sync settings
- Last sync time per device
- "This device" vs "Other devices" distinction
- Remove device authorization
- Sync state metadata in `.markamp/sync-state.json`

### Task 19: Wire Sync Performance Optimization
**Files:** `src/core/SyncEngine.cpp`, `src/core/CloudSyncService.cpp`
**Description:** Optimize sync: delta sync (only changed bytes), checksum verification, parallel uploads, connection pooling.
**Acceptance Criteria:**
- Delta sync: only upload changed portions of large files
- Checksum verification: skip unchanged files
- Parallel uploads: up to 4 concurrent (configurable)
- Connection pooling for HTTP connections
- Sync time: 100 small files in < 10 seconds

### Task 20: Add Cloud Sync Tests
**Files:** `tests/unit/test_cloud_sync.cpp`, `tests/unit/test_encryption_service.cpp`
**Description:** Test sync system: provider operations, conflict detection, encryption, and state management.
**Acceptance Criteria:**
- Provider operations: upload, download, list, delete
- Conflict detection: local and remote changes
- Encryption: encrypt/decrypt round-trip
- Selective sync: pattern matching
- Offline queue: enqueue and drain

## Testing Requirements
- Bidirectional sync: all change combinations
- Conflict detection and resolution
- Encryption round-trip
- Offline queue and resume

## Phase Completion Criteria
- Cloud sync to S3 and WebDAV providers
- Bidirectional sync with conflict resolution
- Auto-sync with configurable intervals
- End-to-end encryption option
- Selective sync and ignore patterns
- Offline support with queue
- All tests pass
