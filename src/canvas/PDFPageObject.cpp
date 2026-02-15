#include "PDFPageObject.h"

#include <sstream>

namespace markamp::canvas
{

PDFPageObject::PDFPageObject()
    : CanvasObject(CanvasObjectType::PDFPage)
{
    set_name("PDF Page");
}

auto PDFPageObject::source_pdf() const -> const std::filesystem::path&
{
    return source_pdf_;
}

auto PDFPageObject::set_source_pdf(const std::filesystem::path& path) -> void
{
    source_pdf_ = path;
    mark_dirty();
}

auto PDFPageObject::page_number() const -> int
{
    return page_number_;
}

auto PDFPageObject::set_page_number(int page) -> void
{
    page_number_ = page;
    mark_dirty();
}

auto PDFPageObject::total_pages() const -> int
{
    return total_pages_;
}

auto PDFPageObject::set_total_pages(int total) -> void
{
    total_pages_ = total;
    mark_dirty();
}

auto PDFPageObject::width() const -> double
{
    return width_;
}

auto PDFPageObject::height() const -> double
{
    return height_;
}

auto PDFPageObject::set_dimensions(double wid, double hei) -> void
{
    width_ = wid;
    height_ = hei;
    mark_dirty();
}

auto PDFPageObject::rendered_image_path() const -> const std::filesystem::path&
{
    return rendered_image_path_;
}

auto PDFPageObject::set_rendered_image_path(const std::filesystem::path& path) -> void
{
    rendered_image_path_ = path;
    mark_dirty();
}

auto PDFPageObject::local_bounds() const -> AABB
{
    return {0.0, 0.0, width_, height_};
}

auto PDFPageObject::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"PDFPage\""
        << ",\"source_pdf\":\"" << source_pdf_.string() << "\""
        << ",\"page_number\":" << page_number_ << ",\"total_pages\":" << total_pages_
        << ",\"width\":" << width_ << ",\"height\":" << height_ << ",\"rendered_image\":\""
        << rendered_image_path_.string() << "\""
        << "}";
    return oss.str();
}

auto PDFPageObject::from_json(const std::string& /*json*/) -> void
{
    // Stub — real implementation with nlohmann::json or similar.
}

auto PDFPageObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<PDFPageObject>();
    copy->set_source_pdf(source_pdf_);
    copy->set_page_number(page_number_);
    copy->set_total_pages(total_pages_);
    copy->set_dimensions(width_, height_);
    copy->set_rendered_image_path(rendered_image_path_);
    copy->set_name(name());
    return copy;
}

} // namespace markamp::canvas
