/// @file StringFormatter.cpp
/// @brief V9 Phase 47 — StringFormatter implementation.

#include "StringFormatter.h"

#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace markamp::core
{

auto StringFormatter::format_number(double value, const NumberFormat& fmt) const -> std::string
{
    // Format with decimal places
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(fmt.decimal_places) << std::abs(value);
    std::string num_str = oss.str();

    // Split integer and decimal parts
    auto dot_pos = num_str.find('.');
    std::string integer_part = num_str.substr(0, dot_pos);
    std::string decimal_part;
    if (dot_pos != std::string::npos)
    {
        decimal_part = num_str.substr(dot_pos + 1);
    }

    // Insert thousands separators
    std::string formatted;
    int digit_count = 0;
    for (auto iter = integer_part.rbegin(); iter != integer_part.rend(); ++iter)
    {
        if (digit_count > 0 && digit_count % 3 == 0)
        {
            formatted.insert(formatted.begin(), fmt.thousands_separator);
        }
        formatted.insert(formatted.begin(), *iter);
        ++digit_count;
    }

    // Prepend negative sign
    if (value < 0.0)
    {
        formatted.insert(formatted.begin(), '-');
    }

    // Append decimal part
    if (fmt.decimal_places > 0 && !decimal_part.empty())
    {
        formatted += fmt.decimal_separator;
        formatted += decimal_part;
    }

    return formatted;
}

auto StringFormatter::format_date(std::chrono::system_clock::time_point time_pt,
                                  DateFormat fmt) const -> std::string
{
    auto time_c = std::chrono::system_clock::to_time_t(time_pt);
    std::tm tm_val{};
    // Use gmtime for thread safety
    gmtime_r(&time_c, &tm_val);

    std::ostringstream oss;
    switch (fmt)
    {
        case DateFormat::kShort:
            oss << std::put_time(&tm_val, "%m/%d/%y");
            break;
        case DateFormat::kMedium:
            oss << std::put_time(&tm_val, "%b %d, %Y");
            break;
        case DateFormat::kLong:
            oss << std::put_time(&tm_val, "%B %d, %Y");
            break;
        case DateFormat::kISO8601:
            oss << std::put_time(&tm_val, "%Y-%m-%d");
            break;
    }
    return oss.str();
}

auto StringFormatter::format_plural(int count,
                                    const std::string& singular,
                                    const std::string& plural) -> std::string
{
    return std::to_string(count) + " " + (count == 1 ? singular : plural);
}

auto StringFormatter::format_template(const std::string& tmpl, const std::vector<std::string>& args)
    -> std::string
{
    std::string result = tmpl;
    for (size_t idx = 0; idx < args.size(); ++idx)
    {
        std::string placeholder = "{" + std::to_string(idx) + "}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos)
        {
            result.replace(pos, placeholder.length(), args[idx]);
            pos += args[idx].length();
        }
    }
    return result;
}

void StringFormatter::set_locale(const std::string& locale_id)
{
    locale_id_ = locale_id;
}

auto StringFormatter::get_locale() const -> const std::string&
{
    return locale_id_;
}

} // namespace markamp::core
