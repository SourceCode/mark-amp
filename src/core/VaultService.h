#pragma once

#include "DocumentModel.h"
#include "EventBus.h"
#include "WikiLinkParser.h"

#include <expected>
#include <filesystem>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class Config;

/// Index entry for fast lookup of documents by various keys.
struct VaultIndexEntry
{
    std::string document_id;
    std::string title;
    std::filesystem::path file_path;
    std::string notebook_id;
    std::vector<std::string> aliases;
    std::vector<std::string> tags;
    int64_t modified_time{0}; // Unix milliseconds
    int64_t file_size{0};

    /// Check if name matches title or any alias (case-insensitive).
    [[nodiscard]] auto matches_name(const std::string& name) const -> bool;
};

/// Manages the vault: file discovery, document indexing, lifecycle, link resolution.
class VaultService
{
public:
    VaultService(EventBus& event_bus, Config& config);
    ~VaultService();

    // Non-copyable, non-movable
    VaultService(const VaultService&) = delete;
    auto operator=(const VaultService&) -> VaultService& = delete;
    VaultService(VaultService&&) = delete;
    auto operator=(VaultService&&) -> VaultService& = delete;

    /// Open a vault directory. Scans all .md files and builds the index.
    [[nodiscard]] auto open_vault(const std::filesystem::path& vault_path)
        -> std::expected<void, std::string>;

    /// Close the current vault and release all resources.
    auto close_vault() -> void;

    /// Get the current vault root path.
    [[nodiscard]] auto vault_path() const -> const std::filesystem::path&;

    /// Check if a vault is currently open.
    [[nodiscard]] auto is_open() const -> bool;

    // --- Document Lifecycle ---

    /// Create a new document in the vault.
    [[nodiscard]] auto create_document(const std::string& title,
                                       const std::string& folder = "",
                                       const Frontmatter& frontmatter = {})
        -> std::expected<std::shared_ptr<DocumentModel>, std::string>;

    /// Open/load a document by file path (relative to vault root).
    [[nodiscard]] auto open_document(const std::filesystem::path& relative_path)
        -> std::expected<std::shared_ptr<DocumentModel>, std::string>;

    /// Save a document to disk.
    [[nodiscard]] auto save_document(const std::string& document_id)
        -> std::expected<void, std::string>;

    /// Delete a document from the vault.
    [[nodiscard]] auto delete_document(const std::string& document_id)
        -> std::expected<void, std::string>;

    /// Rename/move a document within the vault. Updates all wikilinks referencing it.
    [[nodiscard]] auto rename_document(const std::string& document_id, const std::string& new_title)
        -> std::expected<void, std::string>;

    // --- Index Queries ---

    /// Find a document by title or alias (for wikilink resolution).
    [[nodiscard]] auto find_by_name(const std::string& name) const
        -> std::optional<VaultIndexEntry>;

    /// Find all documents matching a tag.
    [[nodiscard]] auto find_by_tag(const std::string& tag) const -> std::vector<VaultIndexEntry>;

    /// List all documents in the vault.
    [[nodiscard]] auto list_documents() const -> std::vector<VaultIndexEntry>;

    /// List all unique tags across the vault.
    [[nodiscard]] auto list_all_tags() const -> std::vector<std::string>;

    /// List all unique tag hierarchies as a tree structure.
    [[nodiscard]] auto list_tag_tree() const
        -> std::vector<std::pair<std::string, std::vector<std::string>>>;

    /// Get the number of indexed documents.
    [[nodiscard]] auto document_count() const -> int;

    // --- Link Resolution ---

    /// Resolve a wikilink target to a document ID.
    [[nodiscard]] auto resolve_wikilink(const std::string& target) const
        -> std::optional<std::string>;

    /// Get all unresolved (broken) wikilinks in the vault.
    [[nodiscard]] auto find_broken_links() const -> std::vector<std::pair<std::string, WikiLink>>;

    // --- File Watching ---

    /// Start watching the vault directory for external file changes.
    auto start_file_watcher() -> void;

    /// Stop the file watcher.
    auto stop_file_watcher() -> void;

    // --- Re-indexing ---

    /// Full re-index of the vault (scans all files).
    auto reindex() -> void;

    /// Incremental re-index of a single document.
    auto reindex_document(const std::string& document_id) -> void;

private:
    EventBus& event_bus_;
    Config& config_;

    std::filesystem::path vault_path_;
    bool is_open_{false};

    mutable std::shared_mutex index_mutex_;
    std::unordered_map<std::string, VaultIndexEntry> index_by_id_;
    std::unordered_map<std::string, std::string> name_to_id_; // title/alias -> doc ID
    std::unordered_map<std::string, std::vector<std::string>> tag_to_ids_;

    std::unordered_map<std::string, std::shared_ptr<DocumentModel>> open_documents_;

    WikiLinkParser link_parser_;

    // File watcher state
    bool watcher_running_{false};

    /// Recursively scan vault directory for .md files, skipping hidden/excluded dirs.
    [[nodiscard]] auto scan_vault_directory() -> std::vector<std::filesystem::path>;

    /// Parse and index a single file. Returns error if file cannot be read.
    [[nodiscard]] auto index_file(const std::filesystem::path& file_path)
        -> std::expected<void, std::string>;

    /// Remove a document from all index maps.
    auto remove_from_index(const std::string& document_id) -> void;

    /// Rebuild the name -> id lookup map from index_by_id_.
    auto rebuild_name_index() -> void;

    /// Rebuild the tag -> ids lookup map from index_by_id_.
    auto rebuild_tag_index() -> void;

    /// Convert a string to lowercase for case-insensitive matching.
    [[nodiscard]] static auto to_lower(const std::string& str) -> std::string;

    /// Extract title from markdown content (frontmatter title, first H1, or filename).
    [[nodiscard]] static auto extract_title(const std::string& content,
                                            const std::filesystem::path& file_path,
                                            const Frontmatter& frontmatter) -> std::string;
};

} // namespace markamp::core
