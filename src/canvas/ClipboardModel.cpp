#include "ClipboardModel.h"

#include <algorithm>

namespace markamp::canvas
{

void ClipboardModel::copy(std::vector<ClipboardEntry> entries)
{
    clipboard_ = std::move(entries);
}
auto ClipboardModel::clipboard() const -> const std::vector<ClipboardEntry>&
{
    return clipboard_;
}
auto ClipboardModel::has_clipboard() const -> bool
{
    return !clipboard_.empty();
}
void ClipboardModel::clear_clipboard()
{
    clipboard_.clear();
}

void ClipboardModel::set_paste_mode(PasteMode mode)
{
    paste_mode_ = mode;
}
auto ClipboardModel::paste_mode() const -> PasteMode
{
    return paste_mode_;
}

void ClipboardModel::set_duplicate_mode(DuplicateMode mode)
{
    dup_mode_ = mode;
}
auto ClipboardModel::duplicate_mode() const -> DuplicateMode
{
    return dup_mode_;
}

void ClipboardModel::set_duplicate_offset(double offset_x, double offset_y)
{
    offset_x_ = offset_x;
    offset_y_ = offset_y;
}

auto ClipboardModel::offset_x() const -> double
{
    return offset_x_;
}
auto ClipboardModel::offset_y() const -> double
{
    return offset_y_;
}

void ClipboardModel::save_template(BoardTemplate tmpl)
{
    templates_.push_back(std::move(tmpl));
}

void ClipboardModel::remove_template(const std::string& template_id)
{
    templates_.erase(std::remove_if(templates_.begin(),
                                    templates_.end(),
                                    [&](const BoardTemplate& tmpl)
                                    { return tmpl.template_id == template_id; }),
                     templates_.end());
}

auto ClipboardModel::templates() const -> const std::vector<BoardTemplate>&
{
    return templates_;
}

} // namespace markamp::canvas
