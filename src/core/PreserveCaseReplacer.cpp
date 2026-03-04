#include "PreserveCaseReplacer.h"

#include <algorithm>
#include <cctype>

namespace markamp::core
{

auto detect_case_pattern(const std::string& text) -> CasePattern
{
    if (text.empty())
    {
        return CasePattern::kMixed;
    }

    bool all_upper = true;
    bool all_lower = true;
    bool has_upper = false;
    bool has_lower = false;

    for (std::size_t idx = 0; idx < text.size(); ++idx)
    {
        const auto chr = text[idx];
        if (std::isalpha(static_cast<unsigned char>(chr)) != 0)
        {
            if (std::isupper(static_cast<unsigned char>(chr)) != 0)
            {
                has_upper = true;
                all_lower = false;
            }
            else
            {
                has_lower = true;
                all_upper = false;
            }
        }
    }

    if (!has_upper && !has_lower)
    {
        return CasePattern::kMixed; // No alpha chars
    }

    if (all_upper)
    {
        return CasePattern::kAllUpper;
    }
    if (all_lower)
    {
        return CasePattern::kAllLower;
    }

    // Check title case: first letter upper, rest lower
    if (std::isupper(static_cast<unsigned char>(text[0])) != 0)
    {
        bool rest_lower = true;
        bool has_internal_upper = false;
        for (std::size_t idx = 1; idx < text.size(); ++idx)
        {
            if (std::isalpha(static_cast<unsigned char>(text[idx])) != 0)
            {
                if (std::isupper(static_cast<unsigned char>(text[idx])) != 0)
                {
                    rest_lower = false;
                    has_internal_upper = true;
                }
            }
        }
        if (rest_lower)
        {
            return CasePattern::kTitleCase;
        }
        if (has_internal_upper)
        {
            return CasePattern::kPascalCase;
        }
    }

    // Check camelCase: first letter lower, has internal uppercase
    if (std::islower(static_cast<unsigned char>(text[0])) != 0 && has_upper)
    {
        return CasePattern::kCamelCase;
    }

    return CasePattern::kMixed;
}

auto apply_case_pattern(const std::string& text, CasePattern pattern) -> std::string
{
    if (text.empty())
    {
        return text;
    }

    std::string result = text;

    switch (pattern)
    {
        case CasePattern::kAllUpper:
            std::transform(result.begin(),
                           result.end(),
                           result.begin(),
                           [](unsigned char chr) { return std::toupper(chr); });
            break;

        case CasePattern::kAllLower:
            std::transform(result.begin(),
                           result.end(),
                           result.begin(),
                           [](unsigned char chr) { return std::tolower(chr); });
            break;

        case CasePattern::kTitleCase:
            std::transform(result.begin(),
                           result.end(),
                           result.begin(),
                           [](unsigned char chr) { return std::tolower(chr); });
            if (!result.empty() && std::isalpha(static_cast<unsigned char>(result[0])) != 0)
            {
                result[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[0])));
            }
            break;

        case CasePattern::kCamelCase:
            // Lowercase the first letter, keep the rest as-is
            if (std::isalpha(static_cast<unsigned char>(result[0])) != 0)
            {
                result[0] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[0])));
            }
            break;

        case CasePattern::kPascalCase:
            // Uppercase the first letter, keep the rest as-is
            if (std::isalpha(static_cast<unsigned char>(result[0])) != 0)
            {
                result[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[0])));
            }
            break;

        case CasePattern::kMixed:
            // No transformation for mixed case
            break;
    }

    return result;
}

auto preserve_case_replace(const std::string& matched_text, const std::string& replacement)
    -> std::string
{
    if (matched_text.empty() || replacement.empty())
    {
        return replacement;
    }

    auto pattern = detect_case_pattern(matched_text);
    return apply_case_pattern(replacement, pattern);
}

} // namespace markamp::core
