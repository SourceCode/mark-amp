/// @file ActiveDocumentTracker.h
/// @brief P04-T04: Single active-document source of truth.
///
/// All navigation surfaces (TabBar, OpenEditorsSection, NavigationHistoryPanel,
/// MainFrame title) subscribe to events published by this tracker instead of
/// maintaining their own notion of the active document.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;

/// Tracks the currently active document and publishes changes.
class ActiveDocumentTracker
{
public:
    explicit ActiveDocumentTracker(EventBus& bus);

    /// Set the active document. Publishes ActiveFileChangedEvent if changed.
    /// Deduplicates rapid re-selections of the same file.
    void set_active(const std::string& file_id, int group_id = -1);

    /// Clear the active document (e.g., on last tab close).
    void clear();

    /// Get the currently active document file ID.
    [[nodiscard]] auto active_file() const -> const std::string& { return active_file_id_; }

    /// Get the active editor group ID.
    [[nodiscard]] auto active_group() const -> int { return active_group_id_; }

    /// Check if any document is active.
    [[nodiscard]] auto has_active() const -> bool { return !active_file_id_.empty(); }

    /// Push the current file to navigation history.
    void push_to_history();

    /// Navigate back in history.
    void navigate_back();

    /// Navigate forward in history.
    void navigate_forward();

    /// Get history size.
    [[nodiscard]] auto history_size() const -> int
    {
        return static_cast<int>(history_.size());
    }

private:
    EventBus& event_bus_;
    std::string active_file_id_;
    int active_group_id_{-1};

    // Simple navigation history
    std::vector<std::string> history_;
    int history_position_{-1};
    static constexpr int kMaxHistorySize = 100;
};

} // namespace markamp::core
