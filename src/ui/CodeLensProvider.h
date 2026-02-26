#pragma once

#include "GutterDecorationProvider.h"

#include <string>
#include <vector>

namespace markamp::ui
{

/**
 * @brief Provides inline Code Lens actions above functions and structural content.
 */
class CodeLensProvider : public IGutterDecorationProvider
{
public:
    [[nodiscard]] auto GetProviderId() const -> std::string override;
    void UpdateContent(const std::string& content) override;
    [[nodiscard]] auto GetDecorations() const -> std::vector<GutterDecoration> override;

private:
    std::vector<GutterDecoration> decorations_;
};

} // namespace markamp::ui
