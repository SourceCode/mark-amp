#include "AVFilterBar.h"

namespace markamp::ui::av
{

void AVFilterBar::set_state(AVFilterBarState new_state)
{
    state_ = std::move(new_state);
}

} // namespace markamp::ui::av
