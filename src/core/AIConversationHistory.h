// ============================================================================
// File: src/core/AIConversationHistory.h
// Phase 26: AI Integration — conversation persistence and search
// ============================================================================
#pragma once

#include "AITypes.h"

#include <expected>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Persists AI conversation sessions, supports search and retention management.
/// Storage: in-memory map (serializable to .markamp/ai-history/).
class AIConversationHistory
{
public:
    AIConversationHistory() = default;

    /// Save a conversation session. Overwrites if session_id exists.
    auto save_conversation(const AISession& session) -> void;

    /// Load a conversation by session ID.
    [[nodiscard]] auto load_conversation(const std::string& session_id) const
        -> std::expected<AISession, std::string>;

    /// List all saved conversations, newest first.
    [[nodiscard]] auto list_conversations() const -> std::vector<AIConversationEntry>;

    /// Search conversations by content substring.
    [[nodiscard]] auto search_conversations(const std::string& query) const
        -> std::vector<AIConversationEntry>;

    /// Delete a conversation by session ID.
    auto delete_conversation(const std::string& session_id) -> bool;

    /// Set retention period in days (default 90). 0 = keep forever.
    auto set_retention_days(int days) -> void;

    /// Get current retention period.
    [[nodiscard]] auto retention_days() const -> int;

    /// Remove conversations older than retention period. Returns count removed.
    auto cleanup_expired(int64_t current_time) -> int;

    /// Get total number of saved conversations.
    [[nodiscard]] auto conversation_count() const -> size_t;

    /// Clear all saved conversations.
    auto clear_all() -> void;

private:
    std::unordered_map<std::string, AISession> conversations_;
    int retention_days_{90};

    /// Build a conversation entry summary from a full session.
    [[nodiscard]] static auto build_entry(const AISession& session) -> AIConversationEntry;
};

} // namespace markamp::core
