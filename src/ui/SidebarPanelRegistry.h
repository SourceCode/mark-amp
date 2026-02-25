#pragma once

#include "ui/SidebarMode.h" // SidebarMode enum

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class wxPanel;
class wxWindow;

namespace markamp::ui
{

/// Factory signature: receives the parent window, returns a new wxPanel*.
using SidebarPanelFactory = std::function<wxPanel*(wxWindow* parent)>;

/// Metadata for a single sidebar panel.
struct SidebarPanelEntry
{
    SidebarMode mode;
    std::string label;     /// Display label for the panel (e.g. "EXPLORER")
    std::string icon_char; /// Unicode icon fallback
    SidebarPanelFactory factory;
    wxPanel* panel{nullptr}; /// Lazily created on first use
};

/// Registry of sidebar panels — supports lazy creation.
/// Panels are created on first access via their registered factory.
class SidebarPanelRegistry
{
public:
    /// Register a panel factory for the given sidebar mode.
    void Register(SidebarMode mode,
                  const std::string& label,
                  const std::string& icon_char,
                  SidebarPanelFactory factory);

    /// Get or lazily create the panel for the given mode.
    /// Returns nullptr if the mode has no registered factory or parent is null.
    auto GetOrCreate(SidebarMode mode, wxWindow* parent) -> wxPanel*;

    /// Get the label for a registered mode (empty string if unregistered).
    [[nodiscard]] auto GetLabel(SidebarMode mode) const -> std::string;

    /// Get the icon char for a registered mode (empty string if unregistered).
    [[nodiscard]] auto GetIconChar(SidebarMode mode) const -> std::string;

    /// Check if a mode has been registered.
    [[nodiscard]] auto IsRegistered(SidebarMode mode) const -> bool;

    /// Return all registered modes in registration order.
    [[nodiscard]] auto AllModes() const -> std::vector<SidebarMode>;

    /// Return the number of registered panels.
    [[nodiscard]] auto Count() const -> size_t;

private:
    std::vector<SidebarPanelEntry> entries_;

    /// Find the entry for a mode, or nullptr if not found.
    auto FindEntry(SidebarMode mode) -> SidebarPanelEntry*;
    [[nodiscard]] auto FindEntry(SidebarMode mode) const -> const SidebarPanelEntry*;
};

} // namespace markamp::ui
