#pragma once

#include "EventBus.h"
#include "WikiLink.h"

#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class VaultService;

/// A single backlink entry: a reference from a source document to a target.
struct BacklinkEntry
{
    std::string source_document_id; // Document containing the link
    std::string source_document_title;
    std::string target_document_id; // Document being linked to
    WikiLink link;                  // The parsed wikilink
    std::string context;            // Surrounding text (1-2 sentences around the link)
    int line_number{0};             // Line number in source document

    [[nodiscard]] auto display_text() const -> std::string;
};

/// A mention is a text occurrence of a document's title or alias
/// that is NOT wrapped in [[wikilink]] syntax.
struct MentionEntry
{
    std::string source_document_id;
    std::string source_document_title;
    std::string mentioned_document_id;
    std::string matched_text; // The text that matched
    std::string context;      // Surrounding text
    int line_number{0};
};

/// Result of querying backlinks for a specific document.
struct BacklinkResult
{
    std::string target_document_id;
    std::vector<BacklinkEntry> linked_mentions;  // Explicit [[wikilinks]]
    std::vector<MentionEntry> unlinked_mentions; // Text mentions without link syntax

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
    [[nodiscard]] auto get_backlink_result(const std::string& document_id) const -> BacklinkResult;

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

    /// (#109) Return the number of documents tracked in the forward index.
    [[nodiscard]] auto indexed_document_count() const -> std::size_t;

    /// (#110) Return the total number of links across all documents.
    [[nodiscard]] auto total_link_count() const -> std::size_t;

    /// (#168) Check if a document has any backlinks (incoming wikilinks).
    [[nodiscard]] auto has_backlinks(const std::string& document_id) const -> bool;

    /// (#169) Return the number of orphan documents.
    [[nodiscard]] auto orphan_count() const -> std::size_t;

    /// (#170) Return the number of outgoing links from a document.
    [[nodiscard]] auto forward_link_count(const std::string& document_id) const -> std::size_t;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;

    mutable std::mutex mutex_;

    // Forward index: source_doc_id -> [BacklinkEntry]
    std::unordered_map<std::string, std::vector<BacklinkEntry>> forward_index_;

    // Reverse index: target_doc_id -> [BacklinkEntry]
    std::unordered_map<std::string, std::vector<BacklinkEntry>> reverse_index_;

    /// Insert a link entry into both forward and reverse indexes.
    auto add_link(const BacklinkEntry& entry) -> void;

    /// Remove all outgoing link entries from a given source document.
    auto remove_links_from(const std::string& source_id) -> void;

    /// Extract surrounding text for context.
    [[nodiscard]] static auto extract_context(const std::string& content,
                                              int line_number,
                                              int context_lines = 1) -> std::string;

    /// Find the line number of a wikilink occurrence within document content.
    [[nodiscard]] static auto find_link_line(const std::string& content, const WikiLink& link)
        -> int;

    /// Scan for unlinked mentions of a title/aliases across all vault documents.
    [[nodiscard]] auto scan_unlinked_mentions(const std::string& target_id,
                                              const std::string& target_title,
                                              const std::vector<std::string>& aliases) const
        -> std::vector<MentionEntry>;

    /// Convert to lowercase for case-insensitive matching.
    [[nodiscard]] static auto to_lower(const std::string& str) -> std::string;

    Subscription vault_opened_sub_;
    Subscription doc_created_sub_;
    Subscription doc_deleted_sub_;
};

} // namespace markamp::core
