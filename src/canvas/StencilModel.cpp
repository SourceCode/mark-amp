#include "StencilModel.h"

#include <algorithm>

namespace markamp::canvas
{

void StencilModel::set_catalog(std::vector<StencilDef> stencils)
{
    catalog_ = std::move(stencils);
}
auto StencilModel::catalog() const -> const std::vector<StencilDef>&
{
    return catalog_;
}
auto StencilModel::catalog_size() const -> int
{
    return static_cast<int>(catalog_.size());
}

auto StencilModel::search(const std::string& query) const -> std::vector<StencilDef>
{
    std::vector<StencilDef> results;
    for (const auto& stencil : catalog_)
    {
        if (stencil.name.find(query) != std::string::npos)
        {
            results.push_back(stencil);
        }
    }
    return results;
}

auto StencilModel::by_category(StencilCategory category) const -> std::vector<StencilDef>
{
    std::vector<StencilDef> results;
    for (const auto& stencil : catalog_)
    {
        if (stencil.category == category)
        {
            results.push_back(stencil);
        }
    }
    return results;
}

void StencilModel::add_favorite(const std::string& stencil_id)
{
    if (!is_favorite(stencil_id))
    {
        favorites_.push_back(stencil_id);
    }
}

void StencilModel::remove_favorite(const std::string& stencil_id)
{
    favorites_.erase(std::remove(favorites_.begin(), favorites_.end(), stencil_id),
                     favorites_.end());
}

auto StencilModel::favorites() const -> const std::vector<std::string>&
{
    return favorites_;
}

auto StencilModel::is_favorite(const std::string& stencil_id) const -> bool
{
    return std::find(favorites_.begin(), favorites_.end(), stencil_id) != favorites_.end();
}

void StencilModel::push_recent(const std::string& stencil_id)
{
    recents_.erase(std::remove(recents_.begin(), recents_.end(), stencil_id), recents_.end());
    recents_.insert(recents_.begin(), stencil_id);
    if (static_cast<int>(recents_.size()) > kMaxRecent)
    {
        recents_.resize(static_cast<size_t>(kMaxRecent));
    }
}

auto StencilModel::recents() const -> const std::vector<std::string>&
{
    return recents_;
}

void StencilModel::set_override_fill(const std::string& color)
{
    override_fill_ = color;
}
void StencilModel::set_override_stroke(const std::string& color)
{
    override_stroke_ = color;
}
void StencilModel::clear_overrides()
{
    override_fill_.clear();
    override_stroke_.clear();
}
auto StencilModel::override_fill() const -> const std::string&
{
    return override_fill_;
}
auto StencilModel::override_stroke() const -> const std::string&
{
    return override_stroke_;
}
auto StencilModel::has_overrides() const -> bool
{
    return !override_fill_.empty() || !override_stroke_.empty();
}

} // namespace markamp::canvas
