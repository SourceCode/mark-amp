/// @file NotebookShellHost.h
/// @brief P08-T01: Typed notebook workbench host.
///
/// Replaces notebook placeholder with a real shell host that supports
/// list, open, create, and empty/loading/error states.
#pragma once

#include "EventBus.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Notebook host load state.
enum class NotebookHostState
{
    kEmpty,
    kLoading,
    kReady,
    kError,
};

/// A notebook resource entry.
struct NotebookEntry
{
    std::string notebook_id;
    std::string title;
    std::string path;
    int cell_count{0};
};

/// Typed notebook workbench host.
class NotebookShellHost
{
public:
    explicit NotebookShellHost(EventBus& bus);

    /// Set the host state.
    void set_state(NotebookHostState state);

    /// Get current state.
    [[nodiscard]] auto state() const -> NotebookHostState { return state_; }

    /// Add a notebook entry to the list.
    void add_entry(const NotebookEntry& entry);

    /// Remove a notebook entry.
    void remove_entry(const std::string& notebook_id);

    /// Get all entries.
    [[nodiscard]] auto entries() const -> const std::vector<NotebookEntry>& { return entries_; }

    /// Get entry count.
    [[nodiscard]] auto entry_count() const -> int { return static_cast<int>(entries_.size()); }

    /// Open a notebook by ID.
    void open_notebook(const std::string& notebook_id);

    /// Create a new notebook.
    void create_notebook(const std::string& title);

    /// Get the active notebook ID.
    [[nodiscard]] auto active_notebook_id() const -> const std::string& { return active_id_; }

private:
    EventBus& event_bus_;
    NotebookHostState state_{NotebookHostState::kEmpty};
    std::vector<NotebookEntry> entries_;
    std::string active_id_;
};

} // namespace markamp::core
