#include "CommandPaletteModel.h"

#include <algorithm>
#include <cctype>

namespace markamp::ui
{

void CommandPaletteModel::add_command(CommandMetadata command)
{
    commands_.push_back(std::move(command));
}

auto CommandPaletteModel::commands() const -> const std::vector<CommandMetadata>&
{
    return commands_;
}

auto CommandPaletteModel::command_count() const -> int
{
    return static_cast<int>(commands_.size());
}

auto CommandPaletteModel::fuzzy_score(const std::string& query, const std::string& candidate) -> int
{
    if (query.empty())
    {
        return 0;
    }

    // Lowercase both
    std::string lower_query = query;
    std::string lower_candidate = candidate;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char character)
                   { return static_cast<char>(std::tolower(character)); });
    std::transform(lower_candidate.begin(),
                   lower_candidate.end(),
                   lower_candidate.begin(),
                   [](unsigned char character)
                   { return static_cast<char>(std::tolower(character)); });

    // Exact prefix match is highest score
    if (lower_candidate.find(lower_query) == 0)
    {
        return 100;
    }

    // Contains match
    if (lower_candidate.find(lower_query) != std::string::npos)
    {
        return 50;
    }

    // Subsequence match
    std::size_t query_pos = 0;
    for (char candidate_char : lower_candidate)
    {
        if (query_pos < lower_query.size() && candidate_char == lower_query[query_pos])
        {
            ++query_pos;
        }
    }

    if (query_pos == lower_query.size())
    {
        return 25;
    }

    return 0;
}

auto CommandPaletteModel::search(const std::string& query) const -> std::vector<CommandMatch>
{
    std::vector<CommandMatch> results;

    for (int idx = 0; idx < command_count(); ++idx)
    {
        const auto& cmd = commands_[static_cast<std::size_t>(idx)];

        // Score against label
        int match_score = fuzzy_score(query, cmd.label);

        // Score against category + label combined
        match_score = std::max(match_score, fuzzy_score(query, cmd.category + " " + cmd.label));

        // Score against aliases
        for (const auto& alias : cmd.aliases)
        {
            match_score = std::max(match_score, fuzzy_score(query, alias));
        }

        if (match_score > 0)
        {
            // Boost pinned commands
            if (cmd.is_pinned)
            {
                match_score += 200;
            }

            // Boost MRU commands
            for (std::size_t mru_idx = 0; mru_idx < mru_history_.size(); ++mru_idx)
            {
                if (mru_history_[mru_idx] == cmd.command_id)
                {
                    // More recent = higher boost
                    match_score += static_cast<int>(50 - mru_idx);
                    break;
                }
            }

            results.push_back({idx, match_score});
        }
    }

    // Sort by score descending
    std::sort(results.begin(),
              results.end(),
              [](const CommandMatch& lhs, const CommandMatch& rhs)
              { return lhs.score > rhs.score; });

    return results;
}

void CommandPaletteModel::record_usage(const std::string& command_id)
{
    // Remove if already present
    mru_history_.erase(std::remove(mru_history_.begin(), mru_history_.end(), command_id),
                       mru_history_.end());

    // Insert at front
    mru_history_.insert(mru_history_.begin(), command_id);

    // Cap size
    if (static_cast<int>(mru_history_.size()) > kMaxMruEntries)
    {
        mru_history_.resize(static_cast<std::size_t>(kMaxMruEntries));
    }
}

auto CommandPaletteModel::mru_history() const -> const std::vector<std::string>&
{
    return mru_history_;
}

void CommandPaletteModel::pin_command(const std::string& command_id)
{
    for (auto& cmd : commands_)
    {
        if (cmd.command_id == command_id)
        {
            cmd.is_pinned = true;
            return;
        }
    }
}

void CommandPaletteModel::unpin_command(const std::string& command_id)
{
    for (auto& cmd : commands_)
    {
        if (cmd.command_id == command_id)
        {
            cmd.is_pinned = false;
            return;
        }
    }
}

auto CommandPaletteModel::needs_confirmation(const std::string& command_id) const -> bool
{
    for (const auto& cmd : commands_)
    {
        if (cmd.command_id == command_id)
        {
            return cmd.is_destructive;
        }
    }
    return false;
}

auto CommandPaletteModel::preview_text(const std::string& command_id) const -> std::string
{
    for (const auto& cmd : commands_)
    {
        if (cmd.command_id == command_id)
        {
            if (cmd.is_destructive)
            {
                return "⚠ " + cmd.description + " — This action cannot be undone.";
            }
            return cmd.description;
        }
    }
    return "";
}

} // namespace markamp::ui
