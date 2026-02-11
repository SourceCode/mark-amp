#pragma once

#include "core/RecentWorkspaces.h"
#include "platform/PlatformAbstraction.h"

#include <wx/app.h>

#include <memory>

namespace markamp::core
{
class EventBus;
class Config;
class AppStateManager;
class RecentWorkspaces;
class CommandHistory;
class MermaidRenderer;
class ThemeRegistry;
class ThemeEngine;
} // namespace markamp::core

namespace markamp::platform
{
class PlatformAbstraction;
} // namespace markamp::platform

namespace markamp::app
{

class MarkAmpApp : public wxApp
{
public:
    bool OnInit() override;
    int OnExit() override;

    // Destructor must be declared here, defined in .cpp where types are complete
    ~MarkAmpApp() override;

    // Application-wide settings
    static constexpr int kDefaultWidth = 1280;
    static constexpr int kDefaultHeight = 800;
    static constexpr int kMinWidth = 800;
    static constexpr int kMinHeight = 600;

private:
    // Core services (owned by the app, lifetime-managed)
    std::unique_ptr<core::EventBus> event_bus_;
    std::unique_ptr<core::Config> config_;
    std::unique_ptr<core::RecentWorkspaces> recent_workspaces_;
    std::unique_ptr<core::AppStateManager> state_manager_;
    std::unique_ptr<core::CommandHistory> command_history_;
    std::unique_ptr<platform::PlatformAbstraction> platform_;
    std::unique_ptr<core::ThemeRegistry> theme_registry_;
    std::unique_ptr<core::ThemeEngine> theme_engine_;
    std::shared_ptr<core::MermaidRenderer> mermaid_renderer_;
};

} // namespace markamp::app
