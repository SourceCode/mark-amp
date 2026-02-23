#include "ui/IconRegistry.h"

#include <fstream>
#include <sstream>

namespace markamp::ui
{

auto IconRegistry::register_icon(const std::string& name, const std::string& svg_data) -> bool
{
    SvgDocument doc;
    if (doc.parse(svg_data))
    {
        icons_[name] = std::move(doc);
        return true;
    }
    return false;
}

auto IconRegistry::load_icon(const std::string& name, const std::filesystem::path& path) -> bool
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return register_icon(name, buffer.str());
}

auto IconRegistry::get_icon(const std::string& name) const -> std::optional<SvgDocument>
{
    auto it = icons_.find(name);
    if (it != icons_.end())
    {
        return it->second;
    }
    return std::nullopt;
}

auto IconRegistry::has_icon(const std::string& name) const -> bool
{
    return icons_.find(name) != icons_.end();
}

auto IconRegistry::clear() -> void
{
    icons_.clear();
}

auto IconRegistry::size() const -> size_t
{
    return icons_.size();
}

} // namespace markamp::ui
