// ============================================================================
// File: src/canvas/CanvasExportService.h
// Phase 12: Canvas Advanced Objects — board export to SVG/PNG/JSON
// ============================================================================
#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Export format options.
enum class ExportFormat : uint8_t
{
    kSvg,
    kPng,
    kJson,
    kPdf
};

/// Configuration for export operations.
struct ExportOptions
{
    double scale{1.0};                                ///< Scale factor (1.0 = 100%)
    bool include_background{true};                    ///< Include board background
    bool include_grid{false};                         ///< Include grid lines
    double margin{20.0};                              ///< Margin around exported content
    CanvasColor background_color{255, 255, 255, 255}; ///< Background color
    int png_dpi{96};                                  ///< DPI for PNG export
    bool transparent_background{false};               ///< Transparent background for PNG
    bool fit_to_content{true};                        ///< Auto-size to content bounds
    double min_width{0.0};                            ///< Minimum export width
    double min_height{0.0};                           ///< Minimum export height
};

/// Result of an export operation.
struct ExportResult
{
    bool success{false};
    std::string data;                 ///< Exported data (SVG/JSON string or file path)
    std::vector<uint8_t> binary_data; ///< Binary data for PNG export
    double width{0.0};                ///< Exported width
    double height{0.0};               ///< Exported height
    size_t object_count{0};           ///< Number of objects exported
    std::string error_message;
};

/// Canvas export service for exporting boards and selections.
///
/// Provides:
///   - SVG export with full visual fidelity
///   - PNG raster export with configurable DPI
///   - JSON export for data interchange
///   - Selection-only export
///   - Configurable scale, margins, and background
class CanvasExportService
{
public:
    explicit CanvasExportService(const Board& board);

    // ── Full Board Export ─────────────────────────────────────────

    /// Export the entire board as SVG.
    [[nodiscard]] auto export_svg(const ExportOptions& options = {}) const -> ExportResult;

    /// Export the entire board as PNG.
    [[nodiscard]] auto export_png(const ExportOptions& options = {}) const -> ExportResult;

    /// Export the entire board as JSON.
    [[nodiscard]] auto export_json(const ExportOptions& options = {}) const -> ExportResult;

    // ── Selection Export ──────────────────────────────────────────

    /// Export only the specified objects as SVG.
    [[nodiscard]] auto export_selection_svg(const std::vector<ObjectId>& ids,
                                            const ExportOptions& options = {}) const
        -> ExportResult;

    /// Export only the specified objects as JSON.
    [[nodiscard]] auto export_selection_json(const std::vector<ObjectId>& ids,
                                             const ExportOptions& options = {}) const
        -> ExportResult;

    // ── Export to File ────────────────────────────────────────────

    /// Export the board to a file at the given path.
    [[nodiscard]] auto export_to_file(const std::string& file_path,
                                      ExportFormat format,
                                      const ExportOptions& options = {}) const -> ExportResult;

    // ── Utility ───────────────────────────────────────────────────

    /// Compute the bounding box of all objects on the board.
    [[nodiscard]] auto content_bounds() const -> AABB;

    /// Compute the bounding box of specific objects.
    [[nodiscard]] auto selection_bounds(const std::vector<ObjectId>& ids) const -> AABB;

    /// Get the supported export formats.
    [[nodiscard]] static auto supported_formats() -> std::vector<ExportFormat>;

    /// Get a file extension for a format.
    [[nodiscard]] static auto format_extension(ExportFormat format) -> std::string;

    /// Get a human-readable name for a format.
    [[nodiscard]] static auto format_name(ExportFormat format) -> std::string;

private:
    const Board& board_;

    /// Generate SVG markup for a set of objects.
    [[nodiscard]] auto generate_svg(const std::vector<const CanvasObject*>& objects,
                                    const AABB& bounds,
                                    const ExportOptions& options) const -> std::string;

    /// Generate JSON for a set of objects.
    [[nodiscard]] auto generate_json(const std::vector<const CanvasObject*>& objects) const
        -> std::string;

    /// Collect all visible objects from the board.
    [[nodiscard]] auto collect_all_objects() const -> std::vector<const CanvasObject*>;

    /// Collect specific objects by ID.
    [[nodiscard]] auto collect_objects(const std::vector<ObjectId>& ids) const
        -> std::vector<const CanvasObject*>;

    /// Generate SVG element for a single object.
    [[nodiscard]] static auto
    object_to_svg(const CanvasObject& obj, const Point2D& offset, double scale) -> std::string;
};

} // namespace markamp::canvas
