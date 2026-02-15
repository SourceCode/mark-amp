# Phase 03 -- Bidirectional Link Parser and Index

## Objective

Implement the backlink index that powers bidirectional linking. When document A links to document B via `[[B]]`, the backlink index records that B has an incoming link from A. The BacklinkIndex is a persistent, incrementally-updated data structure that answers "which documents link to this document?" in O(1). This is the core infrastructure that enables the Backlinks Panel (Phase 06), Graph View (Phase 07-08), and Smart Linking (Phase 11).

## Prerequisites

- Phase 01 (WikiLink, WikiLinkParser, DocumentModel)
- Phase 02 (VaultService -- vault indexing, document lifecycle events)

## Feature References (PRD)

- PRD #1: Bidirectional Linking
- PRD #5: Backlinks Panel
- PRD #36: Backlink Visualization Per Note

## Data Structures to Implement

### File: `src/core/BacklinkIndex.h`

```cpp
#pragma once

#include "WikiLink.h"

#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class VaultService;

/// A single backlink entry: a reference from a source document to a target.
struct BacklinkEntry
{
    std::string source_document_id;   // Document containing the link
    std::string source_document_title;
    std::string target_document_id;   // Document being linked to
    WikiLink link;                    // The parsed wikilink
    std::string context;              // Surrounding text (1-2 sentences around the link)
    int line_number{0};               // Line number in source document

    [[nodiscard]] auto display_text() const -> std::string;
};

/// A mention is a text occurrence of a document's title or alias
/// that is NOT wrapped in [[wikilink]] syntax.
struct MentionEntry
{
    std::string source_document_id;
    std::string source_document_title;
    std::string mentioned_document_id;
    std::string matched_text;         // The text that matched
    std::string context;              // Surrounding text
    int line_number{0};
};

/// Result of querying backlinks for a specific document.
struct BacklinkResult
{
    std::string target_document_id;
    std::vector<BacklinkEntry> linked_mentions;    // Explicit [[wikilinks]]
    std::vector<MentionEntry> unlinked_mentions;   // Text mentions without link syntax

    [[nodiscard]] auto total_count() const -> int;
    [[nodiscard]] auto linked_count() const -> int;
    [[nodiscard]] auto unlinked_count() const -> int;
};

/// Maintains a bidirectional index of all wikilinks across the vault.
/// Supports incremental updates on document save/create/delete.
class BacklinkIndex
{
public:
    BacklinkIndex(EventBus& event_bus, VaultService& vault_service);

    /// Build the full backlink index from all documents in the vault.
    auto rebuild() -> void;

    /// Incrementally update the index for a single document.
    /// Removes old outgoing links and re-scans the document.
    auto update_document(const std::string& document_id) -> void;

    /// Remove all entries for a deleted document.
    auto remove_document(const std::string& document_id) -> void;

    // --- Queries ---

    /// Get all backlinks pointing to a document (explicit wikilinks only).
    [[nodiscard]] auto get_backlinks(const std::string& document_id) const
        -> std::vector<BacklinkEntry>;

    /// Get all backlinks + unlinked mentions for a document.
    [[nodiscard]] auto get_backlink_result(const std::string& document_id) const
        -> BacklinkResult;

    /// Get all outgoing links from a document.
    [[nodiscard]] auto get_outgoing_links(const std::string& document_id) const
        -> std::vector<BacklinkEntry>;

    /// Get all documents linked to or from a given document (union of backlinks + outgoing).
    [[nodiscard]] auto get_connected_documents(const std::string& document_id) const
        -> std::set<std::string>;

    /// Get the total number of backlinks to a document.
    [[nodiscard]] auto backlink_count(const std::string& document_id) const -> int;

    /// Get all unlinked mentions of a document's title/aliases across the vault.
    [[nodiscard]] auto get_unlinked_mentions(const std::string& document_id) const
        -> std::vector<MentionEntry>;

    /// Get orphan documents (no incoming or outgoing links).
    [[nodiscard]] auto get_orphan_documents() const -> std::vector<std::string>;

    /// Get the most-linked-to documents (sorted by backlink count descending).
    [[nodiscard]] auto get_most_linked(int limit = 20) const
        -> std::vector<std::pair<std::string, int>>;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;

    mutable std::mutex mutex_;

    // Forward index: source_doc_id -> [BacklinkEntry]
    std::unordered_map<std::string, std::vector<BacklinkEntry>> forward_index_;

    // Reverse index: target_doc_id -> [BacklinkEntry]
    std::unordered_map<std::string, std::vector<BacklinkEntry>> reverse_index_;

    auto add_link(const BacklinkEntry& entry) -> void;
    auto remove_links_from(const std::string& source_id) -> void;
    auto extract_context(std::string_view content, int line_number, int context_lines = 1) const
        -> std::string;
    auto scan_unlinked_mentions(const std::string& target_id,
                                 const std::string& target_title,
                                 const std::vector<std::string>& aliases) const
        -> std::vector<MentionEntry>;

    Subscription doc_saved_sub_;
    Subscription doc_created_sub_;
    Subscription doc_deleted_sub_;
    Subscription vault_opened_sub_;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`rebuild()`** -- Clear both indexes. Iterate all documents from VaultService::list_documents(). For each document, open and parse it, extract outgoing wikilinks, resolve each link to a target document ID, create BacklinkEntry with context. Build forward and reverse indexes. Publish BacklinkIndexRebuiltEvent.

2. **`update_document(document_id)`** -- Lock mutex. Call remove_links_from(document_id) to clear old entries. Re-scan the document's outgoing links. For each resolved link, add_link(). Publish BacklinkIndexUpdatedEvent.

3. **`get_backlinks(document_id)`** -- Lock mutex. Return reverse_index_[document_id] or empty vector.

4. **`get_backlink_result(document_id)`** -- Get explicit backlinks. Also call scan_unlinked_mentions() to find text mentions. Combine into BacklinkResult.

5. **`scan_unlinked_mentions(target_id, title, aliases)`** -- Iterate all documents (except the target itself). Search each document's markdown content for occurrences of the title or any alias that are NOT inside `[[...]]` syntax. For each occurrence, create MentionEntry with context.

6. **`get_orphan_documents()`** -- Find documents that have no entries in forward_index_ AND no entries in reverse_index_.

7. **`extract_context(content, line_number, context_lines)`** -- Extract the line at line_number plus context_lines before and after. Trim to reasonable length (200 chars).

8. **Event subscribers** -- On DocumentCreatedEvent/FileSavedEvent: call update_document(). On DocumentDeletedEvent: call remove_document(). On VaultOpenedEvent: call rebuild().

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BacklinkIndexRebuiltEvent)
int total_links{0};
int document_count{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BacklinkIndexUpdatedEvent)
std::string document_id;
int backlink_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BacklinksQueryEvent)
std::string document_id;
int result_count{0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_backlink_index.cpp`

1. **Simple backlink** -- Doc A contains `[[Doc B]]`. Verify get_backlinks("B") returns 1 entry from A.
2. **Multiple backlinks** -- Docs A, B, C all link to D. Verify get_backlinks("D") returns 3 entries.
3. **Bidirectional links** -- A links to B, B links to A. Verify both get_backlinks return 1 entry each.
4. **Context extraction** -- Verify BacklinkEntry.context contains surrounding text.
5. **Incremental update** -- A links to B. Update A to link to C instead. Verify B has 0 backlinks, C has 1.
6. **Remove document** -- Delete doc A which linked to B. Verify B's backlink from A is gone.
7. **Outgoing links** -- Doc A has `[[B]]` and `[[C]]`. get_outgoing_links("A") returns 2 entries.
8. **Connected documents** -- A->B, C->A. get_connected_documents("A") returns {B, C}.
9. **Orphan detection** -- Doc X has no links to or from it. Verify X appears in get_orphan_documents().
10. **Unlinked mentions** -- Doc titled "React" exists. Doc B contains text "React is great" without `[[React]]`. Verify get_unlinked_mentions returns 1 MentionEntry.
11. **Most linked** -- Multiple docs with varying backlink counts. get_most_linked(3) returns top 3 sorted descending.
12. **Rebuild clears stale data** -- Modify links, rebuild. Verify index reflects current state.

## Acceptance Criteria

- [ ] Forward and reverse indexes maintain consistent bidirectional mapping
- [ ] Incremental updates correctly remove old links and add new ones
- [ ] Context extraction provides meaningful surrounding text for each backlink
- [ ] Unlinked mention detection finds title/alias occurrences outside of `[[...]]`
- [ ] Orphan document detection returns documents with zero connections
- [ ] Thread-safe access via mutex
- [ ] EventBus subscriptions auto-update index on document changes
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/BacklinkIndex.h` | BacklinkEntry, MentionEntry, BacklinkResult, BacklinkIndex |
| CREATE | `src/core/BacklinkIndex.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 3 backlink events |
| MODIFY | `src/core/PluginContext.h` | Add `BacklinkIndex* backlink_index{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add BacklinkIndex.cpp |
| CREATE | `tests/unit/test_backlink_index.cpp` | 12 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_backlink_index target |

## Architecture Notes

- BacklinkIndex is injected via PluginContext for extension access
- It subscribes to document lifecycle events (EventBus) for automatic incremental updates
- The full rebuild() is called once on vault open; incremental updates handle changes after that
- Unlinked mention scanning is expensive (O(n*m) where n=docs, m=titles) -- consider async or on-demand
- Constructor injection: BacklinkIndex(EventBus&, VaultService&)

## Estimated Complexity

**L** -- Bidirectional index maintenance, incremental updates, unlinked mention scanning, context extraction, event-driven updates, 12 tests.
