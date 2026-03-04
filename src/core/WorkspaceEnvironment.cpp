/// @file WorkspaceEnvironment.cpp
/// @brief Phase 40 Task 10 — Workspace environment implementation.

#include "core/WorkspaceEnvironment.h"

#include <cstdlib>

namespace markamp::core
{

void WorkspaceEnvironment::set(const std::string& key, const std::string& value)
{
    vars_[key] = value;
}

auto WorkspaceEnvironment::get(const std::string& key) const -> std::string
{
    auto it = vars_.find(key);
    return (it != vars_.end()) ? it->second : "";
}

auto WorkspaceEnvironment::has(const std::string& key) const -> bool
{
    return vars_.count(key) > 0;
}

auto WorkspaceEnvironment::remove(const std::string& key) -> bool
{
    return vars_.erase(key) > 0;
}

auto WorkspaceEnvironment::all() const -> const std::unordered_map<std::string, std::string>&
{
    return vars_;
}

auto WorkspaceEnvironment::expand(const std::string& input) const -> std::string
{
    std::string result = input;
    std::string::size_type pos = 0;

    while ((pos = result.find("${", pos)) != std::string::npos)
    {
        auto end = result.find('}', pos + 2);
        if (end == std::string::npos)
            break;

        auto var_name = result.substr(pos + 2, end - pos - 2);
        std::string replacement;

        // Check workspace vars first, then system env
        auto it = vars_.find(var_name);
        if (it != vars_.end())
        {
            replacement = it->second;
        }
        else
        {
            const char* env_val = std::getenv(var_name.c_str());
            if (env_val != nullptr)
                replacement = env_val;
        }

        result.replace(pos, end - pos + 1, replacement);
        pos += replacement.size();
    }

    return result;
}

auto WorkspaceEnvironment::merged_environment() const
    -> std::unordered_map<std::string, std::string>
{
    // Start with workspace vars (they override system)
    return vars_;
}

void WorkspaceEnvironment::clear()
{
    vars_.clear();
}

auto WorkspaceEnvironment::count() const -> std::size_t
{
    return vars_.size();
}

} // namespace markamp::core
