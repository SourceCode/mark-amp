#pragma once

#include "ControlState.h"
#include "ThemeAwareWindow.h"

#include <functional>
#include <string>

namespace markamp::ui
{

/// Phase 41 Task 13: Testable model for chip/tag.
class ChipTagModel
{
public:
    void set_label(const std::string& label);
    [[nodiscard]] auto label() const -> const std::string&;

    void set_removable(bool removable);
    [[nodiscard]] auto is_removable() const -> bool;

    void set_icon_name(const std::string& icon_name);
    [[nodiscard]] auto icon_name() const -> const std::string&;

    void set_selected(bool selected);
    [[nodiscard]] auto is_selected() const -> bool;

private:
    std::string label_;
    std::string icon_name_;
    bool removable_{true};
    bool selected_{false};
};

/// Phase 41 Task 13: Themed chip/tag component.
class ChipTag : public ThemeAwareWindow
{
public:
    using RemoveCallback = std::function<void()>;
    using ClickCallback = std::function<void()>;

    ChipTag(wxWindow* parent, core::ThemeEngine& theme_engine, const std::string& label = {});
    ~ChipTag() override = default;

    void set_label(const std::string& label);
    void set_removable(bool removable);
    void set_icon_name(const std::string& icon_name);
    void set_selected(bool selected);
    void set_on_remove(RemoveCallback callback);
    void set_on_click(ClickCallback callback);

    [[nodiscard]] auto model() const -> const ChipTagModel&;

    static constexpr int kHeight = 24;
    static constexpr int kPaddingH = 8;
    static constexpr int kBorderRadius = 12;
    static constexpr int kCloseSize = 12;
    static constexpr int kIconSize = 14;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    ChipTagModel model_;
    ControlStateTracker state_;
    RemoveCallback on_remove_;
    ClickCallback on_click_;

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
};

} // namespace markamp::ui
