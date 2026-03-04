#pragma once

#include <functional>
#include <string>

namespace markamp::ui
{

/// Phase 44: Guard for unsaved changes before close/navigation.
class UnsavedChangesGuard
{
public:
    enum class Action
    {
        kSave,
        kDiscard,
        kCancel,
    };

    using PromptCallback = std::function<Action(const std::string& document_name)>;

    void set_prompt(PromptCallback callback)
    {
        prompt_ = std::move(callback);
    }

    /// Check if there are unsaved changes. If so, prompt the user.
    /// Returns true if the caller should proceed (save or discard chosen).
    auto check(const std::string& document_name, bool has_changes) -> bool;

    /// Most recent action taken.
    [[nodiscard]] auto last_action() const -> Action
    {
        return last_action_;
    }

private:
    PromptCallback prompt_;
    Action last_action_{Action::kCancel};
};

} // namespace markamp::ui
