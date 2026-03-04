#pragma once

/// @file FloatingToolbarManager.h
/// @brief V13 Phase 30 — Central manager for floating toolbar lifecycle and z-order.

#include "FloatingToolbar.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::ui
{

/// Manages all floating toolbar instances: registration, visibility, z-order,
/// and position persistence.
class FloatingToolbarManager
{
public:
    FloatingToolbarManager(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           core::EventBus& event_bus);

    /// Register a toolbar with a unique ID.
    void RegisterToolbar(const std::string& toolbar_id, std::unique_ptr<FloatingToolbar> toolbar);

    /// Show a registered toolbar at a screen position.
    void ShowToolbar(const std::string& toolbar_id, const wxPoint& position);

    /// Hide a specific toolbar.
    void HideToolbar(const std::string& toolbar_id);

    /// Hide all visible toolbars.
    void HideAll();

    /// Get a registered toolbar by ID (nullptr if not found).
    [[nodiscard]] auto GetToolbar(const std::string& toolbar_id) -> FloatingToolbar*;

    /// Check if a toolbar is currently visible.
    [[nodiscard]] auto IsVisible(const std::string& toolbar_id) const -> bool;

    /// Number of registered toolbars.
    [[nodiscard]] auto toolbar_count() const -> int;

    /// Number of currently visible toolbars.
    [[nodiscard]] auto visible_count() const -> int;

    /// Bring a toolbar to the front of the z-order.
    void BringToFront(const std::string& toolbar_id);

    /// Save current positions of all visible toolbars (in-memory).
    void SavePositions();

    /// Restore previously saved positions.
    void RestorePositions();

    /// Get z-order stack (front to back).
    [[nodiscard]] auto z_order() const -> const std::vector<std::string>&;

private:
    wxWindow* parent_;
    core::ThemeEngine& theme_engine_; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    core::EventBus& event_bus_;       // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

    std::unordered_map<std::string, std::unique_ptr<FloatingToolbar>> toolbars_;
    std::unordered_map<std::string, wxPoint> saved_positions_;
    std::vector<std::string> z_order_; ///< Front to back ordering
};

} // namespace markamp::ui
