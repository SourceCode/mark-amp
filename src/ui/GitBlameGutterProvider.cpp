#include "GitBlameGutterProvider.h"

#include "core/GitCommandRunner.h"

#include <filesystem>

namespace markamp::ui
{

GitBlameGutterProvider::GitBlameGutterProvider() = default;

GitBlameGutterProvider::~GitBlameGutterProvider() = default;

auto GitBlameGutterProvider::GetProviderId() const -> std::string
{
    return "provider.git_blame";
}

void GitBlameGutterProvider::UpdateContent(const std::string& /*content*/)
{
    if (enabled_)
    {
        RefreshBlame();
    }
}

void GitBlameGutterProvider::SetFilePath(const std::string& path)
{
    current_file_path_ = path;

    // Derive workspace root from the file path by walking up to find .git
    auto dir = std::filesystem::path(path).parent_path();
    while (!dir.empty() && dir != dir.root_path())
    {
        if (std::filesystem::exists(dir / ".git"))
        {
            workspace_root_ = dir.string();
            break;
        }
        dir = dir.parent_path();
    }

    if (enabled_)
    {
        RefreshBlame();
    }
}

auto GitBlameGutterProvider::GetDecorations() const -> std::vector<GutterDecoration>
{
    return decorations_;
}

void GitBlameGutterProvider::SetEnabled(bool enabled)
{
    enabled_ = enabled;
    if (enabled_)
    {
        RefreshBlame();
    }
    else
    {
        decorations_.clear();
    }
}

bool GitBlameGutterProvider::IsEnabled() const
{
    return enabled_;
}

void GitBlameGutterProvider::RefreshBlame()
{
    decorations_.clear();

    if (workspace_root_.empty() || current_file_path_.empty())
    {
        return;
    }

    core::GitCommandRunner runner(workspace_root_);

    // Make file_path relative to workspace root
    auto rel_path = std::filesystem::relative(current_file_path_, workspace_root_).string();
    auto blame_lines = runner.GetBlame(rel_path);

    decorations_.reserve(blame_lines.size());

    for (int line_idx = 0; line_idx < static_cast<int>(blame_lines.size()); ++line_idx)
    {
        const auto& blame = blame_lines[static_cast<std::size_t>(line_idx)];

        // Compact blame text: "author · date"
        // Truncate author to 12 chars max
        std::string author = blame.author;
        if (author.length() > 12)
        {
            author = author.substr(0, 11) + "\xE2\x80\xA6"; // ellipsis
        }

        // Use short date (first 10 chars of ISO date)
        std::string date = blame.date;
        if (date.length() > 10)
        {
            date = date.substr(0, 10);
        }

        GutterDecoration dec;
        dec.line = line_idx;
        dec.margin_index = kBlameMargin;
        dec.text = author;
        dec.text += " \xC2\xB7 ";
        dec.text += date;
        dec.tooltip = blame.author + " " + blame.date + " " + blame.commit_hash.substr(0, 8);
        decorations_.push_back(dec);
    }
}

} // namespace markamp::ui
