/// @file SubsystemDoneCriteria.cpp
/// @brief V24 P01-T02: Implementation of subsystem done-criteria evaluation.
#include "SubsystemDoneCriteria.h"

#include <sstream>

namespace markamp::core
{

void SubsystemDoneCriteria::add_criterion(Subsystem subsystem, const DoneCriterion& criterion)
{
    criteria_[static_cast<int>(subsystem)].push_back(criterion);
}

auto SubsystemDoneCriteria::mark_passed(Subsystem subsystem, const std::string& criterion_id,
                                         const std::string& evidence) -> bool
{
    auto it = criteria_.find(static_cast<int>(subsystem));
    if (it == criteria_.end()) return false;

    for (auto& c : it->second) {
        if (c.criterion_id == criterion_id) {
            c.is_passed = true;
            c.evidence_link = evidence;
            return true;
        }
    }
    return false;
}

auto SubsystemDoneCriteria::mark_failed(Subsystem subsystem, const std::string& criterion_id)
    -> bool
{
    auto it = criteria_.find(static_cast<int>(subsystem));
    if (it == criteria_.end()) return false;

    for (auto& c : it->second) {
        if (c.criterion_id == criterion_id) {
            c.is_passed = false;
            c.evidence_link.clear();
            return true;
        }
    }
    return false;
}

auto SubsystemDoneCriteria::get_criteria(Subsystem subsystem) const
    -> std::vector<DoneCriterion>
{
    auto it = criteria_.find(static_cast<int>(subsystem));
    return it != criteria_.end() ? it->second : std::vector<DoneCriterion>{};
}

auto SubsystemDoneCriteria::evaluate(Subsystem subsystem) const -> SubsystemEvaluation
{
    SubsystemEvaluation eval;
    eval.subsystem = subsystem;

    auto it = criteria_.find(static_cast<int>(subsystem));
    if (it == criteria_.end()) return eval;

    for (const auto& c : it->second) {
        ++eval.total_criteria;
        if (c.is_passed) {
            ++eval.passed;
        } else {
            ++eval.failed;
        }
        if (c.is_required) {
            ++eval.required_total;
            if (c.is_passed) {
                ++eval.required_passed;
            } else {
                eval.blocking_criteria.push_back(c.criterion_id);
            }
        }
    }
    return eval;
}

auto SubsystemDoneCriteria::evaluate_all() const -> DoneCriteriaReport
{
    DoneCriteriaReport report;

    for (const auto& [key, criteria] : criteria_) {
        auto subsystem = static_cast<Subsystem>(key);
        auto eval = evaluate(subsystem);
        ++report.total_subsystems;
        if (eval.is_signed_off()) {
            ++report.signed_off;
        } else {
            ++report.blocked;
        }
        report.evaluations.push_back(std::move(eval));
    }
    return report;
}

auto SubsystemDoneCriteria::total_criteria() const noexcept -> int
{
    int count = 0;
    for (const auto& [key, criteria] : criteria_) {
        count += static_cast<int>(criteria.size());
    }
    return count;
}

auto SubsystemDoneCriteria::export_json() const -> std::string
{
    auto report = evaluate_all();
    std::ostringstream ss;
    ss << "{\n  \"total_subsystems\": " << report.total_subsystems
       << ",\n  \"signed_off\": " << report.signed_off
       << ",\n  \"blocked\": " << report.blocked
       << "\n}";
    return ss.str();
}

auto SubsystemDoneCriteria::export_markdown() const -> std::string
{
    auto report = evaluate_all();
    std::ostringstream ss;
    ss << "# Subsystem Done Criteria\n\n";
    ss << "| Subsystem | Required | Passed | Status |\n";
    ss << "|-----------|----------|--------|--------|\n";
    for (const auto& eval : report.evaluations) {
        ss << "| " << subsystem_label(eval.subsystem)
           << " | " << eval.required_total
           << " | " << eval.required_passed
           << " | " << (eval.is_signed_off() ? "✅" : "❌")
           << " |\n";
    }
    return ss.str();
}

void SubsystemDoneCriteria::clear()
{
    criteria_.clear();
}

} // namespace markamp::core
