#include "DragAutoScroll.h"

namespace markamp::ui
{

void DragAutoScroll::update(int cursor_y, int viewport_top, int viewport_height)
{
    const int viewport_bottom = viewport_top + viewport_height;
    const int relative_y = cursor_y - viewport_top;

    if (relative_y < edge_margin_ && cursor_y > viewport_top)
    {
        scroll_dir_ = -1;
        is_scrolling_ = true;
        if (callback_)
        {
            callback_(-scroll_speed_);
        }
    }
    else if (relative_y > viewport_height - edge_margin_ && cursor_y < viewport_bottom)
    {
        scroll_dir_ = 1;
        is_scrolling_ = true;
        if (callback_)
        {
            callback_(scroll_speed_);
        }
    }
    else
    {
        stop();
    }
}

void DragAutoScroll::stop()
{
    is_scrolling_ = false;
    scroll_dir_ = 0;
}

} // namespace markamp::ui
