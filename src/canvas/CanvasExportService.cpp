// ============================================================================
// File: src/canvas/CanvasExportService.cpp
// Phase 12: Canvas Advanced Objects — board export to SVG/PNG/JSON
// ============================================================================
#include "canvas/CanvasExportService.h"

#include <algorithm>
#include <cmath>
#include <fmt/format.h>
#include <fstream>
#include <sstream>

namespace markamp::canvas
{

CanvasExportService::CanvasExportService(const Board& board)
    : board_(board)
{
}

// ── Full Board Export ─────────────────────────────────────────────

auto CanvasExportService::export_svg(const ExportOptions& options) const -> ExportResult
{
    const auto objects = collect_all_objects();
    if (objects.empty())
    {
        return {false, "", {}, 0, 0, 0, "No objects to export"};
    }

    auto bounds = content_bounds();
    bounds = bounds.expanded(options.margin);

    ExportResult result;
    result.data = generate_svg(objects, bounds, options);
    result.width = bounds.width() * options.scale;
    result.height = bounds.height() * options.scale;
    result.object_count = objects.size();
    result.success = true;
    return result;
}

auto CanvasExportService::export_png(const ExportOptions& options) const -> ExportResult
{
    // PNG export generates the SVG first, then converts
    // In production this would use a rasterizer; for now we return
    // the SVG data and indicate it needs rasterization
    auto svg_result = export_svg(options);
    if (!svg_result.success)
    {
        return svg_result;
    }

    // Compute pixel dimensions
    const double px_width = svg_result.width * (static_cast<double>(options.png_dpi) / 96.0);
    const double px_height = svg_result.height * (static_cast<double>(options.png_dpi) / 96.0);

    ExportResult result;
    result.success = true;
    result.data = svg_result.data; // SVG source for rasterization
    result.width = px_width;
    result.height = px_height;
    result.object_count = svg_result.object_count;
    return result;
}

auto CanvasExportService::export_json(const ExportOptions& /*options*/) const -> ExportResult
{
    const auto objects = collect_all_objects();
    if (objects.empty())
    {
        return {false, "", {}, 0, 0, 0, "No objects to export"};
    }

    ExportResult result;
    result.data = generate_json(objects);
    result.object_count = objects.size();
    result.success = true;

    const auto bounds = content_bounds();
    result.width = bounds.width();
    result.height = bounds.height();
    return result;
}

// ── Selection Export ──────────────────────────────────────────────

auto CanvasExportService::export_selection_svg(const std::vector<ObjectId>& ids,
                                               const ExportOptions& options) const -> ExportResult
{
    const auto objects = collect_objects(ids);
    if (objects.empty())
    {
        return {false, "", {}, 0, 0, 0, "No objects found for given IDs"};
    }

    auto bounds = selection_bounds(ids);
    bounds = bounds.expanded(options.margin);

    ExportResult result;
    result.data = generate_svg(objects, bounds, options);
    result.width = bounds.width() * options.scale;
    result.height = bounds.height() * options.scale;
    result.object_count = objects.size();
    result.success = true;
    return result;
}

auto CanvasExportService::export_selection_json(const std::vector<ObjectId>& ids,
                                                const ExportOptions& /*options*/) const
    -> ExportResult
{
    const auto objects = collect_objects(ids);
    if (objects.empty())
    {
        return {false, "", {}, 0, 0, 0, "No objects found for given IDs"};
    }

    ExportResult result;
    result.data = generate_json(objects);
    result.object_count = objects.size();
    result.success = true;

    const auto bounds = selection_bounds(ids);
    result.width = bounds.width();
    result.height = bounds.height();
    return result;
}

// ── Export to File ────────────────────────────────────────────────

auto CanvasExportService::export_to_file(const std::string& file_path,
                                         ExportFormat format,
                                         const ExportOptions& options) const -> ExportResult
{
    ExportResult result;

    switch (format)
    {
        case ExportFormat::kSvg:
            result = export_svg(options);
            break;
        case ExportFormat::kPng:
            result = export_png(options);
            break;
        case ExportFormat::kJson:
            result = export_json(options);
            break;
        case ExportFormat::kPdf:
            return {false, "", {}, 0, 0, 0, "PDF export not yet implemented"};
    }

    if (result.success && !result.data.empty())
    {
        std::ofstream out_file(file_path);
        if (out_file.is_open())
        {
            out_file << result.data;
            result.data = file_path; // Replace data with file path
        }
        else
        {
            result.success = false;
            result.error_message = "Failed to write file: " + file_path;
        }
    }

    return result;
}

// ── Utility ───────────────────────────────────────────────────────

auto CanvasExportService::content_bounds() const -> AABB
{
    AABB result;

    for (const auto& obj_ptr : board_.objects())
    {
        const auto& obj = *obj_ptr;
        const auto obj_bounds = obj.world_bounds();
        result = result.merged(obj_bounds);
    }

    return result;
}

auto CanvasExportService::selection_bounds(const std::vector<ObjectId>& ids) const -> AABB
{
    AABB result;

    for (const auto obj_id : ids)
    {
        const auto* obj = board_.get_object(obj_id);
        if (obj == nullptr)
        {
            continue;
        }
        const auto obj_bounds = obj->world_bounds();
        result = result.merged(obj_bounds);
    }

    return result;
}

auto CanvasExportService::supported_formats() -> std::vector<ExportFormat>
{
    return {ExportFormat::kSvg, ExportFormat::kPng, ExportFormat::kJson, ExportFormat::kPdf};
}

auto CanvasExportService::format_extension(ExportFormat format) -> std::string
{
    switch (format)
    {
        case ExportFormat::kSvg:
            return ".svg";
        case ExportFormat::kPng:
            return ".png";
        case ExportFormat::kJson:
            return ".json";
        case ExportFormat::kPdf:
            return ".pdf";
    }
    return ".bin";
}

auto CanvasExportService::format_name(ExportFormat format) -> std::string
{
    switch (format)
    {
        case ExportFormat::kSvg:
            return "SVG";
        case ExportFormat::kPng:
            return "PNG";
        case ExportFormat::kJson:
            return "JSON";
        case ExportFormat::kPdf:
            return "PDF";
    }
    return "Unknown";
}

// ── Private Helpers ───────────────────────────────────────────────

auto CanvasExportService::generate_svg(const std::vector<const CanvasObject*>& objects,
                                       const AABB& bounds,
                                       const ExportOptions& options) const -> std::string
{
    const double svg_width = bounds.width() * options.scale;
    const double svg_height = bounds.height() * options.scale;

    std::ostringstream svg;
    svg << fmt::format(R"(<?xml version="1.0" encoding="UTF-8"?>)"
                       R"(<svg xmlns="http://www.w3.org/2000/svg" )"
                       R"(width="{:.0f}" height="{:.0f}" )"
                       R"(viewBox="{:.1f} {:.1f} {:.1f} {:.1f}">)",
                       svg_width,
                       svg_height,
                       bounds.min_x,
                       bounds.min_y,
                       bounds.width(),
                       bounds.height());

    // Background
    if (options.include_background && !options.transparent_background)
    {
        svg << fmt::format(R"svg(<rect x="{:.1f}" y="{:.1f}" width="{:.1f}" height="{:.1f}" )svg"
                           R"svg(fill="rgb({},{},{})" />)svg",
                           bounds.min_x,
                           bounds.min_y,
                           bounds.width(),
                           bounds.height(),
                           options.background_color.r,
                           options.background_color.g,
                           options.background_color.b);
    }

    // Objects
    const Point2D offset{0.0, 0.0};
    for (const auto* obj : objects)
    {
        svg << object_to_svg(*obj, offset, options.scale);
    }

    svg << "</svg>";
    return svg.str();
}

auto CanvasExportService::generate_json(const std::vector<const CanvasObject*>& objects) const
    -> std::string
{
    std::ostringstream json;
    json << R"({"objects":[)";
    bool first = true;
    for (const auto* obj : objects)
    {
        if (!first)
        {
            json << ",";
        }
        json << obj->to_json();
        first = false;
    }
    json << "]}";
    return json.str();
}

auto CanvasExportService::collect_all_objects() const -> std::vector<const CanvasObject*>
{
    std::vector<const CanvasObject*> result;
    for (const auto& obj_ptr : board_.objects())
    {
        const auto& obj = *obj_ptr;
        if (obj.is_visible())
        {
            result.push_back(&obj);
        }
    }
    return result;
}

auto CanvasExportService::collect_objects(const std::vector<ObjectId>& ids) const
    -> std::vector<const CanvasObject*>
{
    std::vector<const CanvasObject*> result;
    for (const auto obj_id : ids)
    {
        const auto* obj = board_.get_object(obj_id);
        if (obj != nullptr)
        {
            result.push_back(obj);
        }
    }
    return result;
}

auto CanvasExportService::object_to_svg(const CanvasObject& obj,
                                        const Point2D& /*offset*/,
                                        double /*scale*/) -> std::string
{
    const auto obj_bounds = obj.world_bounds();
    const auto color = obj.custom_color();

    return fmt::format(R"svg(<rect x="{:.1f}" y="{:.1f}" width="{:.1f}" height="{:.1f}" )svg"
                       R"svg(fill="rgba({},{},{},{:.2f})" opacity="{:.2f}" />)svg",
                       obj_bounds.min_x,
                       obj_bounds.min_y,
                       obj_bounds.width(),
                       obj_bounds.height(),
                       color.r,
                       color.g,
                       color.b,
                       static_cast<double>(color.a) / 255.0,
                       obj.opacity());
}

} // namespace markamp::canvas
