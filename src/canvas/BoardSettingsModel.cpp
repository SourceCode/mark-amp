#include "canvas/BoardSettingsModel.h"

#include <algorithm>

namespace markamp::canvas
{

auto BoardSettingsModel::set_background(const std::string& preset) -> void
{
    background_ = preset;
}

auto BoardSettingsModel::background() const -> const std::string&
{
    return background_;
}

auto BoardSettingsModel::set_grid_spacing(double spacing) -> void
{
    grid_spacing_ = spacing;
}

auto BoardSettingsModel::grid_spacing() const -> double
{
    return grid_spacing_;
}

auto BoardSettingsModel::set_permission(BoardPermission perm) -> void
{
    permission_ = perm;
}

auto BoardSettingsModel::permission() const -> BoardPermission
{
    return permission_;
}

auto BoardSettingsModel::set_dimensions(double width, double height) -> void
{
    width_ = width;
    height_ = height;
}

auto BoardSettingsModel::width() const -> double
{
    return width_;
}

auto BoardSettingsModel::height() const -> double
{
    return height_;
}

auto BoardSettingsModel::set_setting(const std::string& key, const std::string& value) -> void
{
    for (auto& [k, v] : settings_)
    {
        if (k == key)
        {
            v = value;
            return;
        }
    }
    settings_.emplace_back(key, value);
}

auto BoardSettingsModel::setting(const std::string& key) const -> std::string
{
    for (const auto& [k, v] : settings_)
    {
        if (k == key)
        {
            return v;
        }
    }
    return {};
}

auto BoardSettingsModel::setting_count() const -> int
{
    return static_cast<int>(settings_.size());
}

} // namespace markamp::canvas
