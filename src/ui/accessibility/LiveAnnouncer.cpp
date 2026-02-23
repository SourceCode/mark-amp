#include "LiveAnnouncer.h"

#include "AccessibilityController.h"
#include "core/Events.h"

namespace markamp::ui::accessibility
{

LiveAnnouncer::LiveAnnouncer(core::EventBus& event_bus)
    : event_bus_(event_bus)
{
    subscribe_to_events();
}

LiveAnnouncer::~LiveAnnouncer() = default;

void LiveAnnouncer::subscribe_to_events()
{
    // Announce when a file is saved successfully
    file_saved_sub_ = event_bus_.subscribe<core::events::FileSavedEvent>(
        [](const core::events::FileSavedEvent& /*event*/)
        { AccessibilityController::get().announce("File saved"); });

    // Announce search results completion
    search_completed_sub_ = event_bus_.subscribe<core::events::SearchCompletedEvent>(
        [](const core::events::SearchCompletedEvent& event)
        {
            const std::string message =
                "Search completed with " + std::to_string(event.result_count) + " results.";
            AccessibilityController::get().announce(message);
        });

    // Announce extension installation
    extension_loaded_sub_ = event_bus_.subscribe<core::events::ExtensionInstalledEvent>(
        [](const core::events::ExtensionInstalledEvent& event) {
            AccessibilityController::get().announce("Extension " + event.extension_id +
                                                    " installed.");
        });
}

} // namespace markamp::ui::accessibility
