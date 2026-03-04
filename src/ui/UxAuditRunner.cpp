#include "UxAuditRunner.h"

namespace markamp::ui
{

auto AuditCheckResult::category_name() const -> std::string
{
    switch (category)
    {
        case AuditCategory::kInteraction:
            return "interaction";
        case AuditCategory::kStyle:
            return "style";
        case AuditCategory::kAccessibility:
            return "accessibility";
        case AuditCategory::kCrossTheme:
            return "cross_theme";
        case AuditCategory::kCrossDpi:
            return "cross_dpi";
        case AuditCategory::kPlatform:
            return "platform";
    }
    return "unknown";
}

void UxAuditRunner::add_check(const std::string& check_id,
                              const std::string& description,
                              AuditCategory category,
                              std::function<bool()> check)
{
    AuditCheck audit;
    audit.check_id = check_id;
    audit.description = description;
    audit.category = category;
    audit.check = std::move(check);
    checks_.push_back(std::move(audit));
}

auto UxAuditRunner::check_count() const -> int
{
    return static_cast<int>(checks_.size());
}

auto UxAuditRunner::checks_in_category(AuditCategory category) const -> int
{
    int count = 0;
    for (const auto& check : checks_)
    {
        if (check.category == category)
        {
            ++count;
        }
    }
    return count;
}

auto UxAuditRunner::run() -> std::vector<AuditCheckResult>
{
    std::vector<AuditCheckResult> results;
    for (const auto& check : checks_)
    {
        AuditCheckResult result;
        result.check_id = check.check_id;
        result.description = check.description;
        result.category = check.category;
        result.passed = check.check && check.check();
        if (!result.passed)
        {
            result.failure_reason = "Check failed: " + check.description;
        }
        results.push_back(result);
    }
    return results;
}

auto UxAuditRunner::run_category(AuditCategory category) -> std::vector<AuditCheckResult>
{
    std::vector<AuditCheckResult> results;
    for (const auto& check : checks_)
    {
        if (check.category == category)
        {
            AuditCheckResult result;
            result.check_id = check.check_id;
            result.description = check.description;
            result.category = check.category;
            result.passed = check.check && check.check();
            if (!result.passed)
            {
                result.failure_reason = "Check failed: " + check.description;
            }
            results.push_back(result);
        }
    }
    return results;
}

auto UxAuditRunner::pass_rate(const std::vector<AuditCheckResult>& results) const -> double
{
    if (results.empty())
    {
        return 1.0;
    }
    int passed = 0;
    for (const auto& result : results)
    {
        if (result.passed)
        {
            ++passed;
        }
    }
    return static_cast<double>(passed) / static_cast<double>(results.size());
}

auto UxAuditRunner::failure_descriptions(const std::vector<AuditCheckResult>& results) const
    -> std::vector<std::string>
{
    std::vector<std::string> failures;
    for (const auto& result : results)
    {
        if (!result.passed)
        {
            failures.push_back(result.failure_reason);
        }
    }
    return failures;
}

} // namespace markamp::ui
