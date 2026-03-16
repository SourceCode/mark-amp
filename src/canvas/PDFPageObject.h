#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <filesystem>
#include <memory>

namespace markamp::canvas
{

/// Represents a single page of an imported PDF displayed on the canvas.
class PDFPageObject : public CanvasObject
{
public:
    PDFPageObject();

    [[nodiscard]] auto source_pdf() const -> const std::filesystem::path&;
    auto set_source_pdf(const std::filesystem::path& path) -> void;

    [[nodiscard]] auto page_number() const -> int;
    auto set_page_number(int page) -> void;

    [[nodiscard]] auto total_pages() const -> int;
    auto set_total_pages(int total) -> void;

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto set_dimensions(double wid, double hei) -> void;

    [[nodiscard]] auto rendered_image_path() const -> const std::filesystem::path&;
    auto set_rendered_image_path(const std::filesystem::path& path) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::filesystem::path source_pdf_;
    int page_number_{0};
    int total_pages_{0};
    double width_{842.0};   // A4 width at 72 DPI
    double height_{1191.0}; // A4 height at 72 DPI
    std::filesystem::path rendered_image_path_;

    // ── Round 3 Batch 10 (#93-96) ────────────────────────────────

    /// (#93) Whether this is the first page.
    [[nodiscard]] auto is_first_page() const noexcept -> bool
    {
        return page_number_ == 0;
    }

    /// (#94) Whether this is the last page.
    [[nodiscard]] auto is_last_page() const noexcept -> bool
    {
        return total_pages_ > 0 && page_number_ == total_pages_ - 1;
    }

    /// (#95) Page area in world units².
    [[nodiscard]] auto area() const noexcept -> double
    {
        return width_ * height_;
    }

    /// (#96) Whether a rendered image path is set.
    [[nodiscard]] auto has_rendered_image() const noexcept -> bool
    {
        return !rendered_image_path_.empty();
    }
};

} // namespace markamp::canvas
