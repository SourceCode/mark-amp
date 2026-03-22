/// @file V25LedgerBootstrap.h
/// @brief V25 P01-T01: Seeds the ExecutionLedger with all 60 V25 tasks.
///
/// Populates the ledger with task IDs, dependencies, subsystem ownership,
/// and priority for every task in the V25 release-readiness wave.
/// Also bootstraps SubsystemDoneCriteria with release-ready evidence templates.
#pragma once

#include "ExecutionLedger.h"
#include "SubsystemDoneCriteria.h"

namespace markamp::core
{

/// Seeds the execution ledger with all 60 V25 tasks.
void bootstrap_v25_tasks(ExecutionLedger& ledger);

/// Seeds subsystem done criteria with V25 release-ready evidence templates.
void bootstrap_v25_criteria(SubsystemDoneCriteria& criteria);

/// Total number of V25 tasks registered by bootstrap.
[[nodiscard]] constexpr auto v25_task_count() noexcept -> int { return 60; }

/// Total number of V25 phases.
[[nodiscard]] constexpr auto v25_phase_count() noexcept -> int { return 20; }

} // namespace markamp::core
