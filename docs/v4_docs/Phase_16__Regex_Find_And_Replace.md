# Phase 16 -- Regex Find and Replace

## Objective

Implement vault-wide regex find and replace: search across all documents using regex patterns, preview matches with context, and apply replacements with capture group support. Extends the existing Find/Replace functionality in EditorPanel to support regex mode and vault-wide scope.

## Prerequisites

- Phase 05 (SearchEngine -- regex search)
- Phase 02 (VaultService -- document access)
- Existing EditorPanel Find/Replace UI

## Feature References (PRD)

- PRD #37: Regex Replace and Search

## Data Structures to Implement

### File: `src/core/FindReplaceService.h`

```cpp
#pragma once

#include <expected>
#include <regex>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class VaultService;

struct FindMatch
{
    std::string document_id;
    std::string file_path;
    int line_number{0};
    int column{0};
    int match_length{0};
    std::string matched_text;
    std::string context_before;   // Text before match on same line
    std::string context_after;    // Text after match on same line
    std::string full_line;
};

struct ReplacePreview
{
    FindMatch match;
    std::string replacement_text;  // What the match will become
    std::string preview_line;      // Full line after replacement
};

struct FindReplaceOptions
{
    std::string pattern;
    std::string replacement;
    bool use_regex{false};
    bool case_sensitive{false};
    bool whole_word{false};
    bool multiline{false};
    bool preserve_case{false};    // Match case pattern in replacement

    enum class Scope : uint8_t { CurrentFile, OpenFiles, Vault } scope{Scope::CurrentFile};
    std::vector<std::string> include_paths;
    std::vector<std::string> exclude_paths;
};

struct FindReplaceResult
{
    std::vector<FindMatch> matches;
    int total_matches{0};
    int files_searched{0};
    int files_with_matches{0};
    double elapsed_ms{0.0};
};

class FindReplaceService
{
public:
    FindReplaceService(EventBus& event_bus, VaultService& vault_service);

    /// Find all matches across scope.
    [[nodiscard]] auto find_all(const FindReplaceOptions& options) -> FindReplaceResult;

    /// Preview replacements without applying them.
    [[nodiscard]] auto preview_replace(const FindReplaceOptions& options)
        -> std::vector<ReplacePreview>;

    /// Replace all matches. Returns count of replacements made.
    [[nodiscard]] auto replace_all(const FindReplaceOptions& options)
        -> std::expected<int, std::string>;

    /// Replace a single match. Returns the new content of the line.
    [[nodiscard]] auto replace_single(const FindMatch& match,
                                       const FindReplaceOptions& options)
        -> std::expected<std::string, std::string>;

    /// Validate a regex pattern. Returns error message if invalid.
    [[nodiscard]] auto validate_pattern(const std::string& pattern) const
        -> std::optional<std::string>;

    /// Get replacement text with capture group substitution.
    [[nodiscard]] auto expand_replacement(const std::string& matched,
                                           const std::smatch& captures,
                                           const std::string& replacement) const
        -> std::string;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;

    [[nodiscard]] auto search_file(const std::string& content,
                                    const std::string& document_id,
                                    const std::string& file_path,
                                    const FindReplaceOptions& options) const
        -> std::vector<FindMatch>;
    [[nodiscard]] auto compile_regex(const FindReplaceOptions& options) const
        -> std::expected<std::regex, std::string>;
    [[nodiscard]] auto find_literal(const std::string& content,
                                     const FindReplaceOptions& options) const
        -> std::vector<FindMatch>;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`find_all(options)`** -- Determine scope (current file, open files, vault). For each file in scope: read content, call search_file(). Aggregate matches. Publish FindCompletedEvent.

2. **`search_file(content, doc_id, path, options)`** -- If use_regex: compile pattern, iterate matches with std::regex_iterator. If literal: use string search (optionally case-insensitive, whole-word). Build FindMatch for each hit with line number, column, context.

3. **`replace_all(options)`** -- Find all matches. For each unique file: apply replacements in reverse order (to preserve positions). Save modified files. Return total replacement count.

4. **`expand_replacement(matched, captures, replacement)`** -- Handle capture group references: `$1`, `$2`, etc. Handle `$0` for full match. Handle `\n` for newlines, `\t` for tabs.

5. **`preview_replace(options)`** -- Find all matches. For each, compute the replacement text. Build ReplacePreview with before/after line.

6. **`validate_pattern(pattern)`** -- Try to compile the regex. Return error message if compilation fails.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FindCompletedEvent)
int match_count{0};
int files_with_matches{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ReplaceCompletedEvent)
int replacements{0};
int files_modified{0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_find_replace.cpp`

1. **Literal find** -- Search "hello" in "hello world hello". Verify 2 matches.
2. **Case insensitive** -- Search "Hello" case-insensitive. Matches "hello".
3. **Whole word** -- Search "the" whole-word. Does not match "there".
4. **Regex find** -- Pattern `\d{3}-\d{4}`. Matches "555-1234" but not "55-1234".
5. **Regex capture groups** -- Pattern `(\w+)@(\w+)`. Replacement `$2/$1`. Verify expansion.
6. **Replace all** -- Replace "foo" with "bar". Verify all occurrences replaced in file.
7. **Replace preserves positions** -- Multiple replacements of different lengths. Verify all correct.
8. **Vault-wide search** -- 5 files, 3 have matches. Verify files_with_matches=3.
9. **Preview replace** -- Preview shows before/after for each match without modifying files.
10. **Invalid regex** -- Pattern `[invalid`. validate_pattern() returns error.
11. **Multiline regex** -- Pattern spanning multiple lines. Verify matches.
12. **Scope filtering** -- Include/exclude path filters. Verify only matching files searched.

## Acceptance Criteria

- [ ] Literal and regex find across current file, open files, or entire vault
- [ ] Regex capture groups ($1, $2) work in replacement strings
- [ ] Case insensitive and whole word options work
- [ ] Replace all modifies files and saves them
- [ ] Preview shows before/after without modifying
- [ ] Invalid regex patterns are caught with clear error messages
- [ ] Path include/exclude filters work for vault-wide search
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/FindReplaceService.h` | FindReplaceService, options, results |
| CREATE | `src/core/FindReplaceService.cpp` | Full implementation |
| MODIFY | `src/ui/EditorPanel.cpp` | Integrate regex mode into Find/Replace bar |
| MODIFY | `src/core/Events.h` | Add 2 find/replace events |
| MODIFY | `src/core/PluginContext.h` | Add `FindReplaceService* find_replace_service{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add FindReplaceService.cpp |
| CREATE | `tests/unit/test_find_replace.cpp` | 12 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_find_replace target |

## Architecture Notes

- Replace operations modify files through VaultService (ensures index updates)
- Replacements are applied in reverse order within each file to preserve positions
- Vault-wide search runs on a background thread to avoid UI blocking
- Constructor injection: FindReplaceService(EventBus&, VaultService&)

## Estimated Complexity

**L** -- Regex compilation, capture group expansion, multi-file replace, preview generation, 12 tests.
