#include "UnsavedChangesGuard.h"

namespace markamp::ui
{

auto UnsavedChangesGuard::check(const std::string& document_name, bool has_changes) -> bool
{
    if (!has_changes)
    {
        last_action_ = Action::kDiscard; // Nothing to save.
        return true;
    }

    if (!prompt_)
    {
        // No prompt configured, default to cancel.
        last_action_ = Action::kCancel;
        return false;
    }

    last_action_ = prompt_(document_name);
    return last_action_ != Action::kCancel;
}

} // namespace markamp::ui
