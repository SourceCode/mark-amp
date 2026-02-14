#include "ExtensionRecommendations.h"

#include <algorithm>
#include <fstream>

namespace markamp::core
{

auto ExtensionRecommendations::load_from_file(const std::string& file_path) -> bool
{
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        return false;
    }

    try
    {
        const nlohmann::json json_obj = nlohmann::json::parse(file);
        load_from_json(json_obj);
        file_path_ = file_path;
        return true;
    }
    catch (const nlohmann::json::parse_error&)
    {
        return false;
    }
}

void ExtensionRecommendations::load_from_json(const nlohmann::json& json_obj)
{
    recommended_.clear();
    unwanted_.clear();

    if (json_obj.contains("recommendations") && json_obj["recommendations"].is_array())
    {
        for (const auto& item : json_obj["recommendations"])
        {
            if (item.is_string())
            {
                recommended_.push_back(item.get<std::string>());
            }
        }
    }

    if (json_obj.contains("unwantedRecommendations") &&
        json_obj["unwantedRecommendations"].is_array())
    {
        for (const auto& item : json_obj["unwantedRecommendations"])
        {
            if (item.is_string())
            {
                unwanted_.push_back(item.get<std::string>());
            }
        }
    }
}

auto ExtensionRecommendations::recommended() const -> const std::vector<std::string>&
{
    return recommended_;
}

auto ExtensionRecommendations::unwanted() const -> const std::vector<std::string>&
{
    return unwanted_;
}

auto ExtensionRecommendations::is_recommended(const std::string& extension_id) const -> bool
{
    return std::find(recommended_.begin(), recommended_.end(), extension_id) != recommended_.end();
}

auto ExtensionRecommendations::is_unwanted(const std::string& extension_id) const -> bool
{
    return std::find(unwanted_.begin(), unwanted_.end(), extension_id) != unwanted_.end();
}

auto ExtensionRecommendations::file_path() const -> const std::string&
{
    return file_path_;
}

} // namespace markamp::core
