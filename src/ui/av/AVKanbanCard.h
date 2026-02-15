#pragma once

#include "core/av/AVTypes.h"

#include <string>
#include <vector>

namespace markamp::ui::av
{

struct AVKanbanCardData
{
    std::string block_id;
    std::string title;
    std::vector<std::pair<std::string, std::string>> fields;
};

/// Stub: Kanban card rendering component.
class AVKanbanCard
{
public:
    AVKanbanCard() = default;
    ~AVKanbanCard() = default;

    void set_data(AVKanbanCardData data);
    [[nodiscard]] auto data() const -> const AVKanbanCardData&
    {
        return data_;
    }

private:
    AVKanbanCardData data_;
};

} // namespace markamp::ui::av
