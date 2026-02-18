/// @file NotebookSessionManager.cpp
/// @brief V8 Phase 15 – Notebook session management implementation.

#include "core/NotebookSessionManager.h"

#include "core/Events.h"

#include <algorithm>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

NotebookSessionManager::NotebookSessionManager(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// Checkpoint management
// ============================================================================

auto NotebookSessionManager::save_checkpoint(const std::string& notebook_id)
    -> std::expected<std::string, std::string>
{
    auto* session = find_session(notebook_id);
    if (session == nullptr)
    {
        return std::unexpected("No active session for notebook: " + notebook_id);
    }

    auto checkpoint_id = generate_checkpoint_id();

    CheckpointInfo info;
    info.checkpoint_id = checkpoint_id;
    info.notebook_id = notebook_id;
    info.created_at = std::chrono::system_clock::now();
    info.cell_count = 0; // Would be populated from actual notebook data.
    info.size_bytes = 0;

    checkpoints_.push_back(std::move(info));

    session->checkpoint_count++;
    session->last_checkpoint = std::chrono::system_clock::now();

    // Publish event.
    events::NotebookCheckpointCreatedEvent evt;
    evt.notebook_id = notebook_id;
    evt.checkpoint_id = checkpoint_id;
    event_bus_.publish(evt);

    return checkpoint_id;
}

auto NotebookSessionManager::restore_checkpoint(const std::string& checkpoint_id)
    -> std::expected<void, std::string>
{
    auto iter = std::find_if(checkpoints_.begin(),
                             checkpoints_.end(),
                             [&checkpoint_id](const CheckpointInfo& checkpoint)
                             { return checkpoint.checkpoint_id == checkpoint_id; });

    if (iter == checkpoints_.end())
    {
        return std::unexpected("Checkpoint not found: " + checkpoint_id);
    }

    // Mark session as dirty (restored state differs from disk).
    mark_dirty(iter->notebook_id);

    return {};
}

auto NotebookSessionManager::list_checkpoints(const std::string& notebook_id) const
    -> std::vector<CheckpointInfo>
{
    std::vector<CheckpointInfo> result;
    for (const auto& checkpoint : checkpoints_)
    {
        if (checkpoint.notebook_id == notebook_id)
        {
            result.push_back(checkpoint);
        }
    }
    return result;
}

auto NotebookSessionManager::delete_checkpoint(const std::string& checkpoint_id) -> void
{
    checkpoints_.erase(std::remove_if(checkpoints_.begin(),
                                      checkpoints_.end(),
                                      [&checkpoint_id](const CheckpointInfo& checkpoint)
                                      { return checkpoint.checkpoint_id == checkpoint_id; }),
                       checkpoints_.end());
}

// ============================================================================
// Dirty state tracking
// ============================================================================

auto NotebookSessionManager::mark_dirty(const std::string& notebook_id) -> void
{
    auto* session = find_session(notebook_id);
    if (session != nullptr)
    {
        session->dirty = true;
    }
}

auto NotebookSessionManager::mark_clean(const std::string& notebook_id) -> void
{
    auto* session = find_session(notebook_id);
    if (session != nullptr)
    {
        session->dirty = false;
        session->last_save = std::chrono::system_clock::now();
    }
}

// ============================================================================
// Auto-save
// ============================================================================

auto NotebookSessionManager::auto_save_tick(const std::string& notebook_id) -> void
{
    auto* session = find_session(notebook_id);
    if (session == nullptr || !session->dirty)
    {
        return;
    }

    // Check if enough time has passed since last checkpoint.
    auto now = std::chrono::system_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(now - session->last_checkpoint).count();

    if (elapsed >= session->auto_save_interval_seconds)
    {
        auto result = save_checkpoint(notebook_id);
        if (result.has_value())
        {
            session->dirty = false;
        }
    }
}

// ============================================================================
// Trust management
// ============================================================================

auto NotebookSessionManager::sign_notebook(const std::string& notebook_id) -> void
{
    auto* session = find_session(notebook_id);
    if (session != nullptr)
    {
        session->trust_level = NotebookTrustLevel::kTrusted;
    }
}

auto NotebookSessionManager::revoke_trust(const std::string& notebook_id) -> void
{
    auto* session = find_session(notebook_id);
    if (session != nullptr)
    {
        session->trust_level = NotebookTrustLevel::kUntrusted;
    }
}

auto NotebookSessionManager::verify_trust(const std::string& notebook_id) const
    -> NotebookTrustLevel
{
    const auto* session = find_session(notebook_id);
    if (session != nullptr)
    {
        return session->trust_level;
    }
    return NotebookTrustLevel::kUntrusted;
}

// ============================================================================
// Session lifecycle
// ============================================================================

auto NotebookSessionManager::start_session(const std::string& notebook_id) -> void
{
    // Avoid duplicates.
    if (find_session(notebook_id) != nullptr)
    {
        return;
    }

    SessionState session;
    session.notebook_id = notebook_id;
    session.dirty = false;
    session.trust_level = NotebookTrustLevel::kUntrusted;
    session.last_save = std::chrono::system_clock::now();
    session.last_checkpoint = std::chrono::system_clock::now();
    session.checkpoint_count = 0;
    sessions_.push_back(std::move(session));
}

auto NotebookSessionManager::end_session(const std::string& notebook_id) -> void
{
    sessions_.erase(std::remove_if(sessions_.begin(),
                                   sessions_.end(),
                                   [&notebook_id](const SessionState& session)
                                   { return session.notebook_id == notebook_id; }),
                    sessions_.end());

    // Also clean up checkpoints for this notebook.
    checkpoints_.erase(std::remove_if(checkpoints_.begin(),
                                      checkpoints_.end(),
                                      [&notebook_id](const CheckpointInfo& checkpoint)
                                      { return checkpoint.notebook_id == notebook_id; }),
                       checkpoints_.end());
}

auto NotebookSessionManager::get_session(const std::string& notebook_id) const
    -> std::optional<SessionState>
{
    const auto* session = find_session(notebook_id);
    if (session != nullptr)
    {
        return *session;
    }
    return std::nullopt;
}

auto NotebookSessionManager::active_sessions() const -> std::vector<SessionState>
{
    return sessions_;
}

auto NotebookSessionManager::set_auto_save_interval(const std::string& notebook_id, int seconds)
    -> void
{
    auto* session = find_session(notebook_id);
    if (session != nullptr)
    {
        session->auto_save_interval_seconds = seconds;
    }
}

// ============================================================================
// Private helpers
// ============================================================================

auto NotebookSessionManager::find_session(const std::string& notebook_id) -> SessionState*
{
    for (auto& session : sessions_)
    {
        if (session.notebook_id == notebook_id)
        {
            return &session;
        }
    }
    return nullptr;
}

auto NotebookSessionManager::find_session(const std::string& notebook_id) const
    -> const SessionState*
{
    for (const auto& session : sessions_)
    {
        if (session.notebook_id == notebook_id)
        {
            return &session;
        }
    }
    return nullptr;
}

auto NotebookSessionManager::generate_checkpoint_id() -> std::string
{
    return "cp-" + std::to_string(next_checkpoint_id_++);
}

} // namespace markamp::core
