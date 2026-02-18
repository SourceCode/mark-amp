/// @file LocaleManager.cpp
/// @brief V9 Phase 47 — LocaleManager implementation.

#include "LocaleManager.h"

namespace markamp::core
{

auto LocaleManager::set_locale(const std::string& locale_id) -> bool
{
    const auto* loc = find_locale(locale_id);
    if (loc == nullptr)
    {
        return false;
    }
    active_locale_ = *loc;
    has_active_ = true;
    return true;
}

auto LocaleManager::get_locale() const -> const LocaleInfo&
{
    return active_locale_;
}

auto LocaleManager::is_rtl() const -> bool
{
    return active_locale_.is_rtl;
}

void LocaleManager::add_locale(LocaleInfo info)
{
    // Set first added locale as active if none set
    if (!has_active_)
    {
        active_locale_ = info;
        has_active_ = true;
    }
    locales_.push_back(std::move(info));
}

auto LocaleManager::find_locale(const std::string& locale_id) const -> const LocaleInfo*
{
    for (const auto& loc : locales_)
    {
        if (loc.locale_id == locale_id)
        {
            return &loc;
        }
    }
    return nullptr;
}

auto LocaleManager::available_locales() const -> std::vector<const LocaleInfo*>
{
    std::vector<const LocaleInfo*> result;
    result.reserve(locales_.size());
    for (const auto& loc : locales_)
    {
        result.push_back(&loc);
    }
    return result;
}

void LocaleManager::load_defaults()
{
    add_locale({"en-US", "English (United States)", "en", "US", LocaleTextDirection::kLTR, false});
    add_locale({"es-ES", "Español (España)", "es", "ES", LocaleTextDirection::kLTR, false});
    add_locale({"fr-FR", "Français (France)", "fr", "FR", LocaleTextDirection::kLTR, false});
    add_locale({"de-DE", "Deutsch (Deutschland)", "de", "DE", LocaleTextDirection::kLTR, false});
    add_locale({"ja-JP", "日本語 (日本)", "ja", "JP", LocaleTextDirection::kLTR, false});
}

auto LocaleManager::locale_count() const -> int
{
    return static_cast<int>(locales_.size());
}

void LocaleManager::clear()
{
    locales_.clear();
    active_locale_ = {};
    has_active_ = false;
}

} // namespace markamp::core
