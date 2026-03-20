/// @file IconValidationGate.cpp
/// @brief P10-T05: Legacy icon debt removal and migration validation.

#include "IconValidationGate.h"

#include "Logger.h"

namespace markamp::core
{

IconValidationGate::IconValidationGate()
{
    register_default_bans();
}

void IconValidationGate::ban_legacy_id(const std::string& legacy_id)
{
    banned_ids_.insert(legacy_id);
}

auto IconValidationGate::is_banned(const std::string& icon_id) const -> bool
{
    return banned_ids_.contains(icon_id);
}

auto IconValidationGate::validate(const std::string& icon_id,
                                   const std::string& location) const -> IconValidationResult
{
    if (is_banned(icon_id))
    {
        return {icon_id, "banned_legacy", location};
    }
    return {icon_id, "", location}; // no issue
}

void IconValidationGate::register_default_bans()
{
    ban_legacy_id("placeholder_icon");
    ban_legacy_id("emoji_placeholder");
    ban_legacy_id("lucide_fallback");
    ban_legacy_id("icon_missing");
    ban_legacy_id("legacy_default");

    MARKAMP_LOG_INFO("Icon validation: {} banned legacy IDs", banned_count());
}

} // namespace markamp::core
