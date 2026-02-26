#pragma once

#include "GutterDecorationProvider.h"

#include <string>

namespace markamp::ui
{

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
};

} // namespace markamp::ui
