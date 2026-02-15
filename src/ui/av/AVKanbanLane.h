#pragma once

#include "core/av/AVTypes.h"

#include <string>
#include <vector>

namespace markamp::ui::av
{

struct AVKanbanLaneData
{
    std::string option_name;
    std::string option_color;
    int card_count{0};
    bool collapsed{false};
    std::vector<std::string> block_ids;
};

/// Stub: Single kanban lane rendering component.
class AVKanbanLane
{
public:
    AVKanbanLane() = default;
    ~AVKanbanLane() = default;

    void set_data(AVKanbanLaneData data);
    [[nodiscard]] auto data() const -> const AVKanbanLaneData&
    {
        return data_;
    }

private:
    AVKanbanLaneData data_;
};

} // namespace markamp::ui::av
