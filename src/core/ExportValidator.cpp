// ============================================================================
// File: src/core/ExportValidator.cpp
// Phase 24: Export & Publishing — Pre-export content validation
// ============================================================================

#include "ExportValidator.h"

#include <algorithm>
#include <regex>
#include <sstream>
#include <unordered_map>

namespace markamp::core
{

// ============================================================================
// Construction
// ============================================================================

ExportValidator::ExportValidator() = default;

// ============================================================================
// Core API
// ============================================================================

auto ExportValidator::validate(const std::string& markdown_content,
                               ExportFormat target_format) const -> std::vector<ValidationIssue>
{
    std::vector<ValidationIssue> issues;

    // Run all checks.
    auto link_issues = check_links(markdown_content);
    auto asset_issues = check_assets(markdown_content);
    auto frontmatter_issues = check_frontmatter(markdown_content);
    auto compat_issues = check_format_compatibility(markdown_content, target_format);
    auto size_issues = check_size(markdown_content);
    auto heading_issues = check_duplicate_headings(markdown_content);

    issues.insert(issues.end(), link_issues.begin(), link_issues.end());
    issues.insert(issues.end(), asset_issues.begin(), asset_issues.end());
    issues.insert(issues.end(), frontmatter_issues.begin(), frontmatter_issues.end());
    issues.insert(issues.end(), compat_issues.begin(), compat_issues.end());
    issues.insert(issues.end(), size_issues.begin(), size_issues.end());
    issues.insert(issues.end(), heading_issues.begin(), heading_issues.end());

    return issues;
}

auto ExportValidator::summary(const std::vector<ValidationIssue>& issues) -> ValidationSummary
{
    ValidationSummary sum;
    for (const auto& issue : issues)
    {
        switch (issue.severity)
        {
            case ValidationSeverity::kError:
                ++sum.errors;
                break;
            case ValidationSeverity::kWarning:
                ++sum.warnings;
                break;
            case ValidationSeverity::kInfo:
                ++sum.infos;
                break;
        }
    }
    return sum;
}

// ============================================================================
// Individual checks
// ============================================================================

auto ExportValidator::check_links(const std::string& content) const -> std::vector<ValidationIssue>
{
    std::vector<ValidationIssue> issues;

    // Check for wiki-style links [[target]] where target is empty.
    auto targets = extract_link_targets(content);
    for (const auto& [target, line] : targets)
    {
        if (target.empty())
        {
            issues.push_back({ValidationSeverity::kError,
                              ValidationCategory::kBrokenLink,
                              "Empty link target",
                              line});
        }
        // We can't verify external URLs without network access, but we can
        // flag obviously malformed ones.
        if (target.find(' ') != std::string::npos && target.find("://") == std::string::npos)
        {
            issues.push_back({ValidationSeverity::kWarning,
                              ValidationCategory::kBrokenLink,
                              "Link target contains spaces: " + target,
                              line});
        }
    }

    return issues;
}

auto ExportValidator::check_assets(const std::string& content) const -> std::vector<ValidationIssue>
{
    std::vector<ValidationIssue> issues;

    auto images = extract_image_paths(content);
    for (const auto& [path, line] : images)
    {
        if (path.empty())
        {
            issues.push_back({ValidationSeverity::kError,
                              ValidationCategory::kMissingAsset,
                              "Empty image path",
                              line});
            continue;
        }

        // Skip URLs (http/https/data).
        if (path.find("://") != std::string::npos || path.substr(0, 5) == "data:")
        {
            continue;
        }

        // For local paths, we flag them as warnings since we can't always
        // verify from the validator context. A real deployment would check
        // the filesystem.
        if (!base_directory_.empty())
        {
            // If we have a base directory, note potential missing assets.
            issues.push_back({ValidationSeverity::kInfo,
                              ValidationCategory::kMissingAsset,
                              "Local asset reference: " + path,
                              line});
        }
    }

    return issues;
}

auto ExportValidator::check_frontmatter(const std::string& content) -> std::vector<ValidationIssue>
{
    std::vector<ValidationIssue> issues;

    // Check if content starts with YAML frontmatter (---).
    if (content.empty())
    {
        issues.push_back({ValidationSeverity::kWarning,
                          ValidationCategory::kEmptyContent,
                          "Document is empty",
                          0});
        return issues;
    }

    // Check for frontmatter block.
    if (content.substr(0, 3) == "---")
    {
        auto end_pos = content.find("---", 3);
        if (end_pos == std::string::npos)
        {
            issues.push_back({ValidationSeverity::kError,
                              ValidationCategory::kInvalidFrontmatter,
                              "Frontmatter block is not closed (missing closing ---)",
                              1});
        }
        else
        {
            // Basic structure check: should contain key: value pairs.
            auto frontmatter = content.substr(3, end_pos - 3);
            if (frontmatter.find(':') == std::string::npos)
            {
                issues.push_back({ValidationSeverity::kWarning,
                                  ValidationCategory::kInvalidFrontmatter,
                                  "Frontmatter contains no key-value pairs",
                                  1});
            }
        }
    }

    return issues;
}

auto ExportValidator::check_format_compatibility(const std::string& content, ExportFormat format)
    -> std::vector<ValidationIssue>
{
    std::vector<ValidationIssue> issues;

    // LaTeX: warn about HTML tags in content.
    if (format == ExportFormat::LaTeX)
    {
        static const std::regex kHtmlTag(R"(<[a-zA-Z][^>]*>)");
        if (std::regex_search(content, kHtmlTag))
        {
            issues.push_back({ValidationSeverity::kWarning,
                              ValidationCategory::kUnsupportedSyntax,
                              "Content contains HTML tags which may not render correctly in LaTeX",
                              0});
        }
    }

    // PlainText: warn about images/formatting that will be stripped.
    if (format == ExportFormat::PlainText)
    {
        static const std::regex kImage(R"(!\[)");
        if (std::regex_search(content, kImage))
        {
            issues.push_back({ValidationSeverity::kInfo,
                              ValidationCategory::kUnsupportedSyntax,
                              "Images will be removed in plain text export",
                              0});
        }
    }

    // PDF: warn about very wide tables.
    if (format == ExportFormat::PDF)
    {
        // Count max pipe characters on a single line as a table width proxy.
        std::istringstream stream(content);
        std::string line;
        int line_num = 0;
        while (std::getline(stream, line))
        {
            ++line_num;
            auto pipe_count = std::count(line.begin(), line.end(), '|');
            if (pipe_count > 8) // More than 8 columns is likely to overflow.
            {
                issues.push_back({ValidationSeverity::kWarning,
                                  ValidationCategory::kUnsupportedSyntax,
                                  "Wide table may overflow page margins in PDF export",
                                  line_num});
                break; // Only warn once.
            }
        }
    }

    return issues;
}

auto ExportValidator::check_size(const std::string& content) -> std::vector<ValidationIssue>
{
    std::vector<ValidationIssue> issues;

    auto size = static_cast<int64_t>(content.size());
    if (size > 1024LL * 1024LL) // > 1MB.
    {
        issues.push_back(
            {ValidationSeverity::kWarning,
             ValidationCategory::kLargeFile,
             "Content is " + std::to_string(size / 1024) + " KB, which may be slow to export",
             0});
    }

    return issues;
}

auto ExportValidator::check_duplicate_headings(const std::string& content)
    -> std::vector<ValidationIssue>
{
    std::vector<ValidationIssue> issues;

    // Extract heading text and detect duplicates.
    static const std::regex kHeading(R"(^(#{1,6})\s+(.+)$)", std::regex::multiline);

    std::unordered_map<std::string, int> heading_counts;
    auto end = std::sregex_iterator();

    // First pass: count occurrences.
    struct HeadingEntry
    {
        std::string heading_text;
        int heading_line;
    };
    std::vector<HeadingEntry> headings;

    std::istringstream stream(content);
    std::string line;
    int line_num = 0;
    while (std::getline(stream, line))
    {
        ++line_num;
        std::smatch match;
        if (std::regex_match(line, match, kHeading))
        {
            const std::string heading_text = match[2].str();
            heading_counts[heading_text]++;
            headings.push_back({heading_text, line_num});
        }
    }

    // Report duplicates.
    for (const auto& [entry_text, entry_line] : headings)
    {
        if (heading_counts[entry_text] > 1)
        {
            issues.push_back({ValidationSeverity::kInfo,
                              ValidationCategory::kDuplicateHeading,
                              "Duplicate heading: " + entry_text,
                              entry_line});
        }
    }

    return issues;
}

// ============================================================================
// Configuration
// ============================================================================

auto ExportValidator::set_base_directory(const std::string& dir) -> void
{
    base_directory_ = dir;
}

auto ExportValidator::set_max_size(int64_t max_bytes) -> void
{
    max_size_ = max_bytes;
}

// ============================================================================
// Helpers
// ============================================================================

auto ExportValidator::extract_image_paths(const std::string& content)
    -> std::vector<std::pair<std::string, int>>
{
    std::vector<std::pair<std::string, int>> results;

    // Match ![alt](path) patterns.
    static const std::regex kImage(R"(!\[[^\]]*\]\(([^)]*)\))");

    std::istringstream stream(content);
    std::string line;
    int line_num = 0;
    while (std::getline(stream, line))
    {
        ++line_num;
        auto begin = std::sregex_iterator(line.begin(), line.end(), kImage);
        auto end = std::sregex_iterator();
        for (auto iter = begin; iter != end; ++iter)
        {
            results.emplace_back((*iter)[1].str(), line_num);
        }
    }

    return results;
}

auto ExportValidator::extract_link_targets(const std::string& content)
    -> std::vector<std::pair<std::string, int>>
{
    std::vector<std::pair<std::string, int>> results;

    // Match [text](target) patterns.  We manually exclude image links
    // (which start with '!') since std::regex does not support lookbehind.
    static const std::regex kLink(R"(\[[^\]]*\]\(([^)]*)\))");

    std::istringstream stream(content);
    std::string line;
    int line_num = 0;
    while (std::getline(stream, line))
    {
        ++line_num;
        auto begin = std::sregex_iterator(line.begin(), line.end(), kLink);
        auto end = std::sregex_iterator();
        for (auto iter = begin; iter != end; ++iter)
        {
            auto match_pos = (*iter).position();
            // Skip image links: if the character before '[' is '!', skip.
            if (match_pos > 0 && line[static_cast<size_t>(match_pos) - 1] == '!')
            {
                continue;
            }
            results.emplace_back((*iter)[1].str(), line_num);
        }
    }

    return results;
}

// (#100) Return the number of available validation checks.
auto ExportValidator::check_count() -> std::size_t
{
    // links, assets, frontmatter, format_compatibility, size, duplicate_headings
    return 6;
}

} // namespace markamp::core
