// ============================================================================
// File: src/core/PrintPreparationService.h
// Phase 24: Export & Publishing — Print preparation and preview
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Data structures
// ============================================================================

/// Standard page sizes.
enum class PageSize : uint8_t
{
    kA4,
    kA5,
    kLetter,
    kLegal,
    kCustom
};

/// Page layout configuration for printing.
struct PrintLayout
{
    PageSize page_size{PageSize::kA4};
    std::string margin_top{"20mm"};
    std::string margin_bottom{"20mm"};
    std::string margin_left{"25mm"};
    std::string margin_right{"25mm"};
    std::string header_text; ///< Header template (supports {{title}}, {{page}})
    std::string footer_text; ///< Footer template (supports {{page}}, {{pages}})
    bool page_numbers{true};
    bool landscape{false};
    int heading_break_level{1}; ///< Insert page breaks before headings at this level (1=h1, 2=h2)
};

/// Print preview data generated for UI display.
struct PrintPreviewData
{
    int estimated_page_count{0};
    std::string content_html;          ///< Fully rendered HTML with print CSS
    std::string print_css;             ///< Standalone print stylesheet
    int64_t content_length{0};         ///< Character count of source body
    std::vector<std::string> sections; ///< Section titles (one per page break)
};

// ============================================================================
// PrintPreparationService
// ============================================================================

/// PrintPreparationService — converts markdown/HTML content into print-ready
/// HTML with proper page layout, breaks, headers, and footers.
class PrintPreparationService
{
public:
    PrintPreparationService();

    // ----- Core API ---------------------------------------------------------

    /// Convert markdown source to print-ready HTML using the given layout.
    [[nodiscard]] auto prepare_for_print(const std::string& content_html,
                                         const PrintLayout& layout = {}) const -> std::string;

    /// Generate a preview data bundle for the UI.
    [[nodiscard]] auto generate_preview(const std::string& content_html,
                                        const std::string& title,
                                        const PrintLayout& layout = {}) const -> PrintPreviewData;

    // ----- Content transforms -----------------------------------------------

    /// Insert CSS page-break-before at heading boundaries.
    [[nodiscard]] auto inject_page_breaks(const std::string& html, int heading_level = 1) const
        -> std::string;

    /// Wrap content with running header/footer HTML.
    [[nodiscard]] auto add_headers_footers(const std::string& html,
                                           const PrintLayout& layout,
                                           const std::string& title) const -> std::string;

    /// Estimate the number of printed pages from content length.
    [[nodiscard]] static auto estimate_page_count(int64_t content_length,
                                                  PageSize page_size = PageSize::kA4) -> int;

    // ----- CSS generation ---------------------------------------------------

    /// Generate the @page + @media print CSS for a layout.
    [[nodiscard]] static auto generate_print_css(const PrintLayout& layout) -> std::string;

    /// Get the CSS dimensions string for a PageSize.
    [[nodiscard]] static auto page_size_css(PageSize ps) -> std::string;

    // ----- Section extraction -----------------------------------------------

    /// Extract section titles from HTML headings (for the preview outline).
    [[nodiscard]] static auto extract_sections(const std::string& html, int heading_level = 1)
        -> std::vector<std::string>;
};

} // namespace markamp::core
