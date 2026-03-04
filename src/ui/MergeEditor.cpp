// ============================================================================
// File: src/ui/MergeEditor.cpp
// Phase 46: Diff Editor and Merge Tool — Merge editor model
// ============================================================================
#include "MergeEditor.h"

namespace markamp::ui
{

void MergeEditorModel::set_merge_result(core::MergeResult result)
{
    merge_result_ = std::move(result);
    current_conflict_ = merge_result_.conflicts.empty() ? -1 : 0;
}

void MergeEditorModel::accept_theirs(size_t conflict_index)
{
    core::MergeEngine::resolve_conflict(
        merge_result_, conflict_index, core::ConflictResolution::AcceptTheirs);
}

void MergeEditorModel::accept_yours(size_t conflict_index)
{
    core::MergeEngine::resolve_conflict(
        merge_result_, conflict_index, core::ConflictResolution::AcceptYours);
}

void MergeEditorModel::accept_both(size_t conflict_index)
{
    core::MergeEngine::resolve_conflict(
        merge_result_, conflict_index, core::ConflictResolution::AcceptBoth);
}

void MergeEditorModel::accept_all_theirs()
{
    for (size_t i = 0; i < merge_result_.conflicts.size(); ++i)
    {
        if (merge_result_.conflicts[i].resolution == core::ConflictResolution::Unresolved)
        {
            core::MergeEngine::resolve_conflict(
                merge_result_, i, core::ConflictResolution::AcceptTheirs);
        }
    }
}

void MergeEditorModel::accept_all_yours()
{
    for (size_t i = 0; i < merge_result_.conflicts.size(); ++i)
    {
        if (merge_result_.conflicts[i].resolution == core::ConflictResolution::Unresolved)
        {
            core::MergeEngine::resolve_conflict(
                merge_result_, i, core::ConflictResolution::AcceptYours);
        }
    }
}

auto MergeEditorModel::navigate_next_conflict() -> int
{
    if (merge_result_.conflicts.empty())
    {
        return -1;
    }
    current_conflict_ = (current_conflict_ + 1) % static_cast<int>(merge_result_.conflicts.size());
    return current_conflict_;
}

auto MergeEditorModel::navigate_prev_conflict() -> int
{
    if (merge_result_.conflicts.empty())
    {
        return -1;
    }
    current_conflict_ = current_conflict_ <= 0
                            ? static_cast<int>(merge_result_.conflicts.size()) - 1
                            : current_conflict_ - 1;
    return current_conflict_;
}

} // namespace markamp::ui
