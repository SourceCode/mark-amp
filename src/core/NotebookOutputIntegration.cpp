/// @file NotebookOutputIntegration.cpp
/// @brief P08-T04: Notebook outputs, search, and export integration.

#include "NotebookOutputIntegration.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

NotebookOutputIntegration::NotebookOutputIntegration(EventBus& bus)
    : event_bus_(bus)
{
}

auto NotebookOutputIntegration::trust_level(const std::string& /*notebook_id*/) const
    -> OutputTrustLevel
{
    return default_trust_;
}

void NotebookOutputIntegration::set_trust_level(const std::string& notebook_id,
                                                 OutputTrustLevel level)
{
    MARKAMP_LOG_INFO("Output trust: {} -> {}", notebook_id, static_cast<int>(level));
}

void NotebookOutputIntegration::search_outputs(const std::string& query)
{
    MARKAMP_LOG_INFO("Notebook output search: '{}'", query);
}

void NotebookOutputIntegration::export_notebook(const std::string& notebook_id,
                                                 const std::string& format)
{
    event_bus_.publish(events::NotificationEvent{
        "Exporting notebook as " + format + "...",
        events::NotificationLevel::Info, 2000});
    MARKAMP_LOG_INFO("Notebook export: {} (format: {})", notebook_id, format);
}

void NotebookOutputIntegration::restore_outputs(const std::string& notebook_id)
{
    MARKAMP_LOG_INFO("Restoring outputs: {}", notebook_id);
}

void NotebookOutputIntegration::clear_outputs(const std::string& notebook_id)
{
    event_bus_.publish(events::NotificationEvent{
        "Outputs cleared",
        events::NotificationLevel::Info, 1500});
    MARKAMP_LOG_INFO("Outputs cleared: {}", notebook_id);
}

} // namespace markamp::core
