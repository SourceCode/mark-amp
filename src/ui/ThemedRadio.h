#pragma once

#include "ControlState.h"
#include "ThemeAwareWindow.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Phase 41 Task 08: Radio item descriptor.
struct RadioItem
{
    std::string id;
    std::string label;
    bool is_disabled{false};
};

/// Phase 41 Task 08: Testable model for radio button group.
class RadioGroupModel
{
public:
    void set_items(std::vector<RadioItem> items);
    [[nodiscard]] auto items() const -> const std::vector<RadioItem>&;

    void set_selected_index(int index);
    [[nodiscard]] auto selected_index() const -> int;
    [[nodiscard]] auto selected_item() const -> const RadioItem*;

    void select_next();
    void select_previous();

    [[nodiscard]] auto item_count() const -> int;

private:
    std::vector<RadioItem> items_;
    int selected_index_{0};
};

/// Phase 41 Task 08: Themed radio button group.
class ThemedRadio : public ThemeAwareWindow
{
public:
    using ChangeCallback = std::function<void(int index, const RadioItem&)>;

    ThemedRadio(wxWindow* parent,
                core::ThemeEngine& theme_engine,
                std::vector<RadioItem> items = {});

    ~ThemedRadio() override = default;

    void set_items(std::vector<RadioItem> items);
    void set_selected_index(int index);
    void set_on_change(ChangeCallback callback);
    void set_enabled(bool enabled);

    [[nodiscard]] auto model() const -> const RadioGroupModel&;

    static constexpr int kCircleRadius = 8;
    static constexpr int kDotRadius = 4;
    static constexpr int kGap = 8;
    static constexpr int kItemHeight = 24;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    RadioGroupModel model_;
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

    [[nodiscard]] auto item_at_y(int y) const -> int;
};

} // namespace markamp::ui
