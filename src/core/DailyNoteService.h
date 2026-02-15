#pragma once

/// @file DailyNoteService.h
/// @brief V4 Phase 09 – Daily Notes and Journaling System.

#include "EventBus.h"

#include <chrono>
#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class Config;
class VaultService;

// ============================================================================
// Daily Note Config
// ============================================================================

struct DailyNoteConfig
{
    std::string save_folder{"daily"};
    std::string filename_format{"%Y-%m-%d"};
    std::string template_path;
    bool auto_create_on_open{true};
    std::string heading_format{"# %B %d, %Y"};
    std::vector<std::string> default_tags{"daily"};
};

// ============================================================================
// Daily Note Entry
// ============================================================================

struct DailyNoteEntry
{
    std::string document_id;
    std::string title;
    std::filesystem::path file_path;
    std::chrono::year_month_day date;
    bool exists{false};
};

// ============================================================================
// Daily Note Service
// ============================================================================

class DailyNoteService
{
public:
    DailyNoteService(EventBus& event_bus, Config& config, VaultService& vault_service);

    /// Open or create today's daily note.
    [[nodiscard]] auto open_today() -> std::expected<std::string, std::string>;

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
    [[nodiscard]] auto list_year(std::chrono::year year) const -> std::vector<DailyNoteEntry>;

    /// Get the daily note configuration.
    [[nodiscard]] auto daily_config() const -> DailyNoteConfig;

    /// Update the daily note configuration.
    auto set_daily_config(const DailyNoteConfig& config) -> void;

private:
    EventBus& event_bus_;
    [[maybe_unused]] Config& config_;
    VaultService& vault_service_;
    DailyNoteConfig daily_config_;

    [[nodiscard]] auto format_filename(std::chrono::year_month_day date) const -> std::string;
    [[nodiscard]] auto format_heading(std::chrono::year_month_day date) const -> std::string;
    [[nodiscard]] auto daily_note_path(std::chrono::year_month_day date) const
        -> std::filesystem::path;
    [[nodiscard]] auto create_daily_note(std::chrono::year_month_day date)
        -> std::expected<std::string, std::string>;
    [[nodiscard]] auto apply_template(std::chrono::year_month_day date) const -> std::string;
    [[nodiscard]] auto format_date(std::chrono::year_month_day date, const std::string& fmt) const
        -> std::string;
};

} // namespace markamp::core
