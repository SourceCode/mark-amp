#pragma once

#include "core/av/AttributeView.h"

#include <string>

namespace markamp::ui::av
{

struct AVGalleryPanelState
{
    std::string av_id;
    std::string view_id;
    int current_page{1};
    int card_columns{3};
};

/// Stub: Gallery panel for attribute view card grid.
class AVGalleryPanel
{
public:
    AVGalleryPanel() = default;
    ~AVGalleryPanel() = default;

    void set_attribute_view(const std::string& av_id);
    [[nodiscard]] auto state() const -> const AVGalleryPanelState&
    {
        return state_;
    }

private:
    AVGalleryPanelState state_;
};

} // namespace markamp::ui::av
