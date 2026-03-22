#include "NotebookChromeCoordinator.h"

namespace markamp::core
{

auto NotebookChromeCoordinator::cell_border_for_state(CellState state) const -> BorderWeightToken
{
    switch (state)
    {
        case CellState::kIdle:
            return cell_.border;
        case CellState::kSelected:
            return cell_.selected_border;
        case CellState::kRunning:
            return BorderWeightToken::kMedium;
        case CellState::kSuccess:
            return BorderWeightToken::kThin;
        case CellState::kError:
            return BorderWeightToken::kMedium;
        case CellState::kQueued:
            return BorderWeightToken::kThin;
    }
    return cell_.border;
}

auto NotebookChromeCoordinator::cell_corner_for_state(CellState /*state*/) const
    -> CornerRadiusToken
{
    // V22 Phase 09: All cell states share the same corner radius.
    return cell_.corner;
}

} // namespace markamp::core
