/// @file SidebarStateSynchronizer.cpp
/// @brief P05-T04: Keeps explorer, open editors, and search sidebar in sync.

#include "SidebarStateSynchronizer.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

SidebarStateSynchronizer::SidebarStateSynchronizer(EventBus& bus)
    : event_bus_(bus)
    , active_file_sub_(bus.subscribe<events::ActiveFileChangedEvent>(
          [this](const events::ActiveFileChangedEvent& evt)
          {
              active_file_id_ = evt.file_id;
              MARKAMP_LOG_DEBUG("SidebarSync: active file -> {}", evt.file_id);
          }))
{
}

void SidebarStateSynchronizer::resync()
{
    // Re-publish current state to trigger sidebar updates
    if (!active_file_id_.empty())
    {
        events::ActiveFileChangedEvent evt;
        evt.file_id = active_file_id_;
        event_bus_.publish(evt);
        MARKAMP_LOG_DEBUG("SidebarSync: resync published for {}", active_file_id_);
    }
}

} // namespace markamp::core
