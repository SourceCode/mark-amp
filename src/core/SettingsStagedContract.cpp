/// @file SettingsStagedContract.cpp
/// @brief V21 Phase 09 — SettingsStagedContract implementation.

#include "SettingsStagedContract.h"
#include <algorithm>

namespace markamp::core
{

void SettingsStagedContract::stage_change(const std::string& setting_id,
                                           const std::string& original,
                                           const std::string& staged) {
    if (!staged_.contains(setting_id)) staged_order_.push_back(setting_id);
    staged_[setting_id] = {.setting_id = setting_id, .original_value = original, .staged_value = staged};
}

void SettingsStagedContract::unstage(const std::string& setting_id) {
    staged_.erase(setting_id);
    std::erase(staged_order_, setting_id);
}

auto SettingsStagedContract::get_staged(const std::string& setting_id) const -> const StagedSetting* {
    auto it = staged_.find(setting_id);
    return it != staged_.end() ? &it->second : nullptr;
}

auto SettingsStagedContract::all_staged() const -> std::vector<const StagedSetting*> {
    std::vector<const StagedSetting*> result;
    for (const auto& id : staged_order_) {
        auto it = staged_.find(id);
        if (it != staged_.end()) result.push_back(&it->second);
    }
    return result;
}

auto SettingsStagedContract::dirty_count() const -> std::size_t {
    std::size_t c = 0;
    for (const auto& [id, s] : staged_) if (s.is_dirty()) ++c;
    return c;
}

auto SettingsStagedContract::has_dirty() const -> bool { return dirty_count() > 0; }

auto SettingsStagedContract::apply_all() -> std::vector<std::string> {
    std::vector<std::string> applied;
    for (const auto& id : staged_order_) {
        auto it = staged_.find(id);
        if (it != staged_.end() && it->second.is_dirty()) {
            if (apply_callback_) apply_callback_(id, it->second.staged_value);
            it->second.original_value = it->second.staged_value;
            applied.push_back(id);
        }
    }
    return applied;
}

void SettingsStagedContract::cancel_all() {
    for (auto& [id, s] : staged_) s.staged_value = s.original_value;
}

void SettingsStagedContract::revert(const std::string& setting_id) {
    auto it = staged_.find(setting_id);
    if (it != staged_.end()) it->second.staged_value = it->second.original_value;
}

void SettingsStagedContract::set_apply_callback(
    std::function<void(const std::string&, const std::string&)> cb) {
    apply_callback_ = std::move(cb);
}

auto SettingsStagedContract::validate_json(const std::string& json) -> JsonValidationResult {
    JsonValidationResult result;
    if (json.empty()) { result.is_valid = false; result.error_message = "Empty JSON"; return result; }

    int braces = 0; int brackets = 0;
    for (std::size_t i = 0; i < json.size(); ++i) {
        if (json[i] == '{') ++braces;
        else if (json[i] == '}') --braces;
        else if (json[i] == '[') ++brackets;
        else if (json[i] == ']') --brackets;

        if (braces < 0 || brackets < 0) {
            result.is_valid = false;
            result.error_line = static_cast<int>(i);
            result.error_message = "Unmatched bracket/brace";
            return result;
        }
    }

    if (braces != 0 || brackets != 0) {
        result.is_valid = false;
        result.error_message = "Unbalanced brackets/braces";
    }
    return result;
}

auto SettingsStagedContract::export_staged() const -> std::vector<std::pair<std::string, std::string>> {
    std::vector<std::pair<std::string, std::string>> result;
    for (const auto& id : staged_order_) {
        auto it = staged_.find(id);
        if (it != staged_.end()) result.emplace_back(id, it->second.staged_value);
    }
    return result;
}

void SettingsStagedContract::import_values(
    const std::vector<std::pair<std::string, std::string>>& values) {
    for (const auto& [id, val] : values) {
        auto it = staged_.find(id);
        if (it != staged_.end()) { it->second.staged_value = val; }
        else { stage_change(id, val, val); }
    }
}

void SettingsStagedContract::register_searchable(const std::string& setting_id,
                                                   const std::string& label,
                                                   const std::string& category) {
    search_entries_.push_back({.setting_id = setting_id, .label = label, .category = category});
}

auto SettingsStagedContract::search(const std::string& query) const -> std::vector<SettingsSearchResult> {
    std::vector<SettingsSearchResult> results;
    for (const auto& entry : search_entries_) {
        if (entry.setting_id.find(query) != std::string::npos ||
            entry.label.find(query) != std::string::npos ||
            entry.category.find(query) != std::string::npos) {
            results.push_back({.setting_id = entry.setting_id, .label = entry.label,
                .category = entry.category, .match_context = entry.label});
        }
    }
    return results;
}

auto SettingsStagedContract::total_tracked() const -> std::size_t { return staged_.size(); }

} // namespace markamp::core
