#include "BreadcrumbBar.h"

#include "LayoutMetrics.h"
#include "TypographyScale.h"
#include "accessibility/AccessibilityController.h"
#include "core/Events.h"
#include "core/Logger.h"
#include "ui/BreadcrumbDropdown.h"
#include "ui/FileTypeIconResolver.h"
#include "ui/FocusManager.h"
#include "ui/FocusRingRenderer.h"
#include "ui/IconManager.h"

#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/dcbuffer.h>
#include <wx/dnd.h>
#include <wx/filename.h>
#include <wx/graphics.h>
#include <wx/menu.h>
#include <wx/settings.h>
#include <wx/sizer.h>

namespace markamp::ui
{

BreadcrumbBar::BreadcrumbBar(wxWindow* parent,
                             DesignSystemContext& design_system,
                             core::EventBus& event_bus)
    : ThemeAwareWindow(parent,
                       design_system.theme,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxSize(-1, design_system.metrics.row_height()),
                       wxTAB_TRAVERSAL | wxNO_BORDER | wxWANTS_CHARS | wxFULL_REPAINT_ON_RESIZE)
    , ds_(design_system)
    , event_bus_(event_bus)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    OnThemeChanged(ds_.theme.current_theme());

    SetCanFocus(true);

    Bind(wxEVT_PAINT, &BreadcrumbBar::OnPaint, this);
    Bind(wxEVT_MOTION, &BreadcrumbBar::OnMouseMove, this);
    Bind(wxEVT_LEFT_DOWN, &BreadcrumbBar::OnMouseDown, this);
    Bind(wxEVT_RIGHT_DOWN, &BreadcrumbBar::OnRightDown, this);
    Bind(wxEVT_LEAVE_WINDOW, &BreadcrumbBar::OnLeaveWindow, this);
    Bind(wxEVT_MOUSEWHEEL, &BreadcrumbBar::OnMouseWheel, this);

    Bind(wxEVT_KEY_DOWN, &BreadcrumbBar::OnKeyDown, this);

    Bind(wxEVT_SET_FOCUS,
         [this](wxFocusEvent& event)
         {
             is_focused_ = true;
             FocusManager::get().set_focus(FocusZoneId::kBreadcrumb, 0);
             Refresh();
             event.Skip();
         });

    Bind(wxEVT_KILL_FOCUS,
         [this](wxFocusEvent& event)
         {
             is_focused_ = false;
             if (FocusManager::get().current_zone() == FocusZoneId::kBreadcrumb)
             {
                 FocusManager::get().set_item(-1);
             }
             Refresh();
             event.Skip();
         });
}

BreadcrumbBar::~BreadcrumbBar() = default;

void BreadcrumbBar::SetSegmentClickCallback(SegmentClickCallback callback)
{
    segment_click_callback_ = std::move(callback);
}

void BreadcrumbBar::SetFilePath(const std::string& full_path, const std::string& workspace_root)
{
    full_path_ = full_path;
    workspace_root_ = workspace_root;

    // Parse the file path relative to workspace if possible
    std::string relative_path = full_path;
    if (!workspace_root.empty() && full_path.starts_with(workspace_root))
    {
        relative_path = full_path.substr(workspace_root.length());
        if (relative_path.starts_with("/"))
        {
            relative_path = relative_path.substr(1);
        }
    }

    // Split into segments
    file_segments_.clear();
    std::string current_seg;
    for (char c : relative_path)
    {
        if (c == '/' || c == '\\')
        {
            if (!current_seg.empty())
            {
                file_segments_.push_back(current_seg);
                current_seg.clear();
            }
        }
        else
        {
            current_seg += c;
        }
    }
    if (!current_seg.empty())
    {
        file_segments_.push_back(current_seg);
    }

    Rebuild();
    Refresh();
}

void BreadcrumbBar::SetHeadingPath(const std::vector<std::string>& headings)
{
    heading_segments_ = headings;
    Rebuild();
    Refresh();
}

void BreadcrumbBar::SetDocumentSymbols(const std::vector<SymbolItem>& symbols, int active_line)
{
    document_symbols_ = symbols;
    active_symbol_line_ = active_line;

    heading_segments_.clear();

    if (!symbols.empty())
    {
        std::vector<SymbolItem> path;
        for (const auto& sym : symbols)
        {
            if (sym.line > active_line)
            {
                break;
            }

            while (!path.empty() && path.back().level >= sym.level)
            {
                path.pop_back();
            }
            path.push_back(sym);
        }

        for (const auto& step : path)
        {
            heading_segments_.push_back(step.text);
        }
    }

    Rebuild();
    Refresh();
}

void BreadcrumbBar::SetTraversalSegments(const std::vector<TraversalSegment>& segments)
{
    traversal_segments_ = segments;
    Rebuild();
    Refresh();
}

auto BreadcrumbBar::GetTraversalSegments() const -> const std::vector<TraversalSegment>&
{
    return traversal_segments_;
}

void BreadcrumbBar::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    Refresh();
}

void BreadcrumbBar::SetMode(BreadcrumbMode mode)
{
    if (mode_ != mode)
    {
        mode_ = mode;
        const int kHeight = (mode_ == BreadcrumbMode::kCompact) ? 24 : 32;
        SetMinSize(wxSize(-1, kHeight));
        SetSize(wxSize(-1, kHeight));
        if (GetParent() != nullptr)
        {
            GetParent()->Layout();
        }
        Rebuild();
        Refresh();
    }
}

void BreadcrumbBar::Rebuild()
{
    segments_.clear();
    hovered_segment_ = -1;

    // 1. Workspace Root (First Segment)
    std::string current_built_path;
    if (!workspace_root_.empty())
    {
        current_built_path = workspace_root_;

        // Extract just the folder name from the workspace root path
        std::string workspace_name = workspace_root_;
        auto last_slash = workspace_name.find_last_of("/\\");
        if (last_slash != std::string::npos && last_slash < workspace_name.length() - 1)
        {
            workspace_name = workspace_name.substr(last_slash + 1);
        }

        segments_.push_back(
            {workspace_name, current_built_path, wxRect(), SegmentType::kWorkspace});
    }

    // 2. File Segments (with collapse logic)
    const size_t kTotalFileSegments = file_segments_.size();

    if (kTotalFileSegments > 5)
    {
        // Collapse middle: keep 1st, "...", then the last 3

        // Add first folder
        current_built_path += "/" + file_segments_[0];
        segments_.push_back(
            {file_segments_[0], current_built_path, wxRect(), SegmentType::kFolder});

        // Add Ellipsis
        segments_.push_back({"...", "", wxRect(), SegmentType::kFolder, false, false, true});

        // Calculate the hidden path sections to correctly build the full_path of later segments
        for (size_t i = 1; i < kTotalFileSegments - 3; ++i)
        {
            current_built_path += "/" + file_segments_[i];
        }

        // Add final 3 segments (last one is the file)
        for (size_t i = kTotalFileSegments - 3; i < kTotalFileSegments; ++i)
        {
            current_built_path += "/" + file_segments_[i];
            bool is_last = (i == kTotalFileSegments - 1);
            segments_.push_back({file_segments_[i],
                                 current_built_path,
                                 wxRect(),
                                 is_last ? SegmentType::kFile : SegmentType::kFolder});
        }
    }
    else
    {
        // Small enough, add all seamlessly
        for (size_t i = 0; i < kTotalFileSegments; ++i)
        {
            current_built_path += "/" + file_segments_[i];
            bool is_last = (i == kTotalFileSegments - 1);
            segments_.push_back({file_segments_[i],
                                 current_built_path,
                                 wxRect(),
                                 is_last ? SegmentType::kFile : SegmentType::kFolder});
        }
    }

    // 3. Heading/Symbol Segments
    for (const auto& heading : heading_segments_)
    {
        std::string heading_path = current_built_path;
        if (!heading_path.empty())
        {
            heading_path += " -> ";
        }
        heading_path += heading;

        segments_.push_back({heading, heading_path, wxRect(), SegmentType::kSymbol});
    }

    // 4. Traversal Segments
    for (const auto& traversal : traversal_segments_)
    {
        const std::string label = traversal.surface_label + ": " + traversal.anchor_label;
        segments_.push_back({label, label, wxRect(), SegmentType::kTraversal});
    }

    // Safety check for keyboard bounds
    if (focused_segment_index_ >= static_cast<int>(segments_.size()))
    {
        focused_segment_index_ = static_cast<int>(segments_.size()) - 1;
    }

    // Request bounds recalculation
    segments_.clear();
}

void BreadcrumbBar::RecalculateSegmentBounds(wxGraphicsContext* gc)
{
    const auto kClientSize = GetClientSize();
    const int kHeight = kClientSize.GetHeight();

    auto font = ds_.typography.font(TypeSlot::kCaption);
    auto bold_font = font;
    bold_font.SetWeight(wxFONTWEIGHT_SEMIBOLD);

    int current_x = ds_.spacing.scaled(SpacingToken::kMd) - scroll_offset_;
    const int kIconSize = 14;

    for (size_t i = 0; i < segments_.size(); ++i)
    {
        auto& seg = segments_[i];
        const int kStartX = current_x;

        std::string icon_name;
        switch (seg.type)
        {
            case SegmentType::kWorkspace:
                icon_name = "cube";
                break;
            case SegmentType::kFolder:
                icon_name = "folder";
                break;
            case SegmentType::kFile:
                icon_name = FileTypeIconResolver::GetFileIcon(seg.text);
                break;
            case SegmentType::kSymbol:
                icon_name = "symbol-misc";
                break;
            case SegmentType::kTraversal:
                icon_name = "link";
                break;
        }

        if (!icon_name.empty())
        {
            current_x += kIconSize + 4;
        }

        gc->SetFont(seg.is_active ? bold_font : font, *wxBLACK);

        double text_w = 0.0;
        double text_h = 0.0;
        double descent = 0.0;
        double external_leading = 0.0;
        gc->GetTextExtent(
            wxString::FromUTF8(seg.text), &text_w, &text_h, &descent, &external_leading);

        current_x += static_cast<int>(text_w) + 4;

        seg.bounds = wxRect(kStartX, 0, current_x - kStartX, kHeight);

        if (i < segments_.size() - 1)
        {
            current_x += kIconSize + 4; // separator spacing
        }
    }

    // Total width is un-scrolled end pos + margin
    content_width_ = current_x + scroll_offset_ + ds_.spacing.scaled(SpacingToken::kMd);
}

void BreadcrumbBar::ClampScrollOffset()
{
    const int kWindowWidth = GetClientSize().GetWidth();
    const int kMaxScroll = std::max(0, content_width_ - kWindowWidth);

    if (scroll_offset_ < 0)
    {
        scroll_offset_ = 0;
    }
    else if (scroll_offset_ > kMaxScroll)
    {
        scroll_offset_ = kMaxScroll;
    }
}

void BreadcrumbBar::OnMouseWheel(wxMouseEvent& event)
{
    const int kWheelRotation = event.GetWheelRotation();
    if (kWheelRotation == 0)
        return;

    // Both vertical and horizontal wheels map to horizontal scrolling here
    scroll_offset_ -= kWheelRotation;
    ClampScrollOffset();
    Refresh();
}

void BreadcrumbBar::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(this);
    std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(paint_dc));

    if (!gc)
    {
        return;
    }

    const auto kClientSize = GetClientSize();
    const int kWidth = kClientSize.GetWidth();
    const int kHeight = kClientSize.GetHeight();

    const auto kBgColor = theme_engine().color(core::ThemeColorToken::BgPanel);
    const auto kHoverColor =
        theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(110);
    const auto kTextColorNormal = theme_engine().color(core::ThemeColorToken::TextMuted);
    const auto kTextColorHover = theme_engine().color(core::ThemeColorToken::TextMain);
    const auto kSeparatorColor = theme_engine().color(core::ThemeColorToken::BorderLight);

    gc->SetBrush(wxBrush(kBgColor));
    gc->SetPen(*wxTRANSPARENT_PEN);
    gc->DrawRectangle(0, 0, kWidth, kHeight);

    if (segments_.empty())
    {
        auto font = ds_.typography.font(TypeSlot::kCaption);
        gc->SetFont(font, kTextColorNormal);
        const wxString kEmptyText = "Open a file to start editing";
        double text_w = 0.0;
        double text_h = 0.0;
        double descent = 0.0;
        double ext_lead = 0.0;
        gc->GetTextExtent(kEmptyText, &text_w, &text_h, &descent, &ext_lead);
        const int kYPos = (kHeight - static_cast<int>(text_h)) / 2;
        gc->DrawText(kEmptyText, ds_.spacing.scaled(SpacingToken::kMd), kYPos);
        return;
    }

    auto font = ds_.typography.font(TypeSlot::kCaption);
    auto bold_font = font;
    bold_font.SetWeight(wxFONTWEIGHT_SEMIBOLD);

    RecalculateSegmentBounds(gc.get());
    ClampScrollOffset();

    int current_x = ds_.spacing.scaled(SpacingToken::kMd) - scroll_offset_;
    const int kIconSize = 14;
    const int kIconY = (kHeight - kIconSize) / 2;

    for (size_t i = 0; i < segments_.size(); ++i)
    {
        auto& seg = segments_[i];

        // 1. Hover Background
        if (hovered_segment_ == static_cast<int>(i) || seg.is_hovered)
        {
            gc->SetBrush(wxBrush(kHoverColor));
            gc->DrawRoundedRectangle(seg.bounds.GetX(), 2, seg.bounds.GetWidth(), kHeight - 4, 4);
        }

        // 2. Render Icon
        const wxColour kFgColor = (hovered_segment_ == static_cast<int>(i) || seg.is_active)
                                      ? kTextColorHover
                                      : kTextColorNormal;

        std::string icon_name;
        switch (seg.type)
        {
            case SegmentType::kWorkspace:
                icon_name = "cube";
                break;
            case SegmentType::kFolder:
                icon_name = "folder";
                break;
            case SegmentType::kFile:
                icon_name = FileTypeIconResolver::GetFileIcon(seg.text);
                break;
            case SegmentType::kSymbol:
                icon_name = "symbol-misc";
                break;
            case SegmentType::kTraversal:
                icon_name = "link";
                break;
        }

        if (!icon_name.empty())
        {
            IconManager::get().draw_icon(
                paint_dc, icon_name, current_x, kIconY, wxSize(kIconSize, kIconSize), kFgColor);
            current_x += kIconSize + 4;
        }

        // 3. Render Text
        gc->SetFont(seg.is_active ? bold_font : font, kFgColor);
        const wxString kWxText = wxString::FromUTF8(seg.text);

        double text_w = 0.0;
        double text_h = 0.0;
        double descent = 0.0;
        double external_leading = 0.0;
        gc->GetTextExtent(kWxText, &text_w, &text_h, &descent, &external_leading);

        gc->DrawText(kWxText, current_x, (kHeight - text_h) / 2.0);
        current_x += static_cast<int>(text_w) + 4;

        // 4. Render Separators
        if (i < segments_.size() - 1)
        {
            gc->SetPen(wxPen(kSeparatorColor, 1));
            const int kChevronX = current_x + (kIconSize / 2);
            const int kChevronY = kHeight / 2;

            if (seg.type == SegmentType::kFile && segments_[i + 1].type == SegmentType::kSymbol)
            {
                // Distinct File -> Symbol separator ::
                gc->StrokeLine(kChevronX - 3, kChevronY - 4, kChevronX - 3, kChevronY + 4);
                gc->StrokeLine(kChevronX + 1, kChevronY - 4, kChevronX + 1, kChevronY + 4);
            }
            else
            {
                // Standard chevron ->
                gc->StrokeLine(kChevronX - 3, kChevronY - 3, kChevronX + 1, kChevronY);
                gc->StrokeLine(kChevronX + 1, kChevronY, kChevronX - 3, kChevronY + 3);
            }
            current_x += kIconSize + 4;
        }

        FocusRingRenderer::get().register_item_bounds(
            FocusZoneId::kBreadcrumb, static_cast<int>(i), this, seg.bounds);
    }

    FocusRingRenderer::get().draw(paint_dc, this, theme_engine());
}

void BreadcrumbBar::OnMouseMove(wxMouseEvent& event)
{
    if (segments_.empty())
    {
        event.Skip();
        return;
    }

    const int kPreviousHover = hovered_segment_;
    hovered_segment_ = -1;

    const wxPoint kPos = event.GetPosition();

    // 1. Drag and Drop Dispatch
    if (event.Dragging() && event.LeftIsDown() && hovered_segment_ >= 0)
    {
        auto& seg = segments_[static_cast<std::size_t>(hovered_segment_)];
        if (!seg.full_path.empty() &&
            (seg.type == SegmentType::kFile || seg.type == SegmentType::kFolder ||
             seg.type == SegmentType::kWorkspace))
        {
            wxFileDataObject file_data;
            file_data.AddFile(seg.full_path);

            wxDropSource drag_source(this);
            drag_source.SetData(file_data);

            const wxDragResult kResult = drag_source.DoDragDrop(wxDrag_CopyOnly);
            MARKAMP_LOG_DEBUG("Breadcrumb Drag and Drop completed with result: {}",
                              static_cast<int>(kResult));

            hovered_segment_ = -1;
            SetCursor(wxNullCursor);
            SetToolTip("");
            Refresh();
            return;
        }
    }

    // 2. Hover Hit-Testing
    for (size_t i = 0; i < segments_.size(); ++i)
    {
        if (segments_[i].bounds.Contains(kPos))
        {
            hovered_segment_ = static_cast<int>(i);
            break;
        }
    }

    if (hovered_segment_ != kPreviousHover)
    {
        if (hovered_segment_ >= 0)
        {
            SetCursor(wxCursor(wxCURSOR_HAND));
            SetToolTip(
                wxString::FromUTF8(segments_[static_cast<size_t>(hovered_segment_)].full_path));
        }
        else
        {
            SetCursor(wxNullCursor);
            SetToolTip("");
        }
        Refresh();
    }

    event.Skip();
}

void BreadcrumbBar::OnLeaveWindow(wxMouseEvent& event)
{
    if (hovered_segment_ != -1)
    {
        hovered_segment_ = -1;
        SetCursor(wxNullCursor);
        SetToolTip("");
        Refresh();
    }
    event.Skip();
}

void BreadcrumbBar::OnSetFocus(wxFocusEvent& event)
{
    is_focused_ = true;
    if (focused_segment_index_ < 0 && !segments_.empty())
    {
        focused_segment_index_ =
            static_cast<int>(segments_.size() - 1); // default to last item (file name)
    }

    if (focused_segment_index_ >= 0 && focused_segment_index_ < static_cast<int>(segments_.size()))
    {
        accessibility::AccessibilityController::get().announce_focus(
            segments_[static_cast<std::size_t>(focused_segment_index_)].full_path,
            "Breadcrumb Segment",
            "Selected");
    }

    Refresh();
    event.Skip();
}

void BreadcrumbBar::OnKillFocus(wxFocusEvent& event)
{
    is_focused_ = false;
    Refresh();
    event.Skip();
}

void BreadcrumbBar::OnKeyDown(wxKeyEvent& event)
{
    if (!is_focused_ || segments_.empty())
    {
        event.Skip();
        return;
    }

    const int kKeyCode = event.GetKeyCode();
    const int kCount = static_cast<int>(segments_.size());

    bool handled = false;

    if (kKeyCode == WXK_LEFT)
    {
        focused_segment_index_--;
        if (focused_segment_index_ < 0)
        {
            focused_segment_index_ = kCount - 1;
        }
        handled = true;
    }
    else if (kKeyCode == WXK_RIGHT)
    {
        focused_segment_index_++;
        if (focused_segment_index_ >= kCount)
        {
            focused_segment_index_ = 0;
        }
        handled = true;
    }
    else if (kKeyCode == WXK_RETURN || kKeyCode == WXK_SPACE)
    {
        if (focused_segment_index_ >= 0 && focused_segment_index_ < kCount)
        {
            const auto& seg = segments_[static_cast<std::size_t>(focused_segment_index_)];
            MARKAMP_LOG_DEBUG("Breadcrumb keyboard activated: {}", seg.full_path);

            hovered_segment_ = focused_segment_index_;
            wxMouseEvent fake_click(wxEVT_LEFT_DOWN);
            OnMouseDown(fake_click);
        }
        handled = true;
    }

    if (handled)
    {
        if (kKeyCode == WXK_LEFT || kKeyCode == WXK_RIGHT)
        {
            accessibility::AccessibilityController::get().announce_focus(
                segments_[static_cast<std::size_t>(focused_segment_index_)].full_path,
                "Breadcrumb Segment",
                "Selected");
        }
        Refresh();
    }
    else
    {
        event.Skip();
    }
}

void BreadcrumbBar::OnRightDown(wxMouseEvent& event)
{
    const wxPoint kMousePos = event.GetPosition();
    int hit_index = -1;

    for (size_t i = 0; i < segments_.size(); ++i)
    {
        if (segments_[i].bounds.Contains(kMousePos))
        {
            hit_index = static_cast<int>(i);
            break;
        }
    }

    if (hit_index >= 0)
    {
        auto& seg = segments_[static_cast<std::size_t>(hit_index)];

        if (!seg.full_path.empty())
        {
            wxMenu menu;
            const int kMenuCopyPath = 1001;
            const int kMenuCopyRelative = 1002;
            const int kMenuReveal = 1003;

            menu.Append(kMenuCopyPath, "Copy Path");
            menu.Append(kMenuCopyRelative, "Copy Relative Path");
            menu.AppendSeparator();

#if defined(__WXOSX__)
            menu.Append(kMenuReveal, "Reveal in Finder");
#else
            menu.Append(kMenuReveal, "Reveal in Explorer");
#endif

            menu.Bind(
                wxEVT_MENU,
                [seg](wxCommandEvent&)
                {
                    if (wxTheClipboard->Open())
                    {
                        wxTheClipboard->SetData(new wxTextDataObject(seg.full_path));
                        wxTheClipboard->Close();
                    }
                },
                kMenuCopyPath);

            menu.Bind(
                wxEVT_MENU,
                [this, seg](wxCommandEvent&)
                {
                    if (!workspace_root_.empty())
                    {
                        std::string rel_path = seg.full_path;
                        if (rel_path.find(workspace_root_) == 0)
                        {
                            rel_path = rel_path.substr(workspace_root_.length());
                            if (!rel_path.empty() && (rel_path[0] == '/' || rel_path[0] == '\\'))
                            {
                                rel_path = rel_path.substr(1);
                            }
                        }
                        if (wxTheClipboard->Open())
                        {
                            wxTheClipboard->SetData(new wxTextDataObject(rel_path));
                            wxTheClipboard->Close();
                        }
                    }
                },
                kMenuCopyRelative);

            menu.Bind(
                wxEVT_MENU,
                [seg](wxCommandEvent&)
                {
                    if (wxFileName::FileExists(seg.full_path) ||
                        wxFileName::DirExists(seg.full_path))
                    {
#if defined(__WXOSX__)
                        wxString cmd = wxString::Format("open -R \"%s\"", seg.full_path);
                        wxExecute(cmd);
#elif defined(__WXMSW__)
                        wxString cmd = wxString::Format("explorer /select,\"%s\"", seg.full_path);
                        wxExecute(cmd);
#else
                        wxLaunchDefaultApplication(wxFileName(seg.full_path).GetPath());
#endif
                    }
                },
                kMenuReveal);

            PopupMenu(&menu);
        }
    }
    else
    {
        event.Skip();
    }
}

void BreadcrumbBar::OnMouseDown(wxMouseEvent& event)
{
    if (hovered_segment_ >= 0 && hovered_segment_ < static_cast<int>(segments_.size()))
    {
        auto& seg = segments_[static_cast<size_t>(hovered_segment_)];

        if (seg.type == SegmentType::kWorkspace)
        {
            core::events::CommandPaletteEvent quick_open_evt;
            quick_open_evt.mode = "quick_open";
            quick_open_evt.opened = true;
            event_bus_.publish(quick_open_evt);
        }
        else if (seg.type == SegmentType::kFolder)
        {
            if (!seg.full_path.empty())
            {
                std::vector<DropdownItem> items;
                std::error_code error_code;

                try
                {
                    for (const auto& entry :
                         std::filesystem::directory_iterator(seg.full_path, error_code))
                    {
                        DropdownItem item;
                        item.text = entry.path().filename().string();
                        item.full_path = entry.path().string();

                        if (entry.is_directory())
                        {
                            item.icon_name = "folder";
                        }
                        else
                        {
                            item.icon_name = FileTypeIconResolver::GetFileIcon(item.text);
                        }

                        items.push_back(item);
                    }

                    // Sort items: folders first, then alphabetized
                    std::sort(items.begin(),
                              items.end(),
                              [](const DropdownItem& left_item, const DropdownItem& right_item)
                              {
                                  const bool kLeftIsFolder = (left_item.icon_name == "folder");
                                  const bool kRightIsFolder = (right_item.icon_name == "folder");
                                  if (kLeftIsFolder != kRightIsFolder)
                                  {
                                      return kLeftIsFolder;
                                  }
                                  return left_item.text < right_item.text;
                              });
                }
                catch (...)
                {
                    // Ignore traversal errors gracefully for permissions
                }

                if (!items.empty())
                {
                    dropdown_ = std::make_unique<BreadcrumbDropdown>(this, ds_, event_bus_, items);
                    const wxPoint kPopupPosition =
                        ClientToScreen(wxPoint(seg.bounds.GetX(), seg.bounds.GetBottom()));
                    dropdown_->Position(kPopupPosition, wxSize(0, 0));
                    dropdown_->Popup();
                }
            }
        }
        else if (seg.type == SegmentType::kSymbol)
        {
            // Find which level in the path was clicked to resolve siblings
            int clicked_depth = -1;
            for (int i = 0; i < static_cast<int>(heading_segments_.size()); ++i)
            {
                if (heading_segments_[static_cast<std::size_t>(i)] == seg.text)
                {
                    clicked_depth = i;
                    break;
                }
            }

            if (clicked_depth >= 0 && !document_symbols_.empty())
            {
                // Reconstruct the active path to find the parent bounds
                std::vector<SymbolItem> path;
                for (const auto& sym : document_symbols_)
                {
                    if (sym.line > active_symbol_line_)
                    {
                        break;
                    }
                    while (!path.empty() && path.back().level >= sym.level)
                    {
                        path.pop_back();
                    }
                    path.push_back(sym);
                }

                // If clicking the first heading, parent is at line 0 (beginning).
                // Otherwise, the parent is the heading directly preceding it in the path.
                int parent_start_line = 0;
                int parent_level = 0;
                if (clicked_depth > 0 && clicked_depth - 1 < static_cast<int>(path.size()))
                {
                    parent_start_line = path[static_cast<std::size_t>(clicked_depth - 1)].line;
                    parent_level = path[static_cast<std::size_t>(clicked_depth - 1)].level;
                }

                // Parent ends when another heading of parent_level or lower overrides it
                int parent_end_line = std::numeric_limits<int>::max();
                for (const auto& sym : document_symbols_)
                {
                    if (sym.line > parent_start_line && sym.level <= parent_level)
                    {
                        parent_end_line = sym.line;
                        break;
                    }
                }

                // Find the specific boundary level of the clicked segment
                int target_level = -1;
                if (clicked_depth < static_cast<int>(path.size()))
                {
                    target_level = path[static_cast<std::size_t>(clicked_depth)].level;
                }

                if (target_level > 0)
                {
                    std::vector<DropdownItem> items;
                    for (const auto& sym : document_symbols_)
                    {
                        // Scan only the range under the parent, identifying identical-level
                        // headings as siblings
                        if (sym.line >= parent_start_line && sym.line < parent_end_line)
                        {
                            if (sym.level == target_level)
                            {
                                DropdownItem item;
                                item.text = sym.text;
                                item.full_path = std::to_string(
                                    sym.line); // Hack: hold the line number for callback jump
                                item.icon_name = "heading"; // Or "symbol-class", etc.
                                items.push_back(item);
                            }
                        }
                    }

                    if (!items.empty())
                    {
                        dropdown_ =
                            std::make_unique<BreadcrumbDropdown>(this, ds_, event_bus_, items);
                        const wxPoint kPopupPosition =
                            ClientToScreen(wxPoint(seg.bounds.GetX(), seg.bounds.GetBottom()));
                        dropdown_->Position(kPopupPosition, wxSize(0, 0));
                        dropdown_->Popup();
                    }
                }
            }
        }
        else if (seg.type == SegmentType::kTraversal)
        {
            // Do nothing for traveral bounds currently, or publish a navigation event
        }
        else if (segment_click_callback_)
        {
            segment_click_callback_(seg.full_path);
        }
    }
    else if (segment_click_callback_ && !file_segments_.empty())
    {
        // Fallback: click empty area -> navigate to full path
        std::string full_path;
        for (size_t idx = 0; idx < file_segments_.size(); ++idx)
        {
            if (idx > 0)
            {
                full_path += "/";
            }
            full_path += file_segments_[idx];
        }
        segment_click_callback_(full_path);
    }
    SetFocus();
    event.Skip();
}

} // namespace markamp::ui
