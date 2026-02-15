#pragma once

#include "core/av/AVTypes.h"

#include <string>

namespace markamp::ui::av
{

struct AVGalleryConfigState
{
    markamp::core::av::AVCardSize card_size{markamp::core::av::AVCardSize::Medium};
    std::string cover_key_id;
    std::string cover_ratio{"16:9"};
    bool hide_empty_values{true};
};

/// Stub: Gallery config dialog/popover.
class AVGalleryConfig
{
public:
    AVGalleryConfig() = default;
    ~AVGalleryConfig() = default;

    void set_state(AVGalleryConfigState new_state);
    [[nodiscard]] auto state() const -> const AVGalleryConfigState&
    {
        return state_;
    }

private:
    AVGalleryConfigState state_;
};

} // namespace markamp::ui::av
