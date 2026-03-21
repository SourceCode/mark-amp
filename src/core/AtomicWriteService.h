/// @file AtomicWriteService.h
/// @brief V20 P05-T02: Atomic writes, backup snapshots, and recovery journals.
///
/// Protects users from partial writes and crash-time data loss by writing
/// to temp files first, then atomically renaming to the target. Maintains
/// backup rotation and recovery journals.
#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Result of an atomic write operation.
struct AtomicWriteResult
{
    bool success{false};
    std::string error_message;
    std::string final_path;
    std::string backup_path;                        ///< Path to backup if created
    size_t bytes_written{0};
    bool used_temp_file{true};

    [[nodiscard]] auto ok() const noexcept -> bool { return success; }
};

/// Recovery journal entry.
struct RecoveryJournalEntry
{
    std::string artifact_id;
    std::string original_path;
    std::string journal_path;
    std::chrono::system_clock::time_point created_at;
    bool is_complete{false};
};

/// Safe write service with atomic operations and recovery support.
class AtomicWriteService
{
public:
    /// Write data atomically (temp file → rename).
    [[nodiscard]] auto write_atomic(const std::string& target_path,
                                     const std::string& content) -> AtomicWriteResult;

    /// Create a backup of an existing file before overwriting.
    [[nodiscard]] auto create_backup(const std::string& source_path) -> AtomicWriteResult;

    /// Record a recovery journal entry (for crash recovery).
    void record_journal_entry(const std::string& artifact_id,
                               const std::string& original_path,
                               const std::string& journal_path);

    /// List pending recovery entries.
    [[nodiscard]] auto pending_recoveries() const -> const std::vector<RecoveryJournalEntry>&
    {
        return journal_entries_;
    }

    /// Mark a journal entry as resolved.
    void resolve_recovery(const std::string& artifact_id);

    /// Clean up old journal entries.
    void cleanup_journals();

    /// Statistics.
    [[nodiscard]] auto write_count() const noexcept -> int { return write_count_; }
    [[nodiscard]] auto backup_count() const noexcept -> int { return backup_count_; }
    [[nodiscard]] auto journal_entry_count() const noexcept -> int
    {
        return static_cast<int>(journal_entries_.size());
    }

    /// Maximum backup rotations to keep.
    static constexpr int kMaxBackupRotations = 3;

private:
    std::vector<RecoveryJournalEntry> journal_entries_;
    int write_count_{0};
    int backup_count_{0};
};

} // namespace markamp::core
