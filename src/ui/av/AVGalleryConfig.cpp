#include "AVGalleryConfig.h"

namespace markamp::ui::av
{

void AVGalleryConfig::set_state(AVGalleryConfigState new_state)
{
    state_ = std::move(new_state);
}

} // namespace markamp::ui::av
