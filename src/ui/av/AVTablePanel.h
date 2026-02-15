#pragma once

#include "core/av/AVColumnType.h"
#include "core/av/AVQueryEngine.h"
#include "core/av/AttributeView.h"

#include <memory>
#include <string>
#include <vector>

namespace markamp::ui::av
{

struct AVCellPosition
{
    int row{-1};
    int col{-1};
    std::string block_id;
    std::string key_id;
};

struct AVTableState
{
    std::string av_id;
    std::string view_id;
    int current_page{1};
    AVCellPosition selected_cell;
    AVCellPosition editing_cell;
    std::vector<std::string> selected_rows;
    bool show_row_numbers{false};
    bool stripe_rows{true};
};

/// Stub: Table panel for attribute view rendering.
/// Full wxWidgets implementation deferred to UI batch.
class AVTablePanel
{
public:
    AVTablePanel() = default;
    ~AVTablePanel() = default;

    void set_attribute_view(const std::string& av_id);
    [[nodiscard]] auto state() const -> const AVTableState&
    {
        return state_;
    }

private:
    AVTableState state_;
};

} // namespace markamp::ui::av
