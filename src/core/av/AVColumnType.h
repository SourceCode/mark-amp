#pragma once

#include "AVTypes.h"
#include "AVValue.h"

#include <expected>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Filter Operator Enumeration
// ════════════════════════════════════════════════════════════

enum class AVFilterOperator : std::uint8_t
{
    IsEmpty,
    IsNotEmpty,
    IsEqual,
    IsNotEqual,
    Contains,
    DoesNotContain,
    StartsWith,
    EndsWith,
    NumEqual,
    NumNotEqual,
    GreaterThan,
    GreaterThanOrEqual,
    LessThan,
    LessThanOrEqual,
    DateEqual,
    DateBefore,
    DateAfter,
    DateOnOrBefore,
    DateOnOrAfter,
    DateBetween,
    DateIsRelative,
    SelectIs,
    SelectIsNot,
    IsTrue,
    IsFalse,
    RelationContains,
    RelationDoesNotContain
};

[[nodiscard]] auto filter_operator_to_string(AVFilterOperator op_code) -> std::string;
[[nodiscard]] auto string_to_filter_operator(const std::string& str) -> AVFilterOperator;

// ════════════════════════════════════════════════════════════
// Sort Order
// ════════════════════════════════════════════════════════════

enum class AVSortOrder : std::uint8_t
{
    Ascending,
    Descending
};

// ════════════════════════════════════════════════════════════
// Validation Result
// ════════════════════════════════════════════════════════════

struct AVValidationResult
{
    bool valid{true};
    std::string error_message;

    [[nodiscard]] static auto ok() -> AVValidationResult
    {
        return {true, {}};
    }
    [[nodiscard]] static auto error(std::string msg) -> AVValidationResult
    {
        return {false, std::move(msg)};
    }
};

// ════════════════════════════════════════════════════════════
// Column Type Handler Interface
// ════════════════════════════════════════════════════════════

class IAVColumnTypeHandler
{
public:
    virtual ~IAVColumnTypeHandler() = default;

    [[nodiscard]] virtual auto key_type() const -> AVKeyType = 0;
    [[nodiscard]] virtual auto validate(const AVValue& value) const -> AVValidationResult = 0;
    [[nodiscard]] virtual auto format(const AVValue& value, const AVKey& key) const
        -> std::string = 0;
    [[nodiscard]] virtual auto compare(const AVValue& val_a, const AVValue& val_b) const -> int = 0;
    [[nodiscard]] virtual auto filter(const AVValue& value,
                                      AVFilterOperator op_code,
                                      const AVValue& operand) const -> bool = 0;
    [[nodiscard]] virtual auto supported_operators() const -> std::vector<AVFilterOperator> = 0;
    [[nodiscard]] virtual auto parse_input(const std::string& input, const AVKey& key) const
        -> std::expected<AVValueVariant, std::string> = 0;
    [[nodiscard]] virtual auto default_value() const -> AVValueVariant = 0;
};

// ════════════════════════════════════════════════════════════
// Column Type Registry
// ════════════════════════════════════════════════════════════

class AVColumnTypeRegistry
{
public:
    AVColumnTypeRegistry();
    ~AVColumnTypeRegistry() = default;

    AVColumnTypeRegistry(const AVColumnTypeRegistry&) = delete;
    auto operator=(const AVColumnTypeRegistry&) -> AVColumnTypeRegistry& = delete;
    AVColumnTypeRegistry(AVColumnTypeRegistry&&) noexcept = default;
    auto operator=(AVColumnTypeRegistry&&) noexcept -> AVColumnTypeRegistry& = default;

    [[nodiscard]] auto get_handler(AVKeyType type) const -> const IAVColumnTypeHandler*;
    [[nodiscard]] auto format_value(const AVValue& value, const AVKey& key) const -> std::string;
    [[nodiscard]] auto
    compare_values(const AVValue& val_a, const AVValue& val_b, AVKeyType type) const -> int;
    [[nodiscard]] auto validate_value(const AVValue& value, const AVKey& key) const
        -> AVValidationResult;
    [[nodiscard]] auto evaluate_filter(const AVValue& value,
                                       AVFilterOperator op_code,
                                       const AVValue& operand,
                                       AVKeyType type) const -> bool;

private:
    std::unordered_map<AVKeyType, std::unique_ptr<IAVColumnTypeHandler>> handlers_;
    void register_all_handlers();
};

} // namespace markamp::core::av
