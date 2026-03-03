#pragma once

#include "GutterDecorationProvider.h"

#include <string>
#include <vector>

namespace markamp::core
{
class GitCommandRunner;
struct BlameLine;
} // namespace markamp::core

namespace markamp::ui
{

/// Phase 18 Task 12: Gutter blame annotation provider.
/// When enabled, produces per-line blame text decorations showing author and date.
class GitBlameGutterProvider : public IGutterDecorationProvider
{
public:
    GitBlameGutterProvider();
    ~GitBlameGutterProvider() override;

    GitBlameGutterProvider(const GitBlameGutterProvider&) = delete;
    auto operator=(const GitBlameGutterProvider&) -> GitBlameGutterProvider& = delete;
    GitBlameGutterProvider(GitBlameGutterProvider&&) = delete;
    auto operator=(GitBlameGutterProvider&&) -> GitBlameGutterProvider& = delete;

    [[nodiscard]] auto GetProviderId() const -> std::string override;
    void UpdateContent(const std::string& content) override;
    void SetFilePath(const std::string& path) override;
    [[nodiscard]] auto GetDecorations() const -> std::vector<GutterDecoration> override;

    /// Toggle blame annotations on/off
    void SetEnabled(bool enabled);
    [[nodiscard]] bool IsEnabled() const;

    /// Margin index used for blame text (must not conflict with other margins)
    static constexpr int kBlameMargin = 5;

    /// Width in pixels for the blame margin (0 = hidden)
    static constexpr int kBlameMarginWidth = 220;

private:
    bool enabled_{false};
    std::string current_file_path_;
    std::string workspace_root_;
    std::vector<GutterDecoration> decorations_;

    void RefreshBlame();
};

} // namespace markamp::ui
