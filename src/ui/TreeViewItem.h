#pragma once

#include "ControlState.h"
#include "ThemeAwareWindow.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Phase 41 Task 18: Tree view item action button.
struct TreeViewAction
{
    std::string id;
    std::string icon_name;
    std::string tooltip;
};

/// Phase 41 Task 18: Testable model for a tree view item.
class TreeViewItemModel
{
public:
    void set_label(const std::string& label);
    [[nodiscard]] auto label() const -> const std::string&;

    void set_icon_name(const std::string& icon_name);
    [[nodiscard]] auto icon_name() const -> const std::string&;

    void set_expandable(bool expandable);
    [[nodiscard]] auto is_expandable() const -> bool;

    void set_expanded(bool expanded);
    [[nodiscard]] auto is_expanded() const -> bool;
    void toggle_expanded();

    void set_depth(int depth);
    [[nodiscard]] auto depth() const -> int;

    void set_selected(bool selected);
    [[nodiscard]] auto is_selected() const -> bool;

    void set_hover_actions(std::vector<TreeViewAction> actions);
    [[nodiscard]] auto hover_actions() const -> const std::vector<TreeViewAction>&;

    [[nodiscard]] auto indent_pixels() const -> int;

    static constexpr int kIndentPerLevel = 16;

private:
    std::string label_;
    std::string icon_name_;
    bool expandable_{false};
    bool expanded_{false};
    int depth_{0};
    bool selected_{false};
    std::vector<TreeViewAction> hover_actions_;
};

/// Phase 41 Task 18: Themed tree view item with hover actions.
class TreeViewItem : public ThemeAwareWindow
{
public:
    using ExpandCallback = std::function<void(bool expanded)>;
    using ActionCallback = std::function<void(const std::string& action_id)>;
    using SelectCallback = std::function<void()>;

    TreeViewItem(wxWindow* parent, core::ThemeEngine& theme_engine);
    ~TreeViewItem() override = default;

    void set_label(const std::string& label);
    void set_icon_name(const std::string& icon_name);
    void set_expandable(bool expandable);
    void set_expanded(bool expanded);
    void set_depth(int depth);
    void set_selected(bool selected);
    void set_hover_actions(std::vector<TreeViewAction> actions);

    void set_on_expand(ExpandCallback callback);
    void set_on_action(ActionCallback callback);
    void set_on_select(SelectCallback callback);

    [[nodiscard]] auto model() const -> const TreeViewItemModel&;

    static constexpr int kRowHeight = 22;
    static constexpr int kIconSize = 16;
    static constexpr int kChevronSize = 12;
    static constexpr int kActionButtonSize = 16;
    static constexpr int kGap = 4;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    TreeViewItemModel model_;
    ControlStateTracker state_;
    ExpandCallback on_expand_;
    ActionCallback on_action_;
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
