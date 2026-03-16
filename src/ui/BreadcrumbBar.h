#pragma once

#include "DesignSystemContext.h"
#include "LayoutMetrics.h"
#include "SpacingGrid.h"
#include "ThemeAwareWindow.h"
#include "core/EventBus.h"

#include <string>
#include <vector>

namespace markamp::ui
{

class BreadcrumbDropdown;

/// A breadcrumb navigation bar that shows the current file path and heading hierarchy.
/// Inspired by VSCode's breadcrumb bar above the editor.
class BreadcrumbBar : public ThemeAwareWindow
{
public:
    BreadcrumbBar(wxWindow* parent, DesignSystemContext& design_system, core::EventBus& event_bus);
    ~BreadcrumbBar() override;

    /// Set the file path segments (e.g. ["src", "ui", "EditorPanel.cpp"])
    void SetFilePath(const std::string& full_path, const std::string& workspace_root);

    /// Set the heading path (e.g. ["Chapter 1", "Section 1.1"])
    void SetHeadingPath(const std::vector<std::string>& headings);

    struct SymbolItem
    {
        std::string text;
        int level{0};
        int line{0};
    };

    /// Set the parsed outline headings for the dropdown
    void SetDocumentSymbols(const std::vector<SymbolItem>& symbols, int active_line);

    using SegmentClickCallback = std::function<void(const std::string& path)>;
    void SetSegmentClickCallback(SegmentClickCallback callback);

    struct TraversalSegment
    {
        std::string surface_label;
        std::string anchor_label;
        int nav_entry_index{-1};
    };

    void SetTraversalSegments(const std::vector<TraversalSegment>& segments);
    [[nodiscard]] auto GetTraversalSegments() const -> const std::vector<TraversalSegment>&;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

public:
    enum class SegmentType
    {
        kWorkspace,
        kFolder,
        kFile,
        kSymbol,
        kTraversal
    };

    struct BreadcrumbSegment
    {
        std::string text;
        std::string full_path;
        wxRect bounds;
        SegmentType type{SegmentType::kFile};
        bool is_hovered{false};
        bool is_active{false};
        bool is_collapsed{false};
    };

    enum class BreadcrumbMode
    {
        kCompact,
        kExpanded
    };

    void SetMode(BreadcrumbMode mode);
    [[nodiscard]] auto GetMode() const -> BreadcrumbMode
    {
        return mode_;
    }

    // ── 100 Editor UX/UI Improvements: Batch 8 — Breadcrumb Accessors (#78–#80) ──

    /// #78 Number of breadcrumb segments.
    [[nodiscard]] inline auto segment_count() const noexcept -> std::size_t
    {
        return segments_.size();
    }

    /// #79 True when document symbols are available.
    [[nodiscard]] inline auto has_symbols() const noexcept -> bool
    {
        return !document_symbols_.empty();
    }

    /// #80 True when the breadcrumb bar is focused.
    [[nodiscard]] inline auto is_focused() const noexcept -> bool
    {
        return is_focused_;
    }

private:
    void Rebuild();
    void RecalculateSegmentBounds(wxGraphicsContext* graphics_context);

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnLeaveWindow(wxMouseEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void ClampScrollOffset();

    DesignSystemContext& ds_;
    core::EventBus& event_bus_;

    std::vector<std::string> file_segments_;
    std::string full_path_;
    std::string workspace_root_;

    std::vector<std::string> heading_segments_;
    std::vector<SymbolItem> document_symbols_;
    int active_symbol_line_{-1};

    std::vector<TraversalSegment> traversal_segments_;
    SegmentClickCallback segment_click_callback_;

    std::vector<BreadcrumbSegment> segments_;
    std::unique_ptr<BreadcrumbDropdown> dropdown_;

    BreadcrumbMode mode_{BreadcrumbMode::kCompact};

    int hovered_segment_{-1};
    int focused_segment_index_{-1};
    int scroll_offset_{0};
    int content_width_{0};

    bool is_focused_{false};
};

} // namespace markamp::ui
