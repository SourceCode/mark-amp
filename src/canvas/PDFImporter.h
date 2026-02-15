#pragma once

#include "canvas/Board.h"

#include <filesystem>
#include <functional>

namespace markamp::canvas
{

/// Configuration for PDF import: layout direction, spacing, DPI, and scale.
struct PDFImportOptions
{
    bool create_frames{true};
    double page_gap{50.0};
    double scale{1.0};
    int dpi{150};
    bool horizontal_layout{false}; // false = vertical stack
};

/// Imports a PDF file into a Board by creating one PDFPageObject per page,
/// positioned according to the chosen layout.
class PDFImporter
{
public:
    using OnProgressCallback = std::function<void(int current_page, int total_pages)>;

    /// Import a PDF into the board. Creates one PDFPageObject per page.
    /// Returns true on success.
    auto import_pdf(const std::filesystem::path& pdf_path,
                    Board& board,
                    const PDFImportOptions& options) -> bool;

    auto set_on_progress(OnProgressCallback callback) -> void;

private:
    OnProgressCallback on_progress_;

    /// Render a single page to a PNG image.  Stubbed — real implementation
    /// requires a PDF rendering library (poppler, mupdf, etc.).
    auto render_page_to_image(const std::filesystem::path& pdf_path,
                              int page_number,
                              int dpi,
                              const std::filesystem::path& output_path) -> bool;
};

} // namespace markamp::canvas
