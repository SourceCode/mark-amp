#pragma once

/// @file CodeActionPreview.h
/// @brief V13 Phase 29 — Preview popup showing diff of proposed code action changes.

#include "core/CodeActionTypes.h"
#include "core/ThemeEngine.h"

#include <wx/popupwin.h>
#include <wx/wx.h>

#include <string>
#include <vector>

namespace markamp::ui
{

/// Diff line type for preview rendering.
enum class DiffLineType
{
    kContext, ///< Unchanged line
    kRemoved, ///< Line removed by the edit
    kAdded    ///< Line added by the edit
};

/// A single line in the diff preview.
struct DiffLine
{
    DiffLineType type{DiffLineType::kContext};
    std::string text;
};

/// Preview popup showing a diff of code action changes.
class CodeActionPreview : public wxPopupTransientWindow
{
public:
    CodeActionPreview(wxWindow* parent, core::ThemeEngine& theme_engine);

    /// Show the preview for the given action applied to the document content.
    void ShowPreview(const core::CodeActionInfo& action,
                     const std::string& document_content,
                     const wxPoint& position);

    /// Hide the preview.
    void HidePreview();

    /// Number of diff lines in the current preview.
    [[nodiscard]] auto diff_line_count() const -> int
    {
        return static_cast<int>(diff_lines_.size());
    }

    /// Whether the current preview contains any additions or removals.
    [[nodiscard]] auto has_changes() const -> bool
    {
        for (const auto& line : diff_lines_)
        {
            if (line.type != DiffLineType::kContext)
            {
                return true;
            }
        }
        return false;
    }

private:
    void OnPaint(wxPaintEvent& event);
    void ApplyTheme();

    [[nodiscard]] static auto ComputeDiff(const core::CodeActionInfo& action,
                                          const std::string& document_content)
        -> std::vector<DiffLine>;

    core::ThemeEngine& theme_engine_; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    core::Subscription theme_sub_;
    std::vector<DiffLine> diff_lines_;
    int line_height_{18};
    int max_width_{500};
    int max_lines_{20};

    wxDECLARE_EVENT_TABLE(); // NOLINT
};

} // namespace markamp::ui
