#include "AVFormatters.h"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace markamp::core::av
{

auto format_number(double value, AVNumberFormat format, const std::string& custom_format)
    -> std::string
{
    std::ostringstream oss;

    switch (format)
    {
        case AVNumberFormat::None:
        {
            // Remove trailing zeros
            oss << std::setprecision(6) << std::defaultfloat << value;
            std::string result = oss.str();
            if (result.find('.') != std::string::npos)
            {
                while (result.back() == '0')
                {
                    result.pop_back();
                }
                if (result.back() == '.')
                {
                    result.pop_back();
                }
            }
            return result;
        }
        case AVNumberFormat::Commas:
        {
            const auto int_part = static_cast<int64_t>(value);
            auto abs_val = std::abs(int_part);
            std::string digits;
            if (abs_val == 0)
            {
                digits = "0";
            }
            while (abs_val > 0)
            {
                digits = char('0' + (abs_val % 10)) + digits;
                abs_val /= 10;
            }
            // Insert commas
            std::string formatted;
            for (size_t idx = 0; idx < digits.size(); ++idx)
            {
                if (idx > 0 && (digits.size() - idx) % 3 == 0)
                {
                    formatted += ',';
                }
                formatted += digits[idx];
            }
            if (int_part < 0)
            {
                formatted = "-" + formatted;
            }
            return formatted;
        }
        case AVNumberFormat::Percent:
            oss << std::fixed << std::setprecision(1) << (value * 100.0) << "%";
            return oss.str();
        case AVNumberFormat::USDollar:
            oss << "$" << std::fixed << std::setprecision(2) << value;
            return oss.str();
        case AVNumberFormat::Euro:
            oss << "€" << std::fixed << std::setprecision(2) << value;
            return oss.str();
        case AVNumberFormat::Pound:
            oss << "£" << std::fixed << std::setprecision(2) << value;
            return oss.str();
        case AVNumberFormat::Yen:
            oss << "¥" << std::fixed << std::setprecision(0) << value;
            return oss.str();
        case AVNumberFormat::Yuan:
            oss << "¥" << std::fixed << std::setprecision(2) << value;
            return oss.str();
        case AVNumberFormat::Ruble:
            oss << std::fixed << std::setprecision(2) << value << " ₽";
            return oss.str();
        case AVNumberFormat::Rupee:
            oss << "₹" << std::fixed << std::setprecision(2) << value;
            return oss.str();
        case AVNumberFormat::Won:
            oss << "₩" << std::fixed << std::setprecision(0) << value;
            return oss.str();
        case AVNumberFormat::Custom:
            if (custom_format.empty())
            {
                oss << value;
            }
            else
            {
                oss << custom_format << value;
            }
            return oss.str();
    }
    return std::to_string(value);
}

auto format_date(int64_t timestamp, bool is_not_time) -> std::string
{
    if (timestamp == 0)
    {
        return {};
    }

    const auto time_val = static_cast<std::time_t>(timestamp / 1000); // milliseconds to seconds
    std::tm time_struct{};
    localtime_r(&time_val, &time_struct);

    std::ostringstream oss;
    oss << std::put_time(&time_struct, is_not_time ? "%Y-%m-%d" : "%Y-%m-%d %H:%M");
    return oss.str();
}

auto format_date_range(int64_t start, int64_t end_ts, bool is_not_time) -> std::string
{
    auto start_str = format_date(start, is_not_time);
    auto end_str = format_date(end_ts, is_not_time);
    if (start_str.empty() && end_str.empty())
    {
        return {};
    }
    if (end_str.empty())
    {
        return start_str;
    }
    return start_str + " → " + end_str;
}

auto format_select(const AVSelectOption& option) -> std::string
{
    return option.name;
}

auto format_mselect(const std::vector<AVSelectOption>& options) -> std::string
{
    std::ostringstream oss;
    bool first = true;
    for (const auto& opt : options)
    {
        if (!first)
        {
            oss << ", ";
        }
        oss << opt.name;
        first = false;
    }
    return oss.str();
}

auto smart_url(const std::string& full_url) -> std::string
{
    std::string result = full_url;

    // Remove protocol
    if (result.starts_with("https://"))
    {
        result = result.substr(8);
    }
    else if (result.starts_with("http://"))
    {
        result = result.substr(7);
    }

    // Remove www.
    if (result.starts_with("www."))
    {
        result = result.substr(4);
    }

    // Remove trailing slash
    if (!result.empty() && result.back() == '/')
    {
        result.pop_back();
    }

    return result;
}

auto format_asset(const AVValueAssetEntry& asset) -> std::string
{
    return asset.name;
}

auto format_masset(const std::vector<AVValueAssetEntry>& assets) -> std::string
{
    std::ostringstream oss;
    bool first = true;
    for (const auto& asset : assets)
    {
        if (!first)
        {
            oss << ", ";
        }
        oss << asset.name;
        first = false;
    }
    return oss.str();
}

auto format_value_for_display(const AVValue& value, const AVKey& key) -> std::string
{
    if (value.is_empty())
    {
        return {};
    }

    switch (key.type)
    {
        case AVKeyType::Number:
        {
            const auto* num_val = value.as_number();
            if (!num_val || !num_val->is_not_empty)
            {
                return {};
            }
            return format_number(num_val->content, key.number_format, key.custom_number_format);
        }
        case AVKeyType::Date:
        {
            const auto* date_val = value.as_date();
            if (!date_val || !date_val->is_not_empty)
            {
                return {};
            }
            if (date_val->has_end_date)
            {
                return format_date_range(
                    date_val->content, date_val->content2, date_val->is_not_time);
            }
            return format_date(date_val->content, date_val->is_not_time);
        }
        case AVKeyType::Select:
        {
            const auto* sel_val = value.as_select();
            return sel_val ? format_select(sel_val->option) : std::string{};
        }
        case AVKeyType::MSelect:
        {
            const auto* msel_val = value.as_mselect();
            return msel_val ? format_mselect(msel_val->options) : std::string{};
        }
        case AVKeyType::URL:
        {
            const auto* url_val = value.as_url();
            return url_val ? smart_url(url_val->content) : std::string{};
        }
        case AVKeyType::MAsset:
        {
            const auto* masset_val = value.as_masset();
            return masset_val ? format_masset(masset_val->assets) : std::string{};
        }
        case AVKeyType::Checkbox:
        {
            const auto* cb_val = value.as_checkbox();
            return (cb_val && cb_val->checked) ? "✓" : "";
        }
        default:
            return value.to_display_string();
    }
}

} // namespace markamp::core::av
