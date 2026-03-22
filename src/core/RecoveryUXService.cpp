/// @file RecoveryUXService.cpp
/// @brief V24 P03-T03: Recovery UX service implementation.
#include "RecoveryUXService.h"

namespace markamp::core
{

void RecoveryUXService::add_recoverable(const RecoverableArtifact& artifact)
{
    recoverables_.push_back(artifact);
}

auto RecoveryUXService::accept(const ArtifactId& id) -> bool
{
    for (auto& r : recoverables_) {
        if (r.artifact_id == id) {
            r.is_accepted = true;
            r.is_discarded = false;
            return true;
        }
    }
    return false;
}

auto RecoveryUXService::discard(const ArtifactId& id) -> bool
{
    for (auto& r : recoverables_) {
        if (r.artifact_id == id) {
            r.is_discarded = true;
            r.is_accepted = false;
            return true;
        }
    }
    return false;
}

void RecoveryUXService::accept_all()
{
    for (auto& r : recoverables_) {
        if (r.is_pending()) {
            r.is_accepted = true;
        }
    }
}

void RecoveryUXService::discard_all()
{
    for (auto& r : recoverables_) {
        if (r.is_pending()) {
            r.is_discarded = true;
        }
    }
}

auto RecoveryUXService::pending_count() const noexcept -> int
{
    int c = 0;
    for (const auto& r : recoverables_) if (r.is_pending()) ++c;
    return c;
}

auto RecoveryUXService::accepted_count() const noexcept -> int
{
    int c = 0;
    for (const auto& r : recoverables_) if (r.is_accepted) ++c;
    return c;
}

auto RecoveryUXService::discarded_count() const noexcept -> int
{
    int c = 0;
    for (const auto& r : recoverables_) if (r.is_discarded) ++c;
    return c;
}

auto RecoveryUXService::result() const -> RecoveryResult
{
    RecoveryResult res;
    res.total = total_count();
    res.accepted = accepted_count();
    res.discarded = discarded_count();
    if (res.accepted == res.total && res.total > 0)
        res.action = RecoveryAction::kAcceptAll;
    else if (res.discarded == res.total && res.total > 0)
        res.action = RecoveryAction::kDiscardAll;
    else if (res.accepted > 0 || res.discarded > 0)
        res.action = RecoveryAction::kSelective;
    return res;
}

void RecoveryUXService::clear()
{
    recoverables_.clear();
}

} // namespace markamp::core
