#pragma once

#include "GutterDecorationProvider.h"

#include <string>
#include <vector>

namespace markamp::ui
{

/// Improvement 47-48: Represents a detected merge conflict region.
struct ConflictRegion
{
    int start_line{0};
    int end_line{0};
};

/**
 * @brief Detects and manages resolution actions for Git Merge Conflicts.
 */
class MergeConflictResolver : public IGutterDecorationProvider
{
public:
    [[nodiscard]] auto GetProviderId() const -> std::string override;
    void UpdateContent(const std::string& content) override;
    [[nodiscard]] auto GetDecorations() const -> std::vector<GutterDecoration> override;

    void ResolveConflict(int start_line, int end_line, const std::string& resolution);

    /// Improvement 47: Get detected conflict regions.
    [[nodiscard]] auto conflict_regions() const -> const std::vector<ConflictRegion>&
    {
        return conflict_regions_;
    }

private:
    std::vector<GutterDecoration> decorations_;
    std::vector<ConflictRegion> conflict_regions_;
};

} // namespace markamp::ui
