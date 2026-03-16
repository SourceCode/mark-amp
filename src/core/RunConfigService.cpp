#include "RunConfigService.h"

#include <algorithm>
#include <fstream>

namespace markamp::core
{

RunConfigService::RunConfigService(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

auto RunConfigService::load_from_file(const std::string& path) -> bool
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return false;
    }

    // Read entire file content.
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    if (content.empty())
    {
        return false;
    }

    // Simple JSON field extraction (production would use nlohmann/json).
    auto extract_string = [](const std::string& json, const std::string& key,
                             std::size_t start_pos = 0) -> std::pair<std::string, std::size_t>
    {
        const std::string search = "\"" + key + "\"";
        auto key_pos = json.find(search, start_pos);
        if (key_pos == std::string::npos)
        {
            return {"", std::string::npos};
        }
        auto colon = json.find(':', key_pos + search.size());
        if (colon == std::string::npos)
        {
            return {"", std::string::npos};
        }
        auto quote_start = json.find('"', colon + 1);
        if (quote_start == std::string::npos)
        {
            return {"", std::string::npos};
        }
        auto quote_end = json.find('"', quote_start + 1);
        if (quote_end == std::string::npos)
        {
            return {"", std::string::npos};
        }
        return {json.substr(quote_start + 1, quote_end - quote_start - 1), quote_end};
    };

    auto extract_bool = [](const std::string& json, const std::string& key,
                           std::size_t start_pos = 0) -> bool
    {
        const std::string search = "\"" + key + "\"";
        auto key_pos = json.find(search, start_pos);
        if (key_pos == std::string::npos)
        {
            return false;
        }
        auto colon = json.find(':', key_pos + search.size());
        if (colon == std::string::npos)
        {
            return false;
        }
        auto val_start = json.find_first_not_of(" \t\n\r", colon + 1);
        if (val_start == std::string::npos)
        {
            return false;
        }
        return json.substr(val_start, 4) == "true";
    };

    // Parse configuration objects: find each config block between { and }.
    configurations_.clear();
    std::size_t pos = 0;
    while (pos < content.size())
    {
        auto obj_start = content.find('{', pos);
        if (obj_start == std::string::npos)
        {
            break;
        }
        auto obj_end = content.find('}', obj_start);
        if (obj_end == std::string::npos)
        {
            break;
        }

        auto obj_text = content.substr(obj_start, obj_end - obj_start + 1);
        auto [name_val, name_end] = extract_string(obj_text, "name");
        auto [cmd_val, cmd_end] = extract_string(obj_text, "command");

        if (!name_val.empty() && !cmd_val.empty())
        {
            RunConfiguration config;
            config.name = name_val;
            config.command = cmd_val;
            config.type = extract_string(obj_text, "type").first;
            config.build_before_run = extract_bool(obj_text, "build_before_run");
            configurations_.push_back(std::move(config));
        }

        pos = obj_end + 1;
    }

    // Parse active configuration name.
    auto [active_val, active_end] = extract_string(content, "active");
    if (!active_val.empty())
    {
        active_name_ = active_val;
    }

    return !configurations_.empty();
}

auto RunConfigService::save_to_file(const std::string& path) const -> bool
{
    std::ofstream file(path);
    if (!file.is_open())
    {
        return false;
    }

    // Serialize configurations to JSON (simplified)
    file << "{\n  \"configurations\": [\n";
    for (size_t i = 0; i < configurations_.size(); ++i)
    {
        const auto& cfg = configurations_[i];
        file << R"({"name": ")" << cfg.name << R"(", )"
             << "\"command\": \"" << cfg.command << "\", "
             << "\"type\": \"" << cfg.type << "\", "
             << "\"build_before_run\": " << (cfg.build_before_run ? "true" : "false") << "}";
        if (i + 1 < configurations_.size())
        {
            file << ",";
        }
        file << "\n";
    }
    file << "  ],\n  \"active\": \"" << active_name_ << "\"\n}\n";
    return true;
}

void RunConfigService::add_configuration(RunConfiguration config)
{
    if (config.name.empty() || config.command.empty())
    {
        return; // Reject invalid configs
    }

    // Remove existing config with same name
    remove_configuration(config.name);
    configurations_.push_back(std::move(config));
}

void RunConfigService::remove_configuration(const std::string& name)
{
    configurations_.erase(std::remove_if(configurations_.begin(),
                                         configurations_.end(),
                                         [&name](const RunConfiguration& cfg)
                                         { return cfg.name == name; }),
                          configurations_.end());

    if (active_name_ == name)
    {
        active_name_.clear();
    }
}

void RunConfigService::update_configuration(const RunConfiguration& config)
{
    for (auto& existing : configurations_)
    {
        if (existing.name == config.name)
        {
            existing = config;
            return;
        }
    }
}

auto RunConfigService::configurations() const -> const std::vector<RunConfiguration>&
{
    return configurations_;
}

auto RunConfigService::find(const std::string& name) const -> const RunConfiguration*
{
    for (const auto& cfg : configurations_)
    {
        if (cfg.name == name)
        {
            return &cfg;
        }
    }
    return nullptr;
}

auto RunConfigService::active_configuration() const -> const RunConfiguration*
{
    return find(active_name_);
}

void RunConfigService::set_active(const std::string& name)
{
    if (find(name) == nullptr)
    {
        return;
    }

    active_name_ = name;

    // Add to recent list
    auto iter = std::find(recent_.begin(), recent_.end(), name);
    if (iter != recent_.end())
    {
        recent_.erase(iter);
    }
    recent_.insert(recent_.begin(), name);

    // Keep at most 5 recent entries
    constexpr int kMaxRecent = 5;
    if (static_cast<int>(recent_.size()) > kMaxRecent)
    {
        recent_.resize(static_cast<size_t>(kMaxRecent));
    }
}

auto RunConfigService::recent_configurations(int max_count) const -> std::vector<std::string>
{
    auto count = std::min(static_cast<int>(recent_.size()), max_count);
    return {recent_.begin(), recent_.begin() + count};
}

} // namespace markamp::core
