#pragma once

#include <cstdint>
#include <string>

namespace markamp::canvas
{

/// PDF import mode.
enum class PdfImportMode : uint8_t
{
    kSinglePage,
    kPageRange,
    kAllPages,
};

/// PDF link state.
enum class PdfLinkState : uint8_t
{
    kLinked,
    kBroken,
    kEmbedded,
};

/// Testable model for PDF Object Controls (Phase 55).
///
/// Encapsulates:
/// - Import mode (single/range/all)
/// - Page navigation (current page, total pages)
/// - Render quality settings
/// - Source link state with recovery
class PdfObjectModel
{
public:
    // ── Import ──────────────────────────────────────────────────────

    void set_import_mode(PdfImportMode mode);
    [[nodiscard]] auto import_mode() const -> PdfImportMode;

    void set_page_range(int start_page, int end_page);
    [[nodiscard]] auto range_start() const -> int;
    [[nodiscard]] auto range_end() const -> int;

    // ── Page navigation ─────────────────────────────────────────────

    void set_total_pages(int total);
    [[nodiscard]] auto total_pages() const -> int;

    void set_current_page(int page);
    [[nodiscard]] auto current_page() const -> int;
    void next_page();
    void prev_page();

    // ── Quality ─────────────────────────────────────────────────────

    void set_render_dpi(int dpi);
    [[nodiscard]] auto render_dpi() const -> int;

    // ── Link state ──────────────────────────────────────────────────

    void set_source_path(const std::string& path);
    [[nodiscard]] auto source_path() const -> const std::string&;

    void set_link_state(PdfLinkState state);
    [[nodiscard]] auto link_state() const -> PdfLinkState;

    void relink(const std::string& new_path);

private:
    PdfImportMode import_mode_{PdfImportMode::kSinglePage};
    int range_start_{1};
    int range_end_{1};
    int total_pages_{1};
    int current_page_{1};
    int render_dpi_{150};
    std::string source_path_;
    PdfLinkState link_state_{PdfLinkState::kLinked};

    // ── Round 6 Batch 7 (#63-70) ────────────────────────────────

    /// (#63) Whether import mode is single page.
    [[nodiscard]] auto is_single_page() const noexcept -> bool
    {
        return import_mode_ == PdfImportMode::kSinglePage;
    }

    /// (#64) Whether import mode is all pages.
    [[nodiscard]] auto is_all_pages() const noexcept -> bool
    {
        return import_mode_ == PdfImportMode::kAllPages;
    }

    /// (#65) Whether at the first page.
    [[nodiscard]] auto is_first_page() const noexcept -> bool
    {
        return current_page_ == 1;
    }

    /// (#66) Whether at the last page.
    [[nodiscard]] auto is_last_page() const noexcept -> bool
    {
        return current_page_ == total_pages_;
    }

    /// (#67) Whether a source path is set.
    [[nodiscard]] auto has_source() const noexcept -> bool
    {
        return !source_path_.empty();
    }

    /// (#68) Whether PDF is linked.
    [[nodiscard]] auto is_linked() const noexcept -> bool
    {
        return link_state_ == PdfLinkState::kLinked;
    }

    /// (#69) Whether PDF link is broken.
    [[nodiscard]] auto is_broken() const noexcept -> bool
    {
        return link_state_ == PdfLinkState::kBroken;
    }

    /// (#70) Whether PDF is embedded.
    [[nodiscard]] auto is_embedded() const noexcept -> bool
    {
        return link_state_ == PdfLinkState::kEmbedded;
    }
};

} // namespace markamp::canvas
