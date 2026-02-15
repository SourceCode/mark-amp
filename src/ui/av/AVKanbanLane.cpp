#include "AVKanbanLane.h"

namespace markamp::ui::av
{

void AVKanbanLane::set_data(AVKanbanLaneData data)
{
    data_ = std::move(data);
}

} // namespace markamp::ui::av
