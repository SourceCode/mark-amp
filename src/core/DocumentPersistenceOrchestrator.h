/// @file DocumentPersistenceOrchestrator.h
/// @brief P04-T01: Single source of truth for document save lifecycle.
///
/// Centralizes save, save-as, save-all, revert, and dirty-state tracking.
/// All shell indicators (tab bar, title bar, status bar) subscribe to
/// events published by this orchestrator rather than tracking state locally.
#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{
class EventBus;
class Config;

/// Orchestrates document persistence operations and dirty-state tracking.
class DocumentPersistenceOrchestrator
{
public:
    DocumentPersistenceOrchestrator(EventBus& bus, Config& cfg);

    // ── Save operations ──

    /// Save the currently active document.
    /// @return true if save succeeded
    auto save(const std::string& file_id) -> bool;

    /// Save the active document under a new path.
    /// @return true if save-as succeeded
    auto save_as(const std::string& file_id, const std::string& new_path) -> bool;

    /// Save all dirty documents.
    /// @return number of files successfully saved
    auto save_all() -> int;

    /// Revert the active document to its last saved state.
    /// @return true if revert succeeded
    auto revert(const std::string& file_id) -> bool;

    // ── Dirty-state tracking ──

    /// Mark a document as dirty (modified since last save).
    void mark_dirty(const std::string& file_id);

    /// Mark a document as clean (saved).
    void mark_clean(const std::string& file_id);

    /// Check if a document is dirty.
    [[nodiscard]] auto is_dirty(const std::string& file_id) const -> bool;

    /// Get all dirty file IDs.
    [[nodiscard]] auto dirty_files() const -> std::vector<std::string>;

    /// Check if any documents are dirty.
    [[nodiscard]] auto has_dirty_files() const -> bool;

    /// Get count of dirty documents.
    [[nodiscard]] auto dirty_count() const -> int;

private:
    EventBus& event_bus_;
    Config& config_;
    std::unordered_set<std::string> dirty_set_;
};

} // namespace markamp::core
