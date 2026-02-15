#pragma once

#include "BlockRef.h"

#include <chrono>
#include <cstdint>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Search method enumeration.
enum class SearchMethod : std::uint8_t
{
    Keyword, // AND all terms (default)
    Phrase,  // Exact phrase match
    Regex,   // Regular expression
    Sql      // Raw SQL WHERE clause (advanced)
};

/// Block types for search filtering.
enum class SearchBlockType : std::uint8_t
{
    Document = 0,
    Heading = 1,
    Paragraph = 2,
    MathBlock = 3,
    Table = 4,
    CodeBlock = 5,
    HtmlBlock = 6,
    List = 7,
    ListItem = 8,
    Blockquote = 9,
    SuperBlock = 10,
    EmbedBlock = 11
};

/// Sort order for search results.
enum class SearchSortOrder : std::uint8_t
{
    BlockType = 0,
    CreatedAsc = 1,
    CreatedDesc = 2,
    UpdatedAsc = 3,
    UpdatedDesc = 4,
    SortAsc = 5,
    SortDesc = 6,
    Relevance = 7
};

/// Grouping mode for search results.
enum class SearchGroupBy : std::uint8_t
{
    NoGroup = 0,
    ByDocument = 1
};

/// A search query with all parameters.
struct SearchQuery
{
    /// The search string.
    std::string query_string;

    /// Search method.
    SearchMethod method{SearchMethod::Keyword};

    /// Block types to include (empty = all types).
    std::set<SearchBlockType> types;

    /// Notebook IDs to search within (empty = all notebooks).
    std::vector<std::string> boxes;

    /// Path prefixes to search within (empty = all paths).
    std::vector<std::string> paths;

    /// Grouping mode.
    SearchGroupBy group_by{SearchGroupBy::NoGroup};

    /// Sort order.
    SearchSortOrder order_by{SearchSortOrder::Relevance};

    /// Pagination.
    int page{1};
    int page_size{20};

    /// Case sensitivity (only for Keyword and Phrase methods).
    bool case_sensitive{false};
};

/// A highlighted range within a text string.
struct HighlightRange
{
    std::size_t start{0};
    std::size_t end{0};
};

/// A single search hit (a matching block).
struct SearchHit
{
    /// The matching block ID.
    BlockId block_id;

    /// The block type.
    SearchBlockType block_type{SearchBlockType::Paragraph};

    /// The root document ID.
    std::string root_id;

    /// The root document title.
    std::string doc_title;

    /// The full block content.
    std::string content;

    /// Content snippet with match context (trimmed around matches).
    std::string snippet;

    /// HTML snippet with <mark> tags around matched terms.
    std::string highlighted_snippet;

    /// Highlight ranges within the content (for non-HTML rendering).
    std::vector<HighlightRange> highlights;

    /// FTS5 relevance score (lower = more relevant).
    double score{0.0};

    /// Block path in the document hierarchy.
    std::string block_path;

    /// Block creation timestamp.
    std::chrono::system_clock::time_point created_at;

    /// Block last update timestamp.
    std::chrono::system_clock::time_point updated_at;
};

/// A group of search hits from the same document.
struct SearchGroup
{
    /// The document root ID.
    std::string root_id;

    /// The document title.
    std::string doc_title;

    /// The document path.
    std::string doc_path;

    /// Hits within this document.
    std::vector<SearchHit> hits;

    /// Total hits in this document (may exceed hits.size() if paginated).
    int total_hits{0};
};

/// Complete search result set.
struct SearchResult
{
    /// Flat list of search hits (when group_by == NoGroup).
    std::vector<SearchHit> hits;

    /// Grouped hits by document (when group_by == ByDocument).
    std::vector<SearchGroup> groups;

    /// Total number of matching blocks (before pagination).
    int total_count{0};

    /// Current page number.
    int page{1};

    /// Page size used.
    int page_size{20};

    /// Search execution time in milliseconds.
    double elapsed_ms{0.0};

    /// The original query.
    SearchQuery query;
};

/// Builds FTS5 match expressions from user query strings.
class FtsQueryBuilder
{
public:
    /// Build an FTS5 MATCH expression for keyword search.
    /// Input: "foo bar" -> Output: "foo AND bar"
    [[nodiscard]] auto build_keyword_query(const std::string& input) const -> std::string;

    /// Build an FTS5 MATCH expression for phrase search.
    /// Input: "foo bar" -> Output: "\"foo bar\""
    [[nodiscard]] auto build_phrase_query(const std::string& input) const -> std::string;

    /// Validate a regex pattern (compile check without executing).
    [[nodiscard]] auto validate_regex(const std::string& pattern) const -> bool;

    /// Escape special FTS5 characters in a query string.
    [[nodiscard]] auto escape_fts(const std::string& input) const -> std::string;
};

/// Extracts snippets from content around match positions.
class SnippetExtractor
{
public:
    /// Extract a context snippet around matched ranges.
    [[nodiscard]] auto extract(std::string_view content,
                               const std::vector<HighlightRange>& matches,
                               int context_chars = 60) const -> std::string;

    /// Build an HTML snippet with <mark> tags around matched ranges.
    [[nodiscard]] auto highlight_html(std::string_view content,
                                      const std::vector<HighlightRange>& matches) const
        -> std::string;
};

} // namespace markamp::core
