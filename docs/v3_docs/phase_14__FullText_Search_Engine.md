# Phase 14 -- Full-Text Search Engine

**Priority:** Critical (core knowledge management feature)
**Estimated Scope:** ~12 files created/modified
**Dependencies:** Existing EventBus, Config, PluginContext; Phase 09 BlockId; SQLite3 with FTS5 extension

---

## Objective

Implement comprehensive full-text search across all knowledgebase content, porting SiYuan's search system. Supports keyword search (AND), phrase search (exact match), regex search, and SQL query mode. Uses SQLite FTS5 for fast text matching with snippet extraction and hit highlighting.

SiYuan's `FullTextSearchBlock` is one of its most complex functions with 15+ parameters controlling search behavior. This phase implements the core search engine; Phase 16 adds the UI panel.

Key capabilities:
- Four search methods: keyword (AND), phrase (exact), regex, SQL
- Filter by block type, notebook (box), and path
- Group results by document or show flat
- Sort by relevance, creation date, update date, or custom sort value
- Paginated results with total count
- Snippet extraction with keyword highlighting
- CJK text support via ICU tokenizer

---

## Prerequisites

- `src/core/EventBus.h` -- publish/subscribe for search events
- `src/core/Config.h` -- search settings
- `src/core/PluginContext.h` -- service injection
- Phase 09 `BlockId` type for block identification
- SQLite3 with FTS5 extension (compile-time requirement)
- `src/core/HtmlSanitizer.h` -- sanitize search result snippets

---

## SiYuan Source Reference

| SiYuan File | Relevant Functions | MarkAmp Mapping |
|---|---|---|
| `kernel/model/search.go` | `FullTextSearchBlock` | `SearchService::search()` |
| `kernel/model/search.go` | `SearchRefLabel` | `SearchService::search_ref_blocks()` |
| `kernel/model/search.go` | `SearchTemplate` | `SearchService::search_templates()` |
| `kernel/model/search.go` | `SearchAsset` | `SearchService::search_assets()` |
| `kernel/model/search.go` | `QuerySQL` | Deferred to Phase 15 |
| `kernel/sql/block_query.go` | FTS5 query construction | `FtsQueryBuilder` |
| `kernel/search/` | Search helpers, tokenizer, highlighting | `SearchHighlighter`, `SnippetExtractor` |

---

## MarkAmp Integration Points

1. **SQLite FTS5** -- The block database includes an FTS5 virtual table mirroring the blocks table. On block insert/update/delete, the FTS index is updated synchronously.
2. **Config** -- Search settings loaded from YAML frontmatter config (case sensitivity, result limit, etc.).
3. **PluginContext** -- `SearchService*` pointer added for extension access.
4. **EditorPanel** -- Find/Replace within current document uses the same search engine (scoped query).
5. **CommandPalette** -- Quick search via command palette triggers global search.
6. **Phase 16** -- SearchPanel UI (sidebar) consumes SearchService results.

---

## Data Structures to Implement

All structures go in `src/core/Search.h`:

```cpp
#pragma once

#include "BlockRef.h"

#include <chrono>
#include <cstdint>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace markamp::core
{

/// Search method enumeration.
enum class SearchMethod : std::uint8_t
{
    Keyword,   // AND all terms (default)
    Phrase,    // Exact phrase match
    Regex,     // Regular expression
    Sql        // Raw SQL WHERE clause (advanced)
};

/// Block types for search filtering (mirrors SiYuan block types).
enum class SearchBlockType : std::uint8_t
{
    Document    = 0,
    Heading     = 1,
    Paragraph   = 2,
    MathBlock   = 3,
    Table       = 4,
    CodeBlock   = 5,
    HtmlBlock   = 6,
    List        = 7,
    ListItem    = 8,
    Blockquote  = 9,
    SuperBlock  = 10,
    EmbedBlock  = 11
};

/// Sort order for search results.
enum class SearchSortOrder : std::uint8_t
{
    BlockType     = 0,
    CreatedAsc    = 1,
    CreatedDesc   = 2,
    UpdatedAsc    = 3,
    UpdatedDesc   = 4,
    SortAsc       = 5,
    SortDesc      = 6,
    Relevance     = 7
};

/// Grouping mode for search results.
enum class SearchGroupBy : std::uint8_t
{
    NoGroup     = 0,
    ByDocument  = 1
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

    /// HTML snippet with `<mark>` tags around matched terms.
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
    /// Returns text centered on the first match with `context_chars` of surrounding text.
    [[nodiscard]] auto extract(std::string_view content,
                                const std::vector<HighlightRange>& matches,
                                int context_chars = 60) const -> std::string;

    /// Build an HTML snippet with `<mark>` tags around matched ranges.
    [[nodiscard]] auto highlight_html(std::string_view content,
                                       const std::vector<HighlightRange>& matches) const
        -> std::string;
};

} // namespace markamp::core
```

---

## Key Functions to Implement

### SearchService class (`src/core/SearchService.h` / `src/core/SearchService.cpp`)

Constructor: `SearchService(EventBus& event_bus, Config& config)`

1. **`auto search(const SearchQuery& query) -> SearchResult`** -- Execute a full-text search against the block database. Dispatches to the appropriate method handler (keyword/phrase/regex/sql). Applies type, box, and path filters. Sorts results. Extracts snippets and highlights. Paginates. Publishes `SearchCompletedEvent`. Measures and records elapsed time.

2. **`auto search_keyword(const SearchQuery& query) -> SearchResult`** -- FTS5 keyword search: splits query into terms, builds AND expression, executes against FTS5 virtual table. Uses `bm25()` ranking function for relevance scoring.

3. **`auto search_phrase(const SearchQuery& query) -> SearchResult`** -- FTS5 phrase search: wraps query in double quotes for exact sequence match.

4. **`auto search_regex(const SearchQuery& query) -> SearchResult`** -- Regex search: compiles pattern via `std::regex`, scans blocks in the database using `REGEXP` function or client-side filtering. Slower than FTS5 but supports full regex syntax.

5. **`auto search_ref_blocks(const std::string& query) -> std::vector<SearchHit>`** -- Search for blocks suitable as reference targets (used by the `((` autocomplete). Matches against block content and block names. Returns top 20 results sorted by relevance.

6. **`auto search_embed_blocks(const std::string& query) -> std::vector<SearchHit>`** -- Search for blocks suitable as embed targets (used by `{{` autocomplete). Similar to `search_ref_blocks` but filters to block types that are meaningful for transclusion.

7. **`auto search_templates(const std::string& query) -> std::vector<std::string>`** -- Search template files by name. Returns matching template file paths.

8. **`auto search_assets(const std::string& query) -> std::vector<std::string>`** -- Search asset files (images, PDFs, etc.) by filename. Returns matching asset paths.

9. **`void update_fts_index(const BlockId& block_id, std::string_view content)`** -- Update the FTS5 index for a single block. Called when block content changes. Handles INSERT, UPDATE, and DELETE operations on the virtual table.

10. **`void rebuild_fts_index()`** -- Full rebuild of the FTS5 index from the blocks table. Called on workspace open or when the index is corrupted.

11. **`auto get_search_statistics() -> std::pair<int, int>`** -- Return (total_indexed_blocks, total_indexed_terms) for diagnostics.

12. **`void configure_fts()`** -- Initialize the FTS5 virtual table with appropriate tokenizer configuration. Uses `unicode61` tokenizer with `remove_diacritics=2` for broad matching. Adds `tokenchars` for CJK character support if enabled in config.

---

## Events to Add

Add to `src/core/Events.h`:

```cpp
// ============================================================================
// Search events (Phase 14)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchCompletedEvent)
std::string query_string;
int result_count{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchIndexUpdatedEvent)
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchIndexRebuiltEvent)
int total_blocks{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GlobalSearchRequestEvent)
std::string query_string;
MARKAMP_DECLARE_EVENT_END;
```

---

## Config Keys to Add

Add to `resources/config_defaults.json`:

| Key | Type | Default | Description |
|---|---|---|---|
| `knowledgebase.search.case_sensitive` | bool | `false` | Default case sensitivity for search |
| `knowledgebase.search.limit` | int | `64` | Maximum results per search query |
| `knowledgebase.search.index_assets` | bool | `false` | Index asset file names in FTS |
| `knowledgebase.search.snippet_context_chars` | int | `60` | Characters of context around match in snippet |
| `knowledgebase.search.highlight_tag` | string | `"mark"` | HTML tag for highlighting matches |
| `knowledgebase.search.enable_regex` | bool | `true` | Enable regex search mode |
| `knowledgebase.search.cjk_support` | bool | `true` | Enable CJK tokenizer for Chinese/Japanese/Korean text |
| `knowledgebase.search.default_sort` | string | `"relevance"` | Default sort order: relevance, created, updated |

---

## Test Cases

File: `tests/unit/test_search.cpp`

```cpp
#include "core/Search.h"
#include "core/SearchService.h"
#include "core/EventBus.h"
#include "core/Config.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
```

1. **Keyword search** -- Index blocks: "The quick brown fox", "The lazy dog", "Quick brown jumps". Search for "quick brown". Verify: 2 hits returned (blocks containing BOTH "quick" AND "brown"). "The lazy dog" not included.

2. **Phrase search** -- Same blocks. Phrase search for "quick brown". Verify: 2 hits (exact sequence "quick brown" appears in first and third blocks). "brown fox" NOT "brown quick" ordering matters.

3. **Regex search** -- Index blocks: "Error: 404", "Warning: 500", "Info: 200". Regex search for `"\d{3}"`. Verify: 3 hits (all contain 3-digit numbers). Regex search for `"Error:\s+\d+"`. Verify: 1 hit.

4. **CJK text search** -- Index block with content containing Chinese characters. Search for a Chinese term. Verify: block found. This validates the tokenizer handles CJK correctly.

5. **Filter by block type** -- Index: heading "Introduction", paragraph "Some text", code block "int x = 0;". Search "Introduction" with `types = {Heading}`. Verify: 1 hit (heading only). Search with `types = {Paragraph, CodeBlock}`. Verify: 0 hits for "Introduction".

6. **Filter by notebook/box** -- Blocks in box "notebook-A" and "notebook-B". Search with `boxes = {"notebook-A"}`. Verify: only blocks from notebook-A returned.

7. **Filter by path** -- Blocks at paths "/docs/guide/intro.md" and "/notes/daily.md". Search with `paths = {"/docs/"}`. Verify: only blocks from paths starting with "/docs/" returned.

8. **Group by document** -- 3 hits from doc X, 2 from doc Y. Search with `group_by = ByDocument`. Verify: `groups` has 2 entries. Doc X group has 3 hits. Doc Y group has 2 hits.

9. **Sort by relevance** -- Index blocks where one mentions the search term 5 times and another mentions it once. Verify: higher-frequency block has lower (better) score and appears first.

10. **Sort by date** -- Index blocks with different creation timestamps. Search with `order_by = CreatedDesc`. Verify: newest block first.

11. **Pagination** -- 25 total matching blocks. Search with `page=1, page_size=10`. Verify: 10 hits returned, `total_count == 25`. `page=3`: 5 hits. `page=4`: 0 hits.

12. **Snippet extraction and highlighting** -- Block content: "The quick brown fox jumps over the lazy dog and runs through the forest". Search for "fox". Verify: `snippet` contains "fox" with surrounding context. `highlighted_snippet` contains `<mark>fox</mark>`. `highlights` vector has one entry with correct start/end offsets.

---

## Acceptance Criteria

- [ ] `SearchService::search()` correctly dispatches to keyword/phrase/regex/sql handlers
- [ ] Keyword search uses FTS5 AND semantics (all terms must match)
- [ ] Phrase search uses FTS5 exact phrase matching
- [ ] Regex search compiles and executes patterns via `std::regex`
- [ ] Type, box, and path filters correctly restrict results
- [ ] Grouping by document produces correct `SearchGroup` entries
- [ ] All 8 sort orders produce correct result ordering
- [ ] Pagination returns correct slices with accurate `total_count`
- [ ] `SnippetExtractor::extract()` produces context-aware snippets
- [ ] `SnippetExtractor::highlight_html()` wraps matches in `<mark>` tags
- [ ] `FtsQueryBuilder` correctly escapes and formats FTS5 expressions
- [ ] FTS5 index updates synchronously on block insert/update/delete
- [ ] `SearchCompletedEvent` fires with query, result count, and elapsed time
- [ ] CJK tokenizer configured when `knowledgebase.search.cjk_support` is true
- [ ] All 12 Catch2 test cases pass
- [ ] No use of `catch(...)` -- all exception handlers use typed catches
- [ ] All query methods marked `[[nodiscard]]`

---

## Files to Create/Modify

### New Files

| File | Description |
|---|---|
| `src/core/Search.h` | `SearchQuery`, `SearchHit`, `SearchGroup`, `SearchResult`, `FtsQueryBuilder`, `SnippetExtractor`, enums |
| `src/core/Search.cpp` | `FtsQueryBuilder`, `SnippetExtractor` implementations |
| `src/core/SearchService.h` | `SearchService` class declaration |
| `src/core/SearchService.cpp` | All 12 service methods, FTS5 integration, event handling |
| `tests/unit/test_search.cpp` | All 12 Catch2 test cases |

### Modified Files

| File | Change |
|---|---|
| `src/core/Events.h` | Add `SearchCompletedEvent`, `SearchIndexUpdatedEvent`, `SearchIndexRebuiltEvent`, `GlobalSearchRequestEvent` |
| `src/core/PluginContext.h` | Add `SearchService* search_service{nullptr};` pointer |
| `src/CMakeLists.txt` | Add `core/Search.cpp`, `core/SearchService.cpp` to sources; ensure SQLite3 FTS5 is linked |
| `tests/CMakeLists.txt` | Add `test_search` test target |
| `resources/config_defaults.json` | Add 8 `knowledgebase.search.*` config keys |
