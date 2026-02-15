#include "AVValidators.h"

#include <algorithm>
#include <regex>

namespace markamp::core::av
{

auto validate_type_match(const AVValue& value) -> AVValidationResult
{
    if (value.is_empty())
    {
        return AVValidationResult::ok();
    }

    // Verify the variant type matches the declared key type
    switch (value.type)
    {
        case AVKeyType::Block:
            return value.as_block() ? AVValidationResult::ok()
                                    : AVValidationResult::error("Type mismatch: expected Block");
        case AVKeyType::Text:
            return value.as_text() ? AVValidationResult::ok()
                                   : AVValidationResult::error("Type mismatch: expected Text");
        case AVKeyType::Number:
            return value.as_number() ? AVValidationResult::ok()
                                     : AVValidationResult::error("Type mismatch: expected Number");
        case AVKeyType::Date:
            return value.as_date() ? AVValidationResult::ok()
                                   : AVValidationResult::error("Type mismatch: expected Date");
        case AVKeyType::Select:
            return value.as_select() ? AVValidationResult::ok()
                                     : AVValidationResult::error("Type mismatch: expected Select");
        case AVKeyType::MSelect:
            return value.as_mselect()
                       ? AVValidationResult::ok()
                       : AVValidationResult::error("Type mismatch: expected MSelect");
        case AVKeyType::URL:
            return value.as_url() ? AVValidationResult::ok()
                                  : AVValidationResult::error("Type mismatch: expected URL");
        case AVKeyType::Email:
            return value.as_email() ? AVValidationResult::ok()
                                    : AVValidationResult::error("Type mismatch: expected Email");
        case AVKeyType::Phone:
            return value.as_phone() ? AVValidationResult::ok()
                                    : AVValidationResult::error("Type mismatch: expected Phone");
        case AVKeyType::MAsset:
            return value.as_masset() ? AVValidationResult::ok()
                                     : AVValidationResult::error("Type mismatch: expected MAsset");
        case AVKeyType::Template:
            return value.as_template()
                       ? AVValidationResult::ok()
                       : AVValidationResult::error("Type mismatch: expected Template");
        case AVKeyType::Created:
            return value.as_created()
                       ? AVValidationResult::ok()
                       : AVValidationResult::error("Type mismatch: expected Created");
        case AVKeyType::Updated:
            return value.as_updated()
                       ? AVValidationResult::ok()
                       : AVValidationResult::error("Type mismatch: expected Updated");
        case AVKeyType::Checkbox:
            return value.as_checkbox()
                       ? AVValidationResult::ok()
                       : AVValidationResult::error("Type mismatch: expected Checkbox");
        case AVKeyType::Relation:
            return value.as_relation()
                       ? AVValidationResult::ok()
                       : AVValidationResult::error("Type mismatch: expected Relation");
        case AVKeyType::Rollup:
            return value.as_rollup() ? AVValidationResult::ok()
                                     : AVValidationResult::error("Type mismatch: expected Rollup");
        case AVKeyType::LineNumber:
            return AVValidationResult::ok(); // computed
    }
    return AVValidationResult::ok();
}

auto validate_text(const AVValueText& text, int max_length) -> AVValidationResult
{
    if (static_cast<int>(text.content.size()) > max_length)
    {
        return AVValidationResult::error("Text exceeds maximum length of " +
                                         std::to_string(max_length));
    }
    return AVValidationResult::ok();
}

auto validate_number(const AVValueNumber& number) -> AVValidationResult
{
    if (number.is_not_empty && std::isnan(number.content))
    {
        return AVValidationResult::error("Number value is NaN");
    }
    if (number.is_not_empty && std::isinf(number.content))
    {
        return AVValidationResult::error("Number value is infinite");
    }
    return AVValidationResult::ok();
}

auto validate_date(const AVValueDate& date) -> AVValidationResult
{
    if (date.has_end_date && date.content2 < date.content)
    {
        return AVValidationResult::error("End date cannot be before start date");
    }
    return AVValidationResult::ok();
}

auto validate_url(const AVValueURL& url_val) -> AVValidationResult
{
    if (url_val.content.empty())
    {
        return AVValidationResult::ok();
    }

    if (!url_val.content.starts_with("http://") && !url_val.content.starts_with("https://") &&
        !url_val.content.starts_with("ftp://") && !url_val.content.starts_with("mailto:"))
    {
        return AVValidationResult::error(
            "URL must start with a valid protocol (http://, https://, ftp://, mailto:)");
    }
    return AVValidationResult::ok();
}

auto validate_email(const AVValueEmail& email) -> AVValidationResult
{
    if (email.content.empty())
    {
        return AVValidationResult::ok();
    }

    const auto at_pos = email.content.find('@');
    if (at_pos == std::string::npos || at_pos == 0 || at_pos == email.content.size() - 1)
    {
        return AVValidationResult::error("Invalid email address format");
    }

    const auto dot_pos = email.content.find('.', at_pos);
    if (dot_pos == std::string::npos || dot_pos == email.content.size() - 1)
    {
        return AVValidationResult::error("Invalid email address: missing domain");
    }
    return AVValidationResult::ok();
}

auto validate_phone(const AVValuePhone& phone) -> AVValidationResult
{
    if (phone.content.empty())
    {
        return AVValidationResult::ok();
    }

    const size_t digit_count = static_cast<size_t>(
        std::count_if(phone.content.begin(),
                      phone.content.end(),
                      [](char chr) { return std::isdigit(static_cast<unsigned char>(chr)); }));

    if (digit_count < 7)
    {
        return AVValidationResult::error("Phone number must contain at least 7 digits");
    }
    return AVValidationResult::ok();
}

auto validate_select(const AVValueSelect& select, const AVKey& key) -> AVValidationResult
{
    if (select.option.name.empty())
    {
        return AVValidationResult::ok();
    }

    const auto found = std::find_if(key.options.begin(),
                                    key.options.end(),
                                    [&select](const AVSelectOption& opt)
                                    { return opt.name == select.option.name; });
    if (found == key.options.end())
    {
        return AVValidationResult::error("Select option not found: " + select.option.name);
    }
    return AVValidationResult::ok();
}

auto validate_mselect(const AVValueMSelect& mselect, const AVKey& key) -> AVValidationResult
{
    for (const auto& opt : mselect.options)
    {
        const auto found = std::find_if(key.options.begin(),
                                        key.options.end(),
                                        [&opt](const AVSelectOption& key_opt)
                                        { return key_opt.name == opt.name; });
        if (found == key.options.end())
        {
            return AVValidationResult::error("MSelect option not found: " + opt.name);
        }
    }
    return AVValidationResult::ok();
}

auto validate_masset(const AVValueMAsset& masset) -> AVValidationResult
{
    for (const auto& asset : masset.assets)
    {
        if (asset.content.empty())
        {
            return AVValidationResult::error("Asset entry must have content (path or URL)");
        }
    }
    return AVValidationResult::ok();
}

auto validate_relation(const AVValueRelation& relation) -> AVValidationResult
{
    for (const auto& bid : relation.block_ids)
    {
        if (bid.empty())
        {
            return AVValidationResult::error("Relation block ID cannot be empty");
        }
    }
    return AVValidationResult::ok();
}

} // namespace markamp::core::av
