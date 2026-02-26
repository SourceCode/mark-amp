#include "MergeConflictResolver.h"

namespace markamp::ui
{

auto MergeConflictResolver::GetProviderId() const -> std::string
{
    return "provider.merge_conflict";
}

auto MergeConflictResolver::GetDecorations() const -> std::vector<GutterDecoration>
{
    return {};
}

void MergeConflictResolver::UpdateContent(const std::string& /*content*/)
{
    // TODO: Phase 14 - Task 12
}

void MergeConflictResolver::ResolveConflict(int /*start_line*/,
                                            int /*end_line*/,
                                            const std::string& /*resolution*/)
{
    // Implementation for resolving a merge conflict region
}

} // namespace markamp::ui
