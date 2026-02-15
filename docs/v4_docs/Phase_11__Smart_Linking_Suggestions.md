# Phase 11 -- Smart Linking Suggestions

## Objective

Implement an intelligent link suggestion engine that recommends relevant wikilinks as the user types. Analyzes the current document's content and the vault's link graph to suggest connections the user might want to make. Includes autocomplete for `[[` link syntax, automatic detection of potential links (unlinked mentions in reverse), and a "suggested links" panel showing relevant documents.

## Prerequisites

- Phase 03 (BacklinkIndex -- unlinked mention detection)
- Phase 02 (VaultService -- document index)
- Phase 05 (SearchEngine -- fuzzy text matching)

## Feature References (PRD)

- PRD #39: Smart Linking Suggestions

## Data Structures to Implement

### File: `src/core/LinkSuggestionService.h`

```cpp
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class VaultService;
class BacklinkIndex;
class SearchEngine;

struct LinkSuggestion
{
    std::string document_id;
    std::string document_title;
    std::string file_path;
    double relevance_score{0.0};
    std::string reason;                // "title match", "shared tags", "graph neighbor"
    std::vector<std::string> shared_tags;
    int shared_links{0};              // Number of documents both link to

    enum class Source : uint8_t
    {
        TitleMatch,        // Current text matches a doc title
        TagOverlap,        // Shares tags with current document
        GraphProximity,    // Connected via link chain
        ContentSimilarity, // Similar content based on search terms
        FrequentCoLink     // Often linked alongside the same documents
    } source{Source::TitleMatch};
};

struct AutocompleteResult
{
    std::string display_text;       // Shown in dropdown
    std::string insert_text;        // Text to insert (e.g., "My Note|alias")
    std::string subtitle;           // Secondary info (folder, tags)
    double score{0.0};
};

class LinkSuggestionService
{
public:
    LinkSuggestionService(EventBus& event_bus,
                          VaultService& vault_service,
                          BacklinkIndex& backlink_index,
                          SearchEngine& search_engine);

    /// Get autocomplete suggestions for [[ link prefix.
    [[nodiscard]] auto autocomplete(const std::string& prefix, int limit = 15) const
        -> std::vector<AutocompleteResult>;

    /// Get smart link suggestions for the current document.
    [[nodiscard]] auto suggest_links(const std::string& document_id, int limit = 10) const
        -> std::vector<LinkSuggestion>;

    /// Detect potential link targets in text (words matching document titles).
    [[nodiscard]] auto detect_link_targets(const std::string& text) const
        -> std::vector<std::pair<std::string, std::string>>;  // (matched_text, doc_id)

private:
    EventBus& event_bus_;
    VaultService& vault_service_;
    BacklinkIndex& backlink_index_;
    SearchEngine& search_engine_;

    [[nodiscard]] auto score_by_tag_overlap(const std::string& doc_a,
                                             const std::string& doc_b) const -> double;
    [[nodiscard]] auto score_by_graph_proximity(const std::string& doc_a,
                                                 const std::string& doc_b) const -> double;
    [[nodiscard]] auto score_by_co_links(const std::string& doc_a,
                                          const std::string& doc_b) const -> double;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`autocomplete(prefix, limit)`** -- Search VaultService name index for titles/aliases starting with prefix. Also fuzzy-match. Score by exact match > prefix match > fuzzy match > recency. Return sorted results with display_text and insert_text.

2. **`suggest_links(document_id, limit)`** -- For the given document: (a) find unlinked mentions of other doc titles in its content, (b) find docs with overlapping tags, (c) find docs that are 2-hop graph neighbors, (d) find docs with similar content via search. Score and deduplicate. Return top N suggestions.

3. **`detect_link_targets(text)`** -- Scan text for substrings matching known document titles/aliases. Use a trie or set of known names for efficient matching. Return matched text spans and their corresponding document IDs.

4. **`score_by_tag_overlap(a, b)`** -- Count shared tags between documents a and b. Score = shared_count / max(tags_a, tags_b). Jaccard-like similarity.

5. **`score_by_graph_proximity(a, b)`** -- If a and b share a common link neighbor (both link to C, or C links to both), score higher. Use adjacency from BacklinkIndex.

6. **`score_by_co_links(a, b)`** -- Count documents that link to both a and b. Higher count = more likely to be related.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LinkAutocompleteTriggerEvent)
std::string prefix;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(LinkSuggestionsReadyEvent)
std::string document_id;
int suggestion_count{0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_link_suggestions.cpp`

1. **Autocomplete exact prefix** -- Docs: "Alpha", "Alpine", "Beta". autocomplete("Al") returns Alpha, Alpine.
2. **Autocomplete fuzzy** -- autocomplete("Alpa") matches "Alpha" via fuzzy.
3. **Autocomplete with alias** -- Doc "Alpha" has alias "A". autocomplete("A") includes it.
4. **Suggest by unlinked mention** -- Doc A contains text "React" but no link. "React" is a doc title. Suggest returns React with source=TitleMatch.
5. **Suggest by tag overlap** -- Doc A and Doc B share 3 tags. Suggest returns B with source=TagOverlap.
6. **Suggest by graph proximity** -- A links to C, B links to C. Suggest for A returns B.
7. **Detect link targets** -- Text "I use React and Redux". Both are doc titles. Returns both with positions.
8. **Score ordering** -- Multiple suggestions with different scores. Verify sorted by relevance descending.
9. **Deduplication** -- Same doc suggested by multiple signals. Verify it appears once with combined score.
10. **Limit** -- 20 possible suggestions, limit=5. Verify only 5 returned.

## Acceptance Criteria

- [ ] `[[` autocomplete returns matching document titles and aliases
- [ ] Fuzzy matching handles typos in autocomplete
- [ ] Smart suggestions detect unlinked mentions of document titles
- [ ] Tag overlap scoring finds topically related documents
- [ ] Graph proximity finds structurally connected documents
- [ ] Results are scored, deduplicated, and sorted by relevance
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/LinkSuggestionService.h` | LinkSuggestionService, LinkSuggestion, AutocompleteResult |
| CREATE | `src/core/LinkSuggestionService.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 2 link suggestion events |
| MODIFY | `src/core/PluginContext.h` | Add `LinkSuggestionService* link_suggestion_service{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add LinkSuggestionService.cpp |
| CREATE | `tests/unit/test_link_suggestions.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_link_suggestions target |

## Architecture Notes

- Autocomplete is performance-sensitive: must return results within 50ms for good UX
- The editor integration (showing autocomplete dropdown after `[[`) is handled by EditorPanel
- suggest_links() can be called lazily (on panel open) since it's not real-time
- Constructor injection with 4 dependencies

## Estimated Complexity

**M** -- Autocomplete with fuzzy matching, multi-signal scoring, text scanning, 10 tests.
