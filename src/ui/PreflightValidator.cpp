#include "PreflightValidator.h"

namespace markamp::ui
{

auto PreflightResult::is_blocking() const -> bool
{
    return severity == PreflightSeverity::kError || severity == PreflightSeverity::kBlocker;
}

auto PreflightResult::message() const -> std::string
{
    std::string msg = what_happened;
    if (!why.empty())
    {
        msg += " — " + why;
    }
    if (!what_to_do.empty())
    {
        msg += " → " + what_to_do;
    }
    return msg;
}

auto PreflightResult::severity_name() const -> std::string
{
    switch (severity)
    {
        case PreflightSeverity::kInfo:
            return "info";
        case PreflightSeverity::kWarning:
            return "warning";
        case PreflightSeverity::kError:
            return "error";
        case PreflightSeverity::kBlocker:
            return "blocker";
    }
    return "unknown";
}

void PreflightValidator::register_check(const std::string& action_id,
                                        const std::string& check_id,
                                        PreflightCheck check)
{
    RegisteredCheck entry;
    entry.action_id = action_id;
    entry.check_id = check_id;
    entry.check = std::move(check);
    checks_.push_back(std::move(entry));
}

void PreflightValidator::clear_checks(const std::string& action_id)
{
    std::vector<RegisteredCheck> remaining;
    for (auto& check : checks_)
    {
        if (check.action_id != action_id)
        {
            remaining.push_back(std::move(check));
        }
    }
    checks_ = std::move(remaining);
}

auto PreflightValidator::check_count(const std::string& action_id) const -> int
{
    int count = 0;
    for (const auto& check : checks_)
    {
        if (check.action_id == action_id)
        {
            ++count;
        }
    }
    return count;
}

auto PreflightValidator::validate(const std::string& action_id) const
    -> std::vector<PreflightResult>
{
    std::vector<PreflightResult> results;
    for (const auto& check : checks_)
    {
        if (check.action_id == action_id)
        {
            results.push_back(check.check(action_id));
        }
    }
    return results;
}

auto PreflightValidator::can_proceed(const std::string& action_id) const -> bool
{
    auto results = validate(action_id);
    for (const auto& result : results)
    {
        if (result.is_blocking())
        {
            return false;
        }
    }
    return true;
}

auto PreflightValidator::highest_severity(const std::string& action_id) const -> PreflightSeverity
{
    auto results = validate(action_id);
    auto highest = PreflightSeverity::kInfo;
    for (const auto& result : results)
    {
        if (static_cast<uint8_t>(result.severity) > static_cast<uint8_t>(highest))
        {
            highest = result.severity;
        }
    }
    return highest;
}

} // namespace markamp::ui
