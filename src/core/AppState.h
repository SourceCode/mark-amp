#pragma once

#include "EventBus.h"
#include "Events.h"
#include "Types.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Centralized application state observable via the EventBus.
struct AppState
{
    // Current state
    std::string active_file_id;
    std::string active_file_content;
    events::ViewMode view_mode{events::ViewMode::Split};
    bool sidebar_visible{true};
    std::string current_theme_id{"midnight-neon"};
    int cursor_line{1};
    int cursor_column{1};
    bool modified_{false};
    std::string encoding{"UTF-8"};

    // File tree
    std::vector<FileNode> file_tree;

    // Derived state
    [[nodiscard]] auto active_file_name() const -> std::string;
    [[nodiscard]] auto is_modified() const -> bool;
};

/// Manages application state and publishes events on state changes.
class AppStateManager
{
public:
    explicit AppStateManager(EventBus& event_bus);

    [[nodiscard]] auto state() const -> const AppState&;

    // State mutations (each publishes the corresponding event)
    void set_active_file(const std::string& file_id);
    void set_view_mode(events::ViewMode mode);
    void set_sidebar_visible(bool visible);
    void set_cursor_position(int line, int column);
    void update_content(const std::string& content);
    void set_modified(bool modified);

private:
    AppState state_;
    EventBus& event_bus_;
};

} // namespace markamp::core
