#pragma once

#include "core/EventBus.h"

namespace markamp::ui::accessibility
{

/// A background service that listens to core application events and translates them
/// into screen reader announcements, acting as an invisible live region.
class LiveAnnouncer
{
public:
    explicit LiveAnnouncer(core::EventBus& event_bus);
    ~LiveAnnouncer();

    LiveAnnouncer(const LiveAnnouncer&) = delete;
    auto operator=(const LiveAnnouncer&) -> LiveAnnouncer& = delete;

    LiveAnnouncer(LiveAnnouncer&&) = delete;
    auto operator=(LiveAnnouncer&&) -> LiveAnnouncer& = delete;

private:
    void subscribe_to_events();

    core::EventBus& event_bus_;
    core::Subscription file_saved_sub_;
    core::Subscription search_completed_sub_;
    core::Subscription extension_loaded_sub_;
};

} // namespace markamp::ui::accessibility
