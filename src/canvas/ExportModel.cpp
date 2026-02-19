#include "ExportModel.h"

#include <algorithm>

namespace markamp::canvas
{

void ExportModel::set_settings(ExportSettings settings)
{
    settings_ = settings;
}
auto ExportModel::settings() const -> const ExportSettings&
{
    return settings_;
}

void ExportModel::set_dpi(int dpi)
{
    settings_.dpi = std::clamp(dpi, 72, 600);
}
void ExportModel::set_scale(double scale)
{
    settings_.scale = std::clamp(scale, 0.1, 4.0);
}

void ExportModel::set_page_tiles(std::vector<PageTile> tiles)
{
    page_tiles_ = std::move(tiles);
}
auto ExportModel::page_tiles() const -> const std::vector<PageTile>&
{
    return page_tiles_;
}
auto ExportModel::page_count() const -> int
{
    return static_cast<int>(page_tiles_.size());
}

void ExportModel::trigger_snapshot()
{
    ++snapshot_count_;
}
auto ExportModel::snapshot_count() const -> int
{
    return snapshot_count_;
}

} // namespace markamp::canvas
