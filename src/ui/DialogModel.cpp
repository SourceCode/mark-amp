#include "DialogModel.h"

#include <algorithm>

namespace markamp::ui
{

void DialogModel::set_severity(DialogSeverity severity)
{
    severity_ = severity;
}

void DialogModel::set_content(const std::string& title, const std::string& message)
{
    title_ = title;
    message_ = message;
}

void DialogModel::set_buttons(std::vector<DialogButton> buttons)
{
    buttons_ = std::move(buttons);
}

auto DialogModel::severity() const -> DialogSeverity
{
    return severity_;
}

auto DialogModel::title() const -> const std::string&
{
    return title_;
}

auto DialogModel::message() const -> const std::string&
{
    return message_;
}

auto DialogModel::ordered_buttons() const -> std::vector<DialogButton>
{
    // macOS convention: Primary action leftmost, Cancel rightmost
    auto ordered = buttons_;
    std::stable_sort(ordered.begin(),
                     ordered.end(),
                     [](const DialogButton& lhs, const DialogButton& rhs)
                     { return static_cast<int>(lhs.role) < static_cast<int>(rhs.role); });
    return ordered;
}

auto DialogModel::default_button_index() const -> int
{
    const auto ordered = ordered_buttons();
    for (int idx = 0; idx < static_cast<int>(ordered.size()); ++idx)
    {
        if (ordered[static_cast<std::size_t>(idx)].is_default)
        {
            return idx;
        }
    }
    // If no explicit default, primary button is default
    for (int idx = 0; idx < static_cast<int>(ordered.size()); ++idx)
    {
        if (ordered[static_cast<std::size_t>(idx)].role == DialogButtonRole::kPrimary)
        {
            return idx;
        }
    }
    return 0;
}

auto DialogModel::prefer_undo(DialogSeverity severity) -> bool
{
    // Only info-level actions should use undo; warning/danger need confirmation
    return severity == DialogSeverity::kInfo;
}

void DialogModel::remember_choice(const std::string& dialog_key, int button_index)
{
    for (auto& choice : remembered_)
    {
        if (choice.dialog_key == dialog_key)
        {
            choice.button_index = button_index;
            return;
        }
    }
    remembered_.push_back({dialog_key, button_index});
}

auto DialogModel::remembered_choice(const std::string& dialog_key) const -> int
{
    for (const auto& choice : remembered_)
    {
        if (choice.dialog_key == dialog_key)
        {
            return choice.button_index;
        }
    }
    return -1;
}

auto DialogModel::has_remembered(const std::string& dialog_key) const -> bool
{
    return remembered_choice(dialog_key) >= 0;
}

void DialogModel::reset_remembered(const std::string& dialog_key)
{
    remembered_.erase(std::remove_if(remembered_.begin(),
                                     remembered_.end(),
                                     [&dialog_key](const RememberedChoice& choice)
                                     { return choice.dialog_key == dialog_key; }),
                      remembered_.end());
}

void DialogModel::reset_all_remembered()
{
    remembered_.clear();
}

} // namespace markamp::ui
