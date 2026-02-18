/// @file CommandRegistry.cpp
/// @brief V9 Phase 36 Tasks 1, 2, 3, 4, 6 — CommandRegistry implementation.

#include "CommandRegistry.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace markamp::core
{

// ── Helpers ──

namespace
{

auto to_lower(const std::string& str) -> std::string
{
    std::string result = str;
    std::transform(result.begin(),
                   result.end(),
                   result.begin(),
                   [](unsigned char character)
                   { return static_cast<char>(std::tolower(character)); });
    return result;
}

} // namespace

// ── Registration ──

void CommandRegistry::register_command(CommandEntry entry)
{
    const auto cmd_id = entry.id;

    if (commands_.contains(cmd_id))
    {
        // Overwrite existing — duplicate detected
        commands_[cmd_id] = std::move(entry);
        return;
    }

    insertion_order_.push_back(cmd_id);
    commands_.emplace(cmd_id, std::move(entry));
}

auto CommandRegistry::unregister_command(const std::string& command_id) -> bool
{
    auto iter = commands_.find(command_id);
    if (iter == commands_.end())
    {
        return false;
    }
    commands_.erase(iter);
    std::erase(insertion_order_, command_id);
    return true;
}

void CommandRegistry::register_commands(std::vector<CommandEntry> entries)
{
    for (auto& entry : entries)
    {
        register_command(std::move(entry));
    }
}

// ── Lookup ──

auto CommandRegistry::get_command(const std::string& command_id) const -> const CommandEntry*
{
    auto iter = commands_.find(command_id);
    if (iter != commands_.end())
    {
        return &iter->second;
    }
    return nullptr;
}

auto CommandRegistry::all_commands() const -> std::vector<const CommandEntry*>
{
    std::vector<const CommandEntry*> result;
    result.reserve(insertion_order_.size());
    for (const auto& cmd_id : insertion_order_)
    {
        auto iter = commands_.find(cmd_id);
        if (iter != commands_.end())
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto CommandRegistry::commands_for_category(const std::string& category) const
    -> std::vector<const CommandEntry*>
{
    std::vector<const CommandEntry*> result;
    for (const auto& cmd_id : insertion_order_)
    {
        auto iter = commands_.find(cmd_id);
        if (iter != commands_.end() && iter->second.category == category)
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto CommandRegistry::active_commands(const ContextKeyService& context) const
    -> std::vector<const CommandEntry*>
{
    std::vector<const CommandEntry*> result;
    for (const auto& cmd_id : insertion_order_)
    {
        auto iter = commands_.find(cmd_id);
        if (iter != commands_.end() && iter->second.is_available(context))
        {
            result.push_back(&iter->second);
        }
    }
    return result;
}

auto CommandRegistry::get_categories() const -> std::vector<std::string>
{
    std::set<std::string> categories;
    for (const auto& [cmd_id, entry] : commands_)
    {
        if (!entry.category.empty())
        {
            categories.insert(entry.category);
        }
    }
    return {categories.begin(), categories.end()};
}

auto CommandRegistry::command_count() const -> std::size_t
{
    return commands_.size();
}

auto CommandRegistry::has_command(const std::string& command_id) const -> bool
{
    return commands_.contains(command_id);
}

// ── Execution ──

auto CommandRegistry::execute_command(const std::string& command_id, CommandSource /*source*/)
    -> bool
{
    auto iter = commands_.find(command_id);
    if (iter == commands_.end() || !iter->second.execute_fn)
    {
        return false;
    }

    record_usage(command_id);

    return iter->second.execute_fn();
}

// ── Fuzzy Search ──

auto CommandRegistry::fuzzy_score(const std::string& query, const std::string& candidate) -> int
{
    if (query.empty())
    {
        return 0;
    }

    auto lower_query = to_lower(query);
    auto lower_candidate = to_lower(candidate);

    // Exact match
    if (lower_candidate == lower_query)
    {
        return 100;
    }

    // Prefix match
    if (lower_candidate.starts_with(lower_query))
    {
        return 90;
    }

    // Substring match
    if (lower_candidate.find(lower_query) != std::string::npos)
    {
        return 75;
    }

    // Fuzzy: characters appear in order but not necessarily consecutive
    std::size_t query_idx = 0;
    int consecutive_bonus = 0;
    bool last_matched = false;

    for (std::size_t candidate_pos = 0;
         candidate_pos < lower_candidate.size() && query_idx < lower_query.size();
         ++candidate_pos)
    {
        if (lower_candidate[candidate_pos] == lower_query[query_idx])
        {
            if (last_matched)
            {
                consecutive_bonus += 5;
            }
            last_matched = true;
            ++query_idx;
        }
        else
        {
            last_matched = false;
        }
    }

    if (query_idx == lower_query.size())
    {
        // All query chars found in order
        const int base_score = 40;
        // Bonus for matching more of the candidate
        auto coverage = static_cast<int>((lower_query.size() * 100) / lower_candidate.size());
        return base_score + (coverage / 5) + consecutive_bonus;
    }

    return 0; // No match
}

auto CommandRegistry::search(const std::string& query, int max_results) const
    -> std::vector<SearchResult>
{
    if (query.empty())
    {
        return {};
    }

    std::vector<SearchResult> results;
    results.reserve(commands_.size());

    for (const auto& cmd_id : insertion_order_)
    {
        auto iter = commands_.find(cmd_id);
        if (iter == commands_.end())
        {
            continue;
        }

        const auto& entry = iter->second;

        // Score against title (primary) and category:title (combined)
        const int title_score = fuzzy_score(query, entry.title);
        const int combined_score = fuzzy_score(query, entry.category + ": " + entry.title);
        int best_score = std::max(title_score, combined_score);

        if (best_score > 0)
        {
            // Boost recently used commands
            auto usage_iter = usage_history_.find(entry.id);
            if (usage_iter != usage_history_.end())
            {
                best_score += std::min(usage_iter->second.use_count, 10);
            }

            results.push_back({&entry, best_score});
        }
    }

    // Sort by score descending
    std::sort(results.begin(),
              results.end(),
              [](const SearchResult& lhs, const SearchResult& rhs)
              { return lhs.score > rhs.score; });

    // Limit results
    if (static_cast<int>(results.size()) > max_results)
    {
        results.resize(static_cast<std::size_t>(max_results));
    }

    return results;
}

// ── Usage Tracking ──

void CommandRegistry::record_usage(const std::string& command_id)
{
    auto& record = usage_history_[command_id];
    record.command_id = command_id;
    record.last_used = std::chrono::steady_clock::now();
    record.use_count++;
}

auto CommandRegistry::get_recently_used(int count) const -> std::vector<CommandUsageRecord>
{
    std::vector<CommandUsageRecord> records;
    records.reserve(usage_history_.size());
    for (const auto& [cmd_id, record] : usage_history_)
    {
        records.push_back(record);
    }

    // Sort by last_used descending
    std::sort(records.begin(),
              records.end(),
              [](const CommandUsageRecord& lhs, const CommandUsageRecord& rhs)
              { return lhs.last_used > rhs.last_used; });

    if (static_cast<int>(records.size()) > count)
    {
        records.resize(static_cast<std::size_t>(count));
    }

    return records;
}

auto CommandRegistry::get_most_frequent(int count) const -> std::vector<CommandUsageRecord>
{
    std::vector<CommandUsageRecord> records;
    records.reserve(usage_history_.size());
    for (const auto& [cmd_id, record] : usage_history_)
    {
        records.push_back(record);
    }

    // Sort by use_count descending
    std::sort(records.begin(),
              records.end(),
              [](const CommandUsageRecord& lhs, const CommandUsageRecord& rhs)
              { return lhs.use_count > rhs.use_count; });

    if (static_cast<int>(records.size()) > count)
    {
        records.resize(static_cast<std::size_t>(count));
    }

    return records;
}

void CommandRegistry::clear_history()
{
    usage_history_.clear();
}

auto CommandRegistry::export_history_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\n  \"usage\": [\n";

    bool first = true;
    for (const auto& [cmd_id, record] : usage_history_)
    {
        if (!first)
        {
            oss << ",\n";
        }
        first = false;
        oss << R"(    {"id": ")" << record.command_id << R"(", "count": )" << record.use_count
            << "}";
    }

    oss << "\n  ]\n}";
    return oss.str();
}

void CommandRegistry::import_history_json(const std::string& json_data)
{
    // Simple JSON parser for {"usage": [{"id": "...", "count": N}, ...]}
    // Find each "id" and "count" pair
    std::size_t pos = 0;
    while (pos < json_data.size())
    {
        auto id_pos = json_data.find("\"id\"", pos);
        if (id_pos == std::string::npos)
        {
            break;
        }

        // Find the value after "id": "
        auto val_start = json_data.find('"', id_pos + 5);
        if (val_start == std::string::npos)
        {
            break;
        }
        val_start++; // skip opening quote
        auto val_end = json_data.find('"', val_start);
        if (val_end == std::string::npos)
        {
            break;
        }

        const std::string command_id = json_data.substr(val_start, val_end - val_start);

        // Find "count": N
        auto count_pos = json_data.find("\"count\"", val_end);
        if (count_pos == std::string::npos)
        {
            break;
        }

        auto colon_pos = json_data.find(':', count_pos + 7);
        if (colon_pos == std::string::npos)
        {
            break;
        }

        // Read the integer value
        auto num_start = colon_pos + 1;
        while (num_start < json_data.size() && json_data[num_start] == ' ')
        {
            num_start++;
        }

        int count = 0;
        while (num_start < json_data.size() && std::isdigit(json_data[num_start]) != 0)
        {
            count = count * 10 + (json_data[num_start] - '0');
            num_start++;
        }

        if (!command_id.empty() && count > 0)
        {
            auto& record = usage_history_[command_id];
            record.command_id = command_id;
            record.use_count = count;
            record.last_used = std::chrono::steady_clock::now();
        }

        pos = num_start;
    }
}

} // namespace markamp::core
