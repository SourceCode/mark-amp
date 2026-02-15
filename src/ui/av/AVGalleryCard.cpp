#include "AVGalleryCard.h"

namespace markamp::ui::av
{

void AVGalleryCard::set_data(AVGalleryCardData data)
{
    data_ = std::move(data);
}

} // namespace markamp::ui::av
