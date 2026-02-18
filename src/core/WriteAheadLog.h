/// WriteAheadLog.h — V7 Phase 32: Write-ahead logging for workspace state
///
/// WAL entries with sequence number, timestamp, operation type, and data
/// payload. Supports replay on startup, compaction, and size limits.

#pragma once

#include "Result.h"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// WAL Entry
// ══════════════════════════════════════════════════════════════════════════════

/// Types of WAL operations.
enum class WalOperation : uint8_t
{
    Set = 1,
    Delete = 2,
    Clear = 3,
    Checkpoint = 4,
};

/// A single WAL entry.
struct WalEntry
{
    uint64_t sequence{0};
    int64_t timestamp_ms{0};
    WalOperation operation{WalOperation::Set};
    std::string key;
    std::string value;
};

// ══════════════════════════════════════════════════════════════════════════════
// Write-Ahead Log
// ══════════════════════════════════════════════════════════════════════════════

/// Append-only write-ahead log with replay and compaction.
class WriteAheadLog
{
public:
    WriteAheadLog() = default;

    /// Open or create a WAL file at the given path.
    [[nodiscard]] auto open(const std::filesystem::path& path) -> Result<void>;

    /// Append an entry to the WAL.
    [[nodiscard]] auto append(WalOperation operation,
                              const std::string& key,
                              const std::string& value = {}) -> Result<uint64_t>;

    /// Replay all entries through a callback.
    [[nodiscard]] auto replay(std::function<void(const WalEntry&)> handler) const -> Result<size_t>;

    /// Get the current sequence number.
    [[nodiscard]] auto sequence() const noexcept -> uint64_t
    {
        return sequence_;
    }

    /// Get the number of entries.
    [[nodiscard]] auto entry_count() const noexcept -> size_t
    {
        return entries_.size();
    }

    /// Compact the WAL: remove superseded entries, keep only latest per key.
    [[nodiscard]] auto compact() -> Result<size_t>;

    /// Set maximum number of entries before auto-compact.
    void set_max_entries(size_t max_entries)
    {
        max_entries_ = max_entries;
    }

    /// Close the WAL.
    void close();

private:
    mutable std::mutex mutex_;
    std::filesystem::path path_;
    std::vector<WalEntry> entries_;
    uint64_t sequence_{0};
    size_t max_entries_{10000};
    bool is_open_{false};
};

} // namespace markamp::core
