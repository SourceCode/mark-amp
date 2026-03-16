#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Export format.
enum class ExportFormat : uint8_t
{
    kPng,
    kSvg,
    kPdf,
};

/// Export scope.
enum class ExportScope : uint8_t
{
    kFullBoard,
    kFrame,
    kSelection,
};

/// Export settings.
struct ExportSettings
{
    ExportFormat format{ExportFormat::kPng};
    ExportScope scope{ExportScope::kFullBoard};
    int dpi{150};
    double scale{1.0};
    bool transparent_bg{false};

    // ── Round 5 Batch 10 (#97-100) ───────────────────────────────

    /// (#97) Whether format is PNG.
    [[nodiscard]] auto is_png() const noexcept -> bool
    {
        return format == ExportFormat::kPng;
    }

    /// (#98) Whether format is SVG.
    [[nodiscard]] auto is_svg() const noexcept -> bool
    {
        return format == ExportFormat::kSvg;
    }

    /// (#99) Whether scope is full board.
    [[nodiscard]] auto is_full_board() const noexcept -> bool
    {
        return scope == ExportScope::kFullBoard;
    }

    /// (#100) Whether scope is selection.
    [[nodiscard]] auto is_selection() const noexcept -> bool
    {
        return scope == ExportScope::kSelection;
    }
};

/// Page tile for print pagination.
struct PageTile
{
    int page_index{0};
    double tile_x{0.0};
    double tile_y{0.0};
    double tile_w{0.0};
    double tile_h{0.0};
};

/// Testable model for Canvas Export/Print/Snapshot (Phase 71).
///
/// Encapsulates:
/// - Export format and scope selection
/// - DPI/scale/transparency controls
/// - Print pagination with page tiling
/// - Quick snapshot state
class ExportModel
{
public:
    // ── Settings ────────────────────────────────────────────────────

    void set_settings(ExportSettings settings);
    [[nodiscard]] auto settings() const -> const ExportSettings&;

    // ── DPI validation ──────────────────────────────────────────────

    void set_dpi(int dpi);
    void set_scale(double scale);

    // ── Pagination ──────────────────────────────────────────────────

    void set_page_tiles(std::vector<PageTile> tiles);
    [[nodiscard]] auto page_tiles() const -> const std::vector<PageTile>&;
    [[nodiscard]] auto page_count() const -> int;

    // ── Snapshot ────────────────────────────────────────────────────

    void trigger_snapshot();
    [[nodiscard]] auto snapshot_count() const -> int;

private:
    ExportSettings settings_;
    std::vector<PageTile> page_tiles_;
    int snapshot_count_{0};
};

} // namespace markamp::canvas
