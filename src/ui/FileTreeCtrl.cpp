#include "FileTreeCtrl.h"

#include "core/Events.h"
#include "core/Logger.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

#include <algorithm>

namespace markamp::ui
{

FileTreeCtrl::FileTreeCtrl(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           core::EventBus& event_bus)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER)
    , event_bus_(event_bus)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &FileTreeCtrl::OnPaint, this);
    Bind(wxEVT_MOTION, &FileTreeCtrl::OnMouseMove, this);
    Bind(wxEVT_LEFT_DOWN, &FileTreeCtrl::OnMouseDown, this);
    Bind(wxEVT_LEAVE_WINDOW, &FileTreeCtrl::OnMouseLeave, this);
    Bind(wxEVT_MOUSEWHEEL, &FileTreeCtrl::OnScroll, this);
}

void FileTreeCtrl::SetFileTree(const std::vector<core::FileNode>& roots)
{
    roots_ = roots;
    UpdateVirtualHeight();
    Refresh();
}

void FileTreeCtrl::SetActiveFileId(const std::string& file_id)
{
    if (active_file_id_ != file_id)
    {
        active_file_id_ = file_id;
        Refresh();
    }
}

void FileTreeCtrl::SetOnFileSelect(FileSelectCallback callback)
{
    on_file_select_ = std::move(callback);
}

// --- Rendering ---

void FileTreeCtrl::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    auto sz = GetClientSize();

    // Background
    dc.SetBrush(theme_engine().brush(core::ThemeColorToken::BgPanel));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(sz);

    // Set clip region
    dc.SetClippingRegion(0, 0, sz.GetWidth(), sz.GetHeight());

    // Font
    dc.SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular));

    int y_offset = -scroll_offset_;
    for (const auto& node : roots_)
    {
        DrawNode(dc, node, 0, y_offset);
    }
}

void FileTreeCtrl::DrawNode(wxDC& dc, const core::FileNode& node, int depth, int& y_offset)
{
    auto sz = GetClientSize();
    int row_top = y_offset;
    int row_w = sz.GetWidth();

    // Only draw if the row is visible
    if (row_top + kRowHeight > 0 && row_top < sz.GetHeight())
    {
        int x = kLeftPadding + depth * kIndentWidth;
        int text_y = row_top + (kRowHeight - dc.GetCharHeight()) / 2;
        int icon_y = row_top + (kRowHeight - kIconSize) / 2;

        bool is_selected = (node.id == active_file_id_ && node.is_file());
        bool is_hovered = (node.id == hovered_node_id_);

        // Row background
        if (is_selected)
        {
            // Accent at 20% opacity
            auto accent = theme_engine().color(core::ThemeColorToken::AccentPrimary);
            auto bg = theme_engine().color(core::ThemeColorToken::BgPanel);
            wxColour blended(
                static_cast<unsigned char>((accent.Red() * 51 + bg.Red() * 204) / 255),
                static_cast<unsigned char>((accent.Green() * 51 + bg.Green() * 204) / 255),
                static_cast<unsigned char>((accent.Blue() * 51 + bg.Blue() * 204) / 255));
            dc.SetBrush(wxBrush(blended));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRectangle(0, row_top, row_w, kRowHeight);
        }
        else if (is_hovered)
        {
            // Accent at 10% opacity
            auto accent = theme_engine().color(core::ThemeColorToken::AccentPrimary);
            auto bg = theme_engine().color(core::ThemeColorToken::BgPanel);
            wxColour blended(
                static_cast<unsigned char>((accent.Red() * 26 + bg.Red() * 229) / 255),
                static_cast<unsigned char>((accent.Green() * 26 + bg.Green() * 229) / 255),
                static_cast<unsigned char>((accent.Blue() * 26 + bg.Blue() * 229) / 255));
            dc.SetBrush(wxBrush(blended));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRectangle(0, row_top, row_w, kRowHeight);
        }

        // Icon
        if (node.is_folder())
        {
            DrawFolderIcon(dc, node.is_open, x, icon_y);
        }
        else
        {
            DrawFileIcon(dc, x, icon_y);
        }

        // Text
        int text_x = x + kIconSize + kIconTextGap;
        if (is_selected)
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::AccentPrimary));
        }
        else if (is_hovered)
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMain));
        }
        else
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMuted));
        }

        // Truncate text with ellipsis if it overflows
        int max_text_width = row_w - text_x - kLeftPadding;
        if (node.is_folder())
        {
            max_text_width -= (kChevronSize + kIconTextGap);
        }
        wxString display_name = node.name;
        auto text_extent = dc.GetTextExtent(display_name);
        if (text_extent.GetWidth() > max_text_width && max_text_width > 0)
        {
            // Simple ellipsis truncation
            while (display_name.length() > 1)
            {
                display_name = display_name.Left(display_name.length() - 1);
                if (dc.GetTextExtent(display_name + "...").GetWidth() <= max_text_width)
                {
                    display_name += "...";
                    break;
                }
            }
        }
        dc.DrawText(display_name, text_x, text_y);

        // Chevron for folders (right-aligned)
        if (node.is_folder())
        {
            int chevron_x = row_w - kLeftPadding - kChevronSize;
            int chevron_y = row_top + (kRowHeight - kChevronSize) / 2;
            DrawChevron(dc, node.is_open, chevron_x, chevron_y);
        }
    }

    y_offset += kRowHeight;

    // Draw children if folder is open
    if (node.is_folder() && node.is_open)
    {
        for (const auto& child : node.children)
        {
            DrawNode(dc, child, depth + 1, y_offset);
        }
    }
}

void FileTreeCtrl::DrawFolderIcon(wxDC& dc, bool is_open, int x, int y)
{
    auto muted = theme_engine().color(core::ThemeColorToken::TextMuted);
    // 70% opacity blend with background
    auto bg = theme_engine().color(core::ThemeColorToken::BgPanel);
    wxColour icon_color(static_cast<unsigned char>((muted.Red() * 179 + bg.Red() * 76) / 255),
                        static_cast<unsigned char>((muted.Green() * 179 + bg.Green() * 76) / 255),
                        static_cast<unsigned char>((muted.Blue() * 179 + bg.Blue() * 76) / 255));

    dc.SetPen(wxPen(icon_color, 1));
    dc.SetBrush(wxBrush(icon_color));

    if (is_open)
    {
        // Open folder: tab on top-left, body angled
        wxPoint tab[] = {
            wxPoint(x, y + 3), wxPoint(x, y + 1), wxPoint(x + 5, y + 1), wxPoint(x + 7, y + 3)};
        dc.DrawPolygon(4, tab);

        wxPoint body[] = {wxPoint(x, y + 3),
                          wxPoint(x + kIconSize - 1, y + 3),
                          wxPoint(x + kIconSize - 3, y + kIconSize - 1),
                          wxPoint(x + 2, y + kIconSize - 1)};
        dc.DrawPolygon(4, body);
    }
    else
    {
        // Closed folder: tab on top, simple rectangle body
        wxPoint tab[] = {
            wxPoint(x, y + 3), wxPoint(x, y + 1), wxPoint(x + 5, y + 1), wxPoint(x + 7, y + 3)};
        dc.DrawPolygon(4, tab);

        dc.DrawRectangle(x, y + 3, kIconSize, kIconSize - 4);
    }
}

void FileTreeCtrl::DrawFileIcon(wxDC& dc, int x, int y)
{
    auto muted = theme_engine().color(core::ThemeColorToken::TextMuted);
    auto bg = theme_engine().color(core::ThemeColorToken::BgPanel);
    wxColour icon_color(static_cast<unsigned char>((muted.Red() * 179 + bg.Red() * 76) / 255),
                        static_cast<unsigned char>((muted.Green() * 179 + bg.Green() * 76) / 255),
                        static_cast<unsigned char>((muted.Blue() * 179 + bg.Blue() * 76) / 255));

    dc.SetPen(wxPen(icon_color, 1));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    // Document with folded corner
    int w = 10;
    int h = kIconSize;
    int fold = 3;
    int bx = x + (kIconSize - w) / 2;

    // Main outline (minus the folded corner)
    wxPoint outline[] = {wxPoint(bx, y),
                         wxPoint(bx + w - fold, y),
                         wxPoint(bx + w, y + fold),
                         wxPoint(bx + w, y + h),
                         wxPoint(bx, y + h)};
    dc.DrawPolygon(5, outline);

    // Fold line
    dc.DrawLine(bx + w - fold, y, bx + w - fold, y + fold);
    dc.DrawLine(bx + w - fold, y + fold, bx + w, y + fold);
}

void FileTreeCtrl::DrawChevron(wxDC& dc, bool is_open, int x, int y)
{
    auto muted = theme_engine().color(core::ThemeColorToken::TextMuted);
    // 50% opacity
    auto bg = theme_engine().color(core::ThemeColorToken::BgPanel);
    wxColour chevron_color(
        static_cast<unsigned char>((muted.Red() * 128 + bg.Red() * 127) / 255),
        static_cast<unsigned char>((muted.Green() * 128 + bg.Green() * 127) / 255),
        static_cast<unsigned char>((muted.Blue() * 128 + bg.Blue() * 127) / 255));

    dc.SetPen(wxPen(chevron_color, 1));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    int cx = x + kChevronSize / 2;
    int cy = y + kChevronSize / 2;

    if (is_open)
    {
        // Down chevron: v shape
        dc.DrawLine(cx - 3, cy - 2, cx, cy + 2);
        dc.DrawLine(cx, cy + 2, cx + 3, cy - 2);
    }
    else
    {
        // Right chevron: > shape
        dc.DrawLine(cx - 2, cy - 3, cx + 2, cy);
        dc.DrawLine(cx + 2, cy, cx - 2, cy + 3);
    }
}

// --- Interaction ---

void FileTreeCtrl::OnMouseMove(wxMouseEvent& event)
{
    auto hit = HitTest(event.GetPosition());
    std::string new_hovered = hit.node != nullptr ? hit.node->id : "";

    if (new_hovered != hovered_node_id_)
    {
        hovered_node_id_ = new_hovered;
        Refresh();
    }
}

void FileTreeCtrl::OnMouseDown(wxMouseEvent& event)
{
    auto hit = HitTest(event.GetPosition());

    if (hit.node == nullptr)
    {
        return;
    }

    if (hit.node->is_folder())
    {
        // Toggle folder expand/collapse
        hit.node->is_open = !hit.node->is_open;
        UpdateVirtualHeight();
        Refresh();
    }
    else
    {
        // Select file
        if (active_file_id_ != hit.node->id)
        {
            active_file_id_ = hit.node->id;
            Refresh();

            if (on_file_select_)
            {
                on_file_select_(*hit.node);
            }

            // Publish event
            core::events::ActiveFileChangedEvent evt;
            evt.file_id = hit.node->id;
            event_bus_.publish(evt);

            MARKAMP_LOG_DEBUG("File selected: {}", hit.node->name);
        }
    }
}

void FileTreeCtrl::OnMouseLeave(wxMouseEvent& /*event*/)
{
    if (!hovered_node_id_.empty())
    {
        hovered_node_id_.clear();
        Refresh();
    }
}

// --- Hit testing ---

auto FileTreeCtrl::HitTest(const wxPoint& point) -> HitResult
{
    int y_offset = -scroll_offset_;
    return HitTestRecursive(point, roots_, 0, y_offset);
}

auto FileTreeCtrl::HitTestRecursive(const wxPoint& point,
                                    std::vector<core::FileNode>& nodes,
                                    int depth,
                                    int& y_offset) -> HitResult
{
    auto sz = GetClientSize();

    for (auto& node : nodes)
    {
        int row_top = y_offset;
        y_offset += kRowHeight;

        if (point.y >= row_top && point.y < row_top + kRowHeight)
        {
            bool on_chevron = false;
            if (node.is_folder())
            {
                int chevron_x = sz.GetWidth() - kLeftPadding - kChevronSize;
                on_chevron = (point.x >= chevron_x);
            }
            return {&node, on_chevron};
        }

        if (node.is_folder() && node.is_open)
        {
            auto result = HitTestRecursive(point, node.children, depth + 1, y_offset);
            if (result.node != nullptr)
            {
                return result;
            }
        }
    }

    return {nullptr, false};
}

// --- Scrolling ---

void FileTreeCtrl::UpdateVirtualHeight()
{
    // Count visible nodes
    std::function<int(const std::vector<core::FileNode>&)> count_visible;
    count_visible = [&](const std::vector<core::FileNode>& nodes) -> int
    {
        int count = 0;
        for (const auto& node : nodes)
        {
            count += 1;
            if (node.is_folder() && node.is_open)
            {
                count += count_visible(node.children);
            }
        }
        return count;
    };

    virtual_height_ = count_visible(roots_) * kRowHeight;
}

void FileTreeCtrl::OnScroll(wxMouseEvent& event)
{
    int delta = event.GetWheelRotation();
    int scroll_amount = kRowHeight * 2; // Scroll 2 rows at a time

    if (delta > 0)
    {
        scroll_offset_ = std::max(0, scroll_offset_ - scroll_amount);
    }
    else if (delta < 0)
    {
        int max_scroll = std::max(0, virtual_height_ - GetClientSize().GetHeight());
        scroll_offset_ = std::min(max_scroll, scroll_offset_ + scroll_amount);
    }

    Refresh();
}

// --- Theme ---

void FileTreeCtrl::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    Refresh();
}

} // namespace markamp::ui
