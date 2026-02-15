#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <filesystem>
#include <memory>
#include <string>

namespace markamp::canvas
{

/// Open Graph / meta tag metadata scraped from a URL.
struct BookmarkMetadata
{
    std::string title;
    std::string description;
    std::string site_name;
    std::string favicon_url;
    std::string image_url;
    std::filesystem::path cached_image_path;
    std::filesystem::path cached_favicon_path;
};

/// A canvas object that displays a rich URL preview card with scraped metadata.
class BookmarkCardObject : public CanvasObject
{
public:
    BookmarkCardObject();

    [[nodiscard]] auto url() const -> const std::string&;
    auto set_url(const std::string& page_url) -> void;

    [[nodiscard]] auto bookmark_metadata() const -> const BookmarkMetadata&;
    auto set_bookmark_metadata(const BookmarkMetadata& meta) -> void;

    [[nodiscard]] auto card_width() const -> double;
    [[nodiscard]] auto card_height() const -> double;
    auto set_card_dimensions(double wid, double hei) -> void;

    [[nodiscard]] auto show_image() const -> bool;
    auto set_show_image(bool show) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string url_;
    BookmarkMetadata metadata_;
    double card_width_{320.0};
    double card_height_{180.0};
    bool show_image_{true};
};

} // namespace markamp::canvas
