#pragma once

#include "core/av/AVFilter.h"

#include <string>
#include <vector>

namespace markamp::ui::av
{

struct AVFilterBarState
{
    bool visible{false};
    int active_count{0};
    markamp::core::av::AVFilterGroup filter_group;
};

/// Stub: Filter bar UI component.
class AVFilterBar
{
public:
    AVFilterBar() = default;
    ~AVFilterBar() = default;

    void set_state(AVFilterBarState new_state);
    [[nodiscard]] auto state() const -> const AVFilterBarState&
    {
        return state_;
    }

private:
    AVFilterBarState state_;
};

} // namespace markamp::ui::av
