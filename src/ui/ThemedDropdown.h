#pragma once

#include "ControlState.h"
#include "ThemeAwareWindow.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Phase 41 Task 06: Dropdown item.
struct DropdownItem
{
    std::string id;
    std::string label;
    std::string icon_name;
    std::string group; ///< Group header for grouped items
    bool is_separator{false};
    bool is_disabled{false};
};

/// Phase 41 Task 06: Testable model for searchable dropdown.
class DropdownModel
{
public:
    void set_items(std::vector<DropdownItem> items);
    [[nodiscard]] auto items() const -> const std::vector<DropdownItem>&;

    void set_selected_index(int index);
    [[nodiscard]] auto selected_index() const -> int;
    [[nodiscard]] auto selected_item() const -> const DropdownItem*;

    void set_search_query(const std::string& query);
    [[nodiscard]] auto search_query() const -> const std::string&;

    /// Get items filtered by search query.
    [[nodiscard]] auto filtered_items() const -> std::vector<DropdownItem>;

    /// Get unique group names in order.
    [[nodiscard]] auto groups() const -> std::vector<std::string>;

    void set_open(bool open);
    [[nodiscard]] auto is_open() const -> bool;

    void set_highlight_index(int index);
    [[nodiscard]] auto highlight_index() const -> int;

    /// Move highlight up/down.
    void move_highlight(int delta);

    /// Select the currently highlighted item.
    void select_highlighted();

    [[nodiscard]] auto item_count() const -> int;

private:
    std::vector<DropdownItem> items_;
    int selected_index_{-1};
    int highlight_index_{0};
    std::string search_query_;
    bool is_open_{false};
};

/// Phase 41 Task 06: Themed dropdown widget.
class ThemedDropdown : public ThemeAwareWindow
{
public:
    using SelectCallback = std::function<void(int index, const DropdownItem&)>;

    ThemedDropdown(wxWindow* parent,
                   core::ThemeEngine& theme_engine,
                   std::vector<DropdownItem> items = {});

    ~ThemedDropdown() override = default;

    void set_items(std::vector<DropdownItem> items);
    void set_selected_index(int index);
    [[nodiscard]] auto selected_index() const -> int;
    [[nodiscard]] auto selected_item() const -> const DropdownItem*;

    void set_on_select(SelectCallback callback);
    void set_enabled(bool enabled);

    [[nodiscard]] auto model() const -> const DropdownModel&;

    static constexpr int kHeight = 28;
    static constexpr int kDropdownMaxHeight = 240;
    static constexpr int kItemHeight = 28;
    static constexpr int kPaddingH = 8;
    static constexpr int kBorderRadius = 4;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    DropdownModel model_;
    ControlStateTracker state_;
    SelectCallback on_select_;

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
};

} // namespace markamp::ui
