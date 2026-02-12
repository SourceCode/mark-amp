#include "FileTreeCtrl.h"

#include "core/Events.h"
#include "core/Logger.h"

#include <wx/dcbuffer.h>
#include <wx/filename.h>
#include <wx/graphics.h>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

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

    LoadIcons();
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

// --- Filtering ---

void FileTreeCtrl::ApplyFilter(const std::string& filter)
{
    filter_text_ = filter;

    if (filter.empty())
    {
        ClearFilter();
        return;
    }

    // Lowercase the filter for case-insensitive match
    std::string lower_filter = filter;
    std::transform(lower_filter.begin(),
                   lower_filter.end(),
                   lower_filter.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    ApplyFilterRecursive(roots_, lower_filter);
    scroll_offset_ = 0;
    UpdateVirtualHeight();
    Refresh();
}

void FileTreeCtrl::ClearFilter()
{
    filter_text_.clear();

    // Mark all nodes visible
    std::function<void(std::vector<core::FileNode>&)> reset_visible;
    reset_visible = [&](std::vector<core::FileNode>& nodes)
    {
        for (auto& node : nodes)
        {
            node.filter_visible = true;
            if (node.is_folder())
            {
                reset_visible(node.children);
            }
        }
    };

    reset_visible(roots_);
    UpdateVirtualHeight();
    Refresh();
}

auto FileTreeCtrl::MatchesFilter(const core::FileNode& node, const std::string& lower_filter) const
    -> bool
{
    std::string lower_name = node.name;
    std::transform(lower_name.begin(),
                   lower_name.end(),
                   lower_name.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
    return lower_name.find(lower_filter) != std::string::npos;
}

void FileTreeCtrl::ApplyFilterRecursive(std::vector<core::FileNode>& nodes,
                                        const std::string& lower_filter)
{
    for (auto& node : nodes)
    {
        if (node.is_folder())
        {
            // Recurse into children first
            ApplyFilterRecursive(node.children, lower_filter);

            // Folder is visible if it matches OR any child is visible
            bool has_visible_child =
                std::any_of(node.children.begin(),
                            node.children.end(),
                            [](const core::FileNode& child) { return child.filter_visible; });

            node.filter_visible = MatchesFilter(node, lower_filter) || has_visible_child;

            // Auto-expand folders with visible children during filtering
            if (has_visible_child)
            {
                node.is_open = true;
            }
        }
        else
        {
            node.filter_visible = MatchesFilter(node, lower_filter);
        }
    }
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
        if (node.filter_visible)
        {
            DrawNode(dc, node, 0, y_offset);
        }
    }
}

void FileTreeCtrl::LoadIcons()
{
    // Get current theme text color
    auto text_color = theme_engine().color(core::ThemeColorToken::TextMain);
    std::string hex_color = text_color.GetAsString(wxC2S_HTML_SYNTAX).ToStdString();

    // Helpers to load SVG from resources
    auto load_svg = [&](const std::string& name) -> wxBitmapBundle
    {
        std::vector<std::string> search_paths = {"resources/icons/lucide/" + name + ".svg",
                                                 "../resources/icons/lucide/" + name + ".svg",
                                                 "../../resources/icons/lucide/" + name + ".svg"};

        for (const auto& path : search_paths)
        {
            if (wxFileName::FileExists(path))
            {
                std::ifstream file(path);
                if (file)
                {
                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    std::string content = buffer.str();

                    // Replace "currentColor" with theme color
                    size_t pos = 0;
                    while ((pos = content.find("currentColor", pos)) != std::string::npos)
                    {
                        content.replace(pos, 12, hex_color);
                        pos += hex_color.length();
                    }

                    // VS Code icons are typically 16x16.
                    return wxBitmapBundle::FromSVG(content.c_str(), wxSize(kIconSize, kIconSize));
                }
            }
        }
        return wxBitmapBundle();
    };

    icon_folder_ = load_svg("folder");
    icon_folder_open_ = load_svg("folder-open");
    icon_file_ = load_svg("file");
    icon_file_text_ = load_svg("file-text");
    icon_chevron_right_ = load_svg("chevron-right");
    icon_chevron_down_ = load_svg("chevron-down");
}

void FileTreeCtrl::DrawNode(wxDC& dc, const core::FileNode& node, int depth, int& y_offset)
{
    auto sz = GetClientSize();
    int row_top = y_offset;
    int row_w = sz.GetWidth();

    // Only draw if the row is visible
    if (row_top + kRowHeight > 0 && row_top < sz.GetHeight())
    {
        // VS Code style layout:
        // [Indent] [Twistie] [Icon] [Text]
        // Twistie is always present in the slot, but only drawn for folders.
        // Icon is always present.

        int content_x = kLeftPadding + depth * kIndentWidth;
        int twistie_x = content_x;
        int icon_x = twistie_x + kTwistieSize;
        int text_x = icon_x + kIconSize + kIconTextGap;

        // Centering vertically
        int text_y = row_top + (kRowHeight - dc.GetCharHeight()) / 2;
        int icon_y = row_top + (kRowHeight - kIconSize) / 2;
        int twistie_y = row_top + (kRowHeight - kTwistieSize) / 2;

        bool is_selected = (node.id == active_file_id_ && node.is_file());
        bool is_hovered = (node.id == hovered_node_id_);

        // Row background
        // VS Code uses full row selection
        if (is_selected)
        {
            dc.SetBrush(wxBrush(theme_engine()
                                    .color(core::ThemeColorToken::AccentPrimary)
                                    .ChangeLightness(180))); // Lighter accent
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRectangle(0, row_top, row_w, kRowHeight);
        }
        else if (is_hovered)
        {
            dc.SetBrush(wxBrush(theme_engine()
                                    .color(core::ThemeColorToken::BgPanel)
                                    .ChangeLightness(110))); // Slightly lighter bg
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRectangle(0, row_top, row_w, kRowHeight);
        }

        // 1. Draw Twistie (Chevron) - LEFT ALIGNED now
        if (node.is_folder())
        {
            wxBitmapBundle* chevron_bundle =
                node.is_open ? &icon_chevron_down_ : &icon_chevron_right_;
            if (chevron_bundle && chevron_bundle->IsOk())
            {
                // Draw chevron slightly smaller or centered in the 16px slot
                wxBitmap bitmap = chevron_bundle->GetBitmap(wxSize(kTwistieSize, kTwistieSize));
                dc.DrawBitmap(bitmap, twistie_x, twistie_y, true);
            }
        }

        // 2. Draw Icon
        wxBitmapBundle* icon_bundle = nullptr;
        if (node.is_folder())
        {
            icon_bundle = node.is_open ? &icon_folder_open_ : &icon_folder_;
        }
        else
        {
            // Simple extension check
            if (node.name.find(".md") != std::string::npos ||
                node.name.find(".txt") != std::string::npos)
            {
                icon_bundle = &icon_file_text_;
            }
            else
            {
                icon_bundle = &icon_file_;
            }
        }

        if (icon_bundle && icon_bundle->IsOk())
        {
            wxBitmap bitmap = icon_bundle->GetBitmap(wxSize(kIconSize, kIconSize));
            dc.DrawBitmap(bitmap, icon_x, icon_y, true);
        }

        // 3. Draw Text
        if (is_selected)
        {
            // Selected text color often contrasts
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMain));
        }
        else if (is_hovered)
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMain));
        }
        else
        {
            // Muted for normal state if consistent with theme, or TextMain
            dc.SetTextForeground(theme_engine().color(
                core::ThemeColorToken::TextMain)); // VS Code usually uses main text color
        }

        // Truncate text with ellipsis if it overflows
        int max_text_width = row_w - text_x - kLeftPadding;
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
        // OBSOLETE: Chevron is now left-aligned and drawn above.
    }

    y_offset += kRowHeight;

    // Draw children if folder is open
    if (node.is_folder() && node.is_open)
    {
        for (const auto& child : node.children)
        {
            if (child.filter_visible)
            {
                DrawNode(dc, child, depth + 1, y_offset);
            }
        }
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
    for (auto& node : nodes)
    {
        if (!node.filter_visible)
        {
            continue;
        }

        int row_top = y_offset;
        y_offset += kRowHeight;

        if (point.y >= row_top && point.y < row_top + kRowHeight)
        {
            bool on_chevron = false;
            if (node.is_folder())
            {
                // VS Code style: Click anywhere on the twistie area to toggle
                // Twistie is at: kLeftPadding + depth * kIndentWidth
                int twistie_x = kLeftPadding + depth * kIndentWidth;

                // Allow clicking slightly wider area for usability?
                // VS Code allows clicking the icon to select, twistie to toggle.
                // Twistie width is 16px.

                on_chevron = (point.x >= twistie_x && point.x < twistie_x + kTwistieSize);
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
            if (!node.filter_visible)
            {
                continue;
            }
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
    LoadIcons();
    Refresh();
}

} // namespace markamp::ui
