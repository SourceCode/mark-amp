#include "AVValue.h"

#include <sstream>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// AVValueRollup::operator==
// ════════════════════════════════════════════════════════════

auto AVValueRollup::operator==(const AVValueRollup& other) const -> bool
{
    return contents == other.contents;
}

// ════════════════════════════════════════════════════════════
// AVValue typed accessors
// ════════════════════════════════════════════════════════════

auto AVValue::as_block() -> AVValueBlock*
{
    return std::get_if<AVValueBlock>(&data);
}
auto AVValue::as_block() const -> const AVValueBlock*
{
    return std::get_if<AVValueBlock>(&data);
}
auto AVValue::as_text() -> AVValueText*
{
    return std::get_if<AVValueText>(&data);
}
auto AVValue::as_text() const -> const AVValueText*
{
    return std::get_if<AVValueText>(&data);
}
auto AVValue::as_number() -> AVValueNumber*
{
    return std::get_if<AVValueNumber>(&data);
}
auto AVValue::as_number() const -> const AVValueNumber*
{
    return std::get_if<AVValueNumber>(&data);
}
auto AVValue::as_date() -> AVValueDate*
{
    return std::get_if<AVValueDate>(&data);
}
auto AVValue::as_date() const -> const AVValueDate*
{
    return std::get_if<AVValueDate>(&data);
}
auto AVValue::as_select() -> AVValueSelect*
{
    return std::get_if<AVValueSelect>(&data);
}
auto AVValue::as_select() const -> const AVValueSelect*
{
    return std::get_if<AVValueSelect>(&data);
}
auto AVValue::as_mselect() -> AVValueMSelect*
{
    return std::get_if<AVValueMSelect>(&data);
}
auto AVValue::as_mselect() const -> const AVValueMSelect*
{
    return std::get_if<AVValueMSelect>(&data);
}
auto AVValue::as_url() -> AVValueURL*
{
    return std::get_if<AVValueURL>(&data);
}
auto AVValue::as_url() const -> const AVValueURL*
{
    return std::get_if<AVValueURL>(&data);
}
auto AVValue::as_email() -> AVValueEmail*
{
    return std::get_if<AVValueEmail>(&data);
}
auto AVValue::as_email() const -> const AVValueEmail*
{
    return std::get_if<AVValueEmail>(&data);
}
auto AVValue::as_phone() -> AVValuePhone*
{
    return std::get_if<AVValuePhone>(&data);
}
auto AVValue::as_phone() const -> const AVValuePhone*
{
    return std::get_if<AVValuePhone>(&data);
}
auto AVValue::as_masset() -> AVValueMAsset*
{
    return std::get_if<AVValueMAsset>(&data);
}
auto AVValue::as_masset() const -> const AVValueMAsset*
{
    return std::get_if<AVValueMAsset>(&data);
}
auto AVValue::as_template() -> AVValueTemplate*
{
    return std::get_if<AVValueTemplate>(&data);
}
auto AVValue::as_template() const -> const AVValueTemplate*
{
    return std::get_if<AVValueTemplate>(&data);
}
auto AVValue::as_created() -> AVValueCreated*
{
    return std::get_if<AVValueCreated>(&data);
}
auto AVValue::as_created() const -> const AVValueCreated*
{
    return std::get_if<AVValueCreated>(&data);
}
auto AVValue::as_updated() -> AVValueUpdated*
{
    return std::get_if<AVValueUpdated>(&data);
}
auto AVValue::as_updated() const -> const AVValueUpdated*
{
    return std::get_if<AVValueUpdated>(&data);
}
auto AVValue::as_checkbox() -> AVValueCheckbox*
{
    return std::get_if<AVValueCheckbox>(&data);
}
auto AVValue::as_checkbox() const -> const AVValueCheckbox*
{
    return std::get_if<AVValueCheckbox>(&data);
}
auto AVValue::as_relation() -> AVValueRelation*
{
    return std::get_if<AVValueRelation>(&data);
}
auto AVValue::as_relation() const -> const AVValueRelation*
{
    return std::get_if<AVValueRelation>(&data);
}
auto AVValue::as_rollup() -> AVValueRollup*
{
    return std::get_if<AVValueRollup>(&data);
}
auto AVValue::as_rollup() const -> const AVValueRollup*
{
    return std::get_if<AVValueRollup>(&data);
}

auto AVValue::is_empty() const -> bool
{
    return std::holds_alternative<std::monostate>(data);
}

auto AVValue::to_display_string() const -> std::string
{
    if (is_empty())
    {
        return {};
    }

    if (const auto* text_val = as_text())
    {
        return text_val->content;
    }
    if (const auto* block_val = as_block())
    {
        return block_val->content;
    }
    if (const auto* num_val = as_number())
    {
        if (!num_val->is_not_empty)
        {
            return {};
        }
        return std::to_string(num_val->content);
    }
    if (const auto* date_val = as_date())
    {
        if (!date_val->is_not_empty)
        {
            return {};
        }
        return date_val->formatted_content.empty() ? std::to_string(date_val->content)
                                                   : date_val->formatted_content;
    }
    if (const auto* sel_val = as_select())
    {
        return sel_val->option.name;
    }
    if (const auto* msel_val = as_mselect())
    {
        std::ostringstream oss;
        bool first = true;
        for (const auto& opt : msel_val->options)
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
    if (const auto* url_val = as_url())
    {
        return url_val->content;
    }
    if (const auto* email_val = as_email())
    {
        return email_val->content;
    }
    if (const auto* phone_val = as_phone())
    {
        return phone_val->content;
    }
    if (const auto* asset_val = as_masset())
    {
        std::ostringstream oss;
        bool first = true;
        for (const auto& asset : asset_val->assets)
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
    if (const auto* tmpl_val = as_template())
    {
        return tmpl_val->content;
    }
    if (const auto* created_val = as_created())
    {
        return created_val->formatted_content.empty() ? std::to_string(created_val->content)
                                                      : created_val->formatted_content;
    }
    if (const auto* updated_val = as_updated())
    {
        return updated_val->formatted_content.empty() ? std::to_string(updated_val->content)
                                                      : updated_val->formatted_content;
    }
    if (const auto* cb_val = as_checkbox())
    {
        return cb_val->checked ? "true" : "false";
    }
    if (const auto* rel_val = as_relation())
    {
        std::ostringstream oss;
        bool first = true;
        for (const auto& rel_content : rel_val->contents)
        {
            if (!first)
            {
                oss << ", ";
            }
            oss << rel_content.content;
            first = false;
        }
        return oss.str();
    }

    return {};
}

} // namespace markamp::core::av
