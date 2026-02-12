#pragma once

#include "core/ThemeEngine.h"

#include <wx/wx.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Grid-based overlay for editing markdown pipe tables.
/// Parses table lines into a 2D grid of cells, provides add/remove
/// row/column operations, and serializes back to aligned markdown.
class TableEditorOverlay : public wxPanel
{
public:
    using CommitCallback =
        std::function<void(const std::string& markdown, int start_line, int end_line)>;

    TableEditorOverlay(wxWindow* parent, core::ThemeEngine& theme_engine, CommitCallback on_commit);

    /// Parse markdown pipe-table lines and populate the grid.
    /// Returns false if lines don't form a valid table.
    auto LoadTable(const std::vector<std::string>& lines, int start_line, int end_line) -> bool;

    /// Update styling from theme.
    void ApplyTheme();

    /// Serialize the grid back to markdown pipe-table format.
    [[nodiscard]] auto SerializeToMarkdown() const -> std::string;

private:
    core::ThemeEngine& theme_engine_;
    CommitCallback on_commit_;

    int start_line_{0};
    int end_line_{0};

    // Grid storage: row × col
    std::vector<std::vector<std::string>> cells_;
    std::vector<std::vector<wxTextCtrl*>> cell_widgets_;
    int num_cols_{0};
    int num_rows_{0};

    // UI
    wxPanel* grid_panel_{nullptr};
    wxPanel* toolbar_panel_{nullptr};

    static constexpr int kCellMinWidth = 60;
    static constexpr int kCellHeight = 24;
    static constexpr int kPadding = 6;

    void CreateToolbar();
    void RebuildGrid();
    void SyncCellsFromWidgets();

    // Toolbar actions
    void OnAddRow();
    void OnAddColumn();
    void OnDeleteRow();
    void OnDeleteColumn();
    void OnCommit();
    void OnCancel();

    // Parse helpers
    static auto ParseTableRow(const std::string& line) -> std::vector<std::string>;
    static auto IsSeparatorRow(const std::string& line) -> bool;
    static auto TrimCell(const std::string& cell) -> std::string;
};

} // namespace markamp::ui
