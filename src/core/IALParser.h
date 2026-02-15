#pragma once

#include "core/Block.h"

#include <optional>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

namespace markamp::core
{

// Result of diffing two IALs.
struct IALDiff
{
    // Keys that were added (present in new, absent in old).
    std::vector<std::pair<std::string, std::string>> added;

    // Keys that were removed (present in old, absent in new).
    std::vector<std::string> removed;

    // Keys that were changed (present in both, different values).
    // Each entry is {key, old_value, new_value}.
    std::vector<std::tuple<std::string, std::string, std::string>> changed;

    // Returns true if there are no differences.
    [[nodiscard]] auto is_empty() const -> bool;
};

// Pure utility class for parsing and serializing IAL strings.
// Stateless — all methods are static. No dependencies.
//
// IAL format: {: key1="value1" key2="value2" }
// Keys: [a-z][a-z0-9_-]*
// Values: double-quoted, with backslash escaping for quotes and backslashes
class IALParser
{
public:
    // Parse an IAL string into a key-value map.
    // Input: `{: id="20260214120000-abc" name="My Block" }`
    // Output: {"id": "20260214120000-abc", "name": "My Block"}
    // Returns empty map if input is empty or malformed.
    [[nodiscard]] static auto parse(const std::string& ial_string) -> InlineAttributeList;

    // Serialize a key-value map to an IAL string.
    // Keys are sorted alphabetically. Values are escaped.
    // Returns empty string if the map is empty.
    [[nodiscard]] static auto serialize(const InlineAttributeList& attrs) -> std::string;

    // Merge two IALs. Values from overlay take precedence over base.
    // Keys only in base are preserved. Keys only in overlay are added.
    [[nodiscard]] static auto merge(const InlineAttributeList& base,
                                    const InlineAttributeList& overlay) -> InlineAttributeList;

    // Compute the difference between two IALs.
    [[nodiscard]] static auto diff(const InlineAttributeList& old_attrs,
                                   const InlineAttributeList& new_attrs) -> IALDiff;

    // Validate an IAL string for correct syntax.
    // Returns true if the string is a valid IAL (or empty).
    [[nodiscard]] static auto is_valid(const std::string& ial_string) -> bool;

    // Extract a single attribute value from an IAL string without full parsing.
    // Optimization for when you only need one value.
    [[nodiscard]] static auto extract_value(const std::string& ial_string, const std::string& key)
        -> std::optional<std::string>;

    // Check if a key name is a built-in attribute (id, name, alias, memo, etc.).
    [[nodiscard]] static auto is_builtin_key(const std::string& key) -> bool;

    // Check if a key name is a user-defined attribute (starts with "custom-").
    [[nodiscard]] static auto is_custom_key(const std::string& key) -> bool;

    // Validate a key name. Must match: [a-z][a-z0-9_-]*
    [[nodiscard]] static auto is_valid_key(const std::string& key) -> bool;

    // Escape a value string for IAL serialization.
    [[nodiscard]] static auto escape_value(const std::string& value) -> std::string;

    // Unescape a value string from IAL parsing.
    [[nodiscard]] static auto unescape_value(const std::string& value) -> std::string;

private:
    static const std::unordered_set<std::string>
        BUILTIN_KEYS; // NOLINT(readability-identifier-naming)
};

} // namespace markamp::core
