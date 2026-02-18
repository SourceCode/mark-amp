// ============================================================================
// File: src/core/MemoryBudgetEnforcer.cpp
// Phase 30: Performance Optimization — Per-subsystem memory budget tracking
// ============================================================================

#include "MemoryBudgetEnforcer.h"

#include <algorithm>
#include <chrono>
#include <numeric>
#include <sstream>

namespace markamp::core
{

MemoryBudgetEnforcer::MemoryBudgetEnforcer()
{
    subsystems_.at(static_cast<size_t>(MemorySubsystem::kEditor)).budget = kEditorBudget;
    subsystems_.at(static_cast<size_t>(MemorySubsystem::kCanvas)).budget = kCanvasBudget;
    subsystems_.at(static_cast<size_t>(MemorySubsystem::kSearchIndex)).budget = kSearchIndexBudget;
    subsystems_.at(static_cast<size_t>(MemorySubsystem::kExtensions)).budget = kExtensionsBudget;
    subsystems_.at(static_cast<size_t>(MemorySubsystem::kGeneral)).budget = kGeneralBudget;
}

void MemoryBudgetEnforcer::allocate(MemorySubsystem sub, size_t bytes)
{
    const auto idx = static_cast<size_t>(sub);
    if (idx < subsystems_.size())
    {
        subsystems_.at(idx).usage += bytes;
    }
}

void MemoryBudgetEnforcer::release(MemorySubsystem sub, size_t bytes)
{
    const auto idx = static_cast<size_t>(sub);
    if (idx < subsystems_.size())
    {
        if (bytes > subsystems_.at(idx).usage)
        {
            subsystems_.at(idx).usage = 0;
        }
        else
        {
            subsystems_.at(idx).usage -= bytes;
        }
    }
}

auto MemoryBudgetEnforcer::usage(MemorySubsystem sub) const -> size_t
{
    const auto idx = static_cast<size_t>(sub);
    if (idx < subsystems_.size())
    {
        return subsystems_.at(idx).usage;
    }
    return 0;
}

auto MemoryBudgetEnforcer::usage_percent(MemorySubsystem sub) const -> double
{
    const auto idx = static_cast<size_t>(sub);
    if (idx < subsystems_.size() && subsystems_.at(idx).budget > 0)
    {
        return static_cast<double>(subsystems_.at(idx).usage) /
               static_cast<double>(subsystems_.at(idx).budget);
    }
    return 0.0;
}

auto MemoryBudgetEnforcer::severity(MemorySubsystem sub) const -> BudgetSeverity
{
    const double pct = usage_percent(sub);
    if (pct >= kCriticalThreshold)
    {
        return BudgetSeverity::kCritical;
    }
    if (pct >= kWarningThreshold)
    {
        return BudgetSeverity::kWarning;
    }
    return BudgetSeverity::kNormal;
}

auto MemoryBudgetEnforcer::budget(MemorySubsystem sub) const -> size_t
{
    const auto idx = static_cast<size_t>(sub);
    if (idx < subsystems_.size())
    {
        return subsystems_.at(idx).budget;
    }
    return 0;
}

void MemoryBudgetEnforcer::set_budget(MemorySubsystem sub, size_t bytes)
{
    const auto idx = static_cast<size_t>(sub);
    if (idx < subsystems_.size())
    {
        subsystems_.at(idx).budget = bytes;
    }
}

auto MemoryBudgetEnforcer::total_usage() const -> size_t
{
    size_t total = 0;
    for (const auto& state : subsystems_)
    {
        total += state.usage;
    }
    return total;
}

auto MemoryBudgetEnforcer::total_budget() const -> size_t
{
    size_t total = 0;
    for (const auto& state : subsystems_)
    {
        total += state.budget;
    }
    return total;
}

auto MemoryBudgetEnforcer::snapshot() const -> MemorySnapshot
{
    MemorySnapshot snap;
    snap.total_usage = 0;
    snap.total_budget = 0;

    for (size_t idx = 0; idx < static_cast<size_t>(MemorySubsystem::kCount); ++idx)
    {
        const auto sub = static_cast<MemorySubsystem>(idx);
        auto& info = snap.subsystems.at(idx);
        info.subsystem = sub;
        info.usage_bytes = subsystems_.at(idx).usage;
        info.budget_bytes = subsystems_.at(idx).budget;
        info.usage_percent = usage_percent(sub);
        info.severity = severity(sub);

        snap.total_usage += info.usage_bytes;
        snap.total_budget += info.budget_bytes;
    }

    if (snap.total_budget > 0)
    {
        snap.total_usage_percent =
            static_cast<double>(snap.total_usage) / static_cast<double>(snap.total_budget);
    }

    snap.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count();

    return snap;
}

auto MemoryBudgetEnforcer::to_json() const -> std::string
{
    const auto snap = snapshot();
    std::ostringstream oss;
    oss << R"({)"
        << "\n";
    oss << R"(  "timestamp_ms": )" << snap.timestamp_ms << ",\n";
    oss << R"(  "total_usage_bytes": )" << snap.total_usage << ",\n";
    oss << R"(  "total_budget_bytes": )" << snap.total_budget << ",\n";
    oss << R"(  "total_usage_percent": )" << snap.total_usage_percent << ",\n";
    oss << R"(  "subsystems": [)"
        << "\n";

    for (size_t idx = 0; idx < static_cast<size_t>(MemorySubsystem::kCount); ++idx)
    {
        const auto& info = snap.subsystems.at(idx);
        oss << R"(    {"name": ")" << subsystem_name(info.subsystem) << R"(", )";
        oss << R"("usage_bytes": )" << info.usage_bytes << ", ";
        oss << R"("budget_bytes": )" << info.budget_bytes << ", ";
        oss << R"("usage_percent": )" << info.usage_percent << ", ";
        oss << R"("severity": ")";
        switch (info.severity)
        {
            case BudgetSeverity::kNormal:
                oss << "normal";
                break;
            case BudgetSeverity::kWarning:
                oss << "warning";
                break;
            case BudgetSeverity::kCritical:
                oss << "critical";
                break;
        }
        oss << R"("})";
        if (idx + 1 < static_cast<size_t>(MemorySubsystem::kCount))
        {
            oss << ",";
        }
        oss << "\n";
    }

    oss << "  ]\n";
    oss << "}";
    return oss.str();
}

void MemoryBudgetEnforcer::reset()
{
    for (auto& state : subsystems_)
    {
        state.usage = 0;
    }
}

} // namespace markamp::core
