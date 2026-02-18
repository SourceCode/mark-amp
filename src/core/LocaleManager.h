/// @file LocaleManager.h
/// @brief V9 Phase 47 — Application locale management.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Text direction for a locale.
enum class LocaleTextDirection : uint8_t
{
    kLTR = 0,
    kRTL = 1,
};

/// Information about a supported locale.
struct LocaleInfo
{
    std::string locale_id;     ///< e.g. "en-US"
    std::string display_name;  ///< e.g. "English (United States)"
    std::string language_code; ///< e.g. "en"
    std::string region_code;   ///< e.g. "US"
    LocaleTextDirection text_direction{LocaleTextDirection::kLTR};
    bool is_rtl{false};
};

/// Manages the active application locale and available locales.
class LocaleManager
{
public:
    LocaleManager() = default;

    // ── Active locale ─────────────────────────────────────────────────
    auto set_locale(const std::string& locale_id) -> bool;
    [[nodiscard]] auto get_locale() const -> const LocaleInfo&;
    [[nodiscard]] auto is_rtl() const -> bool;

    // ── Locale registry ───────────────────────────────────────────────
    void add_locale(LocaleInfo info);
    [[nodiscard]] auto find_locale(const std::string& locale_id) const -> const LocaleInfo*;
    [[nodiscard]] auto available_locales() const -> std::vector<const LocaleInfo*>;
    void load_defaults();

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto locale_count() const -> int;
    void clear();

private:
    std::vector<LocaleInfo> locales_;
    LocaleInfo active_locale_; ///< Defaults to en-US
    bool has_active_{false};
};

} // namespace markamp::core
