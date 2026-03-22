#pragma once

#include "VisualLanguageTokens.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// V22 Phase 02: Shell chrome coordination service.
///
/// Manages workbench proportions, splitter visual states, shell responsiveness,
/// and empty-state configuration for the shell chrome layer.
///
/// Usage:
///   ShellChromeCoordinator coord;
///   auto proportions = coord.workbench_proportions();
///   coord.rebalance_proportions(RebalanceMode::kCompact);
class ShellChromeCoordinator
{
public:
    /// Splitter visual state — controls hover/drag appearance.
    enum class SplitterState : uint8_t
    {
        kRest,    /// Normal: thin visual line, standard border color
        kHover,   /// Hover: widened, accent-tinted background
        kDragging /// Dragging: full accent background, cursor change
    };

    /// Shell responsiveness mode — automatic or manually set.
    enum class ResponsivenessMode : uint8_t
    {
        kWide,    /// Window width ≥ 1200px — full layout
        kDefault, /// Window width 900–1199px — standard layout
        kCompact  /// Window width < 900px — collapsed sidebar, reduced chrome
    };

    /// Rebalance mode for workbench proportions.
    enum class RebalanceMode : uint8_t
    {
        kDefault,  /// Standard proportions (sidebar 260px, panel 200px)
        kCompact,  /// Compact (sidebar 200px, panel 160px)
        kCustom    /// User-customized proportions
    };

    /// Workbench zone proportions (logical pixels).
    struct WorkbenchProportions
    {
        int sidebar_width{260};
        int secondary_sidebar_width{260};
        int panel_height{200};
        int activity_bar_width{48};
        int status_bar_height{22};
    };

    /// Empty shell chrome configuration.
    struct EmptyStateConfig
    {
        std::string title;
        std::string subtitle;
        std::string icon_name;
        bool show_recent_files{true};
        bool show_quick_actions{true};
    };

    ShellChromeCoordinator() = default;

    /// Current workbench proportions.
    [[nodiscard]] auto workbench_proportions() const -> WorkbenchProportions
    {
        return proportions_;
    }

    /// Rebalance proportions to a predefined mode.
    void rebalance_proportions(RebalanceMode mode);

    /// Current splitter visual state.
    [[nodiscard]] auto splitter_visual_state() const -> SplitterState
    {
        return splitter_state_;
    }

    /// Set the splitter state (called by SplitterBar on mouse events).
    void set_splitter_state(SplitterState state)
    {
        splitter_state_ = state;
    }

    /// Current shell responsiveness mode.
    [[nodiscard]] auto responsiveness_mode() const -> ResponsivenessMode
    {
        return responsiveness_mode_;
    }

    /// Update responsiveness based on window width.
    void update_responsiveness(int window_width);

    /// Empty state configuration for the shell.
    [[nodiscard]] auto empty_state_config() const -> EmptyStateConfig
    {
        return empty_state_;
    }

    /// Set the empty state configuration.
    void set_empty_state_config(const EmptyStateConfig& config)
    {
        empty_state_ = config;
    }

    /// Compact mode threshold in pixels.
    static constexpr int kCompactThreshold = 900;
    /// Wide mode threshold in pixels.
    static constexpr int kWideThreshold = 1200;

private:
    WorkbenchProportions proportions_;
    SplitterState splitter_state_{SplitterState::kRest};
    ResponsivenessMode responsiveness_mode_{ResponsivenessMode::kDefault};
    EmptyStateConfig empty_state_{
        "Welcome to MarkAmp",
        "Open a file or create a new document to get started",
        "activity-explorer",
        true,
        true};
};

} // namespace markamp::core
