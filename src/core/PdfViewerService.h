/// @file PdfViewerService.h
/// @brief V9 Phase 42 — PDF viewing service with page navigation, zoom, and text extraction.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// PDF page render quality.
enum class PdfRenderQuality : uint8_t
{
    kLow = 0,    ///< Fast rendering, lower quality
    kMedium = 1, ///< Balanced
    kHigh = 2,   ///< High quality rendering
};

/// Zoom mode for PDF viewing.
enum class PdfZoomMode : uint8_t
{
    kFitWidth = 0,   ///< Fit to width
    kFitPage = 1,    ///< Fit entire page
    kActualSize = 2, ///< 100% zoom
    kCustom = 3,     ///< Custom zoom percentage
};

/// Information about a single PDF page.
struct PdfPageInfo
{
    int page_number{0};
    int width_points{0};
    int height_points{0};
    std::string extracted_text;
    bool has_annotations{false};
    bool has_images{false};
};

/// A loaded PDF document.
struct PdfDocument
{
    std::string document_id;
    std::string file_path;
    std::string title;
    std::string author;
    int total_pages{0};
    int current_page{1};
    double zoom_level{1.0};
    PdfZoomMode zoom_mode{PdfZoomMode::kFitWidth};
    PdfRenderQuality quality{PdfRenderQuality::kMedium};
    bool loaded{false};
    std::vector<PdfPageInfo> pages;
};

/// Service for viewing and navigating PDF documents.
///
/// Manages PDF loading, page navigation, zoom control, and text
/// extraction. Does not perform actual PDF rendering (platform layer).
class PdfViewerService
{
public:
    PdfViewerService() = default;

    // ── Document management ───────────────────────────────────────────
    auto load_document(const std::string& file_path) -> std::string;
    auto close_document(const std::string& document_id) -> bool;
    [[nodiscard]] auto find_document(const std::string& document_id) const -> const PdfDocument*;
    [[nodiscard]] auto document_count() const -> int;

    // ── Navigation ────────────────────────────────────────────────────
    auto go_to_page(const std::string& document_id, int page) -> bool;
    auto next_page(const std::string& document_id) -> bool;
    auto previous_page(const std::string& document_id) -> bool;

    // ── Zoom ──────────────────────────────────────────────────────────
    auto set_zoom(const std::string& document_id, double level) -> bool;
    auto set_zoom_mode(const std::string& document_id, PdfZoomMode mode) -> bool;
    auto zoom_in(const std::string& document_id) -> bool;
    auto zoom_out(const std::string& document_id) -> bool;

    // ── Content ───────────────────────────────────────────────────────
    [[nodiscard]] auto extract_text(const std::string& document_id, int page) const -> std::string;
    [[nodiscard]] auto extract_all_text(const std::string& document_id) const -> std::string;
    auto set_render_quality(const std::string& document_id, PdfRenderQuality quality) -> bool;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto all_documents() const -> std::vector<const PdfDocument*>;
    [[nodiscard]] auto open_document_ids() const -> std::vector<std::string>;
    void close_all();

private:
    std::vector<PdfDocument> documents_;
    int next_id_{1};

    auto find_mut(const std::string& document_id) -> PdfDocument*;
};

} // namespace markamp::core
