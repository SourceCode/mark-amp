// ============================================================================
// File: src/core/HistoryService.h
// Phase 32: Document History & Snapshots — History service declaration
// ============================================================================
#pragma once

#include "HistoryTypes.h"

#include <chrono>
#include <expected>
#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

/// Service for managing document history and snapshots.
/// Creates timestamped copies of documents, supports browsing history,
/// computing diffs, and rolling back to previous versions.
class HistoryService
{
public:
    HistoryService(EventBus& event_bus, Config& config, std::filesystem::path workspace_root);
    ~HistoryService() = default;

    HistoryService(const HistoryService&) = delete;
    auto operator=(const HistoryService&) -> HistoryService& = delete;

    /// Initialize the history system: create directories, load index.
    [[nodiscard]] auto initialize() -> std::expected<void, std::string>;

    // ── History Retrieval ──

    /// Get history entries for a specific document.
    [[nodiscard]] auto get_doc_histories(const std::string& root_id, int page, int page_size) const
        -> std::expected<std::vector<HistoryEntry>, std::string>;

    /// Get history entries for a specific notebook.
    [[nodiscard]] auto get_notebook_history(const std::string& notebook_id,
                                            HistoryOpType type_filter,
                                            const std::string& query) const
        -> std::expected<std::vector<HistoryEntry>, std::string>;

    /// Get all documents that have history entries.
    [[nodiscard]] auto get_documents_with_history() const -> std::vector<HistorySummary>;

    /// Get the content of a specific history entry.
    [[nodiscard]] auto get_history_content(const std::string& history_entry_id) const
        -> std::expected<std::string, std::string>;

    // ── Snapshot Creation ──

    /// Create an automatic history snapshot (debounced + deduplication).
    [[nodiscard]] auto create_auto_snapshot(const std::string& root_id,
                                            const std::string& file_path,
                                            const std::string& content,
                                            const std::string& title)
        -> std::expected<std::string, std::string>;

    /// Create a manual snapshot (always saved, ignores debounce).
    [[nodiscard]] auto create_manual_snapshot(const std::string& root_id,
                                              const std::string& file_path,
                                              const std::string& content,
                                              const std::string& title)
        -> std::expected<std::string, std::string>;

    // ── Rollback ──

    /// Roll back a document to a specific history entry.
    [[nodiscard]] auto rollback_doc(const std::string& root_id, const std::string& history_entry_id)
        -> std::expected<std::string, std::string>;

    // ── Diff ──

    /// Compute the diff between two history entries.
    [[nodiscard]] auto diff_entries(const std::string& old_entry_id,
                                    const std::string& new_entry_id) const
        -> std::expected<DiffResult, std::string>;

    /// Compute diff between a history entry and current content.
    [[nodiscard]] auto diff_with_current(const std::string& history_entry_id,
                                         const std::string& current_content) const
        -> std::expected<DiffResult, std::string>;

    // ── Cleanup ──

    /// Clear all history for a specific document.
    [[nodiscard]] auto clear_doc_history(const std::string& root_id)
        -> std::expected<void, std::string>;

    /// Clear all history in the workspace.
    [[nodiscard]] auto clear_all_history() -> std::expected<void, std::string>;

    /// Run retention cleanup: delete old entries.
    [[nodiscard]] auto run_cleanup() -> std::expected<int, std::string>;

    // ── Configuration ──

    [[nodiscard]] auto history_config() const -> HistoryConfig;

private:
    EventBus& event_bus_;
    Config& config_;
    std::filesystem::path workspace_root_;
    std::filesystem::path history_root_; // workspace_root / "history"

    mutable std::mutex mutex_;

    /// Index of all history entries, keyed by entry ID.
    std::unordered_map<std::string, HistoryEntry> entry_index_;

    /// Per-document: last auto-snapshot timestamp (for debouncing).
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> last_auto_snapshot_;

    /// Create a history entry with the given parameters.
    [[nodiscard]] auto create_entry(const std::string& root_id,
                                    const std::string& file_path,
                                    const std::string& content,
                                    const std::string& title,
                                    HistoryOpType op_type)
        -> std::expected<std::string, std::string>;

    /// Generate the history directory path for the current timestamp.
    [[nodiscard]] auto history_dir_for_now() const -> std::filesystem::path;

    /// Compute SHA-256 hash of content for deduplication.
    [[nodiscard]] static auto compute_hash(const std::string& content) -> std::string;

    /// Load the history index from disk.
    [[nodiscard]] auto load_index() -> std::expected<void, std::string>;

    /// Save the history index to disk.
    [[nodiscard]] auto save_index() const -> std::expected<void, std::string>;
};

} // namespace markamp::core
