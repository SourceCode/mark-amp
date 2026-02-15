# Phase 04 -- Tag System and Metadata Engine

## Objective

Implement a dedicated TagService that maintains a vault-wide tag index with hierarchical nested tag support, tag pane data model, and metadata query engine. Allows querying documents by tag, browsing the tag hierarchy, and computing tag statistics. This powers the Tag Pane/Explorer (PRD #20), nested tags (#parent/child), and metadata-driven filtering.

## Prerequisites

- Phase 01 (ParsedTag, Frontmatter, WikiLinkParser)
- Phase 02 (VaultService -- document index)

## Feature References (PRD)

- PRD #9: Tagging System
- PRD #20: Tag Pane and Explorer
- PRD #23: Custom Metadata / YAML Frontmatter
- PRD #32: Nested Tags

## Data Structures to Implement

### File: `src/core/TagService.h`

```cpp
#pragma once

#include "WikiLink.h"

#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class VaultService;

/// A node in the tag hierarchy tree.
struct TagTreeNode
{
    std::string name;                          // This level's tag name
    std::string full_path;                     // Full tag path (e.g., "project/alpha")
    int document_count{0};                     // Documents directly tagged with this tag
    int recursive_count{0};                    // Documents tagged with this or any descendant
    std::vector<TagTreeNode> children;         // Child tag nodes

    [[nodiscard]] auto is_leaf() const -> bool;
    [[nodiscard]] auto child_count() const -> int;
};

/// Statistics for a single tag.
struct TagStats
{
    std::string tag;
    int count{0};                              // Number of documents with this tag
    std::vector<std::string> document_ids;     // Document IDs with this tag
};

/// Metadata field value statistics for frontmatter fields.
struct MetadataFieldStats
{
    std::string field_name;
    std::map<std::string, int> value_counts;   // value -> number of docs with that value
    int total_documents{0};                    // Docs that have this field at all
};

/// Manages the vault-wide tag index and metadata queries.
class TagService
{
public:
    TagService(EventBus& event_bus, VaultService& vault_service);

    /// Rebuild the tag index from all vault documents.
    auto rebuild() -> void;

    /// Update tag index for a single document.
    auto update_document(const std::string& document_id) -> void;

    /// Remove a document from the tag index.
    auto remove_document(const std::string& document_id) -> void;

    // --- Tag Queries ---

    /// Get the full tag hierarchy tree.
    [[nodiscard]] auto get_tag_tree() const -> std::vector<TagTreeNode>;

    /// Get all tags as a flat sorted list with counts.
    [[nodiscard]] auto get_all_tags() const -> std::vector<TagStats>;

    /// Get documents for a specific tag (including descendants if recursive=true).
    [[nodiscard]] auto get_documents_for_tag(const std::string& tag,
                                              bool recursive = false) const
        -> std::vector<std::string>;

    /// Find tags matching a prefix (for autocomplete).
    [[nodiscard]] auto find_tags_by_prefix(const std::string& prefix) const
        -> std::vector<std::string>;

    /// Get the total number of unique tags.
    [[nodiscard]] auto tag_count() const -> int;

    // --- Metadata Queries ---

    /// Get all unique values for a frontmatter field across the vault.
    [[nodiscard]] auto get_field_values(const std::string& field_name) const
        -> MetadataFieldStats;

    /// Find documents where a frontmatter field matches a value.
    [[nodiscard]] auto find_by_field(const std::string& field_name,
                                      const std::string& value) const
        -> std::vector<std::string>;

    /// Get all frontmatter field names used across the vault.
    [[nodiscard]] auto get_all_field_names() const -> std::vector<std::string>;

    // --- Tag Operations ---

    /// Rename a tag across all documents in the vault.
    [[nodiscard]] auto rename_tag(const std::string& old_tag, const std::string& new_tag)
        -> int;  // Returns number of documents modified

    /// Delete a tag from all documents.
    [[nodiscard]] auto delete_tag(const std::string& tag) -> int;

    /// Merge two tags (all occurrences of source become target).
    [[nodiscard]] auto merge_tags(const std::string& source_tag,
                                   const std::string& target_tag)
        -> int;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;

    mutable std::mutex mutex_;

    // tag_path -> set of document_ids
    std::unordered_map<std::string, std::vector<std::string>> tag_to_docs_;

    // document_id -> set of tag_paths
    std::unordered_map<std::string, std::vector<std::string>> doc_to_tags_;

    // field_name -> { value -> [doc_ids] }
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>
        metadata_index_;

    auto build_tag_tree_recursive(const std::string& prefix) const -> std::vector<TagTreeNode>;

    Subscription doc_saved_sub_;
    Subscription doc_created_sub_;
    Subscription doc_deleted_sub_;
    Subscription vault_opened_sub_;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`rebuild()`** -- Clear all indexes. Iterate VaultService::list_documents(). For each, load tags from frontmatter + inline. Also index all frontmatter fields into metadata_index_. Publish TagIndexRebuiltEvent.

2. **`get_tag_tree()`** -- Build a hierarchical tree from flat tag paths. "project/alpha" becomes TagTreeNode("project") with child TagTreeNode("alpha"). Compute recursive_count by summing descendant document_counts.

3. **`get_documents_for_tag(tag, recursive)`** -- If not recursive: return tag_to_docs_[tag]. If recursive: collect all tags starting with tag + "/" prefix and union their document sets.

4. **`rename_tag(old_tag, new_tag)`** -- For each document with old_tag: modify the document's markdown (replace #old_tag with #new_tag or update frontmatter tags array). Save document. Update index. Return count of modified documents.

5. **`find_tags_by_prefix(prefix)`** -- Iterate tag_to_docs_ keys. Return those starting with prefix, sorted alphabetically. Used for tag autocomplete in the editor.

6. **`get_field_values(field_name)`** -- Return MetadataFieldStats with all distinct values for a frontmatter field and their document counts.

7. **`find_by_field(field_name, value)`** -- Return document IDs where frontmatter[field_name] == value.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagIndexRebuiltEvent)
int unique_tags{0};
int total_tag_assignments{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagRenamedEvent)
std::string old_tag;
std::string new_tag;
int documents_modified{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagDeletedEvent)
std::string tag;
int documents_modified{0};
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `tags.nested_separator` | string | `"/"` | Separator for nested tag hierarchy |
| `tags.case_sensitive` | bool | `false` | Case-sensitive tag matching |
| `tags.show_count` | bool | `true` | Show document count in tag pane |
| `tags.sort_by` | string | `"name"` | Sort order: name, count, recent |

## Test Cases

File: `tests/unit/test_tag_service.cpp`

1. **Simple tag indexing** -- 3 docs with tags. Verify get_all_tags() returns correct counts.
2. **Nested tag hierarchy** -- Tags: #project/alpha, #project/beta. Verify tree has "project" parent with 2 children.
3. **Recursive document query** -- Query #project with recursive=true. Returns docs tagged with any #project/* sub-tag.
4. **Tag prefix search** -- Tags: #react, #redux, #python. find_tags_by_prefix("re") returns ["react", "redux"].
5. **Tag rename** -- Rename #old to #new. Verify all documents updated. Verify index updated.
6. **Tag delete** -- Delete #obsolete. Verify removed from all documents.
7. **Tag merge** -- Merge #frontend into #ui. Verify docs with #frontend now have #ui.
8. **Metadata field query** -- Docs with status: "draft" and "published". get_field_values("status") returns both with counts.
9. **Find by field** -- find_by_field("status", "draft") returns correct doc IDs.
10. **Incremental update** -- Add tag to document. update_document(). Verify tag index reflects change.
11. **Remove document** -- Remove doc. Verify its tags are decremented in the index.
12. **Tag tree recursive count** -- Parent has 2 docs, child has 3 docs. Parent recursive_count == 5.

## Acceptance Criteria

- [ ] Tag index correctly maps tags to documents and vice versa
- [ ] Nested tag hierarchy builds correct tree structure
- [ ] Recursive tag queries include all descendant tags
- [ ] Tag rename/delete/merge modify document content on disk
- [ ] Metadata index supports querying by any frontmatter field
- [ ] Prefix search enables autocomplete
- [ ] Incremental updates work on document save
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/TagService.h` | TagService, TagTreeNode, TagStats, MetadataFieldStats |
| CREATE | `src/core/TagService.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 3 tag events |
| MODIFY | `src/core/PluginContext.h` | Add `TagService* tag_service{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add TagService.cpp |
| CREATE | `tests/unit/test_tag_service.cpp` | 12 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_tag_service target |

## Architecture Notes

- TagService subscribes to document lifecycle events for auto-update
- Tag operations (rename/delete/merge) modify documents through VaultService
- The metadata index is a secondary concern but shares the same rebuild cycle
- Constructor injection: TagService(EventBus&, VaultService&)

## Estimated Complexity

**M** -- Index data structures, tree building, tag operations that modify documents, metadata queries, 12 tests.
