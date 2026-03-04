#pragma once

#include "ControlState.h"
#include "ThemeAwareWindow.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Phase 41 Task 15: Segment item.
struct SegmentItem
{
    std::string id;
    std::string label;
    std::string icon_name;
};

/// Phase 41 Task 15: Testable model for segmented control.
class SegmentedControlModel
{
public:
    void set_items(std::vector<SegmentItem> items);
    [[nodiscard]] auto items() const -> const std::vector<SegmentItem>&;

    void set_selected_index(int index);
    [[nodiscard]] auto selected_index() const -> int;
    [[nodiscard]] auto selected_item() const -> const SegmentItem*;

    [[nodiscard]] auto item_count() const -> int;

private:
    std::vector<SegmentItem> items_;
    int selected_index_{0};
};

/// Phase 41 Task 15: Themed segmented control (button group selector).
class SegmentedControl : public ThemeAwareWindow
{
public:
    using ChangeCallback = std::function<void(int index, const SegmentItem&)>;

    SegmentedControl(wxWindow* parent,
                     core::ThemeEngine& theme_engine,
                     std::vector<SegmentItem> items = {});
    ~SegmentedControl() override = default;

    void set_items(std::vector<SegmentItem> items);
    void set_selected_index(int index);
    void set_on_change(ChangeCallback callback);
    void set_enabled(bool enabled);

    [[nodiscard]] auto model() const -> const SegmentedControlModel&;

    static constexpr int kHeight = 28;
    static constexpr int kPaddingH = 12;
    static constexpr int kBorderRadius = 4;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    SegmentedControlModel model_;
    ControlStateTracker state_;
    IndexedControlState indexed_state_;
    ChangeCallback on_change_;

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);

    [[nodiscard]] auto item_at_x(int x) const -> int;
    [[nodiscard]] auto segment_width() const -> int;
};

} // namespace markamp::ui
