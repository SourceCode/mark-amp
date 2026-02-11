#include "MarkAmpApp.h"

#include "core/AppState.h"
#include "core/Command.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/Logger.h"
#include "core/MermaidRenderer.h"
#include "core/RecentWorkspaces.h"
#include "core/ServiceRegistry.h"
#include "core/ThemeEngine.h"
#include "core/ThemeRegistry.h"
#include "platform/PlatformAbstraction.h"
#include "ui/MainFrame.h"

#include <spdlog/spdlog.h>

namespace markamp::app
{

// Destructor defined here where all forward-declared types are complete
MarkAmpApp::~MarkAmpApp() = default;

bool MarkAmpApp::OnInit()
{
    if (!wxApp::OnInit())
    {
        return false;
    }

    SetAppName("MarkAmp");
    SetVendorName("MarkAmp");

    // 1. Initialize logging
    core::initLogger();
    MARKAMP_LOG_INFO("MarkAmp v{}.{}.{} starting...",
                     MARKAMP_VERSION_MAJOR,
                     MARKAMP_VERSION_MINOR,
                     MARKAMP_VERSION_PATCH);
    MARKAMP_LOG_INFO(
        "wxWidgets version: {}.{}.{}", wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER);
    MARKAMP_LOG_INFO("Platform: {}",
                     wxPlatformInfo::Get().GetOperatingSystemDescription().ToStdString());

    // 2. Create core services
    event_bus_ = std::make_unique<core::EventBus>();
    MARKAMP_LOG_DEBUG("EventBus initialized");

    // 3. Load configuration
    config_ = std::make_unique<core::Config>();
    auto loadResult = config_->load();
    if (!loadResult)
    {
        MARKAMP_LOG_WARN("Config load failed: {}", loadResult.error());
    }
    else
    {
        MARKAMP_LOG_INFO("Configuration loaded");
    }

    // 3b. Initialize recent workspaces
    recent_workspaces_ = std::make_unique<core::RecentWorkspaces>(*config_);
    MARKAMP_LOG_DEBUG("RecentWorkspaces initialized");

    // 4. Initialize app state manager
    state_manager_ = std::make_unique<core::AppStateManager>(*event_bus_);
    MARKAMP_LOG_DEBUG("AppStateManager initialized");

    // 5. Initialize command history
    command_history_ = std::make_unique<core::CommandHistory>();
    MARKAMP_LOG_DEBUG("CommandHistory initialized");

    // 6. Create platform abstraction
    platform_ = platform::create_platform();
    MARKAMP_LOG_DEBUG("Platform abstraction initialized");

    // 7. Initialize theme system
    theme_registry_ = std::make_unique<core::ThemeRegistry>();
    theme_registry_->initialize();
    MARKAMP_LOG_DEBUG("ThemeRegistry initialized with {} themes",
                      theme_registry_->list_themes().size());

    theme_engine_ = std::make_unique<core::ThemeEngine>(*event_bus_, *theme_registry_);
    MARKAMP_LOG_DEBUG("ThemeEngine initialized with theme: {}",
                      theme_engine_->current_theme().name);

    // 8. Create and show the main frame (with frameless custom chrome)
    auto* frame = new ui::MainFrame("MarkAmp",
                                    wxDefaultPosition,
                                    wxSize(kDefaultWidth, kDefaultHeight),
                                    event_bus_.get(),
                                    config_.get(),
                                    recent_workspaces_.get(),
                                    platform_.get(),
                                    theme_engine_.get());

    frame->Show(true);
    SetTopWindow(frame);

    // 9. Initialize Mermaid renderer
    mermaid_renderer_ = std::make_shared<core::MermaidRenderer>();
    core::ServiceRegistry::instance().register_service<core::IMermaidRenderer>(mermaid_renderer_);
    MARKAMP_LOG_INFO("MermaidRenderer initialized (available: {})",
                     mermaid_renderer_->is_available() ? "yes" : "no");

    // 10. Publish app ready event
    core::events::AppReadyEvent readyEvent;
    event_bus_->publish(readyEvent);
    MARKAMP_LOG_INFO("MarkAmp initialization complete");

    return true;
}

int MarkAmpApp::OnExit()
{
    MARKAMP_LOG_INFO("MarkAmp shutting down...");

    // Publish shutdown event
    if (event_bus_)
    {
        core::events::AppShutdownEvent shutdownEvent;
        event_bus_->publish(shutdownEvent);
    }

    // Save configuration
    if (config_)
    {
        auto saveResult = config_->save();
        if (!saveResult)
        {
            MARKAMP_LOG_WARN("Config save failed: {}", saveResult.error());
        }
    }

    // IMPORTANT: Destroy the top-level window BEFORE resetting core services.
    // wxWidgets UI components (CustomChrome, LayoutManager, etc.) hold RAII
    // Subscription objects whose destructors call back into EventBus.
    // If we reset event_bus_ first, those destructors would access freed memory,
    // causing std::terminate via SIGABRT.
    auto* top = GetTopWindow();
    if (top != nullptr)
    {
        top->Destroy();
        DeletePendingObjects();
    }

    // Clean up in reverse order (safe now that all windows are destroyed)
    mermaid_renderer_.reset();
    command_history_.reset();
    state_manager_.reset();
    theme_engine_.reset();
    theme_registry_.reset();
    config_.reset();
    platform_.reset();
    event_bus_.reset();

    MARKAMP_LOG_INFO("MarkAmp shutdown complete");
    spdlog::shutdown();
    return wxApp::OnExit();
}

} // namespace markamp::app
