#pragma once

#include "GutterDecorationProvider.h"

#include <string>
#include <vector>

namespace markamp::ui
{

/**
 * @brief Extracts git diff statuses (added, modified, deleted) to draw tint bars.
 */
class GitGutterProvider : public IGutterDecorationProvider
{
public:
    [[nodiscard]] auto GetProviderId() const -> std::string override;
    void UpdateContent(const std::string& content) override;
    [[nodiscard]] auto GetDecorations() const -> std::vector<GutterDecoration> override;
};

} // namespace markamp::ui
