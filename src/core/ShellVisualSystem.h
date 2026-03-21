/// @file ShellVisualSystem.h
/// @brief V20 P09-T01/T03: Shared shell visual system.
///
/// Standardizes shell background layers, panel chrome, borders, hover states,
/// shadows, and surface hierarchy tokens. Provides shell-layer definitions
/// for consistent tab, tree, toolbar, dialog, and toast presentation.
#pragma once

#include "EventBus.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Shell surface layer for hierarchy and z-ordering.
enum class ShellLayer
{
    kBase,         ///< App background
    kSidebar,      ///< Sidebar/explorer
    kEditor,       ///< Editor area
    kPanel,        ///< Bottom panels
    kStatusBar,    ///< Status bar
    kTitleBar,     ///< Title bar
    kPopover,      ///< Floating menus/dropdowns
    kDialog,       ///< Modal dialogs
    kToast,        ///< Toast notifications
    kOverlay       ///< Full-screen overlays
};

/// Shell chrome token for consistent surface treatment.
struct ShellChromeToken
{
    std::string name;             ///< e.g. "shell.sidebar.bg"
    ShellLayer layer{ShellLayer::kBase};
    std::string background;       ///< Background color
    std::string border_color;     ///< Border color
    std::string text_color;       ///< Text color
    int border_width{0};          ///< Border width (px, 0 = none)
    int shadow_elevation{0};      ///< Shadow elevation (0 = none)
    double opacity{1.0};          ///< Surface opacity

    [[nodiscard]] auto has_border() const noexcept -> bool { return border_width > 0; }
    [[nodiscard]] auto has_shadow() const noexcept -> bool { return shadow_elevation > 0; }
};

/// State token for interactive shell surfaces.
struct ShellStateToken
{
    std::string surface;          ///< e.g. "tab", "tree-row", "toolbar-button"
    std::string state;            ///< e.g. "hover", "active", "disabled", "dirty"
    std::string background;       ///< State background
    std::string foreground;       ///< State foreground
    std::string border_color;     ///< State border
    double opacity{1.0};          ///< State opacity
};

/// Manages shared shell visual system tokens.
class ShellVisualSystem
{
public:
    explicit ShellVisualSystem(EventBus& bus);

    /// Register a shell chrome token.
    void register_chrome(const ShellChromeToken& token);

    /// Register a shell state token.
    void register_state(const ShellStateToken& token);

    /// Look up chrome by name.
    [[nodiscard]] auto chrome(const std::string& name) const -> const ShellChromeToken*;

    /// Look up state by surface+state key.
    [[nodiscard]] auto state(const std::string& surface, const std::string& state_name) const
        -> const ShellStateToken*;

    /// All chrome tokens.
    [[nodiscard]] auto all_chrome() const -> std::vector<ShellChromeToken>;

    /// All state tokens.
    [[nodiscard]] auto all_states() const -> std::vector<ShellStateToken>;

    /// Total tokens.
    [[nodiscard]] auto total_tokens() const noexcept -> int
    {
        return static_cast<int>(chrome_.size() + states_.size());
    }

    /// Populate defaults.
    void register_defaults();

private:
    EventBus& event_bus_;
    std::unordered_map<std::string, ShellChromeToken> chrome_;
    std::unordered_map<std::string, ShellStateToken> states_;  ///< keyed by "surface.state"
};

} // namespace markamp::core
