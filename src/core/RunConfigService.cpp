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
    const std::ifstream file(path);
    if (!file.is_open())
    {
        return false;
    }

    // Read JSON content (simplified: one config per object in an array)
    // In production this would use a JSON library; for now, parse via EventBus signal
    // Placeholder: mark as loaded
    return true;
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
