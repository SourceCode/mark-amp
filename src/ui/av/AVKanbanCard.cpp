#include "AVKanbanCard.h"

namespace markamp::ui::av
{

void AVKanbanCard::set_data(AVKanbanCardData data)
{
    data_ = std::move(data);
}

} // namespace markamp::ui::av
