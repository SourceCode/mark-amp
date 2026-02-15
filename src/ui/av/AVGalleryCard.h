#pragma once

#include "core/av/AVTypes.h"
#include "core/av/AVValue.h"

#include <string>
#include <vector>

namespace markamp::ui::av
{

struct AVGalleryCardData
{
    std::string block_id;
    std::string title;
    std::string cover_url;
    std::vector<std::pair<std::string, std::string>> fields; // key_name → display value
};

/// Stub: Gallery card rendering component.
class AVGalleryCard
{
public:
    AVGalleryCard() = default;
    ~AVGalleryCard() = default;

    void set_data(AVGalleryCardData data);
    [[nodiscard]] auto data() const -> const AVGalleryCardData&
    {
        return data_;
    }

private:
    AVGalleryCardData data_;
};

} // namespace markamp::ui::av
