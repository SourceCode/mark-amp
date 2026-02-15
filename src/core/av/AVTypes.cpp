#include "AVTypes.h"

#include <unordered_map>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// AVKeyType serialization
// ════════════════════════════════════════════════════════════

auto key_type_to_string(AVKeyType type) -> std::string
{
    switch (type)
    {
        case AVKeyType::Block:
            return "block";
        case AVKeyType::Text:
            return "text";
        case AVKeyType::Number:
            return "number";
        case AVKeyType::Date:
            return "date";
        case AVKeyType::Select:
            return "select";
        case AVKeyType::MSelect:
            return "mSelect";
        case AVKeyType::URL:
            return "url";
        case AVKeyType::Email:
            return "email";
        case AVKeyType::Phone:
            return "phone";
        case AVKeyType::MAsset:
            return "mAsset";
        case AVKeyType::Template:
            return "template";
        case AVKeyType::Created:
            return "created";
        case AVKeyType::Updated:
            return "updated";
        case AVKeyType::Checkbox:
            return "checkbox";
        case AVKeyType::Relation:
            return "relation";
        case AVKeyType::Rollup:
            return "rollup";
        case AVKeyType::LineNumber:
            return "lineNumber";
    }
    return "text";
}

auto string_to_key_type(const std::string& str) -> AVKeyType
{
    static const std::unordered_map<std::string, AVKeyType> lookup = {
        {"block", AVKeyType::Block},
        {"text", AVKeyType::Text},
        {"number", AVKeyType::Number},
        {"date", AVKeyType::Date},
        {"select", AVKeyType::Select},
        {"mSelect", AVKeyType::MSelect},
        {"url", AVKeyType::URL},
        {"email", AVKeyType::Email},
        {"phone", AVKeyType::Phone},
        {"mAsset", AVKeyType::MAsset},
        {"template", AVKeyType::Template},
        {"created", AVKeyType::Created},
        {"updated", AVKeyType::Updated},
        {"checkbox", AVKeyType::Checkbox},
        {"relation", AVKeyType::Relation},
        {"rollup", AVKeyType::Rollup},
        {"lineNumber", AVKeyType::LineNumber},
    };

    const auto iter = lookup.find(str);
    if (iter != lookup.end())
    {
        return iter->second;
    }
    return AVKeyType::Text;
}

// ════════════════════════════════════════════════════════════
// AVViewType serialization
// ════════════════════════════════════════════════════════════

auto view_type_to_string(AVViewType type) -> std::string
{
    switch (type)
    {
        case AVViewType::Table:
            return "table";
        case AVViewType::Gallery:
            return "gallery";
        case AVViewType::Kanban:
            return "kanban";
    }
    return "table";
}

auto string_to_view_type(const std::string& str) -> AVViewType
{
    if (str == "gallery")
    {
        return AVViewType::Gallery;
    }
    if (str == "kanban")
    {
        return AVViewType::Kanban;
    }
    return AVViewType::Table;
}

// ════════════════════════════════════════════════════════════
// AVNumberFormat serialization
// ════════════════════════════════════════════════════════════

auto number_format_to_string(AVNumberFormat fmt) -> std::string
{
    switch (fmt)
    {
        case AVNumberFormat::None:
            return "none";
        case AVNumberFormat::Commas:
            return "commas";
        case AVNumberFormat::Percent:
            return "percent";
        case AVNumberFormat::USDollar:
            return "usDollar";
        case AVNumberFormat::Euro:
            return "euro";
        case AVNumberFormat::Pound:
            return "pound";
        case AVNumberFormat::Yen:
            return "yen";
        case AVNumberFormat::Yuan:
            return "yuan";
        case AVNumberFormat::Ruble:
            return "ruble";
        case AVNumberFormat::Rupee:
            return "rupee";
        case AVNumberFormat::Won:
            return "won";
        case AVNumberFormat::Custom:
            return "custom";
    }
    return "none";
}

auto string_to_number_format(const std::string& str) -> AVNumberFormat
{
    static const std::unordered_map<std::string, AVNumberFormat> lookup = {
        {"none", AVNumberFormat::None},
        {"commas", AVNumberFormat::Commas},
        {"percent", AVNumberFormat::Percent},
        {"usDollar", AVNumberFormat::USDollar},
        {"euro", AVNumberFormat::Euro},
        {"pound", AVNumberFormat::Pound},
        {"yen", AVNumberFormat::Yen},
        {"yuan", AVNumberFormat::Yuan},
        {"ruble", AVNumberFormat::Ruble},
        {"rupee", AVNumberFormat::Rupee},
        {"won", AVNumberFormat::Won},
        {"custom", AVNumberFormat::Custom},
    };

    const auto iter = lookup.find(str);
    if (iter != lookup.end())
    {
        return iter->second;
    }
    return AVNumberFormat::None;
}

// ════════════════════════════════════════════════════════════
// AVRollupCalc serialization
// ════════════════════════════════════════════════════════════

auto rollup_calc_to_string(AVRollupCalc calc) -> std::string
{
    switch (calc)
    {
        case AVRollupCalc::None:
            return "none";
        case AVRollupCalc::CountAll:
            return "countAll";
        case AVRollupCalc::CountValues:
            return "countValues";
        case AVRollupCalc::CountUniqueValues:
            return "countUniqueValues";
        case AVRollupCalc::CountEmpty:
            return "countEmpty";
        case AVRollupCalc::CountNotEmpty:
            return "countNotEmpty";
        case AVRollupCalc::PercentEmpty:
            return "percentEmpty";
        case AVRollupCalc::PercentNotEmpty:
            return "percentNotEmpty";
        case AVRollupCalc::Sum:
            return "sum";
        case AVRollupCalc::Average:
            return "average";
        case AVRollupCalc::Median:
            return "median";
        case AVRollupCalc::Min:
            return "min";
        case AVRollupCalc::Max:
            return "max";
        case AVRollupCalc::Range:
            return "range";
        case AVRollupCalc::DateEarliest:
            return "dateEarliest";
        case AVRollupCalc::DateLatest:
            return "dateLatest";
        case AVRollupCalc::Checked:
            return "checked";
        case AVRollupCalc::Unchecked:
            return "unchecked";
        case AVRollupCalc::PercentChecked:
            return "percentChecked";
        case AVRollupCalc::PercentUnchecked:
            return "percentUnchecked";
    }
    return "none";
}

auto string_to_rollup_calc(const std::string& str) -> AVRollupCalc
{
    static const std::unordered_map<std::string, AVRollupCalc> lookup = {
        {"none", AVRollupCalc::None},
        {"countAll", AVRollupCalc::CountAll},
        {"countValues", AVRollupCalc::CountValues},
        {"countUniqueValues", AVRollupCalc::CountUniqueValues},
        {"countEmpty", AVRollupCalc::CountEmpty},
        {"countNotEmpty", AVRollupCalc::CountNotEmpty},
        {"percentEmpty", AVRollupCalc::PercentEmpty},
        {"percentNotEmpty", AVRollupCalc::PercentNotEmpty},
        {"sum", AVRollupCalc::Sum},
        {"average", AVRollupCalc::Average},
        {"median", AVRollupCalc::Median},
        {"min", AVRollupCalc::Min},
        {"max", AVRollupCalc::Max},
        {"range", AVRollupCalc::Range},
        {"dateEarliest", AVRollupCalc::DateEarliest},
        {"dateLatest", AVRollupCalc::DateLatest},
        {"checked", AVRollupCalc::Checked},
        {"unchecked", AVRollupCalc::Unchecked},
        {"percentChecked", AVRollupCalc::PercentChecked},
        {"percentUnchecked", AVRollupCalc::PercentUnchecked},
    };

    const auto iter = lookup.find(str);
    if (iter != lookup.end())
    {
        return iter->second;
    }
    return AVRollupCalc::None;
}

// ════════════════════════════════════════════════════════════
// AVCardSize serialization
// ════════════════════════════════════════════════════════════

auto card_size_to_string(AVCardSize size) -> std::string
{
    switch (size)
    {
        case AVCardSize::Small:
            return "small";
        case AVCardSize::Medium:
            return "medium";
        case AVCardSize::Large:
            return "large";
    }
    return "medium";
}

auto string_to_card_size(const std::string& str) -> AVCardSize
{
    if (str == "small")
    {
        return AVCardSize::Small;
    }
    if (str == "large")
    {
        return AVCardSize::Large;
    }
    return AVCardSize::Medium;
}

} // namespace markamp::core::av
