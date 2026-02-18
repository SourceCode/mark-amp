// ============================================================================
// File: src/core/AIConversationHistory.cpp
// Phase 26: AI Integration — conversation persistence implementation
// ============================================================================

#include "AIConversationHistory.h"

#include <algorithm>
#include <ranges>

namespace markamp::core
{

auto AIConversationHistory::save_conversation(const AISession& session) -> void
{
    conversations_[session.session_id] = session;
}

auto AIConversationHistory::load_conversation(const std::string& session_id) const
    -> std::expected<AISession, std::string>
{
    const auto iter = conversations_.find(session_id);
    if (iter == conversations_.end())
    {
        return std::unexpected("Conversation not found: " + session_id);
    }
    return iter->second;
}

auto AIConversationHistory::list_conversations() const -> std::vector<AIConversationEntry>
{
    std::vector<AIConversationEntry> entries;
    entries.reserve(conversations_.size());
    for (const auto& [session_id, session] : conversations_)
    {
        entries.push_back(build_entry(session));
    }
    // Sort by last_active descending (newest first).
    std::ranges::sort(entries,
                      [](const AIConversationEntry& lhs, const AIConversationEntry& rhs)
                      { return lhs.last_active > rhs.last_active; });
    return entries;
}

auto AIConversationHistory::search_conversations(const std::string& query) const
    -> std::vector<AIConversationEntry>
{
    if (query.empty())
    {
        return list_conversations();
    }

    // Convert query to lowercase for case-insensitive search.
    std::string lower_query = query;
    std::ranges::transform(
        lower_query, lower_query.begin(), [](unsigned char chr) { return std::tolower(chr); });

    std::vector<AIConversationEntry> results;
    for (const auto& [session_id, session] : conversations_)
    {
        bool matched = false;
        for (const auto& msg : session.messages)
        {
            std::string lower_content = msg.content;
            std::ranges::transform(lower_content,
                                   lower_content.begin(),
                                   [](unsigned char chr) { return std::tolower(chr); });
            if (lower_content.find(lower_query) != std::string::npos)
            {
                matched = true;
                break;
            }
        }
        if (matched)
        {
            results.push_back(build_entry(session));
        }
    }
    // Sort by last_active descending.
    std::ranges::sort(results,
                      [](const AIConversationEntry& lhs, const AIConversationEntry& rhs)
                      { return lhs.last_active > rhs.last_active; });
    return results;
}

auto AIConversationHistory::delete_conversation(const std::string& session_id) -> bool
{
    return conversations_.erase(session_id) > 0;
}

auto AIConversationHistory::set_retention_days(int days) -> void
{
    retention_days_ = days;
}

auto AIConversationHistory::retention_days() const -> int
{
    return retention_days_;
}

auto AIConversationHistory::cleanup_expired(int64_t current_time) -> int
{
    if (retention_days_ <= 0)
    {
        return 0; // Keep forever
    }

    const int64_t retention_seconds = static_cast<int64_t>(retention_days_) * 86400;
    const int64_t cutoff = current_time - retention_seconds;

    int removed = 0;
    for (auto iter = conversations_.begin(); iter != conversations_.end();)
    {
        if (iter->second.last_active < cutoff)
        {
            iter = conversations_.erase(iter);
            ++removed;
        }
        else
        {
            ++iter;
        }
    }
    return removed;
}

auto AIConversationHistory::conversation_count() const -> size_t
{
    return conversations_.size();
}

auto AIConversationHistory::clear_all() -> void
{
    conversations_.clear();
}

auto AIConversationHistory::build_entry(const AISession& session) -> AIConversationEntry
{
    AIConversationEntry entry;
    entry.session_id = session.session_id;
    entry.created_at = session.created_at;
    entry.last_active = session.last_active;
    entry.message_count = static_cast<int32_t>(session.messages.size());

    // Title from first user message, or session ID.
    for (const auto& msg : session.messages)
    {
        if (msg.role == AIRole::User)
        {
            entry.title = msg.content.substr(0, 80);
            if (msg.content.size() > 80)
            {
                entry.title += "...";
            }
            break;
        }
    }
    if (entry.title.empty())
    {
        entry.title = "Conversation " + session.session_id.substr(0, 8);
    }
    return entry;
}

} // namespace markamp::core
