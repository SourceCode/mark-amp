/// @file StringFormatter.h
/// @brief V9 Phase 47 — Locale-aware string formatting.
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Number format configuration.
struct NumberFormat
{
    char decimal_separator{'.'};
    char thousands_separator{','};
    int decimal_places{2};
};

/// Date format style.
enum class DateFormat : uint8_t
{
    kShort = 0,   ///< e.g. "02/18/26"
    kMedium = 1,  ///< e.g. "Feb 18, 2026"
    kLong = 2,    ///< e.g. "February 18, 2026"
    kISO8601 = 3, ///< e.g. "2026-02-18"
};

/// Locale-aware string formatting utilities.
class StringFormatter
{
public:
    StringFormatter() = default;

    // ── Formatting ────────────────────────────────────────────────────
    [[nodiscard]] auto format_number(double value, const NumberFormat& fmt = {}) const
        -> std::string;
    [[nodiscard]] auto format_date(std::chrono::system_clock::time_point time_pt,
                                   DateFormat fmt = DateFormat::kISO8601) const -> std::string;
    [[nodiscard]] static auto
    format_plural(int count, const std::string& singular, const std::string& plural) -> std::string;
    [[nodiscard]] static auto format_template(const std::string& tmpl,
                                              const std::vector<std::string>& args) -> std::string;

    // ── Locale ────────────────────────────────────────────────────────
    void set_locale(const std::string& locale_id);
    [[nodiscard]] auto get_locale() const -> const std::string&;

private:
    std::string locale_id_{"en-US"};
};

} // namespace markamp::core
