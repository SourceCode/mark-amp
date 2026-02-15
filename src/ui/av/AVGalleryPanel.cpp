#include "AVGalleryPanel.h"

namespace markamp::ui::av
{

void AVGalleryPanel::set_attribute_view(const std::string& av_id)
{
    state_.av_id = av_id;
    state_.current_page = 1;
}

} // namespace markamp::ui::av
