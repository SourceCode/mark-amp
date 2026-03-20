/// @file MuiIconMigration.cpp
/// @brief P10-T03: Batch icon migration tracking.

#include "MuiIconMigration.h"

#include "Logger.h"

namespace markamp::core
{

void MuiIconMigration::mark_migrated(IconSurface surface, const std::string& icon_id)
{
    const int key = static_cast<int>(surface);
    ++surface_migrated_[key];
    ++surface_total_[key];
    ++migrated_;
    ++total_;
    MARKAMP_LOG_DEBUG("Icon migrated: {} (surface {})", icon_id, key);
}

void MuiIconMigration::mark_pending(IconSurface surface, const std::string& icon_id)
{
    const int key = static_cast<int>(surface);
    ++surface_total_[key];
    ++total_;
    MARKAMP_LOG_DEBUG("Icon pending: {} (surface {})", icon_id, key);
}

auto MuiIconMigration::progress(IconSurface surface) const -> double
{
    const int key = static_cast<int>(surface);
    auto total_iter = surface_total_.find(key);
    if (total_iter == surface_total_.end() || total_iter->second == 0)
    {
        return 1.0;
    }
    auto migrated_iter = surface_migrated_.find(key);
    const int migrated_val = (migrated_iter != surface_migrated_.end()) ? migrated_iter->second : 0;
    return static_cast<double>(migrated_val) / static_cast<double>(total_iter->second);
}

auto MuiIconMigration::overall_progress() const -> double
{
    if (total_ == 0)
    {
        return 1.0;
    }
    return static_cast<double>(migrated_) / static_cast<double>(total_);
}

} // namespace markamp::core
