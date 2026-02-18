#pragma once

/// @file SearchIndexWatcher.h
/// @brief V9 Phase 10 – Search index auto-rebuild watcher.
///
/// Header-only implementation of file change monitoring for search indexing:
///   - Debounced reindex queue
///   - Batch indexing with full/incremental scheduling
///   - Index health monitoring
///   - Stale document detection

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>

namespace markamp::core
{

// ────────────────────────────────────────────────────────────
// IndexHealth
// ────────────────────────────────────────────────────────────

/// Health status of the search index.
struct IndexHealth
{
    /// Total number of documents in the vault.
    int total_docs{0};

    /// Number of documents currently indexed.
    int indexed_docs{0};

    /// Number of documents that need re-indexing.
    int stale_docs{0};

    /// Timestamp of the last full rebuild.
    std::chrono::system_clock::time_point last_rebuild_time;

    /// Approximate size of the index in bytes.
    std::int64_t index_size_bytes{0};

    /// Whether a rebuild is currently in progress.
    bool is_rebuilding{false};

    /// Health ratio (indexed_docs / total_docs), 0.0 to 1.0.
    [[nodiscard]] auto health_ratio() const noexcept -> double
    {
        if (total_docs <= 0)
        {
            return 1.0;
        }
        return static_cast<double>(indexed_docs - stale_docs) / static_cast<double>(total_docs);
    }
};

// ────────────────────────────────────────────────────────────
// SearchIndexWatcher
// ────────────────────────────────────────────────────────────

/// Monitors file changes and manages search index updates.
class SearchIndexWatcher
{
public:
    /// Default debounce interval in milliseconds.
    static constexpr int kDefaultDebounceMs = 500;

    /// Maximum queue size before forcing a flush.
    static constexpr std::size_t kMaxQueueSize = 200;

    /// Stale threshold: rebuild when stale_docs/total_docs exceeds this ratio.
    static constexpr double kStaleThreshold = 0.10;

    SearchIndexWatcher() = default;

    /// Set the debounce interval in milliseconds.
    void set_debounce_ms(int debounce_ms)
    {
        debounce_ms_ = debounce_ms;
    }

    /// Get the current debounce interval.
    [[nodiscard]] auto debounce_ms() const noexcept -> int
    {
        return debounce_ms_;
    }

    // ── File Change Handlers ─────────────────────────────────

    /// Called when a file is modified.
    void on_file_changed(const std::string& path)
    {
        queue_reindex(path);
        health_.stale_docs++;
    }

    /// Called when a new file is created.
    void on_file_created(const std::string& path)
    {
        queue_reindex(path);
        health_.total_docs++;
        health_.stale_docs++;
    }

    /// Called when a file is deleted.
    void on_file_deleted(const std::string& path)
    {
        queue_deletion(path);
        health_.total_docs = std::max(0, health_.total_docs - 1);
        health_.indexed_docs = std::max(0, health_.indexed_docs - 1);
    }

    // ── Queue Management ─────────────────────────────────────

    /// Queue a document for reindexing.
    void queue_reindex(const std::string& document_id)
    {
        // Deduplicate: don't add if already in queue
        for (const auto& entry : reindex_queue_)
        {
            if (entry.document_id == document_id && !entry.is_deletion)
            {
                return;
            }
        }

        QueueEntry entry;
        entry.document_id = document_id;
        entry.is_deletion = false;
        entry.queued_at = std::chrono::system_clock::now();
        reindex_queue_.push_back(std::move(entry));

        // Force flush if queue is too large
        if (reindex_queue_.size() > kMaxQueueSize)
        {
            needs_full_rebuild_ = true;
        }
    }

    /// Queue a document for deletion from the index.
    void queue_deletion(const std::string& document_id)
    {
        QueueEntry entry;
        entry.document_id = document_id;
        entry.is_deletion = true;
        entry.queued_at = std::chrono::system_clock::now();
        reindex_queue_.push_back(std::move(entry));
    }

    /// Process the reindex queue. Returns the number of items processed.
    /// In a real implementation, this would call SearchEngine::index_document()
    /// and SearchEngine::remove_document().
    auto process_queue() -> int
    {
        if (reindex_queue_.empty())
        {
            return 0;
        }

        const auto now = std::chrono::system_clock::now();
        int processed = 0;

        // Process entries older than debounce interval
        while (!reindex_queue_.empty())
        {
            const auto& front = reindex_queue_.front();
            const auto age_ms =
                std::chrono::duration_cast<std::chrono::milliseconds>(now - front.queued_at)
                    .count();

            if (age_ms < debounce_ms_)
            {
                break; // Not yet ready
            }

            if (!front.is_deletion)
            {
                // Would call search_engine_->index_document(front.document_id)
                health_.indexed_docs++;
                health_.stale_docs = std::max(0, health_.stale_docs - 1);
            }
            // Deletions already handled in on_file_deleted

            reindex_queue_.pop_front();
            processed++;
        }

        return processed;
    }

    /// Get the number of pending reindex operations.
    [[nodiscard]] auto pending_count() const noexcept -> int
    {
        return static_cast<int>(reindex_queue_.size());
    }

    /// Check if any indexing work is pending or in progress.
    [[nodiscard]] auto is_indexing() const noexcept -> bool
    {
        return !reindex_queue_.empty() || health_.is_rebuilding;
    }

    // ── Batch Indexing ───────────────────────────────────────

    /// Schedule a full index rebuild.
    void schedule_full_rebuild()
    {
        needs_full_rebuild_ = true;
        reindex_queue_.clear();
    }

    /// Schedule an incremental index update.
    void schedule_incremental()
    {
        needs_incremental_ = true;
    }

    /// Check if a full rebuild has been scheduled.
    [[nodiscard]] auto needs_full_rebuild() const noexcept -> bool
    {
        return needs_full_rebuild_;
    }

    /// Check if an incremental update has been scheduled.
    [[nodiscard]] auto needs_incremental() const noexcept -> bool
    {
        return needs_incremental_;
    }

    /// Mark rebuild as started.
    void begin_rebuild()
    {
        health_.is_rebuilding = true;
    }

    /// Mark rebuild as complete.
    void complete_rebuild()
    {
        health_.is_rebuilding = false;
        health_.stale_docs = 0;
        health_.last_rebuild_time = std::chrono::system_clock::now();
        needs_full_rebuild_ = false;
        needs_incremental_ = false;
    }

    // ── Index Health ─────────────────────────────────────────

    /// Get current index health status.
    [[nodiscard]] auto get_index_health() const -> IndexHealth
    {
        return health_;
    }

    /// Set index health (for external updates).
    void set_index_health(const IndexHealth& health)
    {
        health_ = health;
    }

    /// Check if a rebuild should be triggered based on stale threshold.
    [[nodiscard]] auto should_rebuild() const noexcept -> bool
    {
        if (needs_full_rebuild_)
        {
            return true;
        }
        if (health_.total_docs <= 0)
        {
            return false;
        }
        const double stale_ratio =
            static_cast<double>(health_.stale_docs) / static_cast<double>(health_.total_docs);
        return stale_ratio > kStaleThreshold;
    }

private:
    /// A queued reindex or deletion operation.
    struct QueueEntry
    {
        std::string document_id;
        bool is_deletion{false};
        std::chrono::system_clock::time_point queued_at;
    };

    std::deque<QueueEntry> reindex_queue_;
    IndexHealth health_;
    int debounce_ms_{kDefaultDebounceMs};
    bool needs_full_rebuild_{false};
    bool needs_incremental_{false};
};

} // namespace markamp::core
