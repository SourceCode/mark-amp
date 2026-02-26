#include "GitGutterProvider.h"

namespace markamp::ui
{

auto GitGutterProvider::GetProviderId() const -> std::string
{
    return "provider.git_gutter";
}

auto GitGutterProvider::GetDecorations() const -> std::vector<GutterDecoration>
{
    return {};
}

void GitGutterProvider::UpdateContent(const std::string& /*content*/)
{
    // TODO: Phase 14 - Task 6
}

} // namespace markamp::ui
