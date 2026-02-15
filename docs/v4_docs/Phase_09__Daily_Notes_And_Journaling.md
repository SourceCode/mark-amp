# Phase 09 -- Daily Notes and Journaling System

## Objective

Implement the Daily Notes system: auto-created dated documents with configurable templates, one-click "today's note" access, and calendar-based navigation. Integrates with the existing NotebookConf (which already has daily_note_save_path and daily_note_template_path fields) and VaultService for document creation.

## Prerequisites

- Phase 01 (DocumentModel, Frontmatter)
- Phase 02 (VaultService -- document creation)
- Phase 14 (Templates -- for daily note templates; can use simple substitution if Phase 14 incomplete)

## Feature References (PRD)

- PRD #6: Daily Notes / Journaling
- PRD #30: Calendar Integration (data model for calendar navigation)

## Data Structures to Implement

### File: `src/core/DailyNoteService.h`

```cpp
#pragma once

#include <chrono>
#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;
class VaultService;

struct DailyNoteConfig
{
    std::string save_folder{"daily"};                 // Subfolder for daily notes
    std::string filename_format{"%Y-%m-%d"};          // strftime format
    std::string template_path;                        // Path to template file (empty = no template)
    bool auto_create_on_open{true};                   // Create today's note on app launch
    std::string heading_format{"# %B %d, %Y"};       // Default heading (strftime)
    std::vector<std::string> default_tags{"daily"};   // Tags to add to every daily note
};

struct DailyNoteEntry
{
    std::string document_id;
    std::string title;
    std::filesystem::path file_path;
    std::chrono::year_month_day date;
    bool exists{false};
};

class DailyNoteService
{
public:
    DailyNoteService(EventBus& event_bus, Config& config, VaultService& vault_service);

    /// Open or create today's daily note.
    [[nodiscard]] auto open_today()
        -> std::expected<std::string, std::string>;  // Returns document ID

    /// Open or create a daily note for a specific date.
    [[nodiscard]] auto open_date(std::chrono::year_month_day date)
        -> std::expected<std::string, std::string>;

    /// Get the daily note for a date (without creating it).
    [[nodiscard]] auto get_daily_note(std::chrono::year_month_day date) const
        -> std::optional<DailyNoteEntry>;

    /// Navigate to the next daily note from the current date.
    [[nodiscard]] auto next_daily_note(std::chrono::year_month_day from)
        -> std::expected<std::string, std::string>;

    /// Navigate to the previous daily note from the current date.
    [[nodiscard]] auto previous_daily_note(std::chrono::year_month_day from)
        -> std::expected<std::string, std::string>;

    /// List all daily notes in a month (for calendar view).
    [[nodiscard]] auto list_month(std::chrono::year_month month) const
        -> std::vector<DailyNoteEntry>;

    /// List all daily notes in a year.
    [[nodiscard]] auto list_year(std::chrono::year year) const
        -> std::vector<DailyNoteEntry>;

    /// Get the daily note configuration.
    [[nodiscard]] auto daily_config() const -> DailyNoteConfig;

    /// Update the daily note configuration.
    auto set_daily_config(const DailyNoteConfig& config) -> void;

private:
    EventBus& event_bus_;
    Config& config_;
    VaultService& vault_service_;

    [[nodiscard]] auto format_filename(std::chrono::year_month_day date) const -> std::string;
    [[nodiscard]] auto format_heading(std::chrono::year_month_day date) const -> std::string;
    [[nodiscard]] auto daily_note_path(std::chrono::year_month_day date) const
        -> std::filesystem::path;
    [[nodiscard]] auto create_daily_note(std::chrono::year_month_day date)
        -> std::expected<std::string, std::string>;
    [[nodiscard]] auto apply_template(std::chrono::year_month_day date) const -> std::string;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`open_today()`** -- Get current date via std::chrono. Call open_date() with today.

2. **`open_date(date)`** -- Check if daily note exists for this date (by filename convention). If exists, open it via VaultService. If not, call create_daily_note(date). Publish DailyNoteOpenedEvent. Return document ID.

3. **`create_daily_note(date)`** -- Format filename using strftime. Build initial content: heading + template content. Add default tags to frontmatter. Create document via VaultService::create_document(). Return document ID.

4. **`apply_template(date)`** -- If template_path configured, read template file. Replace variables: `{{date}}`, `{{title}}`, `{{time}}`, `{{yesterday}}`, `{{tomorrow}}`. Return processed content. If no template, return heading only.

5. **`list_month(month)`** -- Enumerate daily note files matching the date pattern for the given month. For each date in the month, check if a file exists. Return DailyNoteEntry for each day (exists or not, for calendar rendering).

6. **`next_daily_note(from)`** -- Scan forward from `from` date to find the next existing daily note. Limit scan to 365 days. If none found, create one for tomorrow.

7. **`previous_daily_note(from)`** -- Scan backward from `from` date. Return nearest existing daily note.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DailyNoteOpenedEvent)
std::string document_id;
std::string date_string;  // YYYY-MM-DD
bool newly_created{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(OpenTodayNoteRequestEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DailyNoteNavigateEvent)
std::string direction;  // "next" or "previous"
std::string from_date;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `daily_notes.folder` | string | `"daily"` | Subfolder for daily notes |
| `daily_notes.filename_format` | string | `"%Y-%m-%d"` | Filename date format |
| `daily_notes.template_path` | string | `""` | Template file path |
| `daily_notes.auto_open` | bool | `false` | Open today's note on app launch |
| `daily_notes.heading_format` | string | `"# %B %d, %Y"` | Heading date format |
| `daily_notes.default_tags` | string | `"daily"` | Comma-separated default tags |

## Test Cases

File: `tests/unit/test_daily_notes.cpp`

1. **Open today creates note** -- No daily note exists. open_today() creates one. Verify file on disk.
2. **Open today returns existing** -- Daily note already exists. open_today() opens it. No duplicate created.
3. **Date formatting** -- Verify filename format produces "2026-02-14" for Feb 14, 2026.
4. **Custom filename format** -- Set format to "%d-%m-%Y". Verify "14-02-2026".
5. **Template application** -- Template with `{{date}}` and `{{title}}`. Verify substituted in content.
6. **Default tags** -- New daily note gets configured default tags in frontmatter.
7. **List month** -- 3 daily notes in January. list_month(Jan) returns 31 entries, 3 with exists=true.
8. **Next daily note** -- Notes exist for Jan 1, Jan 5, Jan 10. next_daily_note(Jan 1) returns Jan 5.
9. **Previous daily note** -- previous_daily_note(Jan 10) returns Jan 5.
10. **Heading format** -- heading_format "# %A, %B %d" produces "# Saturday, February 14".
11. **DailyNoteOpenedEvent published** -- open_date() publishes event with correct date and newly_created flag.

## Acceptance Criteria

- [ ] Today's note is created or opened with a single call
- [ ] Daily notes use configurable filename format (strftime)
- [ ] Templates with date variables are applied to new daily notes
- [ ] Default tags are added to daily note frontmatter
- [ ] Calendar listing returns all days in a month with existence flag
- [ ] Next/previous navigation finds nearest existing daily note
- [ ] Events published for daily note lifecycle
- [ ] All 11 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/DailyNoteService.h` | DailyNoteService, DailyNoteConfig, DailyNoteEntry |
| CREATE | `src/core/DailyNoteService.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 3 daily note events |
| MODIFY | `src/core/PluginContext.h` | Add `DailyNoteService* daily_note_service{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add DailyNoteService.cpp |
| CREATE | `tests/unit/test_daily_notes.cpp` | 11 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_daily_notes target |

## Architecture Notes

- Uses C++20 chrono calendar types (year_month_day, year_month)
- Integrates with existing NotebookConf daily_note_save_path field
- Template variable substitution is simple string replacement (Phase 14 adds full template engine)
- Constructor injection: DailyNoteService(EventBus&, Config&, VaultService&)

## Estimated Complexity

**M** -- Date formatting, template substitution, file existence checks, calendar listing, 11 tests.
