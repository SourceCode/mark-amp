#include "CanvasChromeCoordinator.h"

namespace markamp::core
{

auto CanvasChromeCoordinator::tool_corner_for_state(ToolState state) const -> CornerRadiusToken
{
    switch (state)
    {
        case ToolState::kIdle:
            return palette_.button_corner;
        case ToolState::kActive:
            return CornerRadiusToken::kMd;
        case ToolState::kHover:
            return palette_.button_corner;
        case ToolState::kDisabled:
            return palette_.button_corner;
    }
    return palette_.button_corner;
}

} // namespace markamp::core
