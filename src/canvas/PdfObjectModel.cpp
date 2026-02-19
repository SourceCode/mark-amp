#include "PdfObjectModel.h"

#include <algorithm>

namespace markamp::canvas
{

void PdfObjectModel::set_import_mode(PdfImportMode mode)
{
    import_mode_ = mode;
}
auto PdfObjectModel::import_mode() const -> PdfImportMode
{
    return import_mode_;
}

void PdfObjectModel::set_page_range(int start_page, int end_page)
{
    range_start_ = std::max(1, start_page);
    range_end_ = std::max(range_start_, end_page);
}

auto PdfObjectModel::range_start() const -> int
{
    return range_start_;
}
auto PdfObjectModel::range_end() const -> int
{
    return range_end_;
}

void PdfObjectModel::set_total_pages(int total)
{
    total_pages_ = std::max(1, total);
}
auto PdfObjectModel::total_pages() const -> int
{
    return total_pages_;
}

void PdfObjectModel::set_current_page(int page)
{
    current_page_ = std::clamp(page, 1, total_pages_);
}
auto PdfObjectModel::current_page() const -> int
{
    return current_page_;
}

void PdfObjectModel::next_page()
{
    if (current_page_ < total_pages_)
    {
        ++current_page_;
    }
}

void PdfObjectModel::prev_page()
{
    if (current_page_ > 1)
    {
        --current_page_;
    }
}

void PdfObjectModel::set_render_dpi(int dpi)
{
    render_dpi_ = std::clamp(dpi, 72, 600);
}
auto PdfObjectModel::render_dpi() const -> int
{
    return render_dpi_;
}

void PdfObjectModel::set_source_path(const std::string& path)
{
    source_path_ = path;
}
auto PdfObjectModel::source_path() const -> const std::string&
{
    return source_path_;
}

void PdfObjectModel::set_link_state(PdfLinkState state)
{
    link_state_ = state;
}
auto PdfObjectModel::link_state() const -> PdfLinkState
{
    return link_state_;
}

void PdfObjectModel::relink(const std::string& new_path)
{
    source_path_ = new_path;
    link_state_ = PdfLinkState::kLinked;
}

} // namespace markamp::canvas
