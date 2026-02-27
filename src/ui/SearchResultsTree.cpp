#include "ui/SearchResultsTree.h"

#include "core/ThemeTokens.h"
#include "ui/DesignSystemContext.h"
#include "ui/IconManager.h"
#include "ui/ThemedScrollbar.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

namespace markamp::ui
{

// NOLINTBEGIN
wxBEGIN_EVENT_TABLE(SearchResultsTree, ThemeAwareWindow) EVT_PAINT(SearchResultsTree::OnPaint)
    EVT_SIZE(SearchResultsTree::OnSize) EVT_LEFT_DOWN(SearchResultsTree::OnMouseLeftDown)
        EVT_MOTION(SearchResultsTree::OnMouseMotion)
            EVT_LEAVE_WINDOW(SearchResultsTree::OnMouseLeave)
                EVT_MOUSEWHEEL(SearchResultsTree::OnScroll) wxEND_EVENT_TABLE()
    // NOLINTEND

    SearchResultsTree::SearchResultsTree(wxWindow* parent, core::ThemeEngine& theme_engine)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxBORDER_NONE | wxWANTS_CHARS)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void SearchResultsTree::SetResults(const core::WorkspaceSearchEngine::SearchResult& result)
{
    current_result_ = result;
    RebuildItems();
    Refresh();
}

void SearchResultsTree::ClearResults()
{
    current_result_ = {};
    items_.clear();
    visible_items_.clear();
    scroll_offset_ = 0;
    selected_idx_ = -1;
    hovered_idx_ = -1;
    Refresh();
}

void SearchResultsTree::CollapseAll()
{
    for (auto& item : items_)
    {
        if (item.is_file)
        {
            item.expanded = false;
        }
    }
    UpdateVirtualHeight();
    Refresh();
}

void SearchResultsTree::ExpandAll()
{
    for (auto& item : items_)
    {
        if (item.is_file)
        {
            item.expanded = true;
        }
    }
    UpdateVirtualHeight();
    Refresh();
}

void SearchResultsTree::RebuildItems()
{
    items_.clear();
    std::string current_file;
    SearchResultItem* current_file_node = nullptr;

    for (const auto& match : current_result_.matches)
    {
        if (match.file_path != current_file)
        {
            current_file = match.file_path;
            SearchResultItem file_node;
            file_node.is_file = true;
            file_node.file_path = current_file;

            // Extract file name
            const size_t kPos = current_file.find_last_of("/\\");
            file_node.file_name =
                (kPos != std::string::npos) ? current_file.substr(kPos + 1) : current_file;
            file_node.match_count = 1;

            items_.push_back(file_node);
            current_file_node = &items_.back();
        }
        else if (current_file_node != nullptr)
        {
            current_file_node->match_count++;
        }

        int last_added_line = -1;
        // Add context before
        for (size_t i = 0; i < match.context_before.size(); ++i)
        {
            int ctx_line = match.line_number - static_cast<int>(match.context_before.size()) +
                           static_cast<int>(i);

            // Avoid duplicate context lines if matches are close
            // Since we add matches and context sequentially, last_added_line works
            if (ctx_line > last_added_line)
            {
                SearchResultItem ctx_node;
                ctx_node.is_file = false;
                ctx_node.is_context = true;
                ctx_node.context_line_content = match.context_before[i];
                ctx_node.context_line_number = ctx_line;
                items_.push_back(ctx_node);
                last_added_line = ctx_line;
            }
        }

        SearchResultItem match_node;
        match_node.is_file = false;
        match_node.is_context = false;
        match_node.match = match;
        items_.push_back(match_node);
        last_added_line = match.line_number;

        // Add context after
        for (size_t i = 0; i < match.context_after.size(); ++i)
        {
            int ctx_line = match.line_number + 1 + static_cast<int>(i);
            if (ctx_line > last_added_line)
            {
                SearchResultItem ctx_node;
                ctx_node.is_file = false;
                ctx_node.is_context = true;
                ctx_node.context_line_content = match.context_after[i];
                ctx_node.context_line_number = ctx_line;
                items_.push_back(ctx_node);
                last_added_line = ctx_line;
            }
        }
    }

    UpdateVirtualHeight();
}

void SearchResultsTree::UpdateVirtualHeight()
{
    visible_items_.clear();
    for (auto& item : items_)
    {
        if (item.is_file)
        {
            visible_items_.push_back(&item);
        }
        else
        {
            // Find parent file node
            auto matching_it = std::find_if(items_.rbegin(),
                                            items_.rend(),
                                            [&item](const SearchResultItem& inner_item)
                                            { return inner_item.is_file && &inner_item < &item; });
            if (matching_it != items_.rend() && matching_it->expanded)
            {
                visible_items_.push_back(&item);
            }
        }
    }
}

int SearchResultsTree::GetCount() const
{
    return static_cast<int>(visible_items_.size());
}

int SearchResultsTree::GetCharHeight() const
{
    return 22; // Base height, customize as needed
}

void SearchResultsTree::SetFirstItem(int index)
{
    if (index >= 0 && index <= std::max(0, static_cast<int>(visible_items_.size()) - 1))
    {
        scroll_offset_ = index * GetCharHeight();
        Refresh();
    }
}

int SearchResultsTree::HitTestItem(const wxPoint& point) const
{
    const int kItemY = point.y + scroll_offset_;
    const int kHitIndex = kItemY / GetCharHeight();
    if (kHitIndex >= 0 && kHitIndex < static_cast<int>(visible_items_.size()))
    {
        return kHitIndex;
    }
    return -1;
}

void SearchResultsTree::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void SearchResultsTree::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(this);
    paint_dc.Clear();

    auto bg_color = theme_engine()
                        .resolve_token("sidebar.bg")
                        .value_or(theme_engine().color(core::ThemeColorToken::BgPanel));
    paint_dc.SetBackground(wxBrush(bg_color));
    paint_dc.Clear();

    std::unique_ptr<wxGraphicsContext> graphics_context(wxGraphicsContext::Create(paint_dc));
    if (!graphics_context)
    {
        return;
    }

    int item_y = -scroll_offset_;
    const int kItemHeight = GetCharHeight();
    const int kWidth = GetClientSize().GetWidth();

    auto file_font = theme_engine().font(core::ThemeFontToken::SansBold).Scaled(0.9F);
    auto match_font = theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.85F);
    auto fg_main = theme_engine().color(core::ThemeColorToken::TextMain);
    auto fg_muted = theme_engine().color(core::ThemeColorToken::TextMuted);
    auto hl_bg = theme_engine().color(core::ThemeColorToken::EditorMatchHighlight);

    for (size_t i = 0; i < visible_items_.size(); ++i)
    {
        if (item_y + kItemHeight > 0 && item_y < GetClientSize().GetHeight())
        {
            auto* item = visible_items_[i];

            if (static_cast<int>(i) == selected_idx_)
            {
                graphics_context->SetBrush(
                    wxBrush(theme_engine().color(core::ThemeColorToken::ControlBgSelected)));
                graphics_context->SetPen(*wxTRANSPARENT_PEN);
                graphics_context->DrawRectangle(0, item_y, kWidth, kItemHeight);
            }
            else if (static_cast<int>(i) == hovered_idx_)
            {
                graphics_context->SetBrush(
                    wxBrush(theme_engine().color(core::ThemeColorToken::ControlBgHover)));
                graphics_context->SetPen(*wxTRANSPARENT_PEN);
                graphics_context->DrawRectangle(0, item_y, kWidth, kItemHeight);
            }

            if (item->is_file)
            {
                // Draw chevron
                double text_width = 0;
                double text_height = 0;
                double text_descent = 0;
                double text_ext_leading = 0;
                graphics_context->SetFont(file_font, fg_main);
                const wxString kChevron = item->expanded ? "v " : "> ";
                graphics_context->GetTextExtent(
                    kChevron, &text_width, &text_height, &text_descent, &text_ext_leading);
                graphics_context->DrawText(kChevron, 4, item_y + (kItemHeight - text_height) / 2.0);

                // Draw file name
                graphics_context->DrawText(
                    item->file_name, 4 + text_width, item_y + (kItemHeight - text_height) / 2.0);

                // Draw badge (optional)
                const wxString kCountStr = wxString::Format("%d", item->match_count);
                double count_text_width = 0;
                graphics_context->GetTextExtent(
                    kCountStr, &count_text_width, &text_height, &text_descent, &text_ext_leading);
                graphics_context->SetBrush(
                    wxBrush(theme_engine().color(core::ThemeColorToken::ControlBgHover)));
                graphics_context->SetPen(*wxTRANSPARENT_PEN);
                graphics_context->DrawRoundedRectangle(kWidth - count_text_width - 16,
                                                       item_y + 2,
                                                       count_text_width + 8,
                                                       kItemHeight - 4,
                                                       4.0);
                graphics_context->DrawText(kCountStr,
                                           kWidth - count_text_width - 12,
                                           item_y + (kItemHeight - text_height) / 2.0);
            }
            else
            {
                // Draw line number and match content
                graphics_context->SetFont(match_font, fg_muted);

                int line_number =
                    item->is_context ? item->context_line_number : item->match.line_number;
                const wxString kLineStr = wxString::Format("%d  ", line_number);
                double text_width = 0;
                double text_height = 0;
                double text_descent = 0;
                double text_ext_leading = 0;
                graphics_context->GetTextExtent(
                    kLineStr, &text_width, &text_height, &text_descent, &text_ext_leading);
                graphics_context->DrawText(
                    kLineStr, 20, item_y + (kItemHeight - text_height) / 2.0);

                // Highlight match
                const wxString kContext =
                    item->is_context ? item->context_line_content : item->match.line_content;
                double context_width = 0;
                double context_height = 0;
                if (!kContext.empty())
                {
                    // Task 7: Match Highlighting (Simplified logic for now, using column/length,
                    // requires byte->char logic) We'll highlight a portion. For now, draw text with
                    // matches highlighted. To do properly, we need precise text extent logic. Let's
                    // just draw context for now and a rough highlight.

                    if (item->is_context)
                    {
                        graphics_context->SetFont(match_font, fg_muted);
                    }
                    else
                    {
                        graphics_context->SetFont(match_font, fg_main);
                        graphics_context->GetTextExtent(kContext,
                                                        &context_width,
                                                        &context_height,
                                                        &text_descent,
                                                        &text_ext_leading);

                        // Simple highlight box representation
                        graphics_context->SetBrush(wxBrush(hl_bg));
                        graphics_context->SetPen(*wxTRANSPARENT_PEN);
                        graphics_context->DrawRectangle(
                            20 + text_width,
                            item_y + (kItemHeight - context_height) / 2.0,
                            context_width,
                            context_height); // Rough highlight over the whole line for preview
                    }

                    if (item->is_context)
                    {
                        graphics_context->GetTextExtent(kContext,
                                                        &context_width,
                                                        &context_height,
                                                        &text_descent,
                                                        &text_ext_leading);
                    }

                    graphics_context->DrawText(
                        kContext, 20 + text_width, item_y + (kItemHeight - context_height) / 2.0);
                }
            }
        }
        item_y += kItemHeight;
    }
}

void SearchResultsTree::OnSize(wxSizeEvent& event)
{
    Refresh();
    event.Skip();
}

void SearchResultsTree::OnMouseLeftDown(wxMouseEvent& event)
{
    const int kTargetIndex = HitTestItem(event.GetPosition());
    if (kTargetIndex >= 0)
    {
        selected_idx_ = kTargetIndex;
        auto* item = visible_items_[static_cast<size_t>(kTargetIndex)];
        if (item->is_file)
        {
            // Toggle expand/collapse
            item->expanded = !item->expanded;
            UpdateVirtualHeight();
        }
        else if (!item->is_context && on_result_selected_)
        {
            on_result_selected_(item->match.file_path, item->match.line_number, item->match.column);
        }
        Refresh();
    }
}

auto SearchResultsTree::GetSelectedMatch() const
    -> std::optional<core::WorkspaceSearchEngine::SearchMatch>
{
    if (selected_idx_ >= 0 && selected_idx_ < static_cast<int>(visible_items_.size()))
    {
        auto* item = visible_items_[static_cast<size_t>(selected_idx_)];
        if (!item->is_file && !item->is_context)
        {
            return item->match;
        }
    }
    return std::nullopt;
}

void SearchResultsTree::SelectNextMatch()
{
    if (visible_items_.empty())
    {
        return;
    }

    const int start_idx = selected_idx_ >= 0 ? selected_idx_ + 1 : 0;

    for (int i = start_idx; i < static_cast<int>(visible_items_.size()); ++i)
    {
        if (!visible_items_[static_cast<size_t>(i)]->is_file &&
            !visible_items_[static_cast<size_t>(i)]->is_context)
        {
            selected_idx_ = i;
            SetFirstItem(std::max(0, i - 1)); // Scroll into view roughly
            Refresh();
            auto* item = visible_items_[static_cast<size_t>(i)];
            if (on_result_selected_)
            {
                on_result_selected_(
                    item->match.file_path, item->match.line_number, item->match.column);
            }
            return;
        }
    }

    // Wrap around
    for (int i = 0; i < selected_idx_ && i < static_cast<int>(visible_items_.size()); ++i)
    {
        if (!visible_items_[static_cast<size_t>(i)]->is_file &&
            !visible_items_[static_cast<size_t>(i)]->is_context)
        {
            selected_idx_ = i;
            SetFirstItem(std::max(0, i - 1));
            Refresh();
            auto* item = visible_items_[static_cast<size_t>(i)];
            if (on_result_selected_)
            {
                on_result_selected_(
                    item->match.file_path, item->match.line_number, item->match.column);
            }
            return;
        }
    }
}

void SearchResultsTree::SelectPreviousMatch()
{
    if (visible_items_.empty())
    {
        return;
    }

    const int start_idx =
        selected_idx_ >= 0 ? selected_idx_ - 1 : static_cast<int>(visible_items_.size()) - 1;

    for (int i = start_idx; i >= 0; --i)
    {
        if (!visible_items_[static_cast<size_t>(i)]->is_file &&
            !visible_items_[static_cast<size_t>(i)]->is_context)
        {
            selected_idx_ = i;
            SetFirstItem(std::max(0, i - 1));
            Refresh();
            auto* item = visible_items_[static_cast<size_t>(i)];
            if (on_result_selected_)
            {
                on_result_selected_(
                    item->match.file_path, item->match.line_number, item->match.column);
            }
            return;
        }
    }

    // Wrap around
    for (int i = static_cast<int>(visible_items_.size()) - 1; i > selected_idx_; --i)
    {
        if (!visible_items_[static_cast<size_t>(i)]->is_file &&
            !visible_items_[static_cast<size_t>(i)]->is_context)
        {
            selected_idx_ = i;
            SetFirstItem(std::max(0, i - 1));
            Refresh();
            auto* item = visible_items_[static_cast<size_t>(i)];
            if (on_result_selected_)
            {
                on_result_selected_(
                    item->match.file_path, item->match.line_number, item->match.column);
            }
            return;
        }
    }
}

void SearchResultsTree::OnMouseMotion(wxMouseEvent& event)
{
    const int kTargetIndex = HitTestItem(event.GetPosition());
    if (kTargetIndex != hovered_idx_)
    {
        hovered_idx_ = kTargetIndex;
        Refresh();
    }
}

void SearchResultsTree::OnMouseLeave(wxMouseEvent& /*event*/)
{
    if (hovered_idx_ != -1)
    {
        hovered_idx_ = -1;
        Refresh();
    }
}

void SearchResultsTree::OnScroll(wxMouseEvent& event)
{
    const int kScrollDelta =
        event.GetWheelRotation() > 0 ? -GetCharHeight() * 3 : GetCharHeight() * 3;
    scroll_offset_ += kScrollDelta;
    const int kMaxScrollOffset = std::max(
        0, static_cast<int>(visible_items_.size()) * GetCharHeight() - GetClientSize().GetHeight());
    scroll_offset_ = std::clamp(scroll_offset_, 0, kMaxScrollOffset);
    Refresh();
}

} // namespace markamp::ui
