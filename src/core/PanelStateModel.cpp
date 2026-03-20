/// @file PanelStateModel.cpp
/// @brief P09-T03: Normalized error, empty, and recovery states for panels.

#include "PanelStateModel.h"

#include "Logger.h"

namespace markamp::core
{

void PanelStateModel::set_state(PanelDisplayState state)
{
    state_ = state;
    MARKAMP_LOG_DEBUG("Panel state: {}", static_cast<int>(state));
}

void PanelStateModel::set_state(PanelDisplayState state, const std::string& message)
{
    state_ = state;
    message_ = message;
    MARKAMP_LOG_DEBUG("Panel state: {} ({})", static_cast<int>(state), message);
}

void PanelStateModel::retry()
{
    if (retry_action_)
    {
        MARKAMP_LOG_INFO("Panel retry initiated");
        set_state(PanelDisplayState::kLoading, "Retrying...");
        retry_action_();
    }
}

} // namespace markamp::core
