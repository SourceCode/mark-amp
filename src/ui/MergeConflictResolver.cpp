#include "MergeConflictResolver.h"

#include <sstream>

namespace markamp::ui
{

auto MergeConflictResolver::GetProviderId() const -> std::string
{
    return "provider.merge_conflict";
}

auto MergeConflictResolver::GetDecorations() const -> std::vector<GutterDecoration>
{
    return decorations_;
}

void MergeConflictResolver::UpdateContent(const std::string& content)
{
    // Improvement 47-48: Parse git-style merge conflict markers
    decorations_.clear();
    conflict_regions_.clear();

    std::istringstream stream(content);
    std::string line;
    int line_number = 0;
    int conflict_start = -1;

    while (std::getline(stream, line))
    {
        ++line_number;
        if (line.substr(0, 7) == "<<<<<<<")
        {
            conflict_start = line_number;
        }
        else if (line.substr(0, 7) == "=======" && conflict_start >= 0)
        {
            // Mark ours region with gutter decoration
            GutterDecoration ours_dec;
            ours_dec.line = conflict_start;
            ours_dec.tooltip = "Merge conflict — your changes";
            ours_dec.text = "<<<";
            decorations_.push_back(ours_dec);
        }
        else if (line.substr(0, 7) == ">>>>>>>" && conflict_start >= 0)
        {
            // Mark theirs region
            GutterDecoration theirs_dec;
            theirs_dec.line = line_number;
            theirs_dec.tooltip = "Merge conflict — their changes";
            theirs_dec.text = ">>>";
            decorations_.push_back(theirs_dec);

            // Record the complete conflict region
            ConflictRegion region;
            region.start_line = conflict_start;
            region.end_line = line_number;
            conflict_regions_.push_back(region);

            conflict_start = -1;
        }
    }
}

void MergeConflictResolver::ResolveConflict(int /*start_line*/,
                                            int /*end_line*/,
                                            const std::string& /*resolution*/)
{
    // Implementation for resolving a merge conflict region
}

} // namespace markamp::ui
