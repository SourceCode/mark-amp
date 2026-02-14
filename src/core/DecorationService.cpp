#include "DecorationService.h"

namespace markamp::core
{

const std::vector<DecorationRange> DecorationService::kEmptyRanges;

auto DecorationService::create_decoration_type(DecorationOptions options) -> DecorationTypeHandle
{
    auto handle = next_handle_++;
    types_[handle] = std::move(options);
    return handle;
}

void DecorationService::set_decorations(const std::string& file_uri,
                                        DecorationTypeHandle type_handle,
                                        std::vector<DecorationRange> ranges)
{
    decorations_[file_uri][type_handle] = std::move(ranges);
    fire_change(file_uri);
}

auto DecorationService::get_decorations(const std::string& file_uri,
                                        DecorationTypeHandle type_handle) const
    -> const std::vector<DecorationRange>&
{
    auto file_it = decorations_.find(file_uri);
    if (file_it == decorations_.end())
    {
        return kEmptyRanges;
    }

    auto type_it = file_it->second.find(type_handle);
    return type_it != file_it->second.end() ? type_it->second : kEmptyRanges;
}

auto DecorationService::get_options(DecorationTypeHandle type_handle) const
    -> const DecorationOptions*
{
    auto found = types_.find(type_handle);
    return found != types_.end() ? &found->second : nullptr;
}

void DecorationService::dispose_decoration_type(DecorationTypeHandle type_handle)
{
    types_.erase(type_handle);
    // Remove all decorations for this type across all files
    for (auto& [file_uri, type_map] : decorations_)
    {
        type_map.erase(type_handle);
    }
}

void DecorationService::clear_file_decorations(const std::string& file_uri)
{
    decorations_.erase(file_uri);
    fire_change(file_uri);
}

auto DecorationService::on_change(ChangeListener listener) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    listeners_.emplace_back(listener_id, std::move(listener));
    return listener_id;
}

void DecorationService::remove_listener(std::size_t listener_id)
{
    listeners_.erase(std::remove_if(listeners_.begin(),
                                    listeners_.end(),
                                    [listener_id](const auto& pair)
                                    { return pair.first == listener_id; }),
                     listeners_.end());
}

void DecorationService::fire_change(const std::string& file_uri)
{
    for (const auto& [id, listener] : listeners_)
    {
        listener(file_uri);
    }
}

} // namespace markamp::core
