# Phase 40 -- Publish to Web and Sync Infrastructure

## Objective

Implement vault publishing (export vault as a static website with navigation, search, and graph) and the sync infrastructure for cross-device document synchronization. Publishing generates a complete website from the vault with rendered markdown, interlinked pages, and a client-side search index. Sync provides conflict-free merging of changes between devices using CRDTs or last-write-wins strategy.

## Prerequisites

- Phase 38 (ExportService -- HTML export)
- Phase 02 (VaultService -- vault access)
- Phase 03 (BacklinkIndex -- inter-page links)
- Phase 05 (SearchEngine -- client-side search index generation)
- Phase 07 (GraphEngine -- static graph rendering)

## Feature References (PRD)

- PRD #49: Publish to Web
- PRD #4: Local First + Sync Optional
- PRD #18: Sync Across Devices

## Data Structures to Implement

### File: `src/core/PublishService.h`

```cpp
#pragma once

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;
class VaultService;
class BacklinkIndex;
class SearchEngine;

struct PublishConfig
{
    std::filesystem::path output_dir;
    std::string site_title;
    std::string site_description;
    std::string base_url;            // "https://notes.example.com"
    std::string theme;               // Publishing theme
    bool include_graph{true};        // Generate graph view page
    bool include_search{true};       // Generate search index
    bool include_backlinks{true};    // Show backlinks on each page
    bool include_tags{true};         // Generate tag pages
    bool include_daily_notes{false}; // Include daily notes
    std::vector<std::string> exclude_paths;   // Paths/patterns to exclude
    std::vector<std::string> exclude_tags;    // Exclude docs with these tags
    bool draft_filter{true};         // Exclude publish:false docs

    // Navigation
    bool generate_sidebar{true};
    bool generate_breadcrumb{true};
    std::string home_page;           // Document to use as index
};

struct PublishResult
{
    std::filesystem::path output_dir;
    int pages_generated{0};
    int assets_copied{0};
    int64_t total_bytes{0};
    double elapsed_ms{0.0};
    std::vector<std::string> warnings;
    bool success{false};
};

class PublishService
{
public:
    PublishService(EventBus& event_bus, Config& config,
                   VaultService& vault_service,
                   BacklinkIndex& backlink_index,
                   SearchEngine& search_engine);

    /// Publish the vault as a static website.
    [[nodiscard]] auto publish(const PublishConfig& config) -> std::expected<PublishResult, std::string>;

    /// Preview publish (generate to temp dir, return path).
    [[nodiscard]] auto preview_publish(const PublishConfig& config)
        -> std::expected<std::filesystem::path, std::string>;

    /// Generate a search index (JSON) for client-side search.
    [[nodiscard]] auto generate_search_index(const std::vector<std::string>& doc_ids) const
        -> std::string;

    /// Generate a static graph view page (SVG + interactive JS).
    [[nodiscard]] auto generate_graph_page(const std::vector<std::string>& doc_ids) const
        -> std::string;

private:
    EventBus& event_bus_;
    Config& config_;
    VaultService& vault_service_;
    BacklinkIndex& backlink_index_;
    SearchEngine& search_engine_;

    auto filter_documents(const PublishConfig& config) const -> std::vector<std::string>;
    auto render_page(const std::string& document_id, const PublishConfig& config) const -> std::string;
    auto render_sidebar(const std::vector<std::string>& doc_ids) const -> std::string;
    auto render_tag_page(const std::string& tag, const std::vector<std::string>& doc_ids) const -> std::string;
    auto copy_assets(const PublishConfig& config) const -> int;
    auto generate_css(const PublishConfig& config) const -> std::string;
};

} // namespace markamp::core
```

### File: `src/core/SyncService.h`

```cpp
#pragma once

#include <expected>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;
class VaultService;

enum class ConflictStrategy : uint8_t
{
    LastWriteWins,
    KeepBoth,       // Create conflicting copy
    Manual          // Prompt user
};

struct SyncState
{
    std::string device_id;
    std::string last_sync_timestamp;
    int pending_uploads{0};
    int pending_downloads{0};
    bool is_syncing{false};
};

struct SyncConflict
{
    std::string document_id;
    std::string local_content;
    std::string remote_content;
    std::string local_modified;
    std::string remote_modified;
};

class SyncService
{
public:
    SyncService(EventBus& event_bus, Config& config, VaultService& vault_service);

    /// Configure sync with a remote endpoint.
    [[nodiscard]] auto configure(const std::string& endpoint, const std::string& token)
        -> std::expected<void, std::string>;

    /// Perform a sync cycle (push local changes, pull remote changes).
    [[nodiscard]] auto sync() -> std::expected<SyncState, std::string>;

    /// Get current sync state.
    [[nodiscard]] auto state() const -> SyncState;

    /// Resolve a sync conflict.
    auto resolve_conflict(const SyncConflict& conflict, ConflictStrategy strategy) -> void;

    /// Get pending conflicts.
    [[nodiscard]] auto pending_conflicts() const -> std::vector<SyncConflict>;

    /// Enable/disable auto-sync.
    auto set_auto_sync(bool enabled, int interval_seconds = 300) -> void;

    /// Generate a unique device ID.
    [[nodiscard]] static auto generate_device_id() -> std::string;

private:
    EventBus& event_bus_;
    Config& config_;
    VaultService& vault_service_;

    std::string endpoint_;
    std::string token_;
    std::string device_id_;
    SyncState current_state_;

    auto detect_local_changes() const -> std::vector<std::string>;
    auto push_changes(const std::vector<std::string>& changed_ids) -> int;
    auto pull_changes() -> int;
    auto detect_conflicts() -> std::vector<SyncConflict>;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`publish(config)`** -- Filter documents (exclude drafts, excluded paths/tags). For each, render_page() to produce styled HTML. Generate sidebar, tag pages, search index, graph page. Copy assets. Write all files to output_dir.

2. **`render_page(document_id, config)`** -- Load document. Render markdown to HTML. Add page chrome: header, sidebar, backlinks section, tag links, prev/next navigation. Wrap in HTML template.

3. **`generate_search_index(doc_ids)`** -- For each document: extract title, tags, content snippet. Serialize as JSON array for lunr.js or similar client-side search library.

4. **`generate_graph_page(doc_ids)`** -- Compute graph data. Generate SVG nodes and edges. Add JavaScript for interactive pan/zoom. Wrap in HTML page.

5. **`SyncService::sync()`** -- Detect local changes since last sync. Push to remote endpoint (HTTP API or file sync). Pull remote changes. Detect conflicts (same file modified on both sides). Apply conflict strategy. Update last_sync_timestamp.

6. **`resolve_conflict(conflict, strategy)`** -- LastWriteWins: keep the newer version. KeepBoth: save remote as "filename (conflict).md". Manual: prompt user in UI.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PublishCompletedEvent)
std::string output_dir;
int pages{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PublishProgressEvent)
int current{0};
int total{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SyncCompletedEvent)
int uploaded{0};
int downloaded{0};
int conflicts{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SyncConflictDetectedEvent)
std::string document_id;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `publish.output_dir` | string | `""` | Default publish output directory |
| `publish.base_url` | string | `""` | Published site base URL |
| `publish.theme` | string | `"default"` | Publishing theme |
| `sync.enabled` | bool | `false` | Enable sync |
| `sync.endpoint` | string | `""` | Sync server endpoint |
| `sync.auto_sync` | bool | `false` | Auto-sync on save |
| `sync.interval_seconds` | int | `300` | Auto-sync interval |
| `sync.conflict_strategy` | string | `"last_write_wins"` | Conflict resolution strategy |

## Test Cases

File: `tests/unit/test_publish_sync.cpp`

1. **Publish generates HTML** -- Publish 3 documents. Verify 3 HTML files + index.
2. **Publish filters drafts** -- Doc with publish:false. Verify excluded.
3. **Publish generates sidebar** -- Verify sidebar HTML contains links to all published pages.
4. **Publish search index** -- Verify JSON search index generated.
5. **Publish graph page** -- Verify graph HTML page generated with nodes.
6. **Publish copies assets** -- Vault has images. Verify copied to output.
7. **Sync detect changes** -- Modify 2 documents. detect_local_changes() returns 2.
8. **Conflict detection** -- Same file changed locally and remotely. Verify conflict detected.
9. **Conflict last-write-wins** -- Resolve conflict. Newer version kept.
10. **Conflict keep-both** -- Resolve conflict. Both versions saved.
11. **Device ID generation** -- generate_device_id() returns unique string.
12. **Sync state tracking** -- After sync, state shows correct counts.

## Acceptance Criteria

- [ ] Publish generates a complete static website with navigation
- [ ] Draft documents (publish:false) are excluded
- [ ] Client-side search index generated for published site
- [ ] Static graph view page generated
- [ ] Assets (images) copied to output
- [ ] Sync detects local changes since last sync
- [ ] Conflict detection when same file modified on multiple devices
- [ ] Conflict resolution strategies (last-write-wins, keep-both) work
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/PublishService.h` | PublishService, PublishConfig, PublishResult |
| CREATE | `src/core/PublishService.cpp` | Full implementation |
| CREATE | `src/core/SyncService.h` | SyncService, SyncState, SyncConflict |
| CREATE | `src/core/SyncService.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 4 publish/sync events |
| MODIFY | `src/core/PluginContext.h` | Add `PublishService*`, `SyncService*` |
| MODIFY | `src/CMakeLists.txt` | Add new .cpp files |
| CREATE | `tests/unit/test_publish_sync.cpp` | 12 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_publish_sync target |

## Architecture Notes

- Publishing is a batch export with extra chrome (sidebar, search, graph)
- Sync is designed to work with a simple REST API or file-based sync (S3, WebDAV)
- Conflict detection uses modification timestamps and content hashing
- CRDT-based sync is a future enhancement; v1 uses timestamp-based resolution
- PublishService(EventBus&, Config&, VaultService&, BacklinkIndex&, SearchEngine&)
- SyncService(EventBus&, Config&, VaultService&)

## Estimated Complexity

**XL** -- Static site generation, search index, graph rendering, sync protocol, conflict resolution, 12 tests.
