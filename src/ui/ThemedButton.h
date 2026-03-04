#pragma once

#include "ComponentVariants.h"
#include "ControlState.h"
#include "ThemeAwareWindow.h"

#include <functional>
#include <string>

namespace markamp::ui
{

/// Phase 41 Tasks 02-04: Themed button with variant support.
///
/// Supports five visual variants (Primary, Secondary, Ghost, Danger, Link)
/// plus IconButton mode (24×24 icon-only). Uses ControlStateTracker for
/// hover/press/focus transitions and VariantTokenResolver for themed colors.
class ThemedButton : public ThemeAwareWindow
{
public:
    using ClickCallback = std::function<void()>;

    ThemedButton(wxWindow* parent,
                 core::ThemeEngine& theme_engine,
                 const std::string& label,
                 ControlVariant variant = ControlVariant::kPrimary);

    ~ThemedButton() override = default;

    // ── Configuration ──────────────────────────────────────────────

    void set_label(const std::string& label);
    [[nodiscard]] auto label() const -> const std::string&;

    void set_variant(ControlVariant variant);
    [[nodiscard]] auto variant() const -> ControlVariant;

    void set_icon_name(const std::string& icon_name);
    [[nodiscard]] auto icon_name() const -> const std::string&;

    /// Set icon-only mode (24×24, no label text).
    void set_icon_only(bool icon_only);
    [[nodiscard]] auto is_icon_only() const -> bool;

    void set_enabled(bool enabled);
    [[nodiscard]] auto is_enabled() const -> bool;

    void set_on_click(ClickCallback callback);

    // ── State ──────────────────────────────────────────────────────

    [[nodiscard]] auto state_tracker() const -> const ControlStateTracker&;

    static constexpr int kIconSize = 16;
    static constexpr int kIconOnlySize = 24;
    static constexpr int kPaddingH = 12;
    static constexpr int kPaddingV = 6;
    static constexpr int kBorderRadius = 4;
    static constexpr int kIconLabelGap = 6;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    std::string label_;
    std::string icon_name_;
    ControlVariant variant_{ControlVariant::kPrimary};
    bool icon_only_{false};
    ControlStateTracker state_;
    ClickCallback on_click_;

    void OnPaint(wxPaintEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);

    [[nodiscard]] auto compute_best_size() const -> wxSize;
};

} // namespace markamp::ui
