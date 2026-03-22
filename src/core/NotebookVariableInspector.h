/// @file NotebookVariableInspector.h
/// @brief V24 P08-T04: Variable inspection data model for notebook kernel state.
///
/// Tracks variables exposed by a running kernel session. Provides a
/// queryable model for the variable inspector panel.
#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Type classification for inspected variables.
enum class VariableType
{
    kInteger,
    kFloat,
    kString,
    kBoolean,
    kList,
    kMap,
    kObject,
    kFunction,
    kNull,
    kUnknown
};

/// Returns a label for a VariableType.
[[nodiscard]] constexpr auto variable_type_label(VariableType t) noexcept -> const char*
{
    switch (t) {
        case VariableType::kInteger:  return "int";
        case VariableType::kFloat:    return "float";
        case VariableType::kString:   return "str";
        case VariableType::kBoolean:  return "bool";
        case VariableType::kList:     return "list";
        case VariableType::kMap:      return "map";
        case VariableType::kObject:   return "object";
        case VariableType::kFunction: return "function";
        case VariableType::kNull:     return "null";
        case VariableType::kUnknown:  return "unknown";
    }
    return "unknown";
}

/// A single variable entry in the inspector.
struct InspectedVariable
{
    std::string name;
    VariableType type{VariableType::kUnknown};
    std::string value_repr;          ///< String representation of value
    std::optional<size_t> size;      ///< Collection size if applicable
    std::string scope;               ///< "global", "local", "cell"
    int cell_index{-1};              ///< Cell that defined/last modified this var
    bool is_stale{false};            ///< Whether the variable may be outdated
};

/// Variable inspector data model.
class NotebookVariableInspector
{
public:
    NotebookVariableInspector() = default;

    /// Set/update a variable.
    void set_variable(InspectedVariable var);

    /// Remove a variable.
    auto remove_variable(const std::string& name) -> bool;

    /// Get a variable by name.
    [[nodiscard]] auto get_variable(const std::string& name) const
        -> const InspectedVariable*;

    /// Get all variables.
    [[nodiscard]] auto all_variables() const -> std::vector<const InspectedVariable*>;

    /// Get variables by type.
    [[nodiscard]] auto variables_by_type(VariableType type) const
        -> std::vector<const InspectedVariable*>;

    /// Get variables defined in a specific cell.
    [[nodiscard]] auto variables_for_cell(int cell_index) const
        -> std::vector<const InspectedVariable*>;

    /// Mark all variables from a cell as stale.
    void mark_cell_stale(int cell_index);

    /// Total variable count.
    [[nodiscard]] auto variable_count() const noexcept -> int
    {
        return static_cast<int>(variables_.size());
    }

    /// Clear all variables.
    void clear();

private:
    std::unordered_map<std::string, InspectedVariable> variables_;
    std::vector<std::string> insertion_order_;
};

} // namespace markamp::core
