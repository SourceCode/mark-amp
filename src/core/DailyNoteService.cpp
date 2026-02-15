/// @file DailyNoteService.cpp
/// @brief V4 Phase 09 – Daily Notes and Journaling System implementation.

#include "core/DailyNoteService.h"

#include "core/Config.h"
#include "core/Events.h"
#include "core/VaultService.h"
#include "core/WikiLink.h"

#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

DailyNoteService::DailyNoteService(EventBus& event_bus, Config& config, VaultService& vault_service)
    : event_bus_(event_bus)
    , config_(config)
    , vault_service_(vault_service)
{
}

// ============================================================================
// Public API
// ============================================================================

auto DailyNoteService::open_today() -> std::expected<std::string, std::string>
{
    const auto now = std::chrono::system_clock::now();
    const auto today_tp = std::chrono::floor<std::chrono::days>(now);
    const std::chrono::year_month_day today{today_tp};
    return open_date(today);
}

auto DailyNoteService::open_date(std::chrono::year_month_day date)
    -> std::expected<std::string, std::string>
{
    const auto existing = get_daily_note(date);
    bool newly_created = false;

    std::string doc_id;
    if (existing && existing->exists)
    {
        // Open existing daily note
        const auto rel_path = daily_note_path(date);
        auto result = vault_service_.open_document(rel_path);
        if (!result)
        {
            return std::unexpected(result.error());
        }
        doc_id = result.value()->id();
    }
    else
    {
        // Create new daily note
        auto result = create_daily_note(date);
        if (!result)
        {
            return std::unexpected(result.error());
        }
        doc_id = result.value();
        newly_created = true;
    }

    // Publish event
    auto event = events::DailyNoteOpenedEvent{};
    event.document_id = doc_id;
    event.date_string = format_date(date, "%Y-%m-%d");
    event.newly_created = newly_created;
    event_bus_.publish(event);

    return doc_id;
}

auto DailyNoteService::get_daily_note(std::chrono::year_month_day date) const
    -> std::optional<DailyNoteEntry>
{
    if (!vault_service_.is_open())
    {
        return std::nullopt;
    }

    const auto rel_path = daily_note_path(date);
    const auto vault_root = vault_service_.vault_path();
    const auto abs_path = vault_root / rel_path;

    DailyNoteEntry entry;
    entry.date = date;
    entry.title = format_filename(date);
    entry.file_path = rel_path;
    entry.exists = std::filesystem::exists(abs_path);

    if (entry.exists)
    {
        // Try to find doc ID from the vault index
        const auto docs = vault_service_.list_documents();
        for (const auto& doc : docs)
        {
            if (doc.file_path == rel_path)
            {
                entry.document_id = doc.document_id;
                break;
            }
        }
    }

    return entry;
}

auto DailyNoteService::next_daily_note(std::chrono::year_month_day from)
    -> std::expected<std::string, std::string>
{
    constexpr int kMaxScanDays = 365;
    auto current = from;

    for (int day_offset = 1; day_offset <= kMaxScanDays; ++day_offset)
    {
        const auto next_date = std::chrono::sys_days{current} + std::chrono::days{day_offset};
        const std::chrono::year_month_day candidate{next_date};

        const auto entry = get_daily_note(candidate);
        if (entry && entry->exists)
        {
            return open_date(candidate);
        }
    }

    // No existing note found; create tomorrow's
    const auto tomorrow = std::chrono::sys_days{from} + std::chrono::days{1};
    return open_date(std::chrono::year_month_day{tomorrow});
}

auto DailyNoteService::previous_daily_note(std::chrono::year_month_day from)
    -> std::expected<std::string, std::string>
{
    constexpr int kMaxScanDays = 365;
    auto current = from;

    for (int day_offset = 1; day_offset <= kMaxScanDays; ++day_offset)
    {
        const auto prev_date = std::chrono::sys_days{current} - std::chrono::days{day_offset};
        const std::chrono::year_month_day candidate{prev_date};

        const auto entry = get_daily_note(candidate);
        if (entry && entry->exists)
        {
            return open_date(candidate);
        }
    }

    return std::unexpected("No previous daily note found within 365 days");
}

auto DailyNoteService::list_month(std::chrono::year_month month) const
    -> std::vector<DailyNoteEntry>
{
    std::vector<DailyNoteEntry> entries;

    // Determine last day of this month
    const auto last_day = std::chrono::year_month_day_last{month / std::chrono::last};
    const int days_in_month = static_cast<int>(static_cast<unsigned>(last_day.day()));

    for (int day_num = 1; day_num <= days_in_month; ++day_num)
    {
        const std::chrono::year_month_day date{month /
                                               std::chrono::day{static_cast<unsigned>(day_num)}};
        auto entry = get_daily_note(date);
        if (entry)
        {
            entries.push_back(std::move(*entry));
        }
    }

    return entries;
}

auto DailyNoteService::list_year(std::chrono::year year) const -> std::vector<DailyNoteEntry>
{
    std::vector<DailyNoteEntry> entries;

    for (unsigned month_num = 1; month_num <= 12; ++month_num)
    {
        const auto month = year / std::chrono::month{month_num};
        auto month_entries = list_month(month);
        entries.insert(entries.end(),
                       std::make_move_iterator(month_entries.begin()),
                       std::make_move_iterator(month_entries.end()));
    }

    return entries;
}

auto DailyNoteService::daily_config() const -> DailyNoteConfig
{
    return daily_config_;
}

auto DailyNoteService::set_daily_config(const DailyNoteConfig& config) -> void
{
    daily_config_ = config;
}

// ============================================================================
// Private Helpers
// ============================================================================

auto DailyNoteService::format_filename(std::chrono::year_month_day date) const -> std::string
{
    return format_date(date, daily_config_.filename_format);
}

auto DailyNoteService::format_heading(std::chrono::year_month_day date) const -> std::string
{
    return format_date(date, daily_config_.heading_format);
}

auto DailyNoteService::daily_note_path(std::chrono::year_month_day date) const
    -> std::filesystem::path
{
    const auto filename = format_filename(date) + ".md";
    if (daily_config_.save_folder.empty())
    {
        return std::filesystem::path{filename};
    }
    return std::filesystem::path{daily_config_.save_folder} / filename;
}

auto DailyNoteService::create_daily_note(std::chrono::year_month_day date)
    -> std::expected<std::string, std::string>
{
    const auto title = format_filename(date);
    const auto content = apply_template(date);

    // Build frontmatter with default tags
    Frontmatter fm;
    fm.title = title;
    fm.date = format_date(date, "%Y-%m-%d");
    fm.tags = daily_config_.default_tags;

    auto result = vault_service_.create_document(title, daily_config_.save_folder, fm);
    if (!result)
    {
        return std::unexpected(result.error());
    }

    // If we have template content, set it on the document
    if (!content.empty())
    {
        result.value()->set_markdown(content);
        auto save_result = vault_service_.save_document(result.value()->id());
        if (!save_result)
        {
            return std::unexpected(save_result.error());
        }
    }

    return result.value()->id();
}

auto DailyNoteService::apply_template(std::chrono::year_month_day date) const -> std::string
{
    std::string content;

    // If a template file is configured, try to read it
    if (!daily_config_.template_path.empty())
    {
        const auto vault_root = vault_service_.vault_path();
        const auto template_abs = vault_root / daily_config_.template_path;

        if (std::filesystem::exists(template_abs))
        {
            std::ifstream file(template_abs);
            if (file.is_open())
            {
                content = std::string(std::istreambuf_iterator<char>(file),
                                      std::istreambuf_iterator<char>());
            }
        }
    }

    // If no template, use heading only
    if (content.empty())
    {
        content = format_heading(date) + "\n\n";
        return content;
    }

    // Replace template variables
    const auto date_str = format_date(date, "%Y-%m-%d");
    const auto title_str = format_date(date, "%B %d, %Y");
    const auto time_str = format_date(date, "%H:%M");

    const auto yesterday = std::chrono::sys_days{date} - std::chrono::days{1};
    const auto tomorrow = std::chrono::sys_days{date} + std::chrono::days{1};
    const auto yesterday_str = format_date(std::chrono::year_month_day{yesterday}, "%Y-%m-%d");
    const auto tomorrow_str = format_date(std::chrono::year_month_day{tomorrow}, "%Y-%m-%d");

    auto replace_all = [](std::string& str, const std::string& from, const std::string& to)
    {
        size_t pos = 0;
        while ((pos = str.find(from, pos)) != std::string::npos)
        {
            str.replace(pos, from.length(), to);
            pos += to.length();
        }
    };

    replace_all(content, "{{date}}", date_str);
    replace_all(content, "{{title}}", title_str);
    replace_all(content, "{{time}}", time_str);
    replace_all(content, "{{yesterday}}", yesterday_str);
    replace_all(content, "{{tomorrow}}", tomorrow_str);

    return content;
}

auto DailyNoteService::format_date(std::chrono::year_month_day date, const std::string& fmt) const
    -> std::string
{
    std::tm time_struct = {};
    time_struct.tm_year = static_cast<int>(date.year()) - 1900;
    time_struct.tm_mon = static_cast<int>(static_cast<unsigned>(date.month())) - 1;
    time_struct.tm_mday = static_cast<int>(static_cast<unsigned>(date.day()));

    // Normalize to fill in weekday etc.
    std::mktime(&time_struct);

    std::ostringstream oss;
    oss << std::put_time(&time_struct, fmt.c_str());
    return oss.str();
}

} // namespace markamp::core
