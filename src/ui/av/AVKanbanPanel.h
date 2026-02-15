#pragma once

#include "core/av/AttributeView.h"

#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::ui::av
{

struct AVKanbanPanelState
{
    std::string av_id;
    std::string view_id;
    std::string group_by_key_id;
    std::unordered_set<std::string> collapsed_lanes;
};

/// Stub: Kanban board panel.
class AVKanbanPanel
{
public:
    AVKanbanPanel() = default;
    ~AVKanbanPanel() = default;

    void set_attribute_view(const std::string& av_id);
    [[nodiscard]] auto state() const -> const AVKanbanPanelState&
    {
        return state_;
    }

private:
    AVKanbanPanelState state_;
};

} // namespace markamp::ui::av
