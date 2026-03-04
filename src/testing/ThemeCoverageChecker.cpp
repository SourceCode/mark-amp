// ============================================================================
// File: src/testing/ThemeCoverageChecker.cpp
// Phase 50: UI Integration Testing Harness — Theme coverage checker
// ============================================================================
#include "ThemeCoverageChecker.h"

#include <regex>

namespace markamp::testing
{

auto ThemeCoverageChecker::scan_for_hardcoded_colors(const std::string& content,
                                                     const std::string& file_path) const
    -> std::vector<HardcodedColorFinding>
{
    std::vector<HardcodedColorFinding> results;

    // Match hex colors: #RGB, #RRGGBB, #RRGGBBAA
    static const std::regex hex_pattern(R"(#[0-9a-fA-F]{3,8}\b)", std::regex::optimize);

    // Match rgb/rgba patterns.
    static const std::regex rgb_pattern(R"(rgb\s*\(\s*\d+\s*,\s*\d+\s*,\s*\d+\s*\))",
                                        std::regex::optimize);

    int line_number = 1;
    size_t line_start = 0;

    for (size_t i = 0; i <= content.size(); ++i)
    {
        if (i == content.size() || content[i] == '\n')
        {
            std::string line = content.substr(line_start, i - line_start);

            // Check for hex colors.
            std::sregex_iterator hex_it(line.begin(), line.end(), hex_pattern);
            std::sregex_iterator end;
            for (; hex_it != end; ++hex_it)
            {
                results.push_back({.file_path = file_path,
                                   .line_number = line_number,
                                   .color_value = hex_it->str(),
                                   .context = line});
            }

            // Check for rgb patterns.
            std::sregex_iterator rgb_it(line.begin(), line.end(), rgb_pattern);
            for (; rgb_it != end; ++rgb_it)
            {
                results.push_back({.file_path = file_path,
                                   .line_number = line_number,
                                   .color_value = rgb_it->str(),
                                   .context = line});
            }

            line_start = i + 1;
            ++line_number;
        }
    }

    return results;
}

void ThemeCoverageChecker::record_hardcoded_finding(HardcodedColorFinding finding)
{
    findings_.push_back(std::move(finding));
}

auto ThemeCoverageChecker::generate_report() const -> ThemeCoverageReport
{
    return {.total_color_usages = themed_usages_ + static_cast<int>(findings_.size()),
            .themed_usages = themed_usages_,
            .hardcoded_usages = static_cast<int>(findings_.size()),
            .findings = findings_};
}

} // namespace markamp::testing
