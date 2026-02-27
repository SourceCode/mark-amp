#include "GitGutterProvider.h"

#include "../core/GitCommandRunner.h"

#include <wx/datetime.h>
#include <wx/log.h>

namespace markamp::ui
{

GitGutterProvider::GitGutterProvider() = default;

GitGutterProvider::~GitGutterProvider() = default;

auto GitGutterProvider::GetProviderId() const -> std::string
{
    return "provider.git_gutter";
}

auto GitGutterProvider::GetDecorations() const -> std::vector<GutterDecoration>
{
    return decorations_;
}

void GitGutterProvider::SetFilePath(const std::string& path)
{
    current_file_path_ = path;

    // Use the directory of the file as the workspace root for the git commands
    if (!path.empty())
    {
        size_t last_slash = path.find_last_of("/\\");
        if (last_slash != std::string::npos)
        {
            workspace_root_ = path.substr(0, last_slash);
        }
        else
        {
            workspace_root_ = ".";
        }
        git_runner_ = std::make_unique<core::GitCommandRunner>(workspace_root_);
    }
}

void GitGutterProvider::UpdateContent(const std::string& /*content*/)
{
    decorations_.clear();

    if (current_file_path_.empty() || !git_runner_)
    {
        return;
    }

    // Fetch blame information
    auto blame_lines = git_runner_->GetBlame(current_file_path_);

    // Generate decorations from blame information
    for (size_t i = 0; i < blame_lines.size(); ++i)
    {
        const auto& blame = blame_lines[i];

        // Skip if there's no commit hash (e.g., untracked file or uncommitted lines)
        if (blame.commit_hash.empty() ||
            blame.commit_hash == "0000000000000000000000000000000000000000")
        {
            continue;
        }

        GutterDecoration decoration;
        decoration.line = static_cast<int>(i); // 0-indexed line number for Scintilla
        decoration.margin_index =
            4; // Assuming margin 4 is for Git blame lines, based on Phase 14 details

        // For the text, show the date and the author
        std::string date_str = blame.date;
        long time_val = 0;
        if (wxString(blame.date).ToLong(&time_val))
        {
            const wxDateTime datetime(static_cast<time_t>(time_val));
            date_str = datetime.FormatISODate().ToStdString();
        }

        decoration.text =
            date_str.substr(0, 10) + " " +
            (blame.author.empty() ? blame.commit_hash.substr(0, 8) : blame.author.substr(0, 10));

        decoration.tooltip =
            blame.commit_hash.substr(0, 8) + " - " + blame.author + "\n" + date_str;

        decorations_.push_back(decoration);
    }
}

} // namespace markamp::ui
