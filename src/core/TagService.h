#pragma once

#include "EventBus.h"
#include "WikiLink.h"

#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class VaultService;

/// A node in the tag hierarchy tree.
struct TagTreeNode
{
    std::string name;                  // This level's tag name
    std::string full_path;             // Full tag path (e.g., "project/alpha")
    int document_count{0};             // Documents directly tagged with this tag
    int recursive_count{0};            // Documents tagged with this or any descendant
    std::vector<TagTreeNode> children; // Child tag nodes

    [[nodiscard]] auto is_leaf() const -> bool;
    [[nodiscard]] auto child_count() const -> int;
};

/// Statistics for a single tag.
struct TagStats
{
    std::string tag;
    int count{0};                          // Number of documents with this tag
    std::vector<std::string> document_ids; // Document IDs with this tag
};

/// Metadata field value statistics for frontmatter fields.
struct MetadataFieldStats
{
    std::string field_name;
    std::map<std::string, int> value_counts; // value -> number of docs with that value
    int total_documents{0};                  // Docs that have this field at all
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
    [[nodiscard]] auto get_documents_for_tag(const std::string& tag, bool recursive = false) const
        -> std::vector<std::string>;

    /// Find tags matching a prefix (for autocomplete).
    [[nodiscard]] auto find_tags_by_prefix(const std::string& prefix) const
        -> std::vector<std::string>;

    /// Get the total number of unique tags.
    [[nodiscard]] auto tag_count() const -> int;

    // --- Metadata Queries ---

    /// Get all unique values for a frontmatter field across the vault.
    [[nodiscard]] auto get_field_values(const std::string& field_name) const -> MetadataFieldStats;

    /// Find documents where a frontmatter field matches a value.
    [[nodiscard]] auto find_by_field(const std::string& field_name, const std::string& value) const
        -> std::vector<std::string>;

    /// Get all frontmatter field names used across the vault.
    [[nodiscard]] auto get_all_field_names() const -> std::vector<std::string>;

    // --- Tag Operations ---

    /// Rename a tag across all documents in the vault.
    [[nodiscard]] auto rename_tag(const std::string& old_tag, const std::string& new_tag) -> int;

    /// Delete a tag from all documents.
    [[nodiscard]] auto delete_tag(const std::string& tag) -> int;

    /// Merge two tags (all occurrences of source become target).
    [[nodiscard]] auto merge_tags(const std::string& source_tag, const std::string& target_tag)
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
    auto compute_recursive_count(TagTreeNode& node) const -> void;

    Subscription vault_opened_sub_;
    Subscription doc_created_sub_;
    Subscription doc_deleted_sub_;
};

} // namespace markamp::core
