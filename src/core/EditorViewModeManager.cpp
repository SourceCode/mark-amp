/// @file EditorViewModeManager.cpp
/// @brief P04-T02: Source of truth for editor view mode.

#include "EditorViewModeManager.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

EditorViewModeManager::EditorViewModeManager(EventBus& bus)
    : event_bus_(bus)
{
}

void EditorViewModeManager::set_mode(EditorViewMode mode)
{
    if (mode == current_mode_)
    {
        return;
    }

    const auto previous = current_mode_;
    current_mode_ = mode;

    // Map our mode to the event's mode enum
    events::ViewModeChangedEvent evt;
    switch (mode)
    {
    case EditorViewMode::kEditor:
        evt.mode = events::ViewMode::Editor;
        break;
    case EditorViewMode::kPreview:
        evt.mode = events::ViewMode::Preview;
        break;
    case EditorViewMode::kSplit:
        evt.mode = events::ViewMode::Split;
        break;
    case EditorViewMode::kLivePreview:
        evt.mode = events::ViewMode::LivePreview;
        break;
    }
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("View mode changed: {} -> {}",
                      static_cast<int>(previous),
                      static_cast<int>(mode));
}

void EditorViewModeManager::cycle_mode()
{
    switch (current_mode_)
    {
    case EditorViewMode::kEditor:
        set_mode(EditorViewMode::kSplit);
        break;
    case EditorViewMode::kSplit:
        set_mode(EditorViewMode::kPreview);
        break;
    case EditorViewMode::kPreview:
    case EditorViewMode::kLivePreview:
        set_mode(EditorViewMode::kEditor);
        break;
    }
}

void EditorViewModeManager::toggle_preview()
{
    if (current_mode_ == EditorViewMode::kPreview)
    {
        set_mode(EditorViewMode::kEditor);
    }
    else
    {
        set_mode(EditorViewMode::kPreview);
    }
}

} // namespace markamp::core
