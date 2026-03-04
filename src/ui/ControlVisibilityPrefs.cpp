#include "ControlVisibilityPrefs.h"

#include <algorithm>

namespace markamp::ui
{

void ControlVisibilityPrefs::register_cluster(const std::string& cluster_id,
                                              const std::string& display_name,
                                              bool default_visible)
{
    if (find_cluster(cluster_id) == nullptr)
    {
        ClusterVisibility vis;
        vis.cluster_id = cluster_id;
        vis.display_name = display_name;
        vis.visible = default_visible;
        vis.user_modified = false;
        clusters_.push_back(vis);

        ClusterDefault def;
        def.cluster_id = cluster_id;
        def.default_visible = default_visible;
        defaults_.push_back(def);
    }
}

auto ControlVisibilityPrefs::cluster_count() const -> int
{
    return static_cast<int>(clusters_.size());
}

void ControlVisibilityPrefs::set_visible(const std::string& cluster_id, bool visible)
{
    auto* cluster = find_cluster(cluster_id);
    if (cluster != nullptr)
    {
        cluster->visible = visible;
        cluster->user_modified = true;
    }
}

void ControlVisibilityPrefs::toggle(const std::string& cluster_id)
{
    auto* cluster = find_cluster(cluster_id);
    if (cluster != nullptr)
    {
        cluster->visible = !cluster->visible;
        cluster->user_modified = true;
    }
}

auto ControlVisibilityPrefs::is_visible(const std::string& cluster_id) const -> bool
{
    const auto* cluster = find_cluster(cluster_id);
    if (cluster != nullptr)
    {
        return cluster->visible;
    }
    return true; // Default visible if unknown
}

auto ControlVisibilityPrefs::visible_clusters() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& cluster : clusters_)
    {
        if (cluster.visible)
        {
            result.push_back(cluster.cluster_id);
        }
    }
    return result;
}

auto ControlVisibilityPrefs::hidden_clusters() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& cluster : clusters_)
    {
        if (!cluster.visible)
        {
            result.push_back(cluster.cluster_id);
        }
    }
    return result;
}

auto ControlVisibilityPrefs::all_clusters() const -> const std::vector<ClusterVisibility>&
{
    return clusters_;
}

void ControlVisibilityPrefs::reset_to_defaults()
{
    for (auto& cluster : clusters_)
    {
        cluster.visible = find_default(cluster.cluster_id);
        cluster.user_modified = false;
    }
}

void ControlVisibilityPrefs::reset_cluster(const std::string& cluster_id)
{
    auto* cluster = find_cluster(cluster_id);
    if (cluster != nullptr)
    {
        cluster->visible = find_default(cluster_id);
        cluster->user_modified = false;
    }
}

auto ControlVisibilityPrefs::has_user_modifications() const -> bool
{
    for (const auto& cluster : clusters_)
    {
        if (cluster.user_modified)
        {
            return true;
        }
    }
    return false;
}

auto ControlVisibilityPrefs::user_modified_count() const -> int
{
    int count = 0;
    for (const auto& cluster : clusters_)
    {
        if (cluster.user_modified)
        {
            ++count;
        }
    }
    return count;
}

auto ControlVisibilityPrefs::find_cluster(const std::string& cluster_id) -> ClusterVisibility*
{
    for (auto& cluster : clusters_)
    {
        if (cluster.cluster_id == cluster_id)
        {
            return &cluster;
        }
    }
    return nullptr;
}

auto ControlVisibilityPrefs::find_cluster(const std::string& cluster_id) const
    -> const ClusterVisibility*
{
    for (const auto& cluster : clusters_)
    {
        if (cluster.cluster_id == cluster_id)
        {
            return &cluster;
        }
    }
    return nullptr;
}

auto ControlVisibilityPrefs::find_default(const std::string& cluster_id) const -> bool
{
    for (const auto& def : defaults_)
    {
        if (def.cluster_id == cluster_id)
        {
            return def.default_visible;
        }
    }
    return true;
}

} // namespace markamp::ui
