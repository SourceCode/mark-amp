/// @file ThemeOverrideService.cpp
/// @brief V9 Phase 38 — ThemeOverrideService implementation.

#include "ThemeOverrideService.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

// ── CRUD ──────────────────────────────────────────────────────────────────────

void ThemeOverrideService::set_override(const std::string& token_name,
                                        const std::string& original_value,
                                        const std::string& override_value,
                                        OverrideScope scope)
{
    // Replace if token+scope already exists
    for (auto& existing : overrides_)
    {
        if (existing.token_name == token_name && existing.scope == scope)
        {
            existing.original_value = original_value;
            existing.override_value = override_value;
            return;
        }
    }
    ThemeOverride override_entry;
    override_entry.token_name = token_name;
    override_entry.original_value = original_value;
    override_entry.override_value = override_value;
    override_entry.scope = scope;
    overrides_.push_back(std::move(override_entry));
}

auto ThemeOverrideService::remove_override(const std::string& token_name, OverrideScope scope)
    -> bool
{
    auto iter = std::find_if(overrides_.begin(),
                             overrides_.end(),
                             [&token_name, scope](const ThemeOverride& ovr)
                             { return ovr.token_name == token_name && ovr.scope == scope; });
    if (iter == overrides_.end())
    {
        return false;
    }
    overrides_.erase(iter);
    return true;
}

auto ThemeOverrideService::get_override(const std::string& token_name, OverrideScope scope) const
    -> const ThemeOverride*
{
    for (const auto& ovr : overrides_)
    {
        if (ovr.token_name == token_name && ovr.scope == scope)
        {
            return &ovr;
        }
    }
    return nullptr;
}

auto ThemeOverrideService::list_overrides() const -> const std::vector<ThemeOverride>&
{
    return overrides_;
}

auto ThemeOverrideService::overrides_for_scope(OverrideScope scope) const
    -> std::vector<const ThemeOverride*>
{
    std::vector<const ThemeOverride*> result;
    for (const auto& ovr : overrides_)
    {
        if (ovr.scope == scope)
        {
            result.push_back(&ovr);
        }
    }
    return result;
}

auto ThemeOverrideService::override_count() const -> std::size_t
{
    return overrides_.size();
}

// ── Application ───────────────────────────────────────────────────────────────

auto ThemeOverrideService::apply_overrides(
    const std::vector<std::pair<std::string, std::string>>& base_tokens) const
    -> std::vector<std::pair<std::string, std::string>>
{
    auto result = base_tokens;

    for (const auto& ovr : overrides_)
    {
        bool found = false;
        for (auto& [token, value] : result)
        {
            if (token == ovr.token_name)
            {
                value = ovr.override_value;
                found = true;
                break;
            }
        }
        if (!found)
        {
            result.emplace_back(ovr.token_name, ovr.override_value);
        }
    }
    return result;
}

void ThemeOverrideService::reset_overrides(OverrideScope scope)
{
    overrides_.erase(std::remove_if(overrides_.begin(),
                                    overrides_.end(),
                                    [scope](const ThemeOverride& ovr)
                                    { return ovr.scope == scope; }),
                     overrides_.end());
}

void ThemeOverrideService::reset_all()
{
    overrides_.clear();
}

// ── Export / Import ───────────────────────────────────────────────────────────

auto ThemeOverrideService::export_overrides() const -> std::string
{
    std::ostringstream oss;
    oss << R"({"overrides":[)";
    for (std::size_t idx = 0; idx < overrides_.size(); ++idx)
    {
        const auto& ovr = overrides_[idx];
        if (idx > 0)
        {
            oss << ",";
        }
        oss << R"({"token":")" << ovr.token_name << R"(","original":")" << ovr.original_value
            << R"(","override":")" << ovr.override_value << R"(","scope":")"
            << scope_name(ovr.scope) << R"("})";
    }
    oss << "]}";
    return oss.str();
}

auto ThemeOverrideService::import_overrides(const std::string& json_data) -> int
{
    int count = 0;
    std::string::size_type pos = 0;

    while ((pos = json_data.find(R"("token":")", pos)) != std::string::npos)
    {
        pos += 9;
        auto token_end = json_data.find('"', pos);
        if (token_end == std::string::npos)
        {
            break;
        }
        const std::string token = json_data.substr(pos, token_end - pos);

        std::string original;
        auto orig_pos = json_data.find(R"("original":")", token_end);
        if (orig_pos != std::string::npos && orig_pos < token_end + 200)
        {
            orig_pos += 12;
            auto orig_end = json_data.find('"', orig_pos);
            if (orig_end != std::string::npos)
            {
                original = json_data.substr(orig_pos, orig_end - orig_pos);
            }
        }

        std::string override_val;
        auto ovr_pos = json_data.find(R"("override":")", token_end);
        if (ovr_pos != std::string::npos && ovr_pos < token_end + 300)
        {
            ovr_pos += 12;
            auto ovr_end = json_data.find('"', ovr_pos);
            if (ovr_end != std::string::npos)
            {
                override_val = json_data.substr(ovr_pos, ovr_end - ovr_pos);
            }
        }

        if (!token.empty() && !override_val.empty())
        {
            set_override(token, original, override_val);
            ++count;
        }
    }
    return count;
}

auto ThemeOverrideService::scope_name(OverrideScope scope) -> std::string
{
    switch (scope)
    {
        case OverrideScope::kVault:
            return "vault";
        case OverrideScope::kWorkspace:
            return "workspace";
        case OverrideScope::kGlobal:
            return "global";
    }
    return "vault";
}

} // namespace markamp::core
