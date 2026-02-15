#include "AVColumnType.h"

#include "AVFormatters.h"
#include "AVValidators.h"

#include <algorithm>
#include <unordered_map>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Filter operator serialization
// ════════════════════════════════════════════════════════════

auto filter_operator_to_string(AVFilterOperator op_code) -> std::string
{
    static const std::unordered_map<AVFilterOperator, std::string> lookup = {
        {AVFilterOperator::IsEmpty, "isEmpty"},
        {AVFilterOperator::IsNotEmpty, "isNotEmpty"},
        {AVFilterOperator::IsEqual, "isEqual"},
        {AVFilterOperator::IsNotEqual, "isNotEqual"},
        {AVFilterOperator::Contains, "contains"},
        {AVFilterOperator::DoesNotContain, "doesNotContain"},
        {AVFilterOperator::StartsWith, "startsWith"},
        {AVFilterOperator::EndsWith, "endsWith"},
        {AVFilterOperator::NumEqual, "numEqual"},
        {AVFilterOperator::GreaterThan, "greaterThan"},
        {AVFilterOperator::LessThan, "lessThan"},
        {AVFilterOperator::IsTrue, "isTrue"},
        {AVFilterOperator::IsFalse, "isFalse"},
    };
    const auto iter = lookup.find(op_code);
    return iter != lookup.end() ? iter->second : "unknown";
}

auto string_to_filter_operator(const std::string& str) -> AVFilterOperator
{
    static const std::unordered_map<std::string, AVFilterOperator> lookup = {
        {"isEmpty", AVFilterOperator::IsEmpty},
        {"isNotEmpty", AVFilterOperator::IsNotEmpty},
        {"isEqual", AVFilterOperator::IsEqual},
        {"isNotEqual", AVFilterOperator::IsNotEqual},
        {"contains", AVFilterOperator::Contains},
        {"doesNotContain", AVFilterOperator::DoesNotContain},
        {"startsWith", AVFilterOperator::StartsWith},
        {"endsWith", AVFilterOperator::EndsWith},
        {"numEqual", AVFilterOperator::NumEqual},
        {"greaterThan", AVFilterOperator::GreaterThan},
        {"lessThan", AVFilterOperator::LessThan},
        {"isTrue", AVFilterOperator::IsTrue},
        {"isFalse", AVFilterOperator::IsFalse},
    };
    const auto iter = lookup.find(str);
    return iter != lookup.end() ? iter->second : AVFilterOperator::IsEmpty;
}

// ════════════════════════════════════════════════════════════
// Text Column Handler
// ════════════════════════════════════════════════════════════

class TextColumnHandler final : public IAVColumnTypeHandler
{
public:
    [[nodiscard]] auto key_type() const -> AVKeyType override
    {
        return AVKeyType::Text;
    }

    [[nodiscard]] auto validate(const AVValue& value) const -> AVValidationResult override
    {
        if (value.is_empty())
        {
            return AVValidationResult::ok();
        }
        const auto* text_val = value.as_text();
        if (!text_val)
        {
            return AVValidationResult::error("Expected text value");
        }
        return validate_text(*text_val);
    }

    [[nodiscard]] auto format(const AVValue& value, const AVKey& key) const -> std::string override
    {
        return format_value_for_display(value, key);
    }

    [[nodiscard]] auto compare(const AVValue& val_a, const AVValue& val_b) const -> int override
    {
        const auto str_a = val_a.to_display_string();
        const auto str_b = val_b.to_display_string();
        return str_a.compare(str_b);
    }

    [[nodiscard]] auto filter(const AVValue& value,
                              AVFilterOperator op_code,
                              const AVValue& operand) const -> bool override
    {
        const auto content = value.to_display_string();
        const auto target = operand.to_display_string();
        switch (op_code)
        {
            case AVFilterOperator::IsEmpty:
                return content.empty();
            case AVFilterOperator::IsNotEmpty:
                return !content.empty();
            case AVFilterOperator::IsEqual:
                return content == target;
            case AVFilterOperator::IsNotEqual:
                return content != target;
            case AVFilterOperator::Contains:
                return content.find(target) != std::string::npos;
            case AVFilterOperator::DoesNotContain:
                return content.find(target) == std::string::npos;
            case AVFilterOperator::StartsWith:
                return content.starts_with(target);
            case AVFilterOperator::EndsWith:
                return content.ends_with(target);
            default:
                return false;
        }
    }

    [[nodiscard]] auto supported_operators() const -> std::vector<AVFilterOperator> override
    {
        return {AVFilterOperator::IsEmpty,
                AVFilterOperator::IsNotEmpty,
                AVFilterOperator::IsEqual,
                AVFilterOperator::IsNotEqual,
                AVFilterOperator::Contains,
                AVFilterOperator::DoesNotContain,
                AVFilterOperator::StartsWith,
                AVFilterOperator::EndsWith};
    }

    [[nodiscard]] auto parse_input(const std::string& input, const AVKey& /*key*/) const
        -> std::expected<AVValueVariant, std::string> override
    {
        return AVValueVariant{AVValueText{input}};
    }

    [[nodiscard]] auto default_value() const -> AVValueVariant override
    {
        return AVValueVariant{AVValueText{}};
    }
};

// ════════════════════════════════════════════════════════════
// Number Column Handler
// ════════════════════════════════════════════════════════════

class NumberColumnHandler final : public IAVColumnTypeHandler
{
public:
    [[nodiscard]] auto key_type() const -> AVKeyType override
    {
        return AVKeyType::Number;
    }

    [[nodiscard]] auto validate(const AVValue& value) const -> AVValidationResult override
    {
        if (value.is_empty())
        {
            return AVValidationResult::ok();
        }
        const auto* num_val = value.as_number();
        if (!num_val)
        {
            return AVValidationResult::error("Expected number value");
        }
        return validate_number(*num_val);
    }

    [[nodiscard]] auto format(const AVValue& value, const AVKey& key) const -> std::string override
    {
        const auto* num_val = value.as_number();
        if (!num_val || !num_val->is_not_empty)
        {
            return {};
        }
        return format_number(num_val->content, key.number_format, key.custom_number_format);
    }

    [[nodiscard]] auto compare(const AVValue& val_a, const AVValue& val_b) const -> int override
    {
        const auto* num_a = val_a.as_number();
        const auto* num_b = val_b.as_number();
        const bool a_valid = num_a && num_a->is_not_empty;
        const bool b_valid = num_b && num_b->is_not_empty;
        if (!a_valid && !b_valid)
        {
            return 0;
        }
        if (!a_valid)
        {
            return -1;
        }
        if (!b_valid)
        {
            return 1;
        }
        if (num_a->content < num_b->content)
        {
            return -1;
        }
        if (num_a->content > num_b->content)
        {
            return 1;
        }
        return 0;
    }

    [[nodiscard]] auto filter(const AVValue& value,
                              AVFilterOperator op_code,
                              const AVValue& operand) const -> bool override
    {
        const auto* num_val = value.as_number();
        const auto* op_num = operand.as_number();
        if (op_code == AVFilterOperator::IsEmpty)
        {
            return !num_val || !num_val->is_not_empty;
        }
        if (op_code == AVFilterOperator::IsNotEmpty)
        {
            return num_val && num_val->is_not_empty;
        }
        if (!num_val || !num_val->is_not_empty || !op_num)
        {
            return false;
        }
        switch (op_code)
        {
            case AVFilterOperator::NumEqual:
                return num_val->content == op_num->content;
            case AVFilterOperator::NumNotEqual:
                return num_val->content != op_num->content;
            case AVFilterOperator::GreaterThan:
                return num_val->content > op_num->content;
            case AVFilterOperator::GreaterThanOrEqual:
                return num_val->content >= op_num->content;
            case AVFilterOperator::LessThan:
                return num_val->content < op_num->content;
            case AVFilterOperator::LessThanOrEqual:
                return num_val->content <= op_num->content;
            default:
                return false;
        }
    }

    [[nodiscard]] auto supported_operators() const -> std::vector<AVFilterOperator> override
    {
        return {AVFilterOperator::IsEmpty,
                AVFilterOperator::IsNotEmpty,
                AVFilterOperator::NumEqual,
                AVFilterOperator::NumNotEqual,
                AVFilterOperator::GreaterThan,
                AVFilterOperator::LessThan};
    }

    [[nodiscard]] auto parse_input(const std::string& input, const AVKey& key) const
        -> std::expected<AVValueVariant, std::string> override
    {
        try
        {
            const double val = std::stod(input);
            return AVValueVariant{AVValueNumber{val, true, key.number_format, {}}};
        }
        catch (const std::exception& exc)
        {
            return std::unexpected(std::string("Invalid number: ") + exc.what());
        }
    }

    [[nodiscard]] auto default_value() const -> AVValueVariant override
    {
        return AVValueVariant{AVValueNumber{}};
    }
};

// ════════════════════════════════════════════════════════════
// Checkbox Column Handler
// ════════════════════════════════════════════════════════════

class CheckboxColumnHandler final : public IAVColumnTypeHandler
{
public:
    [[nodiscard]] auto key_type() const -> AVKeyType override
    {
        return AVKeyType::Checkbox;
    }

    [[nodiscard]] auto validate(const AVValue& value) const -> AVValidationResult override
    {
        if (value.is_empty())
        {
            return AVValidationResult::ok();
        }
        if (!value.as_checkbox())
        {
            return AVValidationResult::error("Expected checkbox value");
        }
        return AVValidationResult::ok();
    }

    [[nodiscard]] auto format(const AVValue& value, const AVKey& /*key*/) const
        -> std::string override
    {
        const auto* cb_val = value.as_checkbox();
        return (cb_val && cb_val->checked) ? "✓" : "";
    }

    [[nodiscard]] auto compare(const AVValue& val_a, const AVValue& val_b) const -> int override
    {
        const auto* cb_a = val_a.as_checkbox();
        const auto* cb_b = val_b.as_checkbox();
        const int int_a = (cb_a && cb_a->checked) ? 1 : 0;
        const int int_b = (cb_b && cb_b->checked) ? 1 : 0;
        return int_a - int_b;
    }

    [[nodiscard]] auto filter(const AVValue& value,
                              AVFilterOperator op_code,
                              const AVValue& /*operand*/) const -> bool override
    {
        const auto* cb_val = value.as_checkbox();
        const bool checked = cb_val && cb_val->checked;
        switch (op_code)
        {
            case AVFilterOperator::IsTrue:
                return checked;
            case AVFilterOperator::IsFalse:
                return !checked;
            default:
                return false;
        }
    }

    [[nodiscard]] auto supported_operators() const -> std::vector<AVFilterOperator> override
    {
        return {AVFilterOperator::IsTrue, AVFilterOperator::IsFalse};
    }

    [[nodiscard]] auto parse_input(const std::string& input, const AVKey& /*key*/) const
        -> std::expected<AVValueVariant, std::string> override
    {
        const bool checked = (input == "true" || input == "1" || input == "yes");
        return AVValueVariant{AVValueCheckbox{checked}};
    }

    [[nodiscard]] auto default_value() const -> AVValueVariant override
    {
        return AVValueVariant{AVValueCheckbox{false}};
    }
};

// ════════════════════════════════════════════════════════════
// Select Column Handler
// ════════════════════════════════════════════════════════════

class SelectColumnHandler final : public IAVColumnTypeHandler
{
public:
    [[nodiscard]] auto key_type() const -> AVKeyType override
    {
        return AVKeyType::Select;
    }

    [[nodiscard]] auto validate(const AVValue& value) const -> AVValidationResult override
    {
        if (value.is_empty())
        {
            return AVValidationResult::ok();
        }
        if (!value.as_select())
        {
            return AVValidationResult::error("Expected select value");
        }
        return AVValidationResult::ok();
    }

    [[nodiscard]] auto format(const AVValue& value, const AVKey& /*key*/) const
        -> std::string override
    {
        const auto* sel_val = value.as_select();
        return sel_val ? sel_val->option.name : std::string{};
    }

    [[nodiscard]] auto compare(const AVValue& val_a, const AVValue& val_b) const -> int override
    {
        return val_a.to_display_string().compare(val_b.to_display_string());
    }

    [[nodiscard]] auto filter(const AVValue& value,
                              AVFilterOperator op_code,
                              const AVValue& operand) const -> bool override
    {
        const auto content = value.to_display_string();
        const auto target = operand.to_display_string();
        switch (op_code)
        {
            case AVFilterOperator::IsEmpty:
                return content.empty();
            case AVFilterOperator::IsNotEmpty:
                return !content.empty();
            case AVFilterOperator::SelectIs:
                return content == target;
            case AVFilterOperator::SelectIsNot:
                return content != target;
            default:
                return false;
        }
    }

    [[nodiscard]] auto supported_operators() const -> std::vector<AVFilterOperator> override
    {
        return {AVFilterOperator::IsEmpty,
                AVFilterOperator::IsNotEmpty,
                AVFilterOperator::SelectIs,
                AVFilterOperator::SelectIsNot};
    }

    [[nodiscard]] auto parse_input(const std::string& input, const AVKey& key) const
        -> std::expected<AVValueVariant, std::string> override
    {
        for (const auto& opt : key.options)
        {
            if (opt.name == input)
            {
                return AVValueVariant{AVValueSelect{opt}};
            }
        }
        return std::unexpected("Option not found: " + input);
    }

    [[nodiscard]] auto default_value() const -> AVValueVariant override
    {
        return AVValueVariant{AVValueSelect{}};
    }
};

// ════════════════════════════════════════════════════════════
// Passthrough handler for types that use display string as-is
// ════════════════════════════════════════════════════════════

class PassthroughColumnHandler final : public IAVColumnTypeHandler
{
    AVKeyType type_;

public:
    explicit PassthroughColumnHandler(AVKeyType type)
        : type_(type)
    {
    }

    [[nodiscard]] auto key_type() const -> AVKeyType override
    {
        return type_;
    }
    [[nodiscard]] auto validate(const AVValue& /*value*/) const -> AVValidationResult override
    {
        return AVValidationResult::ok();
    }
    [[nodiscard]] auto format(const AVValue& value, const AVKey& /*key*/) const
        -> std::string override
    {
        return value.to_display_string();
    }

    [[nodiscard]] auto compare(const AVValue& val_a, const AVValue& val_b) const -> int override
    {
        return val_a.to_display_string().compare(val_b.to_display_string());
    }

    [[nodiscard]] auto filter(const AVValue& value,
                              AVFilterOperator op_code,
                              const AVValue& operand) const -> bool override
    {
        const auto content = value.to_display_string();
        const auto target = operand.to_display_string();
        switch (op_code)
        {
            case AVFilterOperator::IsEmpty:
                return content.empty();
            case AVFilterOperator::IsNotEmpty:
                return !content.empty();
            case AVFilterOperator::Contains:
                return content.find(target) != std::string::npos;
            default:
                return false;
        }
    }

    [[nodiscard]] auto supported_operators() const -> std::vector<AVFilterOperator> override
    {
        return {
            AVFilterOperator::IsEmpty, AVFilterOperator::IsNotEmpty, AVFilterOperator::Contains};
    }

    [[nodiscard]] auto parse_input(const std::string& input, const AVKey& /*key*/) const
        -> std::expected<AVValueVariant, std::string> override
    {
        return AVValueVariant{AVValueText{input}};
    }

    [[nodiscard]] auto default_value() const -> AVValueVariant override
    {
        return AVValueVariant{std::monostate{}};
    }
};

// ════════════════════════════════════════════════════════════
// Registry constructor — registers all 17 handlers
// ════════════════════════════════════════════════════════════

void AVColumnTypeRegistry::register_all_handlers()
{
    handlers_[AVKeyType::Text] = std::make_unique<TextColumnHandler>();
    handlers_[AVKeyType::Number] = std::make_unique<NumberColumnHandler>();
    handlers_[AVKeyType::Checkbox] = std::make_unique<CheckboxColumnHandler>();
    handlers_[AVKeyType::Select] = std::make_unique<SelectColumnHandler>();

    // Remaining types use passthrough handler
    const AVKeyType passthrough_types[] = {AVKeyType::Block,
                                           AVKeyType::Date,
                                           AVKeyType::MSelect,
                                           AVKeyType::URL,
                                           AVKeyType::Email,
                                           AVKeyType::Phone,
                                           AVKeyType::MAsset,
                                           AVKeyType::Template,
                                           AVKeyType::Created,
                                           AVKeyType::Updated,
                                           AVKeyType::Relation,
                                           AVKeyType::Rollup,
                                           AVKeyType::LineNumber};
    for (const auto ptype : passthrough_types)
    {
        handlers_[ptype] = std::make_unique<PassthroughColumnHandler>(ptype);
    }
}

AVColumnTypeRegistry::AVColumnTypeRegistry()
{
    register_all_handlers();
}

auto AVColumnTypeRegistry::get_handler(AVKeyType type) const -> const IAVColumnTypeHandler*
{
    const auto iter = handlers_.find(type);
    return iter != handlers_.end() ? iter->second.get() : nullptr;
}

auto AVColumnTypeRegistry::format_value(const AVValue& value, const AVKey& key) const -> std::string
{
    const auto* handler = get_handler(key.type);
    return handler ? handler->format(value, key) : value.to_display_string();
}

auto AVColumnTypeRegistry::compare_values(const AVValue& val_a,
                                          const AVValue& val_b,
                                          AVKeyType type) const -> int
{
    const auto* handler = get_handler(type);
    return handler ? handler->compare(val_a, val_b) : 0;
}

auto AVColumnTypeRegistry::validate_value(const AVValue& value, const AVKey& key) const
    -> AVValidationResult
{
    const auto* handler = get_handler(key.type);
    return handler ? handler->validate(value) : AVValidationResult::ok();
}

auto AVColumnTypeRegistry::evaluate_filter(const AVValue& value,
                                           AVFilterOperator op_code,
                                           const AVValue& operand,
                                           AVKeyType type) const -> bool
{
    const auto* handler = get_handler(type);
    return handler ? handler->filter(value, op_code, operand) : false;
}

} // namespace markamp::core::av
