/// @file NotebookCommandRouter.cpp
/// @brief V20 P03-T04: Notebook command router implementation.

#include "NotebookCommandRouter.h"

#include "Logger.h"

namespace markamp::core
{

NotebookCommandRouter::NotebookCommandRouter(EventBus& bus)
    : event_bus_(bus)
{
    (void)event_bus_;  // Will be used for event publishing
}

void NotebookCommandRouter::set_context(const NotebookContext& context)
{
    ++update_count_;
    context_ = context;

    MARKAMP_LOG_DEBUG("Notebook context set: notebook={}, cell={}, focused={}",
                      context_.active_notebook_id.value, context_.active_cell_id,
                      context_.is_notebook_focused);
}

void NotebookCommandRouter::clear_context()
{
    ++update_count_;
    context_ = {};

    MARKAMP_LOG_DEBUG("Notebook context cleared");
}

void NotebookCommandRouter::set_active_cell(const std::string& cell_id)
{
    context_.active_cell_id = cell_id;

    MARKAMP_LOG_DEBUG("Active cell set: {}", cell_id);
}

} // namespace markamp::core
