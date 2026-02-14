#pragma once

#include "core/OutputChannelService.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Tabbed output channel viewer in the bottom panel (#44).
/// Displays extension output channels with channel selector, color-coded log levels,
/// and scrollable text view. Mirrors VS Code's "Output" panel.
class OutputPanel
{
public:
    OutputPanel() = default;

    /// Set the output channel service to read from.
    void set_service(core::OutputChannelService* service);

    /// Get the currently active channel name.
    [[nodiscard]] auto active_channel() const -> const std::string&;

    /// Set the active channel.
    void set_active_channel(const std::string& channel_name);

    /// Get the list of available channel names.
    [[nodiscard]] auto channel_names() const -> std::vector<std::string>;

    /// Get the content for the active channel.
    [[nodiscard]] auto active_content() const -> std::string;

    /// Whether auto-scroll to bottom is enabled.
    [[nodiscard]] auto auto_scroll() const -> bool;
    void set_auto_scroll(bool enabled);

    /// Clear the active channel's content.
    void clear_active_channel();

    /// Lock scrolling (stop auto-scroll when user scrolls up).
    void lock_scroll();

    /// Unlock scrolling (resume auto-scroll).
    void unlock_scroll();

private:
    core::OutputChannelService* service_{nullptr};
    std::string active_channel_;
    bool auto_scroll_{true};
};

} // namespace markamp::ui
