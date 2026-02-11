#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// 24px status bar at bottom of window.
/// Displays real-time info: ready state, cursor position, encoding, view mode,
/// word count, Mermaid status, and current theme name.
class StatusBarPanel : public ThemeAwareWindow
{
public:
    StatusBarPanel(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    // State setters
    void set_cursor_position(int line, int column);
    void set_encoding(const std::string& encoding);
    void set_ready_state(const std::string& state);
    void set_mermaid_status(const std::string& status, bool active);
    void set_word_count(int count);
    void set_file_modified(bool modified);
    void set_view_mode(core::events::ViewMode mode);

    // Accessors for testing
    [[nodiscard]] auto ready_state() const -> const std::string&
    {
        return ready_state_;
    }
    [[nodiscard]] auto cursor_line() const -> int
    {
        return cursor_line_;
    }
    [[nodiscard]] auto cursor_col() const -> int
    {
        return cursor_col_;
    }
    [[nodiscard]] auto encoding() const -> const std::string&
    {
        return encoding_;
    }
    [[nodiscard]] auto mermaid_status() const -> const std::string&
    {
        return mermaid_status_;
    }
    [[nodiscard]] auto mermaid_active() const -> bool
    {
        return mermaid_active_;
    }
    [[nodiscard]] auto theme_name() const -> const std::string&
    {
        return theme_name_;
    }
    [[nodiscard]] auto word_count() const -> int
    {
        return word_count_;
    }
    [[nodiscard]] auto file_modified() const -> bool
    {
        return file_modified_;
    }
    [[nodiscard]] auto view_mode() const -> core::events::ViewMode
    {
        return view_mode_;
    }

    static constexpr int kHeight = 24;

    /// Data-driven layout item for status bar rendering.
    struct StatusItem
    {
        std::string text;
        wxRect bounds;
        bool is_accent{false};
        bool is_clickable{false};
        std::function<void()> on_click;
    };

    /// Rebuild left/right items from current state. Public for testing.
    void RebuildItems();

    [[nodiscard]] auto left_items() const -> const std::vector<StatusItem>&
    {
        return left_items_;
    }
    [[nodiscard]] auto right_items() const -> const std::vector<StatusItem>&
    {
        return right_items_;
    }

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;

    // Event subscriptions
    core::Subscription theme_name_sub_;
    core::Subscription cursor_sub_;
    core::Subscription content_sub_;
    core::Subscription view_mode_sub_;
    core::Subscription encoding_sub_;
    core::Subscription mermaid_sub_;

    // State
    std::string ready_state_{"READY"};
    int cursor_line_{1};
    int cursor_col_{1};
    std::string encoding_{"UTF-8"};
    std::string mermaid_status_{"ACTIVE"};
    bool mermaid_active_{true};
    std::string theme_name_;
    int word_count_{0};
    bool file_modified_{false};
    core::events::ViewMode view_mode_{core::events::ViewMode::Split};

    // Layout items
    std::vector<StatusItem> left_items_;
    std::vector<StatusItem> right_items_;

    // Event handlers
    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);

    /// Count words in a content string (whitespace-separated tokens).
    static auto count_words(const std::string& content) -> int;

    /// Convert ViewMode to display string.
    static auto view_mode_label(core::events::ViewMode mode) -> std::string;
};

} // namespace markamp::ui
