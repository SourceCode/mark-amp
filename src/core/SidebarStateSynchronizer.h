/// @file SidebarStateSynchronizer.h
/// @brief P05-T04: Keeps explorer, open editors, and search sidebar in sync.
///
/// Subscribes to ActiveFileChangedEvent and ensures all sidebar document views
/// show consistent active/open state.
#pragma once

#include "EventBus.h"

#include <string>

namespace markamp::core
{

/// Synchronizes active-file state across all sidebar panels.
class SidebarStateSynchronizer
{
public:
    explicit SidebarStateSynchronizer(EventBus& bus);

    /// Get the currently synchronized active file.
    [[nodiscard]] auto active_file() const -> const std::string& { return active_file_id_; }

    /// Check if a file is the active one.
    [[nodiscard]] auto is_active(const std::string& file_id) const -> bool
    {
        return file_id == active_file_id_;
    }

    /// Force re-synchronization (e.g., after rename or sidebar mode switch).
    void resync();

private:
    EventBus& event_bus_;
    Subscription active_file_sub_;
    std::string active_file_id_;
};

} // namespace markamp::core
