#include "NodePackRegistry.h"

#include <algorithm>
#include <set>

namespace markamp::node_editor
{

auto NodePackRegistry::register_pack(ExtensionNodePack pack) -> PackId
{
    PackId pid{next_id_++};
    pack.pack_id = pid;
    packs_.emplace(pid.value, std::move(pack));
    return pid;
}

auto NodePackRegistry::unregister_pack(PackId pack_id) -> bool
{
    return packs_.erase(pack_id.value) > 0;
}

auto NodePackRegistry::find_pack(PackId pack_id) const -> const ExtensionNodePack*
{
    auto iter = packs_.find(pack_id.value);
    if (iter == packs_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

auto NodePackRegistry::all_packs() const -> std::vector<PackId>
{
    std::vector<PackId> result;
    result.reserve(packs_.size());
    for (const auto& [key, _] : packs_)
    {
        result.emplace_back(PackId{key});
    }
    return result;
}

auto NodePackRegistry::pack_count() const -> std::size_t
{
    return packs_.size();
}

auto NodePackRegistry::nodes_from_pack(PackId pack_id) const -> std::vector<PackNodeEntry>
{
    const auto* pack = find_pack(pack_id);
    if (pack == nullptr)
    {
        return {};
    }
    return pack->node_entries;
}

auto NodePackRegistry::is_trusted(PackId pack_id) const -> bool
{
    const auto* pack = find_pack(pack_id);
    if (pack == nullptr)
    {
        return false;
    }
    return pack->trust == TrustLevel::kBuiltIn || pack->trust == TrustLevel::kTrusted;
}

auto NodePackRegistry::validate_pack(const ExtensionNodePack& pack) -> PackValidationResult
{
    PackValidationResult result;

    if (pack.display_name.empty())
    {
        result.ok = false;
        result.errors.emplace_back("Pack display_name is empty");
    }
    if (pack.version.empty())
    {
        result.ok = false;
        result.errors.emplace_back("Pack version is empty");
    }
    if (pack.node_entries.empty())
    {
        result.warnings.emplace_back("Pack contains no node entries");
    }
    if (pack.vendor.empty())
    {
        result.warnings.emplace_back("Pack vendor is not specified");
    }

    return result;
}

auto NodePackRegistry::contribution_point_count() const -> std::size_t
{
    std::set<std::string> categories;
    for (const auto& [_, pack] : packs_)
    {
        if (!pack.contribution_category.empty())
        {
            categories.insert(pack.contribution_category);
        }
    }
    return categories.size();
}

auto NodePackRegistry::has_contribution(const std::string& category) const -> bool
{
    for (const auto& [_, pack] : packs_)
    {
        if (pack.contribution_category == category)
        {
            return true;
        }
    }
    return false;
}

void NodePackRegistry::clear()
{
    packs_.clear();
}

} // namespace markamp::node_editor
