#include "AVHeaderRenderer.h"

namespace markamp::ui::av
{

void AVHeaderRenderer::set_columns(std::vector<AVHeaderColumnInfo> columns)
{
    columns_ = std::move(columns);
}

} // namespace markamp::ui::av
