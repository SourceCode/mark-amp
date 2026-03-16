#include "PDFImporter.h"

#include "canvas/PDFPageObject.h"

#include <fstream>
#include <memory>
#include <sstream>

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

    // Read the PDF file to extract page count from the /Pages /Count entry.
    int page_count = 0;
    double page_width = 612.0;   // Default US Letter width in points.
    double page_height = 792.0;  // Default US Letter height in points.

    {
        std::ifstream pdf_file(pdf_path, std::ios::binary);
        if (!pdf_file.is_open())
        {
            return false;
        }

        // Read entire file to search for page metadata.
        std::string content((std::istreambuf_iterator<char>(pdf_file)),
                             std::istreambuf_iterator<char>());

        // Extract page count: look for "/Count N" in the /Pages dictionary.
        auto count_pos = content.find("/Count ");
        if (count_pos != std::string::npos)
        {
            auto num_start = count_pos + 7;
            auto num_end = content.find_first_not_of("0123456789", num_start);
            if (num_end != std::string::npos && num_end > num_start)
            {
                page_count = std::stoi(content.substr(num_start, num_end - num_start));
            }
        }

        // Extract page dimensions: look for "/MediaBox [x1 y1 x2 y2]".
        auto mediabox_pos = content.find("/MediaBox");
        if (mediabox_pos != std::string::npos)
        {
            auto bracket_start = content.find('[', mediabox_pos);
            auto bracket_end = content.find(']', bracket_start);
            if (bracket_start != std::string::npos && bracket_end != std::string::npos)
            {
                auto box_str = content.substr(bracket_start + 1, bracket_end - bracket_start - 1);
                double x_1 = 0;
                double y_1 = 0;
                double x_2 = 0;
                double y_2 = 0;
                std::istringstream box_stream(box_str);
                if (box_stream >> x_1 >> y_1 >> x_2 >> y_2)
                {
                    page_width = x_2 - x_1;
                    page_height = y_2 - y_1;
                }
            }
        }
    }

    // Fallback if no /Count found.
    if (page_count <= 0)
    {
        page_count = 1;
    }

    const double scaled_width = page_width * options.scale;
    const double scaled_height = page_height * options.scale;

    for (int page_idx = 0; page_idx < page_count; ++page_idx)
    {
        auto page = std::make_unique<PDFPageObject>();
        page->set_source_pdf(pdf_path);
        page->set_page_number(page_idx);
        page->set_total_pages(page_count);
        page->set_dimensions(scaled_width, scaled_height);

        if (options.horizontal_layout)
        {
            const double pos_x = static_cast<double>(page_idx) * (scaled_width + options.page_gap);
            page->set_position(pos_x, 0.0);
        }
        else
        {
            const double pos_y = static_cast<double>(page_idx) * (scaled_height + options.page_gap);
            page->set_position(0.0, pos_y);
        }

        if (on_progress_)
        {
            on_progress_(page_idx + 1, page_count);
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
