#include "DragUndoAction.h"

namespace markamp::ui
{

DragUndoAction::DragUndoAction(DragState before, DragState after)
    : before_(std::move(before))
    , after_(std::move(after))
{
}

void DragUndoAction::execute()
{
    // Apply the drag: move item from before_.source to after_.destination.
    // Actual implementation depends on the container being dragged to/from.
    // This command captures the state for undo/redo.
}

void DragUndoAction::undo()
{
    // Reverse: move item from after_.destination back to before_.source.
}

} // namespace markamp::ui
