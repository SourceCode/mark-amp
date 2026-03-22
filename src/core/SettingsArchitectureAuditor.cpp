/// @file SettingsArchitectureAuditor.cpp
/// @brief V21 Phase 08 — SettingsArchitectureAuditor implementation.

#include "SettingsArchitectureAuditor.h"
#include <algorithm>

namespace markamp::core
{

void SettingsArchitectureAuditor::register_setting(CatalogSettingEntry entry)
{
    const auto id = entry.setting_id;
    if (!catalog_.contains(id)) catalog_order_.push_back(id);
    catalog_[id] = std::move(entry);
}

auto SettingsArchitectureAuditor::get_setting(const std::string& setting_id) const
    -> const CatalogSettingEntry* {
    auto it = catalog_.find(setting_id);
    return it != catalog_.end() ? &it->second : nullptr;
}

auto SettingsArchitectureAuditor::all_settings() const -> std::vector<const CatalogSettingEntry*> {
    std::vector<const CatalogSettingEntry*> result;
    for (const auto& id : catalog_order_) {
        auto it = catalog_.find(id);
        if (it != catalog_.end()) result.push_back(&it->second);
    }
    return result;
}

auto SettingsArchitectureAuditor::settings_for_category(const std::string& category) const
    -> std::vector<const CatalogSettingEntry*> {
    std::vector<const CatalogSettingEntry*> result;
    for (const auto& id : catalog_order_) {
        auto it = catalog_.find(id);
        if (it != catalog_.end() && it->second.category == category)
            result.push_back(&it->second);
    }
    return result;
}

auto SettingsArchitectureAuditor::setting_count() const -> std::size_t { return catalog_.size(); }

auto SettingsArchitectureAuditor::categories() const -> std::vector<std::string> {
    std::vector<std::string> cats;
    for (const auto& id : catalog_order_) {
        auto it = catalog_.find(id);
        if (it != catalog_.end()) {
            if (std::find(cats.begin(), cats.end(), it->second.category) == cats.end())
                cats.push_back(it->second.category);
        }
    }
    return cats;
}

void SettingsArchitectureAuditor::set_value(const std::string& setting_id,
                                             SettingScope scope, const std::string& value) {
    scoped_values_[setting_id][scope] = value;
}

auto SettingsArchitectureAuditor::effective_value(const std::string& setting_id) const -> std::string {
    auto it = scoped_values_.find(setting_id);
    if (it != scoped_values_.end()) {
        // Project > Workspace > User > Default
        for (auto scope : {SettingScope::kProject, SettingScope::kWorkspace,
                           SettingScope::kUser, SettingScope::kDefault}) {
            auto sit = it->second.find(scope);
            if (sit != it->second.end()) return sit->second;
        }
    }
    auto cat_it = catalog_.find(setting_id);
    return cat_it != catalog_.end() ? cat_it->second.default_value : "";
}

auto SettingsArchitectureAuditor::effective_scope(const std::string& setting_id) const -> SettingScope {
    auto it = scoped_values_.find(setting_id);
    if (it != scoped_values_.end()) {
        for (auto scope : {SettingScope::kProject, SettingScope::kWorkspace,
                           SettingScope::kUser, SettingScope::kDefault}) {
            if (it->second.contains(scope)) return scope;
        }
    }
    return SettingScope::kDefault;
}

void SettingsArchitectureAuditor::record_ownership(const std::string& setting_id,
                                                    const std::string& owner, bool is_direct) {
    ownership_log_.push_back({.setting_id = setting_id, .owner = owner, .is_direct_write = is_direct});
}

auto SettingsArchitectureAuditor::direct_writers() const -> std::vector<SettingsOwnershipEntry> {
    std::vector<SettingsOwnershipEntry> result;
    for (const auto& e : ownership_log_)
        if (e.is_direct_write) result.push_back(e);
    return result;
}

auto SettingsArchitectureAuditor::resolve_deep_link(const std::string& setting_id) const -> std::string {
    auto it = catalog_.find(setting_id);
    return it != catalog_.end() ? it->second.deep_link : "";
}

auto SettingsArchitectureAuditor::settings_missing_deep_link() const -> std::vector<std::string> {
    std::vector<std::string> result;
    for (const auto& id : catalog_order_) {
        auto it = catalog_.find(id);
        if (it != catalog_.end() && it->second.deep_link.empty())
            result.push_back(id);
    }
    return result;
}

auto SettingsArchitectureAuditor::live_settings() const -> std::vector<const CatalogSettingEntry*> {
    std::vector<const CatalogSettingEntry*> result;
    for (const auto& id : catalog_order_) {
        auto it = catalog_.find(id);
        if (it != catalog_.end() && it->second.application_mode == SettingApplicationMode::kLive)
            result.push_back(&it->second);
    }
    return result;
}

auto SettingsArchitectureAuditor::restart_required_settings() const -> std::vector<const CatalogSettingEntry*> {
    std::vector<const CatalogSettingEntry*> result;
    for (const auto& id : catalog_order_) {
        auto it = catalog_.find(id);
        if (it != catalog_.end() && it->second.application_mode == SettingApplicationMode::kOnRestart)
            result.push_back(&it->second);
    }
    return result;
}

auto SettingsArchitectureAuditor::diagnose() const -> std::vector<SettingsArchitectureDiagnostic> {
    std::vector<SettingsArchitectureDiagnostic> diags;
    for (const auto& id : catalog_order_) {
        auto it = catalog_.find(id);
        if (it == catalog_.end()) continue;
        const auto& setting = it->second;
        if (setting.is_deprecated)
            diags.push_back({.setting_id = id, .issue = "Setting is deprecated", .is_deprecated = true});
        if (setting.deep_link.empty())
            diags.push_back({.setting_id = id, .issue = "Setting has no deep-link", .is_missing_deep_link = true});
    }
    for (const auto& entry : ownership_log_) {
        if (entry.is_direct_write)
            diags.push_back({.setting_id = entry.setting_id,
                .issue = "Direct write by " + entry.owner + " bypasses canonical owner",
                .is_direct_write = true});
    }
    return diags;
}

auto SettingsArchitectureAuditor::catalog_count() const -> std::size_t { return catalog_.size(); }

auto SettingsArchitectureAuditor::deprecated_count() const -> std::size_t {
    std::size_t c = 0;
    for (const auto& [id, s] : catalog_) if (s.is_deprecated) ++c;
    return c;
}

} // namespace markamp::core
