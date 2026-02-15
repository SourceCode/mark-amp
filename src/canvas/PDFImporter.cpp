#include "PDFImporter.h"

#include "canvas/PDFPageObject.h"

#include <memory>

namespace markamp::canvas
{

auto PDFImporter::import_pdf(const std::filesystem::path& pdf_path,
                             Board& board,
                             const PDFImportOptions& options) -> bool
{
    if (!std::filesystem::exists(pdf_path))
    {
        return false;
    }

    // In a real implementation we would call a PDF library to get page count.
    // For now we simulate with a fixed page count based on file existence.
    // The caller is expected to provide pre-rendered page images or the
    // render_page_to_image stub would be replaced with a real renderer.
    constexpr int kSimulatedPageCount = 3;

    const double page_width = 842.0 * options.scale;
    const double page_height = 1191.0 * options.scale;

    for (int page_idx = 0; page_idx < kSimulatedPageCount; ++page_idx)
    {
        auto page = std::make_unique<PDFPageObject>();
        page->set_source_pdf(pdf_path);
        page->set_page_number(page_idx);
        page->set_total_pages(kSimulatedPageCount);
        page->set_dimensions(page_width, page_height);

        if (options.horizontal_layout)
        {
            const double pos_x = static_cast<double>(page_idx) * (page_width + options.page_gap);
            page->set_position(pos_x, 0.0);
        }
        else
        {
            const double pos_y = static_cast<double>(page_idx) * (page_height + options.page_gap);
            page->set_position(0.0, pos_y);
        }

        if (on_progress_)
        {
            on_progress_(page_idx + 1, kSimulatedPageCount);
        }

        board.add_object(std::move(page));
    }

    return true;
}

auto PDFImporter::set_on_progress(OnProgressCallback callback) -> void
{
    on_progress_ = std::move(callback);
}

auto PDFImporter::render_page_to_image(const std::filesystem::path& /*pdf_path*/,
                                       int /*page_number*/,
                                       int /*dpi*/,
                                       const std::filesystem::path& /*output_path*/) -> bool
{
    // Stub — real implementation would use poppler, mupdf, or similar.
    return false;
}

} // namespace markamp::canvas
