#include "ScopedConfig.h"

#include "Logger.h"

namespace markamp::core
{

ScopedConfig::ScopedConfig(Config& app_config, SettingsCatalog* catalog)
    : app_config_(app_config)
    , catalog_(catalog)
{
}

// ── Scoped getters ──

auto ScopedConfig::get_string(std::string_view key, std::string_view default_val) const
    -> std::string
{
    const Config* source = resolve_config(key);
    if (source != nullptr)
    {
        return source->get_string(key, default_val);
    }
    // Fall back to catalog default
    auto cat_default = catalog_default(key);
    if (!cat_default.empty())
    {
        return cat_default;
    }
    return std::string(default_val);
}

auto ScopedConfig::get_int(std::string_view key, int default_val) const -> int
{
    const Config* source = resolve_config(key);
    if (source != nullptr)
    {
        return source->get_int(key, default_val);
    }
    return default_val;
}

auto ScopedConfig::get_bool(std::string_view key, bool default_val) const -> bool
{
    const Config* source = resolve_config(key);
    if (source != nullptr)
    {
        return source->get_bool(key, default_val);
    }
    return default_val;
}

auto ScopedConfig::get_double(std::string_view key, double default_val) const -> double
{
    const Config* source = resolve_config(key);
    if (source != nullptr)
    {
        return source->get_double(key, default_val);
    }
    return default_val;
}

// ── Scoped setters ──

void ScopedConfig::set(std::string_view key, std::string_view value, ConfigScope scope)
{
    switch (scope)
    {
        case ConfigScope::kProject:
            if (project_config_ != nullptr)
            {
                project_config_->set(key, value);
                return;
            }
            MARKAMP_LOG_WARN("ScopedConfig: no project config attached, falling back to app scope");
            break;
        case ConfigScope::kWorkspace:
            if (workspace_config_ != nullptr)
            {
                workspace_config_->set(key, value);
                return;
            }
            MARKAMP_LOG_WARN(
                "ScopedConfig: no workspace config attached, falling back to app scope");
            break;
        case ConfigScope::kApplication:
            break;
    }
    app_config_.set(key, value);
}

void ScopedConfig::set(std::string_view key, int value, ConfigScope scope)
{
    switch (scope)
    {
        case ConfigScope::kProject:
            if (project_config_ != nullptr)
            {
                project_config_->set(key, value);
                return;
            }
            break;
        case ConfigScope::kWorkspace:
            if (workspace_config_ != nullptr)
            {
                workspace_config_->set(key, value);
                return;
            }
            break;
        case ConfigScope::kApplication:
            break;
    }
    app_config_.set(key, value);
}

void ScopedConfig::set(std::string_view key, bool value, ConfigScope scope)
{
    switch (scope)
    {
        case ConfigScope::kProject:
            if (project_config_ != nullptr)
            {
                project_config_->set(key, value);
                return;
            }
            break;
        case ConfigScope::kWorkspace:
            if (workspace_config_ != nullptr)
            {
                workspace_config_->set(key, value);
                return;
            }
            break;
        case ConfigScope::kApplication:
            break;
    }
    app_config_.set(key, value);
}

void ScopedConfig::set(std::string_view key, double value, ConfigScope scope)
{
    switch (scope)
    {
        case ConfigScope::kProject:
            if (project_config_ != nullptr)
            {
                project_config_->set(key, value);
                return;
            }
            break;
        case ConfigScope::kWorkspace:
            if (workspace_config_ != nullptr)
            {
                workspace_config_->set(key, value);
                return;
            }
            break;
        case ConfigScope::kApplication:
            break;
    }
    app_config_.set(key, value);
}

// ── Scope queries ──

auto ScopedConfig::effective_scope(std::string_view key) const -> ConfigScope
{
    if (project_config_ != nullptr && project_config_->has_key(key))
    {
        return ConfigScope::kProject;
    }
    if (workspace_config_ != nullptr && workspace_config_->has_key(key))
    {
        return ConfigScope::kWorkspace;
    }
    return ConfigScope::kApplication;
}

auto ScopedConfig::has_override(std::string_view key, ConfigScope scope) const -> bool
{
    switch (scope)
    {
        case ConfigScope::kProject:
            return project_config_ != nullptr && project_config_->has_key(key);
        case ConfigScope::kWorkspace:
            return workspace_config_ != nullptr && workspace_config_->has_key(key);
        case ConfigScope::kApplication:
            return app_config_.has_key(key);
    }
    return false;
}

void ScopedConfig::reset_in_scope(std::string_view key, ConfigScope scope)
{
    switch (scope)
    {
        case ConfigScope::kProject:
            if (project_config_ != nullptr)
            {
                project_config_->remove(key);
            }
            break;
        case ConfigScope::kWorkspace:
            if (workspace_config_ != nullptr)
            {
                workspace_config_->remove(key);
            }
            break;
        case ConfigScope::kApplication:
            app_config_.remove(key);
            break;
    }
}

void ScopedConfig::reset_to_default(std::string_view key)
{
    reset_in_scope(key, ConfigScope::kProject);
    reset_in_scope(key, ConfigScope::kWorkspace);
    app_config_.remove(key);
}

// ── Scope lifecycle ──

void ScopedConfig::set_workspace_config(Config* workspace_config)
{
    workspace_config_ = workspace_config;
}

void ScopedConfig::set_project_config(Config* project_config)
{
    project_config_ = project_config;
}

// ── Internal ──

auto ScopedConfig::resolve_config(std::string_view key) const -> const Config*
{
    // Most specific first: Project → Workspace → App
    if (project_config_ != nullptr && project_config_->has_key(key))
    {
        return project_config_;
    }
    if (workspace_config_ != nullptr && workspace_config_->has_key(key))
    {
        return workspace_config_;
    }
    if (app_config_.has_key(key))
    {
        return &app_config_;
    }
    return nullptr;
}

auto ScopedConfig::catalog_default(std::string_view key) const -> std::string
{
    if (catalog_ == nullptr)
    {
        return {};
    }
    const auto* entry = catalog_->find_setting(key);
    if (entry != nullptr)
    {
        return entry->default_value;
    }
    return {};
}

// ── Batch 19-22 improvements (#119-121) ──

auto ScopedConfig::scope_count() const -> int
{
    int count = 1; // app_config_ always present
    if (workspace_config_ != nullptr)
    {
        ++count;
    }
    if (project_config_ != nullptr)
    {
        ++count;
    }
    return count;
}

auto ScopedConfig::active_scope_count() const -> int
{
    int count = 0;
    if (app_config_.key_count() > 0)
    {
        ++count;
    }
    if (workspace_config_ != nullptr && workspace_config_->key_count() > 0)
    {
        ++count;
    }
    if (project_config_ != nullptr && project_config_->key_count() > 0)
    {
        ++count;
    }
    return count;
}

auto ScopedConfig::all_overridden_keys() const -> std::vector<std::string>
{
    std::vector<std::string> keys;
    if (workspace_config_ != nullptr)
    {
        for (const auto& key : workspace_config_->all_keys())
        {
            keys.push_back(key);
        }
    }
    if (project_config_ != nullptr)
    {
        for (const auto& key : project_config_->all_keys())
        {
            if (std::find(keys.begin(), keys.end(), key) == keys.end())
            {
                keys.push_back(key);
            }
        }
    }
    return keys;
}

} // namespace markamp::core
