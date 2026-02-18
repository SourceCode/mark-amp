/// @file GitStashService.h
/// @brief Phase 25: Version Control Integration — Git stash management.
/// Provides save/pop/apply/drop operations on an in-memory stash stack
/// backed by GitService working-tree state.

#pragma once

#include <chrono>
#include <expected>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class GitService;

// ============================================================================
// Data structures
// ============================================================================

/// A single stash entry holding a snapshot of the working tree.
struct StashEntry
{
    std::string id;      ///< Unique stash ID (e.g. "stash@{0}").
    std::string message; ///< User-provided stash message.
    std::chrono::system_clock::time_point created;
    int file_count{0};                                        ///< Number of files saved.
    int creation_index{0};                                    ///< Monotonic creation counter.
    std::unordered_map<std::string, std::string> file_states; ///< path -> content snapshot.
};

// ============================================================================
// GitStashService
// ============================================================================

/// Manages a stack of stash entries that snapshot and restore the
/// GitService working tree.
class GitStashService
{
public:
    GitStashService(EventBus& event_bus, GitService& git_service);

    /// Save current working tree changes to a new stash entry.
    /// @param message  Optional description; defaults to "WIP on <branch>".
    /// @return The stash ID on success.
    [[nodiscard]] auto save(const std::string& message = "")
        -> std::expected<std::string, std::string>;

    /// Pop the top stash entry: apply it and remove it from the stack.
    [[nodiscard]] auto pop() -> std::expected<std::string, std::string>;

    /// Apply a stash entry without removing it from the stack.
    /// @param stash_id  The stash ID to apply (defaults to top).
    [[nodiscard]] auto apply(const std::string& stash_id = "")
        -> std::expected<std::string, std::string>;

    /// Drop (delete) a stash entry without applying it.
    /// @param stash_id  The stash ID to drop (defaults to top).
    [[nodiscard]] auto drop(const std::string& stash_id = "") -> std::expected<void, std::string>;

    /// List all stash entries (newest first).
    [[nodiscard]] auto list() const -> std::vector<StashEntry>;

    /// Show the file list for a specific stash entry.
    [[nodiscard]] auto show(const std::string& stash_id) const
        -> std::expected<std::vector<std::string>, std::string>;

    /// Get the number of stash entries.
    [[nodiscard]] auto size() const -> int;

    /// Clear all stash entries.
    auto clear() -> void;

private:
    EventBus& event_bus_;
    GitService& git_service_;

    std::vector<StashEntry> stash_stack_; ///< Newest at back.
    int next_index_{0};

    /// Find a stash entry by ID.  Returns nullptr if not found.
    [[nodiscard]] auto find_stash(const std::string& stash_id) const -> const StashEntry*;

    /// Resolve an empty stash_id to the top entry's ID (if any).
    [[nodiscard]] auto resolve_id(const std::string& stash_id) const
        -> std::expected<std::string, std::string>;

    /// Restore file states from a stash entry into the GitService working tree.
    auto restore_files(const StashEntry& entry) -> void;
};

} // namespace markamp::core
