# Phase 10 -- Backlink Indexing & Discovery

**Priority:** Critical (powers backlink panel, graph view, mention detection)
**Estimated Scope:** ~10 files created/modified
**Dependencies:** Phase 09 (BlockRef, RefParser, RefResolver, RefIndex)

---

## Objective

Implement the backlink indexing system that discovers and maintains bidirectional links between blocks. When block A references block B, block B's backlinks include A. This powers the backlink panel, mention detection, and graph relationships.

SiYuan distinguishes between:
- **Direct backlinks:** explicit `((id))` references where another block links to the target
- **Mentions:** virtual references where the target block's name or alias appears in another block's text without explicit link syntax

The backlink service must be reactive -- when references change (Phase 09 events), the backlink index updates automatically. It must also support graph data generation for local graph visualization around a focal block.

---

## Prerequisites

- Phase 09 completed: `BlockRef.h`, `RefParser`, `RefResolver`, `RefIndex`, all Phase 09 events
- `src/core/EventBus.h` -- subscribe to `BlockRefCreatedEvent`, `BlockRefDeletedEvent`, `BlockRefUpdatedEvent`
- `src/core/Config.h` -- settings for mention detection sensitivity, backlink display
- `src/core/PluginContext.h` -- service injection for `BacklinkService`

---

## SiYuan Source Reference

| SiYuan File | Relevant Functions | MarkAmp Mapping |
|---|---|---|
| `kernel/model/backlink.go` | `GetBacklink2` | `BacklinkService::get_backlinks()` |
| `kernel/model/backlink.go` | `GetBackmentions` | `BacklinkService::get_mentions()` |
| `kernel/model/backlink.go` | `BuildTreeBacklink` | `BacklinkService::build_tree_backlinks()` |
| `kernel/sql/block_ref.go` | `DefRefs` | `BacklinkService::get_def_refs()` |
| `kernel/sql/block_ref.go` | `CacheRef`, `GetRefsCacheByDefID` | `BacklinkIndex` cache layer |
| `kernel/model/backlink.go` | `GetBacklinkDoc` | `BacklinkService::get_backlink_documents()` |
| `kernel/model/graph.go` | `GetGraph`, `GetLocalGraph` | `BacklinkService::get_graph_data()` |

---

## MarkAmp Integration Points

1. **RefIndex (Phase 09)** -- BacklinkService wraps RefIndex and adds mention detection on top. It subscribes to Phase 09 events to keep the backlink cache synchronized.
2. **EventBus** -- Subscribes to `BlockRefCreatedEvent` / `BlockRefDeletedEvent` / `BlockRefUpdatedEvent` to update the backlink index reactively. Publishes `BacklinksChangedEvent` and `MentionsChangedEvent`.
3. **PreviewPanel / HtmlRenderer** -- Backlink count badge rendered in block gutter. Clicking opens backlink panel focused on that block.
4. **PluginContext** -- `BacklinkService*` pointer added for extension access.
5. **ActivityBar** -- New `ActivityBarItem::Backlinks` variant for sidebar panel navigation.

---

## Data Structures to Implement

All structures go in `src/core/Backlink.h`:

```cpp
#pragma once

#include "BlockRef.h"
#include "EventBus.h"

#include <chrono>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class Config;

/// Type of backlink relationship.
enum class BacklinkType : std::uint8_t
{
    DirectRef,   // Explicit ((id)) reference
    Mention      // Virtual reference (name/alias match in text)
};

/// A single backlink item: a block that references the target definition block.
struct BacklinkItem
{
    /// The block that contains the reference (the "source" block).
    BlockId ref_block_id;

    /// The block being referenced (the "definition" or "target" block).
    BlockId def_block_id;

    /// The root document ID containing the referencing block.
    std::string ref_root_id;

    /// The root document title (for display in backlink panel).
    std::string ref_doc_title;

    /// Content of the referencing block (for context display).
    std::string ref_block_content;

    /// Text surrounding the reference for context (up to 50 chars before/after).
    std::string context_before;
    std::string context_after;

    /// Type of backlink.
    BacklinkType type{BacklinkType::DirectRef};

    /// Timestamp when this backlink was last updated.
    std::chrono::system_clock::time_point updated_at;
};

/// A mention item: an occurrence of a block's name/alias in another block.
struct MentionItem
{
    /// The block containing the mention.
    BlockId source_block_id;

    /// The root document containing the source block.
    std::string source_root_id;
    std::string source_doc_title;

    /// The mentioning block's full content.
    std::string source_content;

    /// The matched name/alias text.
    std::string matched_name;

    /// Character offset of the match within source content.
    std::size_t match_offset{0};
    std::size_t match_length{0};

    /// Context surrounding the mention.
    std::string context_before;
    std::string context_after;
};

/// Backlinks grouped by source document for tree display.
struct BacklinkTreeNode
{
    /// Document-level grouping.
    std::string doc_id;
    std::string doc_title;
    std::string doc_path;

    /// All backlinks from this document to the target block.
    std::vector<BacklinkItem> backlinks;

    /// Count of backlinks from this document.
    [[nodiscard]] auto count() const -> std::size_t
    {
        return backlinks.size();
    }
};

/// Graph node for local graph visualization.
struct GraphNode
{
    BlockId block_id;
    std::string label;       // Block title or first line
    std::string node_type;   // "doc", "heading", "paragraph", "list", etc.
    bool is_focal{false};    // True for the central/queried node
    int ref_count{0};        // Total inbound reference count
};

/// Graph edge for local graph visualization.
struct GraphEdge
{
    BlockId source_id;
    BlockId target_id;
    RefType ref_type{RefType::BlockRef};
};

/// Graph data containing nodes and edges for visualization.
struct GraphData
{
    std::vector<GraphNode> nodes;
    std::vector<GraphEdge> edges;
};

/// Cached backlink index for O(1) lookups.
/// Wraps RefIndex with an additional mention-detection layer and
/// document-level grouping.
class BacklinkIndex
{
public:
    /// Update the index when a reference is added.
    void on_ref_added(const BlockId& source, const BlockId& def, RefType type);

    /// Update the index when a reference is removed.
    void on_ref_removed(const BlockId& source, const BlockId& def);

    /// Get all backlink items for a definition block.
    [[nodiscard]] auto get_backlinks(const BlockId& def_block_id) const
        -> std::vector<BacklinkItem>;

    /// Get backlinks grouped by source document (tree display).
    [[nodiscard]] auto get_tree_backlinks(const BlockId& def_block_id) const
        -> std::vector<BacklinkTreeNode>;

    /// Get the backlink count for a block.
    [[nodiscard]] auto backlink_count(const BlockId& def_block_id) const -> std::size_t;

    /// Clear the entire backlink cache.
    void clear();

    /// Rebuild from a RefIndex snapshot.
    void rebuild_from(const RefIndex& ref_index);

private:
    /// def_block_id -> list of BacklinkItems
    std::unordered_map<BlockId, std::vector<BacklinkItem>, BlockIdHash> cache_;

    mutable std::mutex mutex_;
};

} // namespace markamp::core
```

---

## Key Functions to Implement

### BacklinkService class (`src/core/BacklinkService.h` / `src/core/BacklinkService.cpp`)

Constructor: `BacklinkService(EventBus& event_bus, Config& config, RefIndex& ref_index)`

1. **`auto get_backlinks(const BlockId& block_id) -> std::vector<BacklinkItem>`** -- Retrieve all direct backlinks pointing to the given block. Returns `BacklinkItem` entries with full context (surrounding text, document title, block content). Fetches from `BacklinkIndex` cache, populating content fields lazily from block database.

2. **`auto get_mentions(const BlockId& block_id) -> std::vector<MentionItem>`** -- Scan all blocks for occurrences of the given block's name and aliases. Uses the virtual reference detection from `RefParser::detect_virtual_refs()`. Excludes blocks that already have a direct reference to avoid duplicates. Returns `MentionItem` entries with match position and context.

3. **`auto get_backlink_count(const BlockId& block_id) -> std::size_t`** -- Return the total number of direct backlinks to the given block. O(1) lookup from BacklinkIndex.

4. **`auto get_mention_count(const BlockId& block_id) -> std::size_t`** -- Return the total number of mention occurrences for the given block. More expensive than `get_backlink_count()` as it requires scanning.

5. **`auto build_tree_backlinks(const BlockId& block_id) -> std::vector<BacklinkTreeNode>`** -- Group backlinks by source document for tree-style display. Each `BacklinkTreeNode` contains the document metadata and all backlinks from that document.

6. **`auto get_backlink_documents(const BlockId& block_id) -> std::vector<std::string>`** -- Return the list of unique document root IDs that contain backlinks to the given block.

7. **`auto get_graph_data(const BlockId& focal_block_id, int depth) -> GraphData`** -- Generate graph data centered on the focal block. Traverses outgoing and incoming references up to `depth` hops (default: 2). Returns `GraphNode` and `GraphEdge` vectors suitable for rendering a force-directed graph.

8. **`void rebuild_backlink_index()`** -- Full rebuild of the BacklinkIndex from the RefIndex. Called on workspace open and after bulk import. Publishes `BacklinksChangedEvent` for each affected definition block.

9. **`void on_block_ref_created(const BlockId& source, const BlockId& def)`** -- Internal handler called when a `BlockRefCreatedEvent` fires. Updates BacklinkIndex and publishes `BacklinksChangedEvent`.

10. **`void on_block_ref_deleted(const BlockId& source, const BlockId& def)`** -- Internal handler called when a `BlockRefDeletedEvent` fires. Updates BacklinkIndex and publishes `BacklinksChangedEvent`.

---

## Events to Add

Add to `src/core/Events.h`:

```cpp
// ============================================================================
// Backlink events (Phase 10)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BacklinksChangedEvent)
std::string def_block_id;
int backlink_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(MentionsChangedEvent)
std::string block_id;
int mention_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BacklinkPanelOpenRequestEvent)
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphViewOpenRequestEvent)
std::string focal_block_id;
int depth{2};
MARKAMP_DECLARE_EVENT_END;
```

---

## Config Keys to Add

Add to `resources/config_defaults.json`:

| Key | Type | Default | Description |
|---|---|---|---|
| `knowledgebase.backlinks.show_mentions` | bool | `true` | Include virtual mentions in backlink panel |
| `knowledgebase.backlinks.mention_min_length` | int | `4` | Minimum name length for mention detection |
| `knowledgebase.backlinks.context_chars` | int | `50` | Characters of context to show before/after backlink |
| `knowledgebase.backlinks.group_by_document` | bool | `true` | Group backlinks by source document in panel |
| `knowledgebase.graph.default_depth` | int | `2` | Default traversal depth for local graph |
| `knowledgebase.graph.max_nodes` | int | `200` | Maximum nodes to include in graph data |

---

## Test Cases

File: `tests/unit/test_backlinks.cpp`

```cpp
#include "core/Backlink.h"
#include "core/BacklinkService.h"
#include "core/BlockRef.h"
#include "core/EventBus.h"
#include "core/Config.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
```

1. **Direct backlink discovery** -- Block A refs block B via `((B_id))`. Call `get_backlinks(B_id)`. Verify: one `BacklinkItem` returned with `ref_block_id == A_id`, `type == BacklinkType::DirectRef`.

2. **Multiple backlinks to same block** -- Blocks A, C, D all reference block B. Verify: `get_backlinks(B_id)` returns 3 items. `get_backlink_count(B_id) == 3`.

3. **Mention detection** -- Block B has name "Project Alpha". Block E contains text "Working on Project Alpha today". No explicit ref. Verify: `get_mentions(B_id)` returns one `MentionItem` with `matched_name == "Project Alpha"` and correct offset.

4. **Mention excludes direct ref blocks** -- Block A has direct ref to B and also mentions B's name. Verify: `get_mentions(B_id)` does NOT include A (already a direct ref). `get_backlinks(B_id)` includes A.

5. **Tree backlinks grouped by document** -- Blocks A1, A2 (in doc X) and B1 (in doc Y) reference block C. Verify: `build_tree_backlinks(C_id)` returns 2 `BacklinkTreeNode` entries, one for doc X (with 2 backlinks) and one for doc Y (with 1 backlink).

6. **Backlink count update on ref created** -- Add ref A->B. Verify `BacklinksChangedEvent` published with `backlink_count == 1`. Add ref C->B. Verify event published with `backlink_count == 2`.

7. **Backlink count update on ref deleted** -- Start with refs A->B and C->B. Remove ref A->B. Verify `BacklinksChangedEvent` published with `backlink_count == 1`.

8. **Graph data generation** -- Create a small reference network: A->B, B->C, C->D, E->B. Call `get_graph_data(B_id, depth=1)`. Verify: nodes include A, B, C, E (all within 1 hop of B). Edges include A->B, B->C, E->B. B is marked `is_focal == true`.

9. **Graph depth limiting** -- Same network as test 8. Call `get_graph_data(B_id, depth=0)`. Verify: only node B returned with no edges.

10. **Full index rebuild** -- Populate RefIndex with multiple refs. Call `rebuild_backlink_index()`. Verify: BacklinkIndex matches RefIndex state. Verify: `BacklinksChangedEvent` published for each affected def block.

---

## Acceptance Criteria

- [ ] `BacklinkService::get_backlinks()` returns correct direct backlinks with context text
- [ ] `BacklinkService::get_mentions()` detects virtual mentions and excludes blocks with direct refs
- [ ] `BacklinkService::build_tree_backlinks()` groups backlinks by source document correctly
- [ ] `BacklinkService::get_graph_data()` generates correct node/edge data with depth limiting
- [ ] `BacklinkIndex` updates reactively via `BlockRefCreatedEvent` / `BlockRefDeletedEvent` subscriptions
- [ ] `BacklinksChangedEvent` fires with correct count whenever backlinks change
- [ ] `MentionsChangedEvent` fires when mention detection results change
- [ ] Mention detection respects `knowledgebase.backlinks.mention_min_length` config
- [ ] Graph data respects `knowledgebase.graph.max_nodes` limit
- [ ] Context extraction respects `knowledgebase.backlinks.context_chars` setting
- [ ] Thread-safe: `BacklinkIndex` mutex guards concurrent access
- [ ] All 10 Catch2 test cases pass
- [ ] No use of `catch(...)` -- all exception handlers use typed catches
- [ ] All query methods marked `[[nodiscard]]`

---

## Files to Create/Modify

### New Files

| File | Description |
|---|---|
| `src/core/Backlink.h` | `BacklinkItem`, `MentionItem`, `BacklinkTreeNode`, `GraphNode`, `GraphEdge`, `GraphData`, `BacklinkIndex` |
| `src/core/Backlink.cpp` | `BacklinkIndex` method implementations |
| `src/core/BacklinkService.h` | `BacklinkService` class declaration |
| `src/core/BacklinkService.cpp` | All 10 service methods, event subscriptions, mention detection |
| `tests/unit/test_backlinks.cpp` | All 10 Catch2 test cases |

### Modified Files

| File | Change |
|---|---|
| `src/core/Events.h` | Add `BacklinksChangedEvent`, `MentionsChangedEvent`, `BacklinkPanelOpenRequestEvent`, `GraphViewOpenRequestEvent` |
| `src/core/PluginContext.h` | Add `BacklinkService* backlink_service{nullptr};` pointer |
| `src/CMakeLists.txt` | Add `core/Backlink.cpp`, `core/BacklinkService.cpp` to sources |
| `tests/CMakeLists.txt` | Add `test_backlinks` test target |
| `resources/config_defaults.json` | Add 6 `knowledgebase.backlinks.*` and `knowledgebase.graph.*` config keys |
