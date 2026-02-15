#pragma once

#include "core/av/AVTypes.h"

#include <string>
#include <vector>

namespace markamp::ui::av
{

struct AVHeaderColumnInfo
{
    std::string key_id;
    std::string name;
    std::string icon;
    markamp::core::av::AVKeyType type{markamp::core::av::AVKeyType::Text};
    int width{200};
    bool pinned{false};
};

/// Stub: Column header renderer for table view.
class AVHeaderRenderer
{
public:
    AVHeaderRenderer() = default;
    ~AVHeaderRenderer() = default;

    void set_columns(std::vector<AVHeaderColumnInfo> columns);
    [[nodiscard]] auto columns() const -> const std::vector<AVHeaderColumnInfo>&
    {
        return columns_;
    }

private:
    std::vector<AVHeaderColumnInfo> columns_;
};

} // namespace markamp::ui::av
