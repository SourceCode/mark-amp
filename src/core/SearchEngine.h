#pragma once

#include "EventBus.h"

#include <chrono>
#include <cstdint>
#include <mutex>
#include <optional>
#include <regex>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class VaultService;

enum class SearchMethod : uint8_t
{
    Keyword, // AND all terms
    Phrase,  // Exact phrase
    Regex,   // Regular expression
    Fuzzy    // Fuzzy/approximate matching
};

enum class SearchSortOrder : uint8_t
{
    Relevance,
    ModifiedDesc,
    ModifiedAsc,
    CreatedDesc,
    CreatedAsc,
    Alphabetical
};

struct SearchScope
{
    std::vector<std::string> folders;         // Restrict to folders
    std::vector<std::string> tags;            // Require these tags
    std::vector<std::string> exclude_folders; // Exclude these folders
    std::string frontmatter_field;            // Filter by frontmatter field
    std::string frontmatter_value;            // Required field value
};

struct SearchQuery
{
    std::string query_string;
    SearchMethod method{SearchMethod::Keyword};
    SearchSortOrder sort{SearchSortOrder::Relevance};
    SearchScope scope;
    bool case_sensitive{false};
    int page{1};
    int page_size{20};
    int max_results{200};
};

struct HighlightSpan
{
    size_t start{0};
    size_t length{0};
};

struct SearchHit
{
    std::string document_id;
    std::string document_title;
    std::string file_path;
    std::string snippet;                   // Context around match
    std::string highlighted_snippet;       // With <mark> tags
    std::vector<HighlightSpan> highlights; // Offset/length pairs
    double score{0.0};                     // TF-IDF relevance score
    int match_count{0};                    // Number of matches in this document
    int line_number{0};                    // First match line
};

struct SearchResult
{
    std::vector<SearchHit> hits;
    int total_count{0};
    int page{1};
    int page_size{20};
    double elapsed_ms{0.0};
    SearchQuery query;
};

/// In-process inverted index for full-text search.
/// Uses TF-IDF scoring with BM25 ranking.
class SearchEngine
{
public:
    SearchEngine(EventBus& event_bus, VaultService& vault_service);

    /// Build the full search index from all vault documents.
    auto rebuild_index() -> void;

    /// Incrementally update the index for a single document.
    auto index_document(const std::string& document_id) -> void;

    /// Remove a document from the index.
    auto remove_document(const std::string& document_id) -> void;

    /// Execute a search query.
    [[nodiscard]] auto search(const SearchQuery& query) -> SearchResult;

    /// Get search suggestions/completions for a prefix.
    [[nodiscard]] auto suggest(const std::string& prefix, int limit = 10) const
        -> std::vector<std::string>;

    /// Get index statistics.
    [[nodiscard]] auto index_stats() const -> std::pair<int, int>; // (doc_count, term_count)

    // ── Batch 19-22 (#135-136) ──

    /// (#135) Return the number of indexed documents.
    [[nodiscard]] auto document_count() const -> int;

    /// (#136) Return the number of unique indexed terms.
    [[nodiscard]] auto term_count() const -> int;

    /// (#171) Return the average document length for BM25 scoring.
    [[nodiscard]] auto avg_doc_length() const -> double;

    /// (#172) Check if a specific document is present in the index.
    [[nodiscard]] auto has_document(const std::string& document_id) const -> bool;

    /// (#173) Check if the search index has any documents.
    [[nodiscard]] auto is_indexed() const -> bool;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;

    mutable std::mutex mutex_;

    // Inverted index: term -> { doc_id -> [positions] }
    struct PostingList
    {
        std::unordered_map<std::string, std::vector<int>> doc_positions;
        int total_frequency{0}; // Total occurrences across all docs
    };
    std::unordered_map<std::string, PostingList> inverted_index_;

    // Document metadata for scoring
    struct DocMeta
    {
        std::string title;
        std::string file_path;
        std::string content; // Raw content for snippet extraction and regex
        int total_terms{0};
        int64_t modified_time{0};
    };
    std::unordered_map<std::string, DocMeta> doc_meta_;

    int total_documents_{0};
    double avg_doc_length_{0.0};

    // Tokenization
    [[nodiscard]] auto tokenize(std::string_view text) const -> std::vector<std::string>;
    [[nodiscard]] auto normalize_token(std::string_view token) const -> std::string;

    // Search methods
    [[nodiscard]] auto search_keyword(const SearchQuery& query) -> std::vector<SearchHit>;
    [[nodiscard]] auto search_phrase(const SearchQuery& query) -> std::vector<SearchHit>;
    [[nodiscard]] auto search_regex(const SearchQuery& query) -> std::vector<SearchHit>;
    [[nodiscard]] auto search_fuzzy(const SearchQuery& query) -> std::vector<SearchHit>;

    // Scoring
    [[nodiscard]] auto bm25_score(const std::string& term, const std::string& doc_id) const
        -> double;

    // Snippet extraction
    [[nodiscard]] auto extract_snippet(const std::string& doc_id,
                                       const std::vector<std::string>& terms,
                                       int context_chars = 80) const -> std::string;

    // Edit distance for fuzzy matching
    [[nodiscard]] static auto levenshtein_distance(std::string_view str_a, std::string_view str_b)
        -> int;

    auto apply_scope(std::vector<SearchHit>& hits, const SearchScope& scope) const -> void;

    auto recompute_avg_doc_length() -> void;

    Subscription doc_created_sub_;
    Subscription doc_deleted_sub_;
    Subscription vault_opened_sub_;
};

} // namespace markamp::core
