/// StringGuards.cpp — V7 Phase 07: String length guards

#include "StringGuards.h"

namespace markamp::core
{

auto truncate_safe(std::string_view input, size_t max_len) -> std::string
{
    if (input.size() <= max_len)
    {
        return std::string(input);
    }
    if (max_len < 4)
    {
        // Too short for ellipsis
        return std::string(input.substr(0, max_len));
    }
    // Reserve space for "..."
    size_t target = max_len - 3;
    // Walk backward to avoid splitting a multi-byte UTF-8 character.
    // UTF-8 continuation bytes start with 0b10xxxxxx (0x80–0xBF).
    while (target > 0 && (static_cast<unsigned char>(input[target]) & 0xC0) == 0x80)
    {
        --target;
    }
    std::string result;
    result.reserve(target + 3);
    result.append(input.data(), target);
    result.append("...");
    return result;
}

auto apply_length_guard(std::string_view input, size_t max_len, std::string_view /*field_name*/)
    -> std::string
{
    return truncate_safe(input, max_len);
}

} // namespace markamp::core
