# Phase 05 -- Full-Text Search Engine and Index

## Objective

Implement vault-wide full-text search using an in-process inverted index (no SQLite dependency for v4 -- pure C++ implementation). Supports keyword AND search, phrase search, fuzzy matching, and regex search. Integrates with VaultService for incremental index updates on document changes. This replaces the v3 FTS5-based search with a self-contained solution suitable for the Obsidian-style vault model.

## Prerequisites

- Phase 01 (DocumentModel, Frontmatter)
- Phase 02 (VaultService -- document listing and lifecycle events)

## Feature References (PRD)

- PRD #7: Full-Text Search
- PRD #24: Search Filters (regex, boolean, scoped)

## Data Structures to Implement

### File: `src/core/SearchEngine.h`

```cpp
#pragma once

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
    Keyword,    // AND all terms
    Phrase,     // Exact phrase
    Regex,      // Regular expression
    Fuzzy       // Fuzzy/approximate matching
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
    std::vector<std::string> folders;           // Restrict to folders
    std::vector<std::string> tags;              // Require these tags
    std::vector<std::string> exclude_folders;   // Exclude these folders
    std::string frontmatter_field;              // Filter by frontmatter field
    std::string frontmatter_value;              // Required field value
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
    std::string snippet;                      // Context around match
    std::string highlighted_snippet;          // With <mark> tags
    std::vector<HighlightSpan> highlights;    // Offset/length pairs
    double score{0.0};                        // TF-IDF relevance score
    int match_count{0};                       // Number of matches in this document
    int line_number{0};                       // First match line
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

private:
    EventBus& event_bus_;
    VaultService& vault_service_;

    mutable std::mutex mutex_;

    // Inverted index: term -> { doc_id -> [positions] }
    struct PostingList
    {
        std::unordered_map<std::string, std::vector<int>> doc_positions;
        int total_frequency{0};  // Total occurrences across all docs
    };
    std::unordered_map<std::string, PostingList> inverted_index_;

    // Document metadata for scoring
    struct DocMeta
    {
        std::string title;
        std::string file_path;
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
    [[nodiscard]] auto bm25_score(const std::string& term,
                                   const std::string& doc_id) const -> double;

    // Snippet extraction
    [[nodiscard]] auto extract_snippet(const std::string& doc_id,
                                        const std::vector<std::string>& terms,
                                        int context_chars = 80) const -> std::string;

    auto apply_scope(std::vector<SearchHit>& hits, const SearchScope& scope) const -> void;

    Subscription doc_saved_sub_;
    Subscription doc_created_sub_;
    Subscription doc_deleted_sub_;
    Subscription vault_opened_sub_;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`rebuild_index()`** -- Clear inverted_index_ and doc_meta_. Iterate all vault documents. For each: read content, tokenize, build posting lists with term positions. Compute avg_doc_length_. Publish SearchIndexRebuiltEvent.

2. **`tokenize(text)`** -- Split text on whitespace and punctuation. Lowercase tokens (unless case_sensitive). Strip markdown syntax characters. Remove stop words. Return vector of normalized tokens with their positions.

3. **`search_keyword(query)`** -- Tokenize query string. For each term, look up posting list. Intersect document sets (AND semantics). Score each matching document using BM25. Sort by score. Extract snippets.

4. **`search_phrase(query)`** -- Tokenize query. Look up posting lists. Find documents where all terms appear and their positions are consecutive (position[i+1] == position[i] + 1).

5. **`search_regex(query)`** -- Compile regex pattern. Scan all indexed document content for matches. Score by match count.

6. **`search_fuzzy(query)`** -- For each term, find similar terms in the index using Levenshtein distance (edit distance <= 2). Expand the query to include fuzzy matches. Score with a penalty for fuzziness.

7. **`bm25_score(term, doc_id)`** -- Standard BM25 formula: score = IDF * (tf * (k1 + 1)) / (tf + k1 * (1 - b + b * dl/avgdl)). k1=1.2, b=0.75.

8. **`apply_scope(hits, scope)`** -- Filter hits by folder inclusion/exclusion, tag requirement, frontmatter field match. Remove non-matching hits.

9. **`extract_snippet(doc_id, terms, context_chars)`** -- Find the first occurrence of a query term in the document. Extract surrounding text. Highlight matches with `<mark>` tags.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchIndexRebuiltEvent)
int document_count{0};
int term_count{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchCompletedEvent)
std::string query_string;
int result_count{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GlobalSearchRequestEvent)
std::string query_string;
SearchMethod method{SearchMethod::Keyword};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_search_engine.cpp`

1. **Keyword AND search** -- Index 3 docs. Search "quick brown". Verify only docs with BOTH terms returned.
2. **Phrase search** -- Search "quick brown fox". Verify only docs with exact phrase returned.
3. **Regex search** -- Search `"\d{3}"`. Verify docs with 3-digit numbers found.
4. **Fuzzy search** -- Index "algorithm". Search "algoritm" (typo). Verify match found with fuzzy.
5. **BM25 relevance** -- Doc A mentions "test" 5 times. Doc B mentions once. Verify A scores higher.
6. **Case insensitive** -- Index "Hello World". Search "hello". Verify match found.
7. **Scope -- folder filter** -- 2 docs in /notes/, 1 in /archive/. Scope to /notes/. Verify 2 results.
8. **Scope -- tag filter** -- Scope search to docs with #project tag. Verify only tagged docs returned.
9. **Pagination** -- 25 results. page=2, page_size=10. Verify 10 results on page 2, total_count=25.
10. **Snippet extraction** -- Verify snippet contains query terms with surrounding context.
11. **Incremental index** -- Add new document. Index it. Search for its content. Verify found.
12. **Remove from index** -- Remove document. Search for its content. Verify not found.
13. **Suggestions** -- Index terms starting with "algo". suggest("al") returns "algorithm", etc.

## Acceptance Criteria

- [ ] Keyword AND search returns only documents containing all terms
- [ ] Phrase search matches exact token sequences
- [ ] Regex search compiles and applies patterns correctly
- [ ] BM25 scoring ranks more relevant documents higher
- [ ] Scope filters restrict results by folder, tag, frontmatter
- [ ] Pagination returns correct slices with accurate total_count
- [ ] Incremental indexing adds/removes documents correctly
- [ ] Snippet extraction highlights query terms in context
- [ ] All 13 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/SearchEngine.h` | SearchEngine, query types, result types |
| CREATE | `src/core/SearchEngine.cpp` | Full implementation with BM25 scoring |
| MODIFY | `src/core/Events.h` | Add 3 search events |
| MODIFY | `src/core/PluginContext.h` | Add `SearchEngine* search_engine{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add SearchEngine.cpp |
| CREATE | `tests/unit/test_search_engine.cpp` | 13 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_search_engine target |

## Architecture Notes

- Pure C++ implementation -- no SQLite dependency for the search index
- Inverted index is rebuilt on vault open, incrementally updated after
- BM25 parameters (k1=1.2, b=0.75) are standard defaults
- Constructor injection: SearchEngine(EventBus&, VaultService&)
- Thread safety via mutex on all index mutations and reads

## Estimated Complexity

**XL** -- Inverted index, BM25 scoring, 4 search methods, tokenizer, snippet extraction, scope filtering, 13 tests.
