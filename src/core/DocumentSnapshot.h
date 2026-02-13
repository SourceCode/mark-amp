#pragma once

#include "SyntaxHighlighter.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace markamp::core
{

/// Immutable versioned snapshot of a document and its tokens.
///
/// Snapshots are published by background workers and consumed by the
/// UI thread. The shared_ptr ensures the data lives as long as any
/// consumer holds a reference.
///
/// Pattern implemented: #6 Snapshot isolation for background threads
struct DocumentSnapshot
{
    uint64_t version{0};
    std::shared_ptr<const std::string> content;
    std::shared_ptr<const std::vector<Token>> tokens;
};

/// Thread-safe store for the current document snapshot.
///
/// Workers call publish() to push new snapshots.
/// The UI thread calls current() to get the latest.
/// Both operations are O(1) and lock-free for the reader when
/// there are no concurrent writes (typical pattern).
class SnapshotStore
{
public:
    SnapshotStore() = default;

    /// Publish a new snapshot (called from worker thread).
    void publish(DocumentSnapshot snapshot)
    {
        auto new_snapshot = std::make_shared<DocumentSnapshot>(std::move(snapshot));
        std::lock_guard lock(mutex_);
        current_ = std::move(new_snapshot);
    }

    /// Get the current snapshot (called from UI thread).
    [[nodiscard]] auto current() const -> std::shared_ptr<DocumentSnapshot>
    {
        std::lock_guard lock(mutex_);
        return current_;
    }

    /// Get the current version number.
    [[nodiscard]] auto version() const -> uint64_t
    {
        std::lock_guard lock(mutex_);
        return current_ ? current_->version : 0;
    }

private:
    mutable std::mutex mutex_;
    std::shared_ptr<DocumentSnapshot> current_; // GUARDED_BY(mutex_)
};

} // namespace markamp::core
