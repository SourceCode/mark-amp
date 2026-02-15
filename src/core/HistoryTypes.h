// ============================================================================
// File: src/core/HistoryTypes.h
// Phase 32: Document History & Snapshots — History types
// ============================================================================
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// The type of operation that created a history entry.
enum class HistoryOpType : uint8_t
{
    Update,  // Content was modified
    Delete,  // Document or block was deleted
    Clean,   // Cleanup operation (history purge)
    Format,  // Formatting change (no semantic change)
    Snapshot // Manual user-created snapshot
};

/// A single history entry representing a saved version of a document.
struct HistoryEntry
{
    std::string id;           // Unique entry ID (UUID)
    std::string root_id;      // Document root ID (file ID)
    std::string notebook_id;  // Notebook/folder containing the document
    std::string file_path;    // Relative path to the original document
    std::string history_path; // Path to the history copy (within history/ dir)
    std::string title;        // Document title at the time of snapshot
    HistoryOpType op_type{HistoryOpType::Update};
    std::string content_hash;    // SHA-256 hash of the content (for deduplication)
    std::size_t content_size{0}; // Size in bytes of the saved content
    std::chrono::system_clock::time_point created;

    [[nodiscard]] auto is_update() const -> bool
    {
        return op_type == HistoryOpType::Update;
    }

    [[nodiscard]] auto is_manual_snapshot() const -> bool
    {
        return op_type == HistoryOpType::Snapshot;
    }

    [[nodiscard]] auto op_type_display() const -> std::string_view
    {
        switch (op_type)
        {
            case HistoryOpType::Update:
                return "Modified";
            case HistoryOpType::Delete:
                return "Deleted";
            case HistoryOpType::Clean:
                return "Cleaned";
            case HistoryOpType::Format:
                return "Formatted";
            case HistoryOpType::Snapshot:
                return "Snapshot";
        }
        return "Unknown";
    }
};

/// A diff line type.
enum class DiffLineType : uint8_t
{
    Context,  // Unchanged line (shown for context)
    Addition, // Line added in the new version
    Deletion  // Line removed from the old version
};

/// A single line in a diff.
struct DiffLine
{
    DiffLineType type{DiffLineType::Context};
    int old_line_number{-1}; // Line number in old version (-1 if addition)
    int new_line_number{-1}; // Line number in new version (-1 if deletion)
    std::string content;
};

/// A contiguous group of diff lines (hunk).
struct DiffHunk
{
    int old_start{0};
    int old_count{0};
    int new_start{0};
    int new_count{0};
    std::vector<DiffLine> lines;

    [[nodiscard]] auto header() const -> std::string
    {
        return "@@ -" + std::to_string(old_start) + "," + std::to_string(old_count) + " +" +
               std::to_string(new_start) + "," + std::to_string(new_count) + " @@";
    }
};

/// Result of comparing two document versions.
struct DiffResult
{
    std::string old_title;
    std::string new_title;
    std::string old_timestamp;
    std::string new_timestamp;
    std::vector<DiffHunk> hunks;
    int additions{0};
    int deletions{0};
    int context_lines{0};

    [[nodiscard]] auto is_identical() const -> bool
    {
        return additions == 0 && deletions == 0;
    }

    [[nodiscard]] auto total_changes() const -> int
    {
        return additions + deletions;
    }

    [[nodiscard]] auto change_summary() const -> std::string
    {
        return "+" + std::to_string(additions) + " -" + std::to_string(deletions);
    }
};

/// Summary of history for a document.
struct HistorySummary
{
    std::string root_id;
    std::string title;
    int entry_count{0};
    std::chrono::system_clock::time_point oldest_entry;
    std::chrono::system_clock::time_point newest_entry;
    std::size_t total_storage_bytes{0};
};

/// Configuration for the history system.
struct HistoryConfig
{
    int retention_days{30};             // Delete entries older than this
    int max_entries_per_doc{100};       // Maximum entries per document
    int auto_save_interval_seconds{60}; // Minimum seconds between auto-saves
    int diff_context_lines{3};          // Context lines in diff output
    bool auto_save_enabled{true};       // Enable automatic history on save
};

} // namespace markamp::core
