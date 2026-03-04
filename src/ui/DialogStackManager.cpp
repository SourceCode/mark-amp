#include "DialogStackManager.h"

#include <algorithm>
#include <cmath>

namespace markamp::ui
{

// ── DialogStackModel ───────────────────────────────────────────────

auto DialogStackModel::push(const std::string& title, bool is_modal) -> uint64_t
{
    auto entry_id = next_id_++;
    stack_.push_back(
        {.id = entry_id, .title = title, .is_modal = is_modal, .z_order = next_z_order_++});
    return entry_id;
}

void DialogStackModel::pop()
{
    if (!stack_.empty())
    {
        stack_.pop_back();
    }
}

void DialogStackModel::remove(uint64_t dialog_id)
{
    std::erase_if(stack_, [dialog_id](const DialogEntry& entry) { return entry.id == dialog_id; });
}

auto DialogStackModel::top_id() const -> uint64_t
{
    return stack_.empty() ? 0 : stack_.back().id;
}

auto DialogStackModel::stack() const -> const std::vector<DialogEntry>&
{
    return stack_;
}

auto DialogStackModel::is_empty() const -> bool
{
    return stack_.empty();
}
auto DialogStackModel::depth() const -> int
{
    return static_cast<int>(stack_.size());
}

void DialogStackModel::bring_to_top(uint64_t dialog_id)
{
    auto iter = std::ranges::find_if(
        stack_, [dialog_id](const DialogEntry& entry) { return entry.id == dialog_id; });
    if (iter != stack_.end())
    {
        auto found_entry = *iter;
        stack_.erase(iter);
        found_entry.z_order = next_z_order_++;
        stack_.push_back(found_entry);
    }
}

// ── WizardModel ────────────────────────────────────────────────────

void WizardModel::set_steps(std::vector<WizardStep> steps)
{
    steps_ = std::move(steps);
    current_ = 0;
}
auto WizardModel::steps() const -> const std::vector<WizardStep>&
{
    return steps_;
}
auto WizardModel::current_step() const -> int
{
    return current_;
}
auto WizardModel::current_step_info() const -> const WizardStep*
{
    return current_ >= 0 && current_ < static_cast<int>(steps_.size())
               ? &steps_[static_cast<size_t>(current_)]
               : nullptr;
}
auto WizardModel::step_count() const -> int
{
    return static_cast<int>(steps_.size());
}

auto WizardModel::next() -> bool
{
    if (current_ < static_cast<int>(steps_.size()) - 1)
    {
        ++current_;
        return true;
    }
    return false;
}

auto WizardModel::previous() -> bool
{
    if (current_ > 0)
    {
        --current_;
        return true;
    }
    return false;
}

void WizardModel::go_to_step(int index)
{
    if (index >= 0 && index < static_cast<int>(steps_.size()))
    {
        current_ = index;
    }
}

void WizardModel::mark_step_complete(int index)
{
    if (index >= 0 && index < static_cast<int>(steps_.size()))
    {
        steps_[static_cast<size_t>(index)].is_complete = true;
    }
}

auto WizardModel::can_proceed() const -> bool
{
    if (steps_.empty())
    {
        return false;
    }
    return steps_[static_cast<size_t>(current_)].is_complete ||
           steps_[static_cast<size_t>(current_)].is_optional;
}

auto WizardModel::is_last_step() const -> bool
{
    return current_ == static_cast<int>(steps_.size()) - 1;
}
auto WizardModel::is_first_step() const -> bool
{
    return current_ == 0;
}

auto WizardModel::progress() const -> double
{
    if (steps_.empty())
    {
        return 0.0;
    }
    int completed = 0;
    for (const auto& step : steps_)
    {
        if (step.is_complete)
        {
            ++completed;
        }
    }
    return static_cast<double>(completed) / static_cast<double>(steps_.size());
}

// ── InputDialogModel ───────────────────────────────────────────────

void InputDialogModel::set_value(const std::string& value)
{
    value_ = value;
}
auto InputDialogModel::value() const -> const std::string&
{
    return value_;
}
void InputDialogModel::set_prompt(const std::string& prompt)
{
    prompt_ = prompt;
}
auto InputDialogModel::prompt() const -> const std::string&
{
    return prompt_;
}
void InputDialogModel::set_validator(Validator validator)
{
    validator_ = std::move(validator);
}

auto InputDialogModel::validate() const -> std::string
{
    if (validator_)
    {
        return validator_(value_);
    }
    return {};
}

auto InputDialogModel::is_valid() const -> bool
{
    return validate().empty();
}

} // namespace markamp::ui
