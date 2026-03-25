/// @file V27SettingsDesignCoordinator.cpp
/// @brief V27 Phase 12 — Settings, theme gallery, control redesign orchestration.
#include "core/V27SettingsDesignCoordinator.h"
#include <sstream>
namespace markamp::core
{
void V27SettingsDesignCoordinator::register_category(SettingsCategoryChromePolicy policy) {
    categories_.push_back(std::move(policy));
}

auto V27SettingsDesignCoordinator::category_count() const -> int {
    return static_cast<int>(categories_.size());
}

auto V27SettingsDesignCoordinator::emoji_free_count() const -> int {
    int n = 0;
    for (const auto& c : categories_) { if (c.emoji_free) { ++n; } }
    return n;
}

auto V27SettingsDesignCoordinator::all_emoji_free() const -> bool {
    for (const auto& c : categories_) { if (!c.emoji_free) { return false; } }
    return !categories_.empty();
}

auto V27SettingsDesignCoordinator::canonical_icon_count() const -> int {
    int n = 0;
    for (const auto& c : categories_) { if (c.uses_canonical_icon) { ++n; } }
    return n;
}

auto V27SettingsDesignCoordinator::total_rows() const -> int {
    int n = 0;
    for (const auto& c : categories_) { n += c.row_count; }
    return n;
}

auto V27SettingsDesignCoordinator::categories() const -> const std::vector<SettingsCategoryChromePolicy>& {
    return categories_;
}

auto V27SettingsDesignCoordinator::summary() const -> std::string {
    std::ostringstream os;
    os << "# V27 Settings Redesign Status\n\n";
    os << "Categories: " << category_count() << " | Emoji-free: " << emoji_free_count()
       << " | Canonical icons: " << canonical_icon_count() << " | Total rows: " << total_rows() << "\n";
    return os.str();
}
} // namespace markamp::core
