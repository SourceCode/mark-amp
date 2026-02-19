#include "FindReplaceModel.h"

#include <sstream>

namespace markamp::ui
{

void FindReplaceModel::set_find_text(const std::string& text)
{
    find_text_ = text;
}
auto FindReplaceModel::find_text() const -> const std::string&
{
    return find_text_;
}

void FindReplaceModel::set_replace_text(const std::string& text)
{
    replace_text_ = text;
}
auto FindReplaceModel::replace_text() const -> const std::string&
{
    return replace_text_;
}

void FindReplaceModel::set_case_sensitive(bool enabled)
{
    case_sensitive_ = enabled;
}
auto FindReplaceModel::case_sensitive() const -> bool
{
    return case_sensitive_;
}

void FindReplaceModel::set_regex(bool enabled)
{
    regex_ = enabled;
}
auto FindReplaceModel::is_regex() const -> bool
{
    return regex_;
}

void FindReplaceModel::set_whole_word(bool enabled)
{
    whole_word_ = enabled;
}
auto FindReplaceModel::whole_word() const -> bool
{
    return whole_word_;
}

void FindReplaceModel::set_scope(FindScope scope)
{
    scope_ = scope;
}
auto FindReplaceModel::scope() const -> FindScope
{
    return scope_;
}

void FindReplaceModel::set_match_count(int count)
{
    match_count_ = count;
    if (match_count_ <= 0)
    {
        current_index_ = 0;
    }
    else if (current_index_ >= match_count_)
    {
        current_index_ = 0;
    }
}

auto FindReplaceModel::match_count() const -> int
{
    return match_count_;
}
auto FindReplaceModel::current_index() const -> int
{
    return current_index_;
}

void FindReplaceModel::next_match()
{
    if (match_count_ <= 0)
        return;
    current_index_ = (current_index_ + 1) % match_count_;
}

void FindReplaceModel::prev_match()
{
    if (match_count_ <= 0)
        return;
    current_index_ = (current_index_ - 1 + match_count_) % match_count_;
}

auto FindReplaceModel::status_text() const -> std::string
{
    if (match_count_ <= 0)
    {
        return "No results";
    }
    std::ostringstream oss;
    oss << (current_index_ + 1) << " of " << match_count_;
    return oss.str();
}

auto FindReplaceModel::needs_confirmation(ReplaceAction action) -> bool
{
    // Only replace-all needs confirmation; single and selection are scoped
    return action == ReplaceAction::kReplaceAll;
}

} // namespace markamp::ui
