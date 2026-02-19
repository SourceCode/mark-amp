#include "IntegrityModel.h"

#include <algorithm>

namespace markamp::canvas
{

void IntegrityModel::add_issue(ValidationIssue issue)
{
    issues_.push_back(std::move(issue));
}
void IntegrityModel::clear_issues()
{
    issues_.clear();
}
auto IntegrityModel::issues() const -> const std::vector<ValidationIssue>&
{
    return issues_;
}

auto IntegrityModel::error_count() const -> int
{
    return static_cast<int>(std::count_if(issues_.begin(),
                                          issues_.end(),
                                          [](const ValidationIssue& vi)
                                          { return vi.severity == ValidationSeverity::kError; }));
}

auto IntegrityModel::warning_count() const -> int
{
    return static_cast<int>(std::count_if(issues_.begin(),
                                          issues_.end(),
                                          [](const ValidationIssue& vi)
                                          { return vi.severity == ValidationSeverity::kWarning; }));
}

auto IntegrityModel::is_valid() const -> bool
{
    return error_count() == 0;
}

void IntegrityModel::set_precondition_result(bool passed, const std::string& reason)
{
    precondition_passed_ = passed;
    precondition_reason_ = reason;
}

auto IntegrityModel::precondition_passed() const -> bool
{
    return precondition_passed_;
}
auto IntegrityModel::precondition_reason() const -> const std::string&
{
    return precondition_reason_;
}

void IntegrityModel::set_orphan_count(int count)
{
    orphan_count_ = std::max(0, count);
}
auto IntegrityModel::orphan_count() const -> int
{
    return orphan_count_;
}

void IntegrityModel::set_invalid_link_count(int count)
{
    invalid_link_count_ = std::max(0, count);
}
auto IntegrityModel::invalid_link_count() const -> int
{
    return invalid_link_count_;
}

} // namespace markamp::canvas
