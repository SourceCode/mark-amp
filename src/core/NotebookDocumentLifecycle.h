/// @file NotebookDocumentLifecycle.h
/// @brief P08-T02: Notebook document lifecycle with persistence and tab state.
///
/// Makes notebook open, close, rename, remove, and save participate in
/// normal workbench resource flows.
#pragma once

#include <string>
#include <unordered_set>

namespace markamp::core
{
class EventBus;

/// Manages notebook document lifecycle in the workbench.
class NotebookDocumentLifecycle
{
public:
    explicit NotebookDocumentLifecycle(EventBus& bus);

    /// Open a notebook document.
    void open(const std::string& notebook_id, const std::string& path);

    /// Close a notebook document.
    void close(const std::string& notebook_id);

    /// Rename a notebook.
    void rename(const std::string& notebook_id, const std::string& new_title);

    /// Save a notebook.
    void save(const std::string& notebook_id);

    /// Mark a notebook as dirty.
    void mark_dirty(const std::string& notebook_id);

    /// Clear dirty state.
    void clear_dirty(const std::string& notebook_id);

    /// Check if a notebook is dirty.
    [[nodiscard]] auto is_dirty(const std::string& notebook_id) const -> bool;

    /// Check if a notebook is open.
    [[nodiscard]] auto is_open(const std::string& notebook_id) const -> bool;

    /// Get count of open notebooks.
    [[nodiscard]] auto open_count() const -> int
    {
        return static_cast<int>(open_notebooks_.size());
    }

private:
    EventBus& event_bus_;
    std::unordered_set<std::string> open_notebooks_;
    std::unordered_set<std::string> dirty_notebooks_;
};

} // namespace markamp::core
