#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"
#include "core/WorkspaceSearchEngine.h"

#include <wx/panel.h>
#include <wx/timer.h>

#include <optional>
#include <string>
#include <vector>

namespace markamp::ui
{
struct DesignSystemContext;
class IconManager;
class ThemedScrollbar;

struct SearchResultItem
{
    bool is_file{false};
    bool expanded{true};
    std::string file_path;
    std::string file_name;
    int match_count{0};

    // For match item
    core::WorkspaceSearchEngine::SearchMatch match;
    bool is_selected{false};
    bool is_hovered{false};
};

class SearchResultsTree : public ThemeAwareWindow
{
public:
    SearchResultsTree(wxWindow* parent, core::ThemeEngine& theme_engine);
    ~SearchResultsTree() override = default;

    SearchResultsTree(const SearchResultsTree&) = delete;
    SearchResultsTree& operator=(const SearchResultsTree&) = delete;
    SearchResultsTree(SearchResultsTree&&) = delete;
    SearchResultsTree& operator=(SearchResultsTree&&) = delete;

    void SetResults(const core::WorkspaceSearchEngine::SearchResult& result);
    void ClearResults();
    void CollapseAll();
    void ExpandAll();

    using ResultSelectCallback =
        std::function<void(const std::string& file_path, int line, int col)>;
    void SetOnResultSelected(ResultSelectCallback callback)
    {
        on_result_selected_ = std::move(callback);
    }

    [[nodiscard]] auto GetSelectedMatch() const
        -> std::optional<core::WorkspaceSearchEngine::SearchMatch>;

    void SelectNextMatch();
    void SelectPreviousMatch();

    [[nodiscard]] auto GetCurrentResult() const -> const core::WorkspaceSearchEngine::SearchResult&
    {
        return current_result_;
    }

    // Scrollbar integration
    [[nodiscard]] int GetCount() const;
    [[nodiscard]] int GetCharHeight() const override;
    void SetFirstItem(int index);
    int HitTestItem(const wxPoint& point) const;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnScroll(wxMouseEvent& event);

    void RebuildItems();
    void UpdateVirtualHeight();

    core::WorkspaceSearchEngine::SearchResult current_result_;
    std::vector<SearchResultItem> items_;
    std::vector<SearchResultItem*> visible_items_;

    int scroll_offset_{0};
    int hovered_idx_{-1};
    int selected_idx_{-1};

    ResultSelectCallback on_result_selected_;

    // NOLINTBEGIN
    wxDECLARE_EVENT_TABLE();
    // NOLINTEND
};

} // namespace markamp::ui
