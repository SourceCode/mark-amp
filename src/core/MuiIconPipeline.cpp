/// @file MuiIconPipeline.cpp
/// @brief P10-T02: Unified MUI-aware icon rendering pipeline.

#include "MuiIconPipeline.h"

#include "Logger.h"

namespace markamp::core
{

MuiIconPipeline::MuiIconPipeline()
{
    // Built-in legacy aliases
    add_alias("lucide_folder", "mui_folder");
    add_alias("lucide_file", "mui_description");
    add_alias("lucide_search", "mui_search");
    add_alias("lucide_settings", "mui_settings");
    add_alias("lucide_x", "mui_close");
    add_alias("emoji_canvas", "mui_dashboard");
}

auto MuiIconPipeline::resolve(const std::string& icon_id) const -> std::string
{
    auto iter = aliases_.find(icon_id);
    if (iter != aliases_.end())
    {
        return iter->second;
    }
    return icon_id;
}

void MuiIconPipeline::add_alias(const std::string& legacy_id, const std::string& mui_id)
{
    aliases_[legacy_id] = mui_id;
}

auto MuiIconPipeline::is_alias(const std::string& icon_id) const -> bool
{
    return aliases_.contains(icon_id);
}

auto MuiIconPipeline::render(const IconRenderRequest& request) const -> std::string
{
    const auto resolved = resolve(request.icon_id);
    MARKAMP_LOG_DEBUG("Icon render: {} -> {} ({}px, state={}, scale={})",
                      request.icon_id, resolved, request.size_px,
                      static_cast<int>(request.state), request.scale_factor);
    return resolved;
}

} // namespace markamp::core
