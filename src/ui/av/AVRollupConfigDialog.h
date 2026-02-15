#pragma once

#include "core/av/AVTypes.h"

#include <string>

namespace markamp::ui::av
{

struct AVRollupConfigState
{
    std::string relation_key_id;
    std::string target_key_id;
    markamp::core::av::AVRollupCalc calc{markamp::core::av::AVRollupCalc::None};
};

/// Stub: Rollup column configuration dialog.
class AVRollupConfigDialog
{
public:
    AVRollupConfigDialog() = default;
    ~AVRollupConfigDialog() = default;

    void set_state(AVRollupConfigState new_state);
    [[nodiscard]] auto state() const -> const AVRollupConfigState&
    {
        return state_;
    }

private:
    AVRollupConfigState state_;
};

} // namespace markamp::ui::av
