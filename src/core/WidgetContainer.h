/// @file WidgetContainer.h
/// @brief V9 Phase 41 — Widget container with sizing, visibility, and drag reorder.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// State of a widget within a container.
enum class WidgetState : uint8_t
{
    kActive = 0,   ///< Normal, interactive
    kLoading = 1,  ///< Loading data
    kError = 2,    ///< Error state
    kEmpty = 3,    ///< No data to display
    kDisabled = 4, ///< Disabled by user/extension
};

/// A widget instance within a container.
struct ContainedWidget
{
    std::string widget_id;    ///< Unique identifier
    std::string title;        ///< Display title
    std::string content_type; ///< "list", "tree", "chart", "custom"
    WidgetState state{WidgetState::kActive};
    int width{0};    ///< 0 = auto
    int height{200}; ///< Height in pixels
    int position{0}; ///< Position index within container
    bool visible{true};
    bool draggable{true};
    std::string error_message; ///< Error text when state is kError
};

/// A container that holds and manages multiple widgets.
///
/// Supports drag-and-drop reordering, visibility toggling,
/// and state management for contained widgets.
class WidgetContainer
{
public:
    WidgetContainer() = default;
    explicit WidgetContainer(std::string container_id);

    // ── Widget management ─────────────────────────────────────────────
    void add_widget(ContainedWidget widget);
    auto remove_widget(const std::string& widget_id) -> bool;
    [[nodiscard]] auto find_widget(const std::string& widget_id) const -> const ContainedWidget*;
    [[nodiscard]] auto widget_count() const -> int;

    // ── Ordering ──────────────────────────────────────────────────────
    auto move_widget(const std::string& widget_id, int new_position) -> bool;
    auto swap_widgets(const std::string& widget_a, const std::string& widget_b) -> bool;

    // ── State ─────────────────────────────────────────────────────────
    auto set_widget_state(const std::string& widget_id, WidgetState state) -> bool;
    auto set_widget_visible(const std::string& widget_id, bool visible) -> bool;
    auto set_widget_height(const std::string& widget_id, int height) -> bool;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto all_widgets() const -> const std::vector<ContainedWidget>&;
    [[nodiscard]] auto visible_widgets() const -> std::vector<const ContainedWidget*>;
    [[nodiscard]] auto ordered_widgets() const -> std::vector<const ContainedWidget*>;
    [[nodiscard]] auto container_id() const -> const std::string&;
    [[nodiscard]] auto total_height() const -> int;

    void clear_all();

private:
    std::string container_id_;
    std::vector<ContainedWidget> widgets_;

    auto find_mut(const std::string& widget_id) -> ContainedWidget*;
    void reindex_positions();
};

} // namespace markamp::core
