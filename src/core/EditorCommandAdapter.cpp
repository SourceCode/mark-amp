/// @file EditorCommandAdapter.cpp
/// @brief P04-T03: Adapter between event-driven editor commands and active editor.

#include "EditorCommandAdapter.h"

#include "CommandFeedback.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

EditorCommandAdapter::EditorCommandAdapter(EventBus& bus, CommandFeedbackHelper& feedback)
    : event_bus_(bus)
    , feedback_(feedback)
{
    subscribe_all();
}

void EditorCommandAdapter::set_editor_available_callback(EditorAvailableCallback callback)
{
    editor_available_ = std::move(callback);
}

void EditorCommandAdapter::guarded_dispatch(const std::string& command_name)
{
    if (!editor_available_ || !editor_available_())
    {
        feedback_.report_disabled(command_name, "No active editor");
        return;
    }
    MARKAMP_LOG_DEBUG("Editor command dispatched: {}", command_name);
}

void EditorCommandAdapter::subscribe_all()
{
    // Helper macro to reduce repetition
    #define SUBSCRIBE_CMD(EventType, CmdName) \
        subscriptions_.push_back( \
            event_bus_.subscribe<events::EventType>( \
                [this](const events::EventType& /*evt*/) \
                { guarded_dispatch(CmdName); }))

    SUBSCRIBE_CMD(FindRequestEvent, "Find");
    SUBSCRIBE_CMD(ReplaceRequestEvent, "Replace");
    SUBSCRIBE_CMD(DuplicateLineRequestEvent, "Duplicate Line");
    SUBSCRIBE_CMD(ToggleCommentRequestEvent, "Toggle Comment");
    SUBSCRIBE_CMD(SortLinesAscRequestEvent, "Sort Lines Ascending");
    SUBSCRIBE_CMD(SortLinesDescRequestEvent, "Sort Lines Descending");
    SUBSCRIBE_CMD(TransformUpperRequestEvent, "Transform Uppercase");
    SUBSCRIBE_CMD(TransformLowerRequestEvent, "Transform Lowercase");
    SUBSCRIBE_CMD(TransformTitleRequestEvent, "Transform Title Case");
    SUBSCRIBE_CMD(GoToLineRequestEvent, "Go To Line");
    SUBSCRIBE_CMD(ToggleBlockCommentRequestEvent, "Toggle Block Comment");
    SUBSCRIBE_CMD(AddLineCommentRequestEvent, "Add Line Comment");
    SUBSCRIBE_CMD(RemoveLineCommentRequestEvent, "Remove Line Comment");
    SUBSCRIBE_CMD(CursorUndoRequestEvent, "Cursor Undo");
    SUBSCRIBE_CMD(CursorRedoRequestEvent, "Cursor Redo");
    SUBSCRIBE_CMD(AddCursorBelowRequestEvent, "Add Cursor Below");
    SUBSCRIBE_CMD(AddCursorAboveRequestEvent, "Add Cursor Above");
    SUBSCRIBE_CMD(AddCursorNextOccurrenceRequestEvent, "Add Cursor Next Occurrence");
    SUBSCRIBE_CMD(DuplicateSelectionOrLineRequestEvent, "Duplicate Selection or Line");
    SUBSCRIBE_CMD(DeleteDuplicateLinesRequestEvent, "Delete Duplicate Lines");

    #undef SUBSCRIBE_CMD

    MARKAMP_LOG_INFO("EditorCommandAdapter: {} command subscriptions active",
                     subscriptions_.size());
}

} // namespace markamp::core
