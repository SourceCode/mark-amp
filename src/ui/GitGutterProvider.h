#pragma once

#include "GutterDecorationProvider.h"

#include <memory>
#include <string>
#include <vector>

namespace markamp::core
{
class GitCommandRunner;
}

namespace markamp::ui
{

/**
 * @brief Extracts git diff statuses (added, modified, deleted) to draw tint bars.
 */
class GitGutterProvider : public IGutterDecorationProvider
{
public:
    GitGutterProvider();
    ~GitGutterProvider() override;

    GitGutterProvider(const GitGutterProvider&) = delete;
    auto operator=(const GitGutterProvider&) -> GitGutterProvider& = delete;
    GitGutterProvider(GitGutterProvider&&) = delete;
    auto operator=(GitGutterProvider&&) -> GitGutterProvider& = delete;

    [[nodiscard]] auto GetProviderId() const -> std::string override;
    void UpdateContent(const std::string& content) override;
    void SetFilePath(const std::string& path) override;
    [[nodiscard]] auto GetDecorations() const -> std::vector<GutterDecoration> override;

private:
    std::string workspace_root_;
    std::string current_file_path_;
    std::vector<GutterDecoration> decorations_;
    std::unique_ptr<core::GitCommandRunner> git_runner_;
};

} // namespace markamp::ui
