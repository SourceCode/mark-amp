#pragma once

#include "EventBus.h"
#include "Search.h"

#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace markamp::core
{

class Config;

/// Full-text search service for all knowledgebase content.
/// Supports keyword, phrase, regex, and SQL search methods with
/// FTS5 backing, pagination, snippets, and highlighting.
class SearchService
{
public:
    SearchService(EventBus& event_bus, Config& config);

    /// Execute a full-text search against the block database.
    [[nodiscard]] auto search(const SearchQuery& query) -> SearchResult;

    /// FTS5 keyword search (AND all terms).
    [[nodiscard]] auto search_keyword(const SearchQuery& query) -> SearchResult;

    /// FTS5 phrase search (exact sequence).
    [[nodiscard]] auto search_phrase(const SearchQuery& query) -> SearchResult;

    /// Regex search (client-side pattern matching).
    [[nodiscard]] auto search_regex(const SearchQuery& query) -> SearchResult;

    /// Search for ref-target blocks (for (( autocomplete)).
    [[nodiscard]] auto search_ref_blocks(const std::string& query) -> std::vector<SearchHit>;

    /// Search for embed-target blocks (for {{ autocomplete)).
    [[nodiscard]] auto search_embed_blocks(const std::string& query) -> std::vector<SearchHit>;

    /// Search template files by name.
    [[nodiscard]] auto search_templates(const std::string& query) -> std::vector<std::string>;

    /// Search asset files by filename.
    [[nodiscard]] auto search_assets(const std::string& query) -> std::vector<std::string>;

    /// Update the FTS5 index for a single block.
    void update_fts_index(const BlockId& block_id, std::string_view content);

    /// Full rebuild of the FTS5 index from the blocks table.
    void rebuild_fts_index();

    /// Return (total_indexed_blocks, total_indexed_terms) for diagnostics.
    [[nodiscard]] auto get_search_statistics() -> std::pair<int, int>;

    /// Initialize the FTS5 virtual table with tokenizer configuration.
    void configure_fts();

    // ── Batch 19-22 (#137-139) ──

    /// (#137) Return the number of groups in a search result.
    [[nodiscard]] static auto result_group_count(const SearchResult& result) -> std::size_t;

    /// (#138) Check if a search result has any document groups.
    [[nodiscard]] static auto is_grouped(const SearchResult& result) -> bool;

    /// (#139) Return the total hit count from a search result.
    [[nodiscard]] static auto total_hit_count(const SearchResult& result) -> std::size_t;

    /// (#174) Check if the FTS index has been initialized.
    [[nodiscard]] auto has_fts_index() const -> bool;

    /// (#175) Return the number of indexed blocks.
    [[nodiscard]] auto indexed_block_count() const -> int;

private:
    EventBus& event_bus_;
    Config& config_;
    FtsQueryBuilder query_builder_;
    SnippetExtractor snippet_extractor_;
    mutable std::mutex search_mutex_;

    /// Apply type, box, and path filters to a result set.
    void apply_filters(SearchResult& result, const SearchQuery& query) const;

    /// Sort results according to the specified order.
    void sort_results(SearchResult& result, SearchSortOrder order) const;

    /// Paginate results.
    void paginate_results(SearchResult& result, int page, int page_size) const;

    /// Group results by document.
    void group_by_document(SearchResult& result) const;
};

} // namespace markamp::core
