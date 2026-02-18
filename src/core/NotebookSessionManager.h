/// @file NotebookSessionManager.h
/// @brief V8 Phase 15 – Notebook session management: checkpoint/restore, auto-save, trust.

#pragma once

#include "core/EventBus.h"

#include <chrono>
#include <expected>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Trust levels
// ============================================================================

/// Notebook trust level determines whether outputs (HTML/JS) are trusted.
enum class NotebookTrustLevel : uint8_t
{
    kUntrusted,        ///< Outputs are not trusted (sanitize HTML/JS)
    kTrusted,          ///< User has explicitly trusted the notebook
    kSignatureVerified ///< Notebook signature matches stored hash
};

// ============================================================================
// Checkpoint info
// ============================================================================

/// Information about a saved checkpoint.
struct CheckpointInfo
{
    std::string checkpoint_id;
    std::string notebook_id;
    std::chrono::system_clock::time_point created_at;
    int cell_count{0};
    int64_t size_bytes{0};
};

// ============================================================================
// Session state
// ============================================================================

/// Current session state of a notebook.
struct SessionState
{
    std::string notebook_id;
    bool dirty{false};
    NotebookTrustLevel trust_level{NotebookTrustLevel::kUntrusted};
    std::chrono::system_clock::time_point last_save;
    std::chrono::system_clock::time_point last_checkpoint;
    int checkpoint_count{0};
    int auto_save_interval_seconds{30};
};

// ============================================================================
// NotebookSessionManager
// ============================================================================

class NotebookSessionManager
{
public:
    explicit NotebookSessionManager(EventBus& event_bus);

    /// Create a checkpoint of the current notebook state.
    [[nodiscard]] auto save_checkpoint(const std::string& notebook_id)
        -> std::expected<std::string, std::string>;

    /// Restore a notebook from a specific checkpoint.
    [[nodiscard]] auto restore_checkpoint(const std::string& checkpoint_id)
        -> std::expected<void, std::string>;

    /// List all checkpoints for a notebook.
    [[nodiscard]] auto list_checkpoints(const std::string& notebook_id) const
        -> std::vector<CheckpointInfo>;

    /// Delete a specific checkpoint.
    auto delete_checkpoint(const std::string& checkpoint_id) -> void;

    /// Mark the active notebook session as dirty (unsaved changes).
    auto mark_dirty(const std::string& notebook_id) -> void;

    /// Mark the active notebook session as clean (just saved).
    auto mark_clean(const std::string& notebook_id) -> void;

    /// Called periodically by a timer. Creates a checkpoint if the notebook is dirty.
    auto auto_save_tick(const std::string& notebook_id) -> void;

    /// Sign (trust) a notebook — mark all existing outputs as trusted.
    auto sign_notebook(const std::string& notebook_id) -> void;

    /// Revoke trust from a notebook.
    auto revoke_trust(const std::string& notebook_id) -> void;

    /// Verify the trust level of a notebook.
    [[nodiscard]] auto verify_trust(const std::string& notebook_id) const -> NotebookTrustLevel;

    /// Get the session state for a notebook.
    [[nodiscard]] auto get_session(const std::string& notebook_id) const
        -> std::optional<SessionState>;

    /// Start a new session for a notebook.
    auto start_session(const std::string& notebook_id) -> void;

    /// End a session for a notebook.
    auto end_session(const std::string& notebook_id) -> void;

    /// Get all active sessions.
    [[nodiscard]] auto active_sessions() const -> std::vector<SessionState>;

    /// Set auto-save interval (in seconds).
    auto set_auto_save_interval(const std::string& notebook_id, int seconds) -> void;

private:
    EventBus& event_bus_;
    std::vector<SessionState> sessions_;
    std::vector<CheckpointInfo> checkpoints_;
    int next_checkpoint_id_{1};

    /// Find a session by notebook ID.
    [[nodiscard]] auto find_session(const std::string& notebook_id) -> SessionState*;

    [[nodiscard]] auto find_session(const std::string& notebook_id) const -> const SessionState*;

    /// Generate a unique checkpoint ID.
    [[nodiscard]] auto generate_checkpoint_id() -> std::string;
};

} // namespace markamp::core
