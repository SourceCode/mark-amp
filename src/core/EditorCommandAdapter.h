/// @file EditorCommandAdapter.h
/// @brief P04-T03: Adapter between event-driven editor commands and active editor widget.
///
/// Validates active editor existence before routing commands and publishes
/// feedback on failure via CommandFeedback.
#pragma once

#include "EventBus.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{
class CommandFeedbackHelper;

/// Routes editor command events to the active editor with validation.
///
/// Subscribes to ~20 editor command request events and validates that an
/// active editor exists before dispatching. If no editor is active or the
/// command is inapplicable, publishes user-facing feedback.
class EditorCommandAdapter
{
public:
    EditorCommandAdapter(EventBus& bus, CommandFeedbackHelper& feedback);

    /// Set the active editor availability callback.
    using EditorAvailableCallback = std::function<bool()>;
    void set_editor_available_callback(EditorAvailableCallback callback);

    /// Get the number of command subscriptions active.
    [[nodiscard]] auto subscription_count() const -> int
    {
        return static_cast<int>(subscriptions_.size());
    }

private:
    void subscribe_all();
    void guarded_dispatch(const std::string& command_name);

    EventBus& event_bus_;
    CommandFeedbackHelper& feedback_;
    EditorAvailableCallback editor_available_;
    std::vector<Subscription> subscriptions_;
};

} // namespace markamp::core
