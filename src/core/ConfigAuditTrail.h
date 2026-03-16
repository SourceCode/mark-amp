#pragma once

#include <chrono>
#include <cstddef>
#include <string>
#include <vector>

namespace markamp::core
{

/// A single entry in the config audit trail.
struct AuditEntry
{
    std::chrono::steady_clock::time_point timestamp;
    std::string key;
    std::string old_value;
    std::string new_value;
    std::string source; // "user", "profile", "migration", "import", etc.
};

/// In-memory ring buffer of the last N config changes for debugging and undo.
class ConfigAuditTrail
{
public:
    /// Construct with a maximum capacity (default 100).
    explicit ConfigAuditTrail(std::size_t max_entries = 100);

    /// Record a config change.
    void record(std::string key,
                std::string old_value,
                std::string new_value,
                std::string source = "user");

    /// Return all recorded entries (oldest first).
    [[nodiscard]] auto entries() const -> std::vector<AuditEntry>;

    /// Return the number of recorded entries.
    [[nodiscard]] auto size() const -> std::size_t;

    /// Return the maximum capacity.
    [[nodiscard]] auto capacity() const -> std::size_t;

    /// Clear all entries.
    void clear();

    /// Return the most recent entry, or nullptr if empty.
    [[nodiscard]] auto last_entry() const -> const AuditEntry*;

    /// (#92) Return entries for a specific config key.
    [[nodiscard]] auto entries_for_key(const std::string& key) const -> std::vector<AuditEntry>;

    // ── Batch 19-22 (#124-126) ──

    /// (#124) Return the number of distinct keys changed so far.
    [[nodiscard]] auto unique_key_count() const -> std::size_t;

    /// (#125) Return whether the ring buffer has wrapped.
    [[nodiscard]] auto is_full() const -> bool;

    /// (#126) Return the key with the highest number of recorded changes.
    [[nodiscard]] auto most_changed_key() const -> std::string;

private:
    std::vector<AuditEntry> ring_;
    std::size_t max_entries_;
    std::size_t head_{0};
    std::size_t count_{0};
};

} // namespace markamp::core
