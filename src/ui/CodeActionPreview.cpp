#include "CodeActionPreview.h"

#include <wx/dcbuffer.h>

#include <sstream>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(CodeActionPreview, wxPopupTransientWindow) // NOLINT
    EVT_PAINT(CodeActionPreview::OnPaint) wxEND_EVENT_TABLE()  // NOLINT

    CodeActionPreview::CodeActionPreview(wxWindow* parent, core::ThemeEngine& theme_engine)
    : wxPopupTransientWindow(parent, wxBORDER_NONE)
    , theme_engine_(theme_engine)
    , theme_sub_(theme_engine_.subscribe_theme_change([this](const std::string& /*theme_id*/)
                                                      { ApplyTheme(); }))
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void CodeActionPreview::ShowPreview(const core::CodeActionInfo& action,
                                    const std::string& document_content,
                                    const wxPoint& position)
{
    diff_lines_ = ComputeDiff(action, document_content);

    if (diff_lines_.empty())
    {
        return;
    }

    if (static_cast<int>(diff_lines_.size()) > max_lines_)
    {
        diff_lines_.resize(static_cast<size_t>(max_lines_));
        DiffLine truncation;
        truncation.type = DiffLineType::kContext;
        truncation.text = "...";
        diff_lines_.push_back(std::move(truncation));
    }

    const int height = static_cast<int>(diff_lines_.size()) * line_height_ + 8;
    SetSize(wxSize(max_width_, std::min(height, 300)));
    SetPosition(position);
    Popup();
}

void CodeActionPreview::HidePreview()
{
    Dismiss();
}

void CodeActionPreview::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC device_context(this);
    const auto preview_size = GetSize();

    const auto& bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    const auto& border_color = theme_engine_.color(core::ThemeColorToken::BorderLight);
    const auto& text_color = theme_engine_.color(core::ThemeColorToken::TextMain);
    const auto& removed_bg = theme_engine_.color(core::ThemeColorToken::DiffRemovedBg);
    const auto& added_bg = theme_engine_.color(core::ThemeColorToken::DiffInsertedBg);

    // Background with border
    device_context.SetBrush(wxBrush(bg_color));
    device_context.SetPen(wxPen(border_color));
    device_context.DrawRoundedRectangle(0, 0, preview_size.x, preview_size.y, 3);

    device_context.SetFont(
        wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    int current_y = 4;
    for (const auto& diff_line : diff_lines_)
    {
        wxColour row_bg;
        wxColour row_fg;
        std::string prefix;

        switch (diff_line.type)
        {
            case DiffLineType::kRemoved:
            {
                row_bg = removed_bg;
                row_fg = wxColour(255, 160, 160);
                prefix = "- ";
                break;
            }
            case DiffLineType::kAdded:
            {
                row_bg = added_bg;
                row_fg = wxColour(160, 255, 160);
                prefix = "+ ";
                break;
            }
            case DiffLineType::kContext:
            {
                row_bg = bg_color;
                row_fg = text_color;
                prefix = "  ";
                break;
            }
        }

        device_context.SetBrush(wxBrush(row_bg));
        device_context.SetPen(*wxTRANSPARENT_PEN);
        device_context.DrawRectangle(1, current_y, preview_size.x - 2, line_height_);

        device_context.SetTextForeground(row_fg);
        device_context.DrawText(prefix + diff_line.text, 8, current_y + 1);

        current_y += line_height_;
    }
}

void CodeActionPreview::ApplyTheme()
{
    Refresh();
}

auto CodeActionPreview::ComputeDiff(const core::CodeActionInfo& action,
                                    const std::string& document_content) -> std::vector<DiffLine>
{
    std::vector<DiffLine> result;

    std::vector<std::string> doc_lines;
    std::istringstream stream(document_content);
    std::string current_line;
    while (std::getline(stream, current_line))
    {
        doc_lines.push_back(current_line);
    }

    for (const auto& edit : action.edits)
    {
        const int context_start = std::max(0, edit.start_line - 1);
        for (int line_idx = context_start; line_idx < edit.start_line; ++line_idx)
        {
            if (line_idx >= 0 && line_idx < static_cast<int>(doc_lines.size()))
            {
                DiffLine ctx_line;
                ctx_line.type = DiffLineType::kContext;
                ctx_line.text = doc_lines[static_cast<size_t>(line_idx)];
                result.push_back(std::move(ctx_line));
            }
        }

        for (int line_idx = edit.start_line; line_idx <= edit.end_line; ++line_idx)
        {
            if (line_idx >= 0 && line_idx < static_cast<int>(doc_lines.size()))
            {
                DiffLine removed_line;
                removed_line.type = DiffLineType::kRemoved;
                removed_line.text = doc_lines[static_cast<size_t>(line_idx)];
                result.push_back(std::move(removed_line));
            }
        }

        if (!edit.new_text.empty())
        {
            std::istringstream new_stream(edit.new_text);
            std::string new_line;
            while (std::getline(new_stream, new_line))
            {
                DiffLine added_line;
                added_line.type = DiffLineType::kAdded;
                added_line.text = new_line;
                result.push_back(std::move(added_line));
            }
        }

        const int context_end = std::min(static_cast<int>(doc_lines.size()) - 1, edit.end_line + 1);
        if (context_end > edit.end_line && context_end < static_cast<int>(doc_lines.size()))
        {
            DiffLine ctx_line;
            ctx_line.type = DiffLineType::kContext;
            ctx_line.text = doc_lines[static_cast<size_t>(context_end)];
            result.push_back(std::move(ctx_line));
        }
    }

    return result;
}

} // namespace markamp::ui
