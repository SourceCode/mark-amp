#pragma once

/// @file ReplaceUndoBuffer.h
/// @brief Phase 34 – Snapshot store for project-wide Replace All undo.

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// A single file snapshot for undo.
struct FileSnapshot
{
    std::string file_path;
    std::string original_content;
};

/// Manages file content snapshots for undoing project-wide replace operations.
/// Holds only the most recent Replace All batch (single-undo policy).
class ReplaceUndoBuffer
{
public:
    /// Begin a new batch. Clears any previous undo data.
    void begin_batch();

    /// Save a snapshot of a file's content before modification.
    void snapshot(const std::string& file_path, const std::string& content);

    /// Undo all changes in the current batch.
    /// Returns the list of file snapshots that should be restored.
    [[nodiscard]] auto undo_all() -> std::vector<FileSnapshot>;

    /// Whether there is an undo operation available.
    [[nodiscard]] auto has_undo() const -> bool;

    /// Number of files in the current batch.
    [[nodiscard]] auto file_count() const -> std::size_t;

    /// Clear all undo data.
    void clear();

private:
    std::unordered_map<std::string, std::string> snapshots_;
    bool has_batch_{false};
};

} // namespace markamp::core
