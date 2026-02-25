#include "ui/FileTreeCtrl.h"

#include "FileTreeCtrl.h"
#include "SidebarSkeletonPlaceholder.h"
#include "ThemeAwareWindow.h"
#include "ThemedScrollbar.h"
#include "core/CommandRegistry.h"
#include "core/Events.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "FileTypeIconResolver.h"
#include "IconManager.h"
#include "SidebarSkeletonPlaceholder.h"
#include "core/Logger.h"
#include "ui/FocusManager.h"
#include "ui/FocusRingRenderer.h"
#include "ui/accessibility/AccessibilityController.h"

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
    Bind(wxEVT_SET_FOCUS, &FileTreeCtrl::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &FileTreeCtrl::OnKillFocus, this);
    Bind(wxEVT_LEFT_DCLICK, &FileTreeCtrl::OnDoubleClick, this);
    Bind(wxEVT_RIGHT_DOWN, &FileTreeCtrl::OnRightClick, this);
    Bind(wxEVT_LEAVE_WINDOW, &FileTreeCtrl::OnMouseLeave, this);
    Bind(wxEVT_MOUSEWHEEL, &FileTreeCtrl::OnScroll, this);
    Bind(wxEVT_KEY_DOWN, &FileTreeCtrl::OnKeyDown, this);
    Bind(wxEVT_SIZE, &FileTreeCtrl::OnSize, this);

    Bind(wxEVT_SCROLLWIN_THUMBTRACK, &FileTreeCtrl::OnScrollbarDrag, this);
    Bind(wxEVT_SCROLLWIN_THUMBRELEASE, &FileTreeCtrl::OnScrollbarDrag, this);

    // Allow focus for keyboard events
    SetCanFocus(true);

    // R3 Fix 4: Type-ahead timer — clears buffer after 500ms
    type_ahead_timer_.SetOwner(this);
    Bind(wxEVT_TIMER, &FileTreeCtrl::OnTypeAheadTimerExpired, this);

    keyboard_mode_sub_ = event_bus_.subscribe<core::events::KeyboardModeChangedEvent>(
        [this](const core::events::KeyboardModeChangedEvent& /*evt*/) { Refresh(); });

    loading_skeleton_ = new SidebarSkeletonPlaceholder(
        this, theme_engine, SidebarSkeletonPlaceholder::Style::kList);
    loading_skeleton_->Hide();

    scrollbar_ = new ThemedScrollbar(this, theme_engine, this);
    scrollbar_->Hide();

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

void FileTreeCtrl::OnSize(wxSizeEvent& event)
{
    auto sz = GetClientSize();
    if (loading_skeleton_ != nullptr)
    {
        loading_skeleton_->SetSize(sz);
    }

    if (scrollbar_ != nullptr)
    {
        scrollbar_->SetSize(
            sz.GetWidth() - ThemedScrollbar::kWidth, 0, ThemedScrollbar::kWidth, sz.GetHeight());
    }

    UpdateScrollbar();
    event.Skip();
}

void FileTreeCtrl::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    auto sz = GetClientSize();

    // Background
    dc.SetBrush(theme_engine().brush(core::ThemeColorToken::BgPanel));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(sz);

    if (is_loading_)
    {
        return; // Let the child placeholder draw itself
    }

    // Set clip region
    dc.SetClippingRegion(0, 0, sz.GetWidth(), sz.GetHeight());

    // Font
    dc.SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular));

    int current_index = 0;
    int y_offset = -scroll_offset_;
    for (const auto& node : roots_)
    {
        if (node.filter_visible)
        {
            DrawNode(dc, node, 0, y_offset, current_index);
        }
    }

    // Draw animated focus ring
    FocusRingRenderer::get().draw(dc, this, theme_engine());
}

void FileTreeCtrl::ShowLoadingState()
{
    is_loading_ = true;
    if (loading_skeleton_ != nullptr)
    {
        loading_skeleton_->SetSize(GetClientSize());
        loading_skeleton_->ShowAndAnimate();
    }
    Refresh();
}

void FileTreeCtrl::HideLoadingState()
{
    is_loading_ = false;
    if (loading_skeleton_ != nullptr)
    {
        loading_skeleton_->HideAndStop();
    }
    Refresh();
}

void FileTreeCtrl::SetScrollOffset(int offset)
{
    scroll_offset_ = std::clamp(offset, 0, std::max(0, virtual_height_ - GetClientSize().y));
    UpdateScrollbar();
    Refresh();
}

void FileTreeCtrl::LoadIcons()
{
    // Icons are now loaded dynamically via IconManager
}

void FileTreeCtrl::DrawNode(
    wxDC& dc, const core::FileNode& node, int depth, int& y_offset, int& current_index)
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
            dc.SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::AccentPrimary)));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRectangle(0, row_top, row_w, kRowHeight);
        }
        else if (is_hovered)
        {
            // R20 Fix 23: Full-width hover row highlight with distinct color
            dc.SetBrush(wxBrush(theme_engine()
                                    .color(core::ThemeColorToken::BgPanel)
                                    .ChangeLightness(115))); // Slightly brighter for hover
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRectangle(0, row_top, row_w, kRowHeight);
        }

        // R16 Fix 26: Faint bottom border on each row
        dc.SetPen(
            wxPen(theme_engine().color(core::ThemeColorToken::BorderLight).ChangeLightness(95), 1));
        dc.DrawLine(content_x, row_top + kRowHeight - 1, row_w, row_top + kRowHeight - 1);

        // R3 Fix 3: Phase 06 Task 6: Register focus ring bounds
        wxRect rect(1, row_top + 1, row_w - 2, kRowHeight - 2);
        FocusRingRenderer::get().register_item_bounds(
            FocusZoneId::kSidebar, current_index, this, rect);

        // 1. Draw Twistie (Chevron) - LEFT ALIGNED now
        wxColour chevron_color = theme_engine().color(core::ThemeColorToken::TextMuted);
        if (node.is_folder())
        {
            std::string chevron_icon =
                node.is_open
                    ? "panel-minimize"
                    : "panel-menu"; // using panel menu as a placeholder for chevron-right/down if
                                    // they don't exist, wait, we don't have chevron icons in
                                    // IconLibrary yet, let's use the closest ones or add them.
                                    // Actually, wait. I will check IconLibrary.cpp. 'panel-menu' is
                                    // not a chevron. I'll just use drawing for now or rely on the
                                    // fact that I should add them to IconLibrary. For now, since I
                                    // don't see chevron-right/down in IconLibrary, I will add them
                                    // in a parallel edit. Let's assume they are "chevron-right" and
                                    // "chevron-down"
            // Let's use drawing code for chevrons if we don't have SVG, or I can just define them.
            // Actually, I can just use "activity-explorer" as a placeholder? No.
            // Let's look at IconLibrary.cpp... There are no chevrons. I will add them to
            // IconLibrary.cpp in a separate replacement chunk if needed, or just use
            // `panel-maximize` temporarily. Actually let's assume they will be added:
            // "chevron-right", "chevron-down"
            std::string chevron_name = node.is_open ? "chevron-down" : "chevron-right";

            // Just use IconManager.
            auto bmp = IconManager::get().get_icon_bitmap(
                chevron_name, wxSize(kTwistieSize, kTwistieSize), chevron_color);
            if (bmp.IsOk())
            {
                dc.DrawBitmap(bmp, twistie_x, twistie_y, true);
            }
        }

        // 2. Draw Icon
        std::string icon_name;
        wxColour icon_color = theme_engine().color(core::ThemeColorToken::TextMuted);

        if (node.is_folder())
        {
            icon_name = node.is_open ? "filetype-folder"
                                     : "filetype-folder"; // We only have one folder icon right now
            icon_color =
                theme_engine().color(core::ThemeColorToken::AccentPrimary).ChangeLightness(110);
        }
        else
        {
            icon_name = FileTypeIconResolver::GetFileIcon(node.name);

            // R20 Fix 22: File icon color tint by extension
            const auto& fname = node.name;
            auto ends_with = [&fname](const char* ext) -> bool
            {
                return fname.size() >= std::strlen(ext) &&
                       fname.compare(fname.size() - std::strlen(ext), std::strlen(ext), ext) == 0;
            };
            if (ends_with(".md") || ends_with(".txt"))
            {
                icon_color = theme_engine().color(core::ThemeColorToken::AccentPrimary);
            }
            else if (ends_with(".json") || ends_with(".yml") || ends_with(".yaml"))
            {
                icon_color =
                    theme_engine().color(core::ThemeColorToken::SyntaxNumber); // warm accent
            }
            else if (ends_with(".cpp") || ends_with(".h") || ends_with(".hpp") || ends_with(".c"))
            {
                icon_color = theme_engine().color(core::ThemeColorToken::SyntaxKeyword);
            }
            else if (ends_with(".js") || ends_with(".ts") || ends_with(".jsx") || ends_with(".tsx"))
            {
                icon_color = theme_engine().color(core::ThemeColorToken::SuccessColor);
            }
            else if (ends_with(".html") || ends_with(".htm") || ends_with(".css"))
            {
                icon_color = theme_engine().color(core::ThemeColorToken::AccentSecondary);
            }
            else if (ends_with(".py") || ends_with(".rb") || ends_with(".go") || ends_with(".rs"))
            {
                icon_color = theme_engine().color(core::ThemeColorToken::ErrorColor);
            }
        }

        if (!icon_name.empty())
        {
            auto bmp = IconManager::get().get_icon_bitmap(
                icon_name, wxSize(kIconSize, kIconSize), icon_color);
            if (bmp.IsOk())
            {
                dc.DrawBitmap(bmp, icon_x, icon_y, true);
            }
        }

        // 3. Draw Text
        // Fix 4: Distinct colors for selected vs hovered vs normal
        // V8 Phase 1 Task 3: Bold font + TextMain for open folders
        // 13. Active File Styling: Bold text of currently open file
        bool is_open_folder = node.is_folder() && node.is_open;
        bool should_bold = is_open_folder || is_selected;
        if (should_bold)
        {
            wxFont bold_font = dc.GetFont();
            bold_font.SetWeight(wxFONTWEIGHT_BOLD);
            dc.SetFont(bold_font);
        }

        if (is_selected)
        {
            dc.SetTextForeground(wxColour(255, 255, 255));
        }
        else if (is_hovered)
        {
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMain));
        }
        else if (is_open_folder)
        {
            // Open folders use TextMain for emphasis
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
        // R20 Fix 24: Bold matched filter characters in file names
        auto font_normal = dc.GetFont();
        wxFont font_bold = font_normal;
        font_bold.SetWeight(wxFONTWEIGHT_BOLD);

        if (!filter_text_.empty() && !node.is_folder())
        {
            // Find the match position in display_name (case-insensitive)
            std::string lower_display;
            for (char ch : display_name.ToStdString())
            {
                lower_display += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            }
            std::string lower_filter = filter_text_;
            std::transform(lower_filter.begin(),
                           lower_filter.end(),
                           lower_filter.begin(),
                           [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
            auto match_pos = lower_display.find(lower_filter);
            if (match_pos != std::string::npos)
            {
                // Draw text in segments: before, match (bold), after

                wxString before_text = display_name.Left(match_pos);
                wxString match_text = display_name.Mid(match_pos, lower_filter.size());
                wxString after_text = display_name.Mid(match_pos + lower_filter.size());

                int draw_x = text_x;
                if (!before_text.empty())
                {
                    dc.DrawText(before_text, draw_x, text_y);
                    draw_x += dc.GetTextExtent(before_text).GetWidth();
                }
                dc.SetFont(font_bold);
                dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::AccentPrimary));
                dc.DrawText(match_text, draw_x, text_y);
                draw_x += dc.GetTextExtent(match_text).GetWidth();
                dc.SetFont(node.is_folder() ? font_bold : font_normal);
                dc.SetTextForeground(is_selected ? theme_engine()
                                                       .color(core::ThemeColorToken::AccentPrimary)
                                                       .ChangeLightness(80)
                                     : is_hovered
                                         ? theme_engine().color(core::ThemeColorToken::TextMain)
                                         : theme_engine().color(core::ThemeColorToken::TextMuted));
                if (!after_text.empty())
                {
                    dc.DrawText(after_text, draw_x, text_y);
                }
            }
            else
            {
                dc.SetFont(node.is_folder() ? font_bold : font_normal);
                dc.DrawText(display_name, text_x, text_y);
            }
        }
        else
        {
            dc.SetFont(node.is_folder() ? font_bold : font_normal);
            dc.DrawText(display_name, text_x, text_y);
        }

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

        // V8 Phase 1 Task 3: Restore regular font after open folder text draw
        if (should_bold)
        {
            dc.SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular));
        }

        // Chevron for folders (right-aligned)
        // OBSOLETE: Chevron is now left-aligned and drawn above.
    }

    y_offset += kRowHeight;
    current_index++;

    // Draw children if folder is open
    if (node.is_folder() && node.is_open)
    {
        bool has_visible_children = false;
        for (const auto& child : node.children)
        {
            if (child.filter_visible)
            {
                has_visible_children = true;
                DrawNode(dc, child, depth + 1, y_offset, current_index);
            }
        }

        // Fix 8: Show placeholder for empty open folders
        if (!has_visible_children)
        {
            int empty_x = kLeftPadding + (depth + 1) * kIndentWidth + kTwistieSize;
            int empty_y = y_offset + (kRowHeight - dc.GetCharHeight()) / 2;
            dc.SetTextForeground(theme_engine().color(core::ThemeColorToken::TextMuted));
            dc.DrawText("No files in folder", empty_x, empty_y);
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

            // Publish Breadcrumb Event
            core::events::SidebarBreadcrumbUpdateEvent breadcrumb_evt;
            breadcrumb_evt.panel_id = "explorer";

            std::vector<std::string> path;
            std::function<bool(const std::vector<core::FileNode>&, const std::string&)> find_path;
            find_path = [&](const std::vector<core::FileNode>& nodes,
                            const std::string& target_id) -> bool
            {
                for (const auto& n : nodes)
                {
                    path.push_back(n.name);
                    if (n.id == target_id)
                        return true;
                    if (n.is_folder() && find_path(n.children, target_id))
                        return true;
                    path.pop_back();
                }
                return false;
            };

            if (!find_path(roots_, hit.node->id))
            {
                path = {hit.node->name};
            }
            breadcrumb_evt.breadcrumb_path = path;
            event_bus_.publish(breadcrumb_evt);

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
    UpdateScrollbar();
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

    UpdateScrollbar();
    Refresh();
}

void FileTreeCtrl::OnScrollbarDrag(wxScrollWinEvent& event)
{
    scroll_offset_ = event.GetPosition();
    Refresh();
}

void FileTreeCtrl::UpdateScrollbar()
{
    if (scrollbar_ != nullptr)
    {
        auto sz = GetClientSize();
        scrollbar_->UpdateScrollPosition(scroll_offset_, sz.GetHeight(), virtual_height_);
        if (virtual_height_ > sz.GetHeight())
        {
            scrollbar_->Show();
        }
        else
        {
            scrollbar_->Hide();
        }
    }
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

                // Announce new focus
                accessibility::AccessibilityController::get().announce_focus(
                    up_node->name,
                    "Tree Item",
                    up_node->is_folder() ? (up_node->is_open ? "Expanded" : "Collapsed") : "");

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

                // Announce new focus
                accessibility::AccessibilityController::get().announce_focus(
                    down_node->name,
                    "Tree Item",
                    down_node->is_folder() ? (down_node->is_open ? "Expanded" : "Collapsed") : "");

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

                // Announce state change
                accessibility::AccessibilityController::get().notify_state_change(
                    node->name, node->is_open ? "Expanded" : "Collapsed");

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
                accessibility::AccessibilityController::get().notify_state_change(
                    node->name, node->is_open ? "Expanded" : "Collapsed");
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
                accessibility::AccessibilityController::get().notify_state_change(node->name,
                                                                                  "Expanded");
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
                accessibility::AccessibilityController::get().notify_state_change(node->name,
                                                                                  "Collapsed");
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

                    accessibility::AccessibilityController::get().announce_focus(
                        visible_nodes[static_cast<size_t>(parent_idx)]->name,
                        "Tree Item",
                        "Expanded");
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

                auto* node = visible_nodes[0];
                accessibility::AccessibilityController::get().announce_focus(
                    node->name,
                    "Tree Item",
                    node->is_folder() ? (node->is_open ? "Expanded" : "Collapsed") : "");
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

                auto* node = visible_nodes[static_cast<size_t>(focused_node_index_)];
                accessibility::AccessibilityController::get().announce_focus(
                    node->name,
                    "Tree Item",
                    node->is_folder() ? (node->is_open ? "Expanded" : "Collapsed") : "");
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

void FileTreeCtrl::OnSetFocus(wxFocusEvent& event)
{
    if (focused_node_index_ < 0 && !roots_.empty())
    {
        focused_node_index_ = 0;
    }
    FocusManager::get().set_focus(FocusZoneId::kSidebar, focused_node_index_);
    Refresh();

    // Announce initially focused item
    auto visible_nodes = GetVisibleNodes();
    if (focused_node_index_ >= 0 && focused_node_index_ < static_cast<int>(visible_nodes.size()))
    {
        auto* node = visible_nodes[static_cast<size_t>(focused_node_index_)];
        accessibility::AccessibilityController::get().announce_focus(
            node->name,
            "Tree Item",
            node->is_folder() ? (node->is_open ? "Expanded" : "Collapsed") : "");
    }

    event.Skip();
}

void FileTreeCtrl::OnKillFocus(wxFocusEvent& event)
{
    if (FocusManager::get().current_zone() == FocusZoneId::kSidebar)
    {
        FocusManager::get().set_item(-1);
    }
    Refresh();
    event.Skip();
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
