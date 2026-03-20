// ============================================================================
// File: src/canvas/AutosaveModel.h
// Phase 01 W13: Autosave & Recovery — UI-independent autosave state model
// ============================================================================
#pragma once

#include <cstdint>
#include <string>

namespace markamp::canvas
{

/// Autosave lifecycle state.
enum class AutosaveState : uint8_t
{
    kIdle,    ///< No save pending
    kPending, ///< Dirty flag set, waiting for debounce timer
    kSaving,  ///< Save in progress
    kFailed,  ///< Last save attempt failed
};

/// Testable model for canvas board autosave and crash recovery.
///
/// Tracks:
///   - Dirty flag (board modified since last save)
///   - Autosave enabled/disabled and interval
///   - Save lifecycle state (idle → pending → saving → idle/failed)
///   - Recovery file path for crash restore
class AutosaveModel
{
public:
    // ── Dirty tracking ──────────────────────────────────────────────

    /// Mark the board as having unsaved changes.
    void mark_dirty() noexcept { dirty_ = true; }

    /// Clear the dirty flag after a successful save.
    void clear_dirty() noexcept { dirty_ = false; }

    /// Whether the board has unsaved changes.
    [[nodiscard]] auto is_dirty() const noexcept -> bool { return dirty_; }

    // ── Enabled / interval ──────────────────────────────────────────

    /// Set whether autosave is enabled.
    void set_enabled(bool enabled) noexcept { enabled_ = enabled; }

    /// Whether autosave is enabled.
    [[nodiscard]] auto is_enabled() const noexcept -> bool { return enabled_; }

    /// Set the autosave interval in milliseconds.
    void set_interval_ms(int ms) noexcept { interval_ms_ = ms; }

    /// Get the autosave interval in milliseconds.
    [[nodiscard]] auto interval_ms() const noexcept -> int { return interval_ms_; }

    // ── State lifecycle ─────────────────────────────────────────────

    /// Set the current autosave state.
    void set_state(AutosaveState state) noexcept { state_ = state; }

    /// Get the current autosave state.
    [[nodiscard]] auto state() const noexcept -> AutosaveState { return state_; }

    // ── Recovery ────────────────────────────────────────────────────

    /// Set the recovery file path.
    void set_recovery_path(const std::string& path) { recovery_path_ = path; }

    /// Get the recovery file path.
    [[nodiscard]] auto recovery_path() const noexcept -> const std::string& { return recovery_path_; }

    /// Whether recovery data exists.
    [[nodiscard]] auto has_recovery() const noexcept -> bool { return !recovery_path_.empty(); }

    // ── Helpers ─────────────────────────────────────────────────────

    /// Human-readable name for an autosave state.
    [[nodiscard]] static auto state_name(AutosaveState state) noexcept -> const char*
    {
        switch (state)
        {
            case AutosaveState::kIdle: return "idle";
            case AutosaveState::kPending: return "pending";
            case AutosaveState::kSaving: return "saving";
            case AutosaveState::kFailed: return "failed";
        }
        return "unknown";
    }

private:
    bool dirty_{false};
    bool enabled_{true};
    int interval_ms_{30000}; ///< Default: 30 seconds
    AutosaveState state_{AutosaveState::kIdle};
    std::string recovery_path_;
};

} // namespace markamp::canvas
