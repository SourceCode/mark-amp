#include "FileTreeCtrl.h"

#include "core/Events.h"
#include "core/Logger.h"

#include <wx/clipbrd.h>
#include <wx/dcbuffer.h>
#include <wx/filename.h>
#include <wx/graphics.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/textdlg.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
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
    Bind(wxEVT_LEFT_DCLICK, &FileTreeCtrl::OnDoubleClick, this);
    Bind(wxEVT_RIGHT_DOWN, &FileTreeCtrl::OnRightClick, this);
    Bind(wxEVT_LEAVE_WINDOW, &FileTreeCtrl::OnMouseLeave, this);
    Bind(wxEVT_MOUSEWHEEL, &FileTreeCtrl::OnScroll, this);
    Bind(wxEVT_KEY_DOWN, &FileTreeCtrl::OnKeyDown, this);

    // Allow focus for keyboard events
    SetCanFocus(true);

    // R3 Fix 4: Type-ahead timer — clears buffer after 500ms
    type_ahead_timer_.SetOwner(this);
    Bind(wxEVT_TIMER, &FileTreeCtrl::OnTypeAheadTimerExpired, this);

    LoadIcons();
}

void FileTreeCtrl::SetFileTree(const std::vector<core::FileNode>& roots)
{
    roots_ = roots;

    // R17 Fix 19: Sort folders before files (recursive)
    std::function<void(std::vector<core::FileNode>&)> sort_recursive;
    sort_recursive = [&sort_recursive](std::vector<core::FileNode>& nodes)
    {
        std::sort(nodes.begin(),
                  nodes.end(),
                  [](const core::FileNode& left, const core::FileNode& right)
                  {
                      if (left.is_folder() != right.is_folder())
                      {
                          return left.is_folder(); // folders first
                      }
                      // Case-insensitive alphabetical within each group
                      std::string l_name = left.name;
                      std::string r_name = right.name;
                      std::transform(l_name.begin(),
                                     l_name.end(),
                                     l_name.begin(),
                                     [](unsigned char chr)
                                     { return static_cast<char>(std::tolower(chr)); });
                      std::transform(r_name.begin(),
                                     r_name.end(),
                                     r_name.begin(),
                                     [](unsigned char chr)
                                     { return static_cast<char>(std::tolower(chr)); });
                      return l_name < r_name;
                  });
        for (auto& node : nodes)
        {
            if (node.is_folder())
            {
                sort_recursive(node.children);
            }
        }
    };
    sort_recursive(roots_);

    UpdateVirtualHeight();
    Refresh();
}

void FileTreeCtrl::SetActiveFileId(const std::string& file_id)
{
    if (active_file_id_ != file_id)
    {
        active_file_id_ = file_id;

        // R3 Fix 2: Auto-expand parent folders so the node is visible
        ExpandAncestors(file_id);
        UpdateVirtualHeight();

        EnsureNodeVisible(file_id);
        Refresh();
    }
}

// Fix 1: Auto-scroll so node_id is visible
void FileTreeCtrl::EnsureNodeVisible(const std::string& node_id)
{
    auto visible = GetVisibleNodes();
    int row_index = 0;
    for (const auto* node : visible)
    {
        if (node->id == node_id)
        {
            const int node_top = row_index * kRowHeight;
            const int node_bottom = node_top + kRowHeight;
            const int client_height = GetClientSize().GetHeight();

            if (node_top < scroll_offset_)
            {
                scroll_offset_ = node_top;
            }
            else if (node_bottom > scroll_offset_ + client_height)
            {
                scroll_offset_ = node_bottom - client_height;
            }
            return;
        }
        ++row_index;
    }
}

// R4 Fix 15: Collapse all folders in the tree
void FileTreeCtrl::CollapseAllNodes()
{
    std::function<void(core::FileNode&)> collapse_recursive;
    collapse_recursive = [&collapse_recursive](core::FileNode& target)
    {
        if (target.is_folder())
        {
            target.is_open = false;
            for (auto& child : target.children)
            {
                collapse_recursive(child);
            }
        }
    };
    for (auto& root : roots_)
    {
        collapse_recursive(root);
    }
    UpdateVirtualHeight();
    Refresh();
}

void FileTreeCtrl::SetOnFileSelect(FileSelectCallback callback)
{
    on_file_select_ = std::move(callback);
}

void FileTreeCtrl::SetOnFileOpen(FileOpenCallback callback)
{
    on_file_open_ = std::move(callback);
}

void FileTreeCtrl::SetWorkspaceRoot(const std::string& root_path)
{
    workspace_root_ = root_path;
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

        bool is_selected = (node.id == active_file_id_);
        bool is_hovered = (node.id == hovered_node_id_);

        // Fix 6: Draw indent guide lines (VS Code style vertical lines at each indent level)
        if (depth > 0)
        {
            dc.SetPen(wxPen(
                theme_engine().color(core::ThemeColorToken::BorderLight).ChangeLightness(90), 1));
            for (int guide_depth = 1; guide_depth <= depth; ++guide_depth)
            {
                const int guide_x = kLeftPadding + guide_depth * kIndentWidth - (kIndentWidth / 2);
                dc.DrawLine(guide_x, row_top, guide_x, row_top + kRowHeight);
            }
        }

        // Row background
        // VS Code uses full row selection
        if (is_selected)
        {
            dc.SetBrush(wxBrush(theme_engine()
                                    .color(core::ThemeColorToken::AccentPrimary)
                                    .ChangeLightness(180))); // Lighter accent
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRectangle(0, row_top, row_w, kRowHeight);

            // R16 Fix 25: 2px accent left border on selected row
            dc.SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::AccentPrimary)));
            dc.DrawRectangle(0, row_top, 2, kRowHeight);
        }
        else if (is_hovered)
        {
            dc.SetBrush(wxBrush(theme_engine()
                                    .color(core::ThemeColorToken::BgPanel)
                                    .ChangeLightness(110))); // Slightly lighter bg
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRectangle(0, row_top, row_w, kRowHeight);
        }

        // R16 Fix 26: Faint bottom border on each row
        dc.SetPen(
            wxPen(theme_engine().color(core::ThemeColorToken::BorderLight).ChangeLightness(95), 1));
        dc.DrawLine(content_x, row_top + kRowHeight - 1, row_w, row_top + kRowHeight - 1);

        // R3 Fix 3: Focus ring for keyboard navigation
        if (node.id == GetFocusedNodeId())
        {
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            wxPen focus_pen(theme_engine().color(core::ThemeColorToken::AccentPrimary),
                            1,
                            wxPENSTYLE_SHORT_DASH);
            dc.SetPen(focus_pen);
            dc.DrawRectangle(1, row_top + 1, row_w - 2, kRowHeight - 2);
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
            // Fix 5: Expanded text-file extension check for common editable formats
            const auto& name = node.name;
            const auto has_ext = [&name](const char* ext)
            {
                return name.size() >= std::strlen(ext) &&
                       name.compare(name.size() - std::strlen(ext), std::strlen(ext), ext) == 0;
            };
            if (has_ext(".md") || has_ext(".txt") || has_ext(".json") || has_ext(".yml") ||
                has_ext(".yaml") || has_ext(".toml") || has_ext(".xml") || has_ext(".html") ||
                has_ext(".htm") || has_ext(".css") || has_ext(".js") || has_ext(".ts") ||
                has_ext(".jsx") || has_ext(".tsx") || has_ext(".sh") || has_ext(".py") ||
                has_ext(".rb") || has_ext(".go") || has_ext(".rs") || has_ext(".c") ||
                has_ext(".cpp") || has_ext(".h") || has_ext(".hpp") || has_ext(".java") ||
                has_ext(".swift") || has_ext(".kt") || has_ext(".cfg") || has_ext(".ini") ||
                has_ext(".env") || has_ext(".log") || has_ext(".csv") || has_ext(".sql"))
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
        // Fix 4: Distinct colors for selected vs hovered vs normal
        if (is_selected)
        {
            dc.SetTextForeground(
                theme_engine().color(core::ThemeColorToken::AccentPrimary).ChangeLightness(80));
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

        // R5 Fix 14: Draw file metadata (size or child count) right-aligned in muted text
        {
            std::string meta_text;
            if (node.is_file())
            {
                try
                {
                    const auto fsize = std::filesystem::file_size(node.id);
                    if (fsize < 1024)
                    {
                        meta_text = std::to_string(fsize) + " B";
                    }
                    else if (fsize < 1024 * 1024)
                    {
                        meta_text = std::to_string(fsize / 1024) + " KB";
                    }
                    else
                    {
                        meta_text = std::to_string(fsize / (1024 * 1024)) + " MB";
                    }
                }
                catch (const std::filesystem::filesystem_error& /*err*/)
                {
                    // Untitled file — no size
                }
            }
            else
            {
                const auto child_count = node.children.size();
                meta_text = std::to_string(child_count) + (child_count == 1 ? " item" : " items");
            }

            if (!meta_text.empty())
            {
                dc.SetTextForeground(
                    theme_engine().color(core::ThemeColorToken::TextMuted).ChangeLightness(85));
                auto meta_extent = dc.GetTextExtent(meta_text);
                int meta_x = row_w - meta_extent.GetWidth() - kLeftPadding;
                if (meta_x > text_x + 40) // only draw if there's room
                {
                    dc.DrawText(meta_text, meta_x, text_y);
                }
            }
        }

        // Chevron for folders (right-aligned)
        // OBSOLETE: Chevron is now left-aligned and drawn above.
    }

    y_offset += kRowHeight;

    // Draw children if folder is open
    if (node.is_folder() && node.is_open)
    {
        bool has_visible_children = false;
        for (const auto& child : node.children)
        {
            if (child.filter_visible)
            {
                has_visible_children = true;
                DrawNode(dc, child, depth + 1, y_offset);
            }
        }

        // Fix 8: Show placeholder for empty open folders
        if (!has_visible_children)
        {
            int empty_x = kLeftPadding + (depth + 1) * kIndentWidth + kTwistieSize;
            int empty_y = y_offset + (kRowHeight - dc.GetCharHeight()) / 2;
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMuted));
            dc.DrawText("(empty)", empty_x, empty_y);
            y_offset += kRowHeight;
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

        // Fix 6: Show file size / child count tooltip on hover
        if (hit.node != nullptr)
        {
            // R4 Fix 18: Show relative path instead of absolute
            std::string tip;
            if (!workspace_root_.empty())
            {
                const auto rel = std::filesystem::relative(hit.node->id, workspace_root_);
                tip = rel.string();
            }
            else
            {
                tip = hit.node->id;
            }
            if (hit.node->is_file())
            {
                try
                {
                    const auto file_size = std::filesystem::file_size(hit.node->id);
                    if (file_size < 1024)
                    {
                        tip += "  (" + std::to_string(file_size) + " B)";
                    }
                    else if (file_size < 1024 * 1024)
                    {
                        tip += "  (" + std::to_string(file_size / 1024) + " KB)";
                    }
                    else
                    {
                        tip += "  (" + std::to_string(file_size / (1024 * 1024)) + " MB)";
                    }
                }
                catch (const std::filesystem::filesystem_error& /*err*/)
                {
                    // File may not exist on disk (e.g., untitled)
                }
            }
            else
            {
                const auto num_children = hit.node->children.size();
                tip += "  (" + std::to_string(num_children) + " items)";
            }
            SetToolTip(tip);
        }
        else
        {
            UnsetToolTip();
        }

        // R4 Fix 10: Cursor feedback — hand for all rows (like VS Code)
        if (hit.node != nullptr)
        {
            SetCursor(wxCURSOR_HAND);
        }
        else
        {
            SetCursor(wxCURSOR_DEFAULT);
        }

        Refresh();
    }
}

void FileTreeCtrl::OnMouseDown(wxMouseEvent& event)
{
    // Fix 1: Acquire focus so keyboard navigation works immediately
    SetFocus();

    auto hit = HitTest(event.GetPosition());

    // Fix 7: Click empty area below nodes deselects
    if (hit.node == nullptr)
    {
        active_file_id_.clear();
        focused_node_index_ = -1;
        Refresh();
        return;
    }

    // Sync focused_node_index_ with clicked node
    auto visible = GetVisibleNodes();
    for (size_t idx = 0; idx < visible.size(); ++idx)
    {
        if (visible[idx]->id == hit.node->id)
        {
            focused_node_index_ = static_cast<int>(idx);
            break;
        }
    }

    if (hit.node->is_folder())
    {
        // R3 Fix 1: Clicking anywhere on the folder row toggles open/close
        hit.node->is_open = !hit.node->is_open;
        UpdateVirtualHeight();
        // Always select/highlight the folder
        active_file_id_ = hit.node->id;
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

            // R5 Fix 4: Single-click also opens the file (VS Code behavior)
            if (on_file_open_)
            {
                on_file_open_(*hit.node);
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
        // Fix 4: Reset cursor on leave
        SetCursor(wxCURSOR_DEFAULT);
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

// --- Double-click to open (QoL feature 2) ---

void FileTreeCtrl::OnDoubleClick(wxMouseEvent& event)
{
    auto hit = HitTest(event.GetPosition());
    if (hit.node == nullptr)
    {
        return;
    }

    if (hit.node->is_folder())
    {
        // Double-click on folder toggles it (same as single click)
        hit.node->is_open = !hit.node->is_open;
        UpdateVirtualHeight();
        Refresh();
    }
    else
    {
        // Double-click on file: open in tab
        if (on_file_open_)
        {
            on_file_open_(*hit.node);
        }
        MARKAMP_LOG_DEBUG("File double-clicked (open): {}", hit.node->name);
    }
}

// --- Right-click context menu (QoL feature 6) ---

namespace
{
constexpr int kCtxOpen = 100;
constexpr int kCtxRevealInFinder = 101;
constexpr int kCtxCopyPath = 102;
constexpr int kCtxCopyRelativePath = 103;
constexpr int kCtxCollapseAll = 104;
constexpr int kCtxExpandAll = 105;
constexpr int kCtxNewFile = 106;
constexpr int kCtxDeleteFile = 107;
constexpr int kCtxRename = 108;
constexpr int kCtxNewFolder = 109;
constexpr int kCtxOpenInTerminal = 110;       // R4 Fix 5
constexpr int kCtxOpenContainingFolder = 111; // R4 Fix 6
} // namespace

void FileTreeCtrl::OnRightClick(wxMouseEvent& event)
{
    // Fix 1: Acquire focus on right-click too
    SetFocus();

    auto hit = HitTest(event.GetPosition());
    if (hit.node == nullptr)
    {
        // R2 Fix 1: Show workspace-level context menu on empty area
        ShowEmptyAreaContextMenu();
        return;
    }

    // Select the node
    active_file_id_ = hit.node->id;
    Refresh();

    ShowFileContextMenu(*hit.node);
}

void FileTreeCtrl::ShowFileContextMenu(core::FileNode& node)
{
    wxMenu menu;

    if (node.is_file())
    {
        menu.Append(kCtxOpen, "Open");
        menu.AppendSeparator();
    }
    else
    {
        menu.Append(kCtxExpandAll, "Expand All");
        menu.Append(kCtxCollapseAll, "Collapse All");
        menu.AppendSeparator();
    }

    // R2 Fix 6: "New Folder…" option
    menu.Append(kCtxNewFile, "New File\u2026");
    menu.Append(kCtxNewFolder, "New Folder\u2026");
    menu.AppendSeparator();
    // R2 Fix 3: "Rename…" option — R5 Fix 8: keyboard shortcut hint
    menu.Append(kCtxRename, "Rename\u2026\tF2");
    // R2 Fix 2: "Delete File…" option — R5 Fix 8: keyboard shortcut hint
    menu.Append(kCtxDeleteFile, "Delete\u2026\t\u232B");
    menu.AppendSeparator();
    menu.Append(kCtxRevealInFinder, "Reveal in Finder");
    menu.AppendSeparator();
    menu.Append(kCtxCopyPath, "Copy Path");
    menu.Append(kCtxCopyRelativePath, "Copy Relative Path");

    // R4 Fix 5: Open in Terminal
    menu.AppendSeparator();
    menu.Append(kCtxOpenInTerminal, "Open in Terminal");
    // R4 Fix 6: Open Containing Folder (for files, opens parent directory)
    if (node.is_file())
    {
        menu.Append(kCtxOpenContainingFolder, "Open Containing Folder");
    }

    const std::string node_path = node.id;
    const bool is_file = node.is_file();

    menu.Bind(
        wxEVT_MENU,
        [this, node_path, is_file, &node](wxCommandEvent& cmd_event)
        {
            switch (cmd_event.GetId())
            {
                case kCtxOpen:
                    if (is_file && on_file_open_)
                    {
                        on_file_open_(node);
                    }
                    break;
                case kCtxRevealInFinder:
                {
#ifdef __APPLE__
                    wxExecute(wxString::Format("open -R \"%s\"", node_path));
#elif defined(__linux__)
                    wxExecute(
                        wxString::Format("xdg-open \"%s\"",
                                         std::filesystem::path(node_path).parent_path().string()));
#endif
                    break;
                }
                case kCtxCopyPath:
                    if (wxTheClipboard->Open())
                    {
                        wxTheClipboard->SetData(new wxTextDataObject(node_path));
                        wxTheClipboard->Close();
                    }
                    break;
                case kCtxCopyRelativePath:
                {
                    std::string relative_path = node_path;
                    if (!workspace_root_.empty())
                    {
                        const auto rel = std::filesystem::relative(node_path, workspace_root_);
                        relative_path = rel.string();
                    }
                    if (wxTheClipboard->Open())
                    {
                        wxTheClipboard->SetData(new wxTextDataObject(relative_path));
                        wxTheClipboard->Close();
                    }
                    break;
                }
                case kCtxCollapseAll:
                {
                    // Recursively collapse all folders
                    std::function<void(core::FileNode&)> collapse_all;
                    collapse_all = [&collapse_all](core::FileNode& target)
                    {
                        if (target.is_folder())
                        {
                            target.is_open = false;
                            for (auto& child : target.children)
                            {
                                collapse_all(child);
                            }
                        }
                    };
                    collapse_all(node);
                    UpdateVirtualHeight();
                    Refresh();
                    break;
                }
                case kCtxNewFile:
                {
                    wxTextEntryDialog name_dlg(this, "Enter file name:", "New File", "untitled.md");
                    if (name_dlg.ShowModal() == wxID_OK)
                    {
                        const std::string file_name = name_dlg.GetValue().ToStdString();
                        std::string dir_path = node_path;
                        if (is_file)
                        {
                            dir_path = std::filesystem::path(node_path).parent_path().string();
                        }
                        const std::string new_file_path =
                            (std::filesystem::path(dir_path) / file_name).string();

                        // Create the file on disk
                        std::ofstream new_stream(new_file_path);
                        new_stream.close();

                        // Open in tab via the file open callback
                        if (on_file_open_)
                        {
                            core::FileNode new_node;
                            new_node.id = new_file_path;
                            new_node.name = file_name;
                            new_node.type = core::FileNodeType::File;
                            on_file_open_(new_node);
                        }

                        // R2 Fix 5: Trigger workspace refresh
                        core::events::WorkspaceRefreshRequestEvent refresh_evt;
                        event_bus_.publish(refresh_evt);
                    }
                    break;
                }
                case kCtxExpandAll:
                {
                    // Recursively expand all folders
                    std::function<void(core::FileNode&)> expand_all;
                    expand_all = [&expand_all](core::FileNode& target)
                    {
                        if (target.is_folder())
                        {
                            target.is_open = true;
                            for (auto& child : target.children)
                            {
                                expand_all(child);
                            }
                        }
                    };
                    expand_all(node);
                    UpdateVirtualHeight();
                    Refresh();
                    break;
                }
                // R2 Fix 2: Delete file/folder
                case kCtxDeleteFile:
                {
                    const std::string display_name =
                        std::filesystem::path(node_path).filename().string();
                    const wxString confirm_msg =
                        is_file ? wxString::Format("Delete file '%s'?", display_name)
                                : wxString::Format("Delete folder '%s' and all its contents?",
                                                   display_name);

                    const int result = wxMessageBox(
                        confirm_msg, "Confirm Delete", wxYES_NO | wxICON_WARNING, this);

                    if (result == wxYES)
                    {
                        std::error_code err_code;
                        if (is_file)
                        {
                            std::filesystem::remove(node_path, err_code);
                        }
                        else
                        {
                            std::filesystem::remove_all(node_path, err_code);
                        }

                        if (!err_code)
                        {
                            core::events::WorkspaceRefreshRequestEvent refresh_evt;
                            event_bus_.publish(refresh_evt);
                        }
                    }
                    break;
                }
                // R2 Fix 3: Rename file/folder
                case kCtxRename:
                {
                    const std::string current_name =
                        std::filesystem::path(node_path).filename().string();
                    wxTextEntryDialog rename_dlg(
                        this, "New name:", "Rename", wxString(current_name));
                    if (rename_dlg.ShowModal() == wxID_OK)
                    {
                        const std::string new_name = rename_dlg.GetValue().ToStdString();
                        if (!new_name.empty() && new_name != current_name)
                        {
                            const std::string new_path =
                                (std::filesystem::path(node_path).parent_path() / new_name)
                                    .string();
                            std::error_code err_code;
                            std::filesystem::rename(node_path, new_path, err_code);
                            if (!err_code)
                            {
                                core::events::WorkspaceRefreshRequestEvent refresh_evt;
                                event_bus_.publish(refresh_evt);
                            }
                        }
                    }
                    break;
                }
                // R2 Fix 6: New Folder
                case kCtxNewFolder:
                {
                    wxTextEntryDialog folder_dlg(
                        this, "Enter folder name:", "New Folder", "new-folder");
                    if (folder_dlg.ShowModal() == wxID_OK)
                    {
                        const std::string folder_name = folder_dlg.GetValue().ToStdString();
                        std::string dir_path = node_path;
                        if (is_file)
                        {
                            dir_path = std::filesystem::path(node_path).parent_path().string();
                        }
                        const std::string new_dir_path =
                            (std::filesystem::path(dir_path) / folder_name).string();

                        std::error_code err_code;
                        std::filesystem::create_directory(new_dir_path, err_code);
                        if (!err_code)
                        {
                            core::events::WorkspaceRefreshRequestEvent refresh_evt;
                            event_bus_.publish(refresh_evt);
                        }
                    }
                    break;
                }
                // R4 Fix 5: Open in Terminal
                case kCtxOpenInTerminal:
                {
                    std::string dir_path = node_path;
                    if (is_file)
                    {
                        dir_path = std::filesystem::path(node_path).parent_path().string();
                    }
#ifdef __APPLE__
                    wxExecute(wxString::Format("open -a Terminal \"%s\"", dir_path));
#elif defined(__linux__)
                    wxExecute(wxString::Format("x-terminal-emulator --working-directory=\"%s\"",
                                               dir_path));
#endif
                    break;
                }
                // R4 Fix 6: Open Containing Folder (opens parent in Finder)
                case kCtxOpenContainingFolder:
                {
                    const std::string parent_dir =
                        std::filesystem::path(node_path).parent_path().string();
#ifdef __APPLE__
                    wxExecute(wxString::Format("open \"%s\"", parent_dir));
#elif defined(__linux__)
                    wxExecute(wxString::Format("xdg-open \"%s\"", parent_dir));
#endif
                    break;
                }
                default:
                    break;
            }
        });

    PopupMenu(&menu);
}

// --- Keyboard navigation (QoL feature 7) ---

void FileTreeCtrl::OnKeyDown(wxKeyEvent& event)
{
    const int key_code = event.GetKeyCode();
    auto visible_nodes = GetVisibleNodes();

    if (visible_nodes.empty())
    {
        event.Skip();
        return;
    }

    // Clamp focused index
    if (focused_node_index_ < 0)
    {
        focused_node_index_ = 0;
    }
    if (focused_node_index_ >= static_cast<int>(visible_nodes.size()))
    {
        focused_node_index_ = static_cast<int>(visible_nodes.size()) - 1;
    }

    switch (key_code)
    {
        case WXK_UP:
            if (focused_node_index_ > 0)
            {
                --focused_node_index_;
                auto* up_node = visible_nodes[static_cast<size_t>(focused_node_index_)];
                active_file_id_ = up_node->id;
                EnsureNodeVisible(active_file_id_);
                Refresh();
                // Fix 2: Fire select callback so keyboard nav opens files
                if (up_node->is_file() && on_file_select_)
                {
                    on_file_select_(*up_node);
                }
            }
            break;

        case WXK_DOWN:
            if (focused_node_index_ < static_cast<int>(visible_nodes.size()) - 1)
            {
                ++focused_node_index_;
                auto* down_node = visible_nodes[static_cast<size_t>(focused_node_index_)];
                active_file_id_ = down_node->id;
                EnsureNodeVisible(active_file_id_);
                Refresh();
                // Fix 2: Fire select callback so keyboard nav opens files
                if (down_node->is_file() && on_file_select_)
                {
                    on_file_select_(*down_node);
                }
            }
            break;

        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
        {
            auto* node = visible_nodes[static_cast<size_t>(focused_node_index_)];
            if (node->is_folder())
            {
                node->is_open = !node->is_open;
                UpdateVirtualHeight();
                Refresh();
            }
            else if (on_file_open_)
            {
                on_file_open_(*node);
            }
            break;
        }

        case WXK_SPACE:
        {
            auto* node = visible_nodes[static_cast<size_t>(focused_node_index_)];
            if (node->is_folder())
            {
                node->is_open = !node->is_open;
                UpdateVirtualHeight();
                Refresh();
            }
            break;
        }

        case WXK_RIGHT:
        {
            auto* node = visible_nodes[static_cast<size_t>(focused_node_index_)];
            if (node->is_folder() && !node->is_open)
            {
                node->is_open = true;
                UpdateVirtualHeight();
                Refresh();
            }
            break;
        }

        case WXK_LEFT:
        {
            auto* node = visible_nodes[static_cast<size_t>(focused_node_index_)];
            if (node->is_folder() && node->is_open)
            {
                // Collapse open folder
                node->is_open = false;
                UpdateVirtualHeight();
                Refresh();
            }
            else
            {
                // Fix 3: Navigate to parent folder (like VS Code)
                const int parent_idx = FindParentIndex(visible_nodes, focused_node_index_);
                if (parent_idx >= 0)
                {
                    focused_node_index_ = parent_idx;
                    active_file_id_ = visible_nodes[static_cast<size_t>(parent_idx)]->id;
                    EnsureNodeVisible(active_file_id_);
                    Refresh();
                }
            }
            break;
        }

        // Fix 8: Home/End keys jump to first/last node
        case WXK_HOME:
        {
            if (!visible_nodes.empty())
            {
                focused_node_index_ = 0;
                active_file_id_ = visible_nodes[0]->id;
                EnsureNodeVisible(active_file_id_);
                Refresh();
            }
            break;
        }

        case WXK_END:
        {
            if (!visible_nodes.empty())
            {
                focused_node_index_ = static_cast<int>(visible_nodes.size()) - 1;
                active_file_id_ = visible_nodes[static_cast<size_t>(focused_node_index_)]->id;
                EnsureNodeVisible(active_file_id_);
                Refresh();
            }
            break;
        }

        // R4 Fix 2: Delete key deletes focused file/folder
        case WXK_DELETE:
        case WXK_BACK:
        {
            auto* del_node = visible_nodes[static_cast<size_t>(focused_node_index_)];
            const std::string del_path = del_node->id;
            const std::string del_name = std::filesystem::path(del_path).filename().string();
            const bool del_is_file = del_node->is_file();
            const wxString confirm_msg =
                del_is_file
                    ? wxString::Format("Delete file '%s'?", del_name)
                    : wxString::Format("Delete folder '%s' and all its contents?", del_name);

            const int del_result =
                wxMessageBox(confirm_msg, "Confirm Delete", wxYES_NO | wxICON_WARNING, this);

            if (del_result == wxYES)
            {
                std::error_code err_code;
                if (del_is_file)
                {
                    std::filesystem::remove(del_path, err_code);
                }
                else
                {
                    std::filesystem::remove_all(del_path, err_code);
                }
                if (!err_code)
                {
                    core::events::WorkspaceRefreshRequestEvent refresh_evt;
                    event_bus_.publish(refresh_evt);
                }
            }
            break;
        }

        // R4 Fix 3: F2 key renames focused file/folder
        case WXK_F2:
        {
            auto* rename_node = visible_nodes[static_cast<size_t>(focused_node_index_)];
            const std::string rename_path = rename_node->id;
            const std::string current_name = std::filesystem::path(rename_path).filename().string();
            wxTextEntryDialog rename_dlg(this, "New name:", "Rename", wxString(current_name));
            if (rename_dlg.ShowModal() == wxID_OK)
            {
                const std::string new_name = rename_dlg.GetValue().ToStdString();
                if (!new_name.empty() && new_name != current_name)
                {
                    const std::string new_path =
                        (std::filesystem::path(rename_path).parent_path() / new_name).string();
                    std::error_code err_code;
                    std::filesystem::rename(rename_path, new_path, err_code);
                    if (!err_code)
                    {
                        core::events::WorkspaceRefreshRequestEvent refresh_evt;
                        event_bus_.publish(refresh_evt);
                    }
                }
            }
            break;
        }

        default:
        {
            // R3 Fix 4: Type-ahead search — letter keys jump to matching node
            const int unicode_key = event.GetUnicodeKey();
            if (unicode_key >= 32 && unicode_key < 127)
            {
                const char typed = static_cast<char>(std::tolower(unicode_key));
                type_ahead_buffer_ += typed;
                type_ahead_timer_.Start(500, wxTIMER_ONE_SHOT);

                // Find first visible node whose name starts with the buffer
                for (size_t idx = 0; idx < visible_nodes.size(); ++idx)
                {
                    std::string lower_name = visible_nodes[idx]->name;
                    std::transform(
                        lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
                    if (lower_name.rfind(type_ahead_buffer_, 0) == 0)
                    {
                        focused_node_index_ = static_cast<int>(idx);
                        active_file_id_ = visible_nodes[idx]->id;
                        EnsureNodeVisible(active_file_id_);
                        Refresh();
                        break;
                    }
                }
            }
            else
            {
                event.Skip();
            }
            break;
        }
    }
}

auto FileTreeCtrl::GetVisibleNodes() -> std::vector<core::FileNode*>
{
    std::vector<core::FileNode*> result;
    CollectVisibleNodes(result, roots_);
    return result;
}

void FileTreeCtrl::CollectVisibleNodes(std::vector<core::FileNode*>& result,
                                       std::vector<core::FileNode>& nodes)
{
    for (auto& node : nodes)
    {
        if (!node.filter_visible)
        {
            continue;
        }
        result.push_back(&node);
        if (node.is_folder() && node.is_open)
        {
            CollectVisibleNodes(result, node.children);
        }
    }
}

// Fix 3: Find the parent folder index in the visible node list
auto FileTreeCtrl::FindParentIndex(const std::vector<core::FileNode*>& visible, int child_index)
    -> int
{
    if (child_index <= 0 || child_index >= static_cast<int>(visible.size()))
    {
        return -1;
    }

    // The child's id contains the parent directory path
    const std::string child_path = visible[static_cast<size_t>(child_index)]->id;
    const std::string parent_dir = std::filesystem::path(child_path).parent_path().string();

    // Walk backward to find a folder whose id matches the parent directory
    for (int idx = child_index - 1; idx >= 0; --idx)
    {
        if (visible[static_cast<size_t>(idx)]->id == parent_dir &&
            visible[static_cast<size_t>(idx)]->is_folder())
        {
            return idx;
        }
    }
    return -1;
}

// R2 Fix 1: Show context menu when right-clicking empty area below the tree
void FileTreeCtrl::ShowEmptyAreaContextMenu()
{
    wxMenu menu;
    menu.Append(kCtxNewFile, "New File\u2026");
    menu.Append(kCtxNewFolder, "New Folder\u2026");
    menu.AppendSeparator();
    menu.Append(kCtxExpandAll, "Expand All");
    menu.Append(kCtxCollapseAll, "Collapse All");

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& cmd_event)
        {
            switch (cmd_event.GetId())
            {
                case kCtxNewFile:
                {
                    wxTextEntryDialog name_dlg(this, "Enter file name:", "New File", "untitled.md");
                    if (name_dlg.ShowModal() == wxID_OK)
                    {
                        const std::string file_name = name_dlg.GetValue().ToStdString();
                        if (!workspace_root_.empty())
                        {
                            const std::string new_file_path =
                                (std::filesystem::path(workspace_root_) / file_name).string();
                            std::ofstream new_stream(new_file_path);
                            new_stream.close();

                            if (on_file_open_)
                            {
                                core::FileNode new_node;
                                new_node.id = new_file_path;
                                new_node.name = file_name;
                                new_node.type = core::FileNodeType::File;
                                on_file_open_(new_node);
                            }

                            core::events::WorkspaceRefreshRequestEvent refresh_evt;
                            event_bus_.publish(refresh_evt);
                        }
                    }
                    break;
                }
                case kCtxNewFolder:
                {
                    wxTextEntryDialog folder_dlg(
                        this, "Enter folder name:", "New Folder", "new-folder");
                    if (folder_dlg.ShowModal() == wxID_OK)
                    {
                        const std::string folder_name = folder_dlg.GetValue().ToStdString();
                        if (!workspace_root_.empty())
                        {
                            const std::string new_dir_path =
                                (std::filesystem::path(workspace_root_) / folder_name).string();
                            std::error_code err_code;
                            std::filesystem::create_directory(new_dir_path, err_code);
                            if (!err_code)
                            {
                                core::events::WorkspaceRefreshRequestEvent refresh_evt;
                                event_bus_.publish(refresh_evt);
                            }
                        }
                    }
                    break;
                }
                case kCtxExpandAll:
                {
                    std::function<void(core::FileNode&)> expand_all;
                    expand_all = [&expand_all](core::FileNode& target)
                    {
                        if (target.is_folder())
                        {
                            target.is_open = true;
                            for (auto& child : target.children)
                            {
                                expand_all(child);
                            }
                        }
                    };
                    for (auto& root : roots_)
                    {
                        expand_all(root);
                    }
                    UpdateVirtualHeight();
                    Refresh();
                    break;
                }
                case kCtxCollapseAll:
                {
                    std::function<void(core::FileNode&)> collapse_all;
                    collapse_all = [&collapse_all](core::FileNode& target)
                    {
                        if (target.is_folder())
                        {
                            target.is_open = false;
                            for (auto& child : target.children)
                            {
                                collapse_all(child);
                            }
                        }
                    };
                    for (auto& root : roots_)
                    {
                        collapse_all(root);
                    }
                    UpdateVirtualHeight();
                    Refresh();
                    break;
                }
                default:
                    break;
            }
        });

    PopupMenu(&menu);
}

} // namespace markamp::ui

// R3 Fix 4: Clear the type-ahead buffer when the timer fires
void markamp::ui::FileTreeCtrl::OnTypeAheadTimerExpired(wxTimerEvent& /*event*/)
{
    type_ahead_buffer_.clear();
}

// R3 Fix 3: Return the id of the node with keyboard focus
auto markamp::ui::FileTreeCtrl::GetFocusedNodeId() const -> std::string
{
    if (focused_node_index_ < 0)
    {
        return {};
    }
    // We need a const version of GetVisibleNodes; build inline
    std::vector<const core::FileNode*> nodes;
    std::function<void(const std::vector<core::FileNode>&)> collect;
    collect = [&](const std::vector<core::FileNode>& src)
    {
        for (const auto& node : src)
        {
            if (!node.filter_visible)
            {
                continue;
            }
            nodes.push_back(&node);
            if (node.is_folder() && node.is_open)
            {
                collect(node.children);
            }
        }
    };
    collect(roots_);
    if (focused_node_index_ < static_cast<int>(nodes.size()))
    {
        return nodes[static_cast<size_t>(focused_node_index_)]->id;
    }
    return {};
}

// R3 Fix 2: Expand ancestor folders so that a given node_id becomes visible
auto markamp::ui::FileTreeCtrl::ExpandAncestors(const std::string& node_id) -> bool
{
    // Recursively walk the tree; on the way back up, open each ancestor
    std::function<bool(std::vector<core::FileNode>&)> expand;
    expand = [&](std::vector<core::FileNode>& nodes) -> bool
    {
        for (auto& node : nodes)
        {
            if (node.id == node_id)
            {
                return true; // Found
            }
            if (node.is_folder())
            {
                if (expand(node.children))
                {
                    node.is_open = true;
                    return true; // Ancestor
                }
            }
        }
        return false;
    };
    return expand(roots_);
}
