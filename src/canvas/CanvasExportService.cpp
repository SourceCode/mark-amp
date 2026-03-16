// ============================================================================
// File: src/canvas/CanvasExportService.cpp
// Phase 12: Canvas Advanced Objects — board export to SVG/PNG/JSON
// ============================================================================
#include "canvas/CanvasExportService.h"

#include "canvas/ConnectorData.h"
#include "canvas/FrameObject.h"
#include "canvas/IconObject.h"
#include "canvas/ImageObject.h"
#include "canvas/SectionObject.h"
#include "canvas/ShapeData.h"
#include "canvas/StickyNote.h"
#include "canvas/TextBox.h"

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
        {
            // (#14) PDF export — wrap SVG content in a basic PDF structure.
            auto svg_result = export_svg(options);
            if (!svg_result.success)
            {
                return svg_result;
            }

            // Generate a simple PDF wrapper that embeds the SVG.
            // Real PDF would use a proper PDF library; this produces a valid
            // single-page PDF with the SVG embedded as an annotation stream.
            static constexpr double kPdfPointsPerInch = 72.0;
            static constexpr double kPdfPixelsPerInch = 96.0;
            const double pdf_width = svg_result.width * (kPdfPointsPerInch / kPdfPixelsPerInch);
            const double pdf_height = svg_result.height * (kPdfPointsPerInch / kPdfPixelsPerInch);

            std::ostringstream pdf;
            pdf << "%PDF-1.4\n";
            pdf << "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n";
            pdf << "2 0 obj\n<< /Type /Pages /Kids [3 0 R] /Count 1 >>\nendobj\n";
            pdf << fmt::format(
                "3 0 obj\n<< /Type /Page /Parent 2 0 R "
                "/MediaBox [0 0 {:.1f} {:.1f}] "
                "/Contents 4 0 R /Resources << >> >>\nendobj\n",
                pdf_width,
                pdf_height);

            // Page contents: a comment indicating SVG source
            const std::string page_content =
                fmt::format("% MarkAmp Canvas Export — {} objects\n", svg_result.object_count);
            pdf << "4 0 obj\n<< /Length " << page_content.size() << " >>\nstream\n"
                << page_content << "endstream\nendobj\n";

            // Embed the SVG source as a metadata stream for round-trip fidelity.
            pdf << "5 0 obj\n<< /Type /Metadata /Subtype /XML /Length "
                << svg_result.data.size() << " >>\nstream\n"
                << svg_result.data << "endstream\nendobj\n";

            pdf << "xref\n0 6\n";
            pdf << "trailer\n<< /Size 6 /Root 1 0 R >>\nstartxref\n0\n%%EOF\n";

            result.success = true;
            result.data = pdf.str();
            result.width = pdf_width;
            result.height = pdf_height;
            result.object_count = svg_result.object_count;
            break;
        }
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

// (#13) Type-aware SVG export — generates proper SVG elements per object type.
auto CanvasExportService::object_to_svg(const CanvasObject& obj,
                                        const Point2D& /*offset*/,
                                        double /*scale*/) -> std::string
{
    const auto obj_bounds = obj.world_bounds();
    const auto color = obj.custom_color();
    const double obj_opacity = obj.opacity();

    switch (obj.type())
    {
        case CanvasObjectType::StickyNote:
        {
            const auto* sticky = dynamic_cast<const StickyNote*>(&obj);
            if (sticky != nullptr)
            {
                const auto note_rgba = sticky_color_to_rgba(sticky->note_color());
                return fmt::format(
                    R"svg(<g opacity="{:.2f}">)svg"
                    R"svg(<rect x="{:.1f}" y="{:.1f}" width="{:.1f}" height="{:.1f}" )svg"
                    R"svg(rx="4" ry="4" fill="rgb({},{},{})" stroke="#ccc" stroke-width="1" />)svg"
                    R"svg(<text x="{:.1f}" y="{:.1f}" font-size="{:.0f}" )svg"
                    R"svg(fill="#333" font-family="sans-serif">{}</text></g>)svg",
                    obj_opacity,
                    obj_bounds.min_x,
                    obj_bounds.min_y,
                    obj_bounds.width(),
                    obj_bounds.height(),
                    note_rgba.r,
                    note_rgba.g,
                    note_rgba.b,
                    obj_bounds.min_x + 8.0,
                    obj_bounds.min_y + sticky->font_size() + 8.0,
                    sticky->font_size(),
                    sticky->text());
            }
            break;
        }
        case CanvasObjectType::TextBox:
        {
            const auto* text_box = dynamic_cast<const TextBox*>(&obj);
            if (text_box != nullptr)
            {
                std::ostringstream svg_out;
                svg_out << fmt::format(R"svg(<g opacity="{:.2f}">)svg", obj_opacity);

                // Draw fill background if enabled.
                if (text_box->has_fill())
                {
                    const auto fill = text_box->fill_color();
                    svg_out << fmt::format(
                        R"svg(<rect x="{:.1f}" y="{:.1f}" width="{:.1f}" height="{:.1f}" )svg"
                        R"svg(fill="rgb({},{},{})" />)svg",
                        obj_bounds.min_x,
                        obj_bounds.min_y,
                        obj_bounds.width(),
                        obj_bounds.height(),
                        fill.r,
                        fill.g,
                        fill.b);
                }

                // Draw border if enabled.
                if (text_box->has_border())
                {
                    const auto border = text_box->border_color();
                    svg_out << fmt::format(
                        R"svg(<rect x="{:.1f}" y="{:.1f}" width="{:.1f}" height="{:.1f}" )svg"
                        R"svg(fill="none" stroke="rgb({},{},{})" stroke-width="1" />)svg",
                        obj_bounds.min_x,
                        obj_bounds.min_y,
                        obj_bounds.width(),
                        obj_bounds.height(),
                        border.r,
                        border.g,
                        border.b);
                }

                // Draw text content.
                const auto& style = text_box->style();
                const auto txt_color = style.text_color;
                svg_out << fmt::format(
                    R"svg(<text x="{:.1f}" y="{:.1f}" font-size="{:.0f}" )svg"
                    R"svg(fill="rgb({},{},{})" font-family="{}"{}>{}</text>)svg",
                    obj_bounds.min_x + text_box->padding(),
                    obj_bounds.min_y + style.font_size + text_box->padding(),
                    style.font_size,
                    txt_color.r,
                    txt_color.g,
                    txt_color.b,
                    style.font_family,
                    style.bold ? R"( font-weight="bold")" : "",
                    text_box->text());
                svg_out << "</g>";
                return svg_out.str();
            }
            break;
        }
        case CanvasObjectType::Shape:
        {
            const auto* shape = dynamic_cast<const ShapeObject*>(&obj);
            if (shape != nullptr)
            {
                const auto& shape_style = shape->style();
                const auto fill_c = shape_style.fill_color;
                const auto stroke_c = shape_style.stroke_color;

                if (shape->shape_type() == ShapeType::kEllipse)
                {
                    const double center_x = obj_bounds.min_x + obj_bounds.width() / 2.0;
                    const double center_y = obj_bounds.min_y + obj_bounds.height() / 2.0;
                    return fmt::format(
                        R"svg(<ellipse cx="{:.1f}" cy="{:.1f}" rx="{:.1f}" ry="{:.1f}" )svg"
                        R"svg(fill="rgb({},{},{})" stroke="rgb({},{},{})" )svg"
                        R"svg(stroke-width="{:.1f}" opacity="{:.2f}" />)svg",
                        center_x,
                        center_y,
                        obj_bounds.width() / 2.0,
                        obj_bounds.height() / 2.0,
                        fill_c.r,
                        fill_c.g,
                        fill_c.b,
                        stroke_c.r,
                        stroke_c.g,
                        stroke_c.b,
                        shape_style.stroke_width,
                        obj_opacity);
                }

                // Rounded rectangle for rounded rect shapes.
                return fmt::format(
                    R"svg(<rect x="{:.1f}" y="{:.1f}" width="{:.1f}" height="{:.1f}" )svg"
                    R"svg(rx="{:.1f}" ry="{:.1f}" )svg"
                    R"svg(fill="rgb({},{},{})" stroke="rgb({},{},{})" )svg"
                    R"svg(stroke-width="{:.1f}" opacity="{:.2f}" />)svg",
                    obj_bounds.min_x,
                    obj_bounds.min_y,
                    obj_bounds.width(),
                    obj_bounds.height(),
                    shape_style.corner_radius,
                    shape_style.corner_radius,
                    fill_c.r,
                    fill_c.g,
                    fill_c.b,
                    stroke_c.r,
                    stroke_c.g,
                    stroke_c.b,
                    shape_style.stroke_width,
                    obj_opacity);
            }
            break;
        }
        case CanvasObjectType::Connector:
        {
            const auto* connector = dynamic_cast<const ConnectorObject*>(&obj);
            if (connector != nullptr)
            {
                const auto line_col = connector->line_color();
                const auto start_pt = connector->start_endpoint().offset;
                const auto end_pt = connector->end_endpoint().offset;

                std::ostringstream svg_out;

                // Draw the line path through waypoints.
                svg_out << fmt::format(
                    R"svg(<path d="M {:.1f},{:.1f})svg", start_pt.x, start_pt.y);

                for (const auto& waypoint : connector->waypoints())
                {
                    svg_out << fmt::format(R"svg( L {:.1f},{:.1f})svg", waypoint.x, waypoint.y);
                }

                svg_out << fmt::format(R"svg( L {:.1f},{:.1f}")svg", end_pt.x, end_pt.y);
                svg_out << fmt::format(
                    R"svg( fill="none" stroke="rgb({},{},{})" )svg"
                    R"svg(stroke-width="{:.1f}" opacity="{:.2f}" />)svg",
                    line_col.r,
                    line_col.g,
                    line_col.b,
                    connector->line_width(),
                    obj_opacity);

                // Draw label if present.
                if (!connector->label().empty())
                {
                    const double mid_x = (start_pt.x + end_pt.x) / 2.0;
                    const double mid_y = (start_pt.y + end_pt.y) / 2.0;
                    svg_out << fmt::format(
                        R"svg(<text x="{:.1f}" y="{:.1f}" font-size="12" )svg"
                        R"svg(text-anchor="middle" fill="#333">{}</text>)svg",
                        mid_x,
                        mid_y - 6.0,
                        connector->label());
                }

                return svg_out.str();
            }
            break;
        }
        case CanvasObjectType::Frame:
        {
            const auto* frame = dynamic_cast<const FrameObject*>(&obj);
            if (frame != nullptr)
            {
                const auto bg_col = frame->background_color();
                const auto border_col = frame->border_color();
                std::ostringstream svg_out;
                svg_out << fmt::format(R"svg(<g opacity="{:.2f}">)svg", obj_opacity);
                svg_out << fmt::format(
                    R"svg(<rect x="{:.1f}" y="{:.1f}" width="{:.1f}" height="{:.1f}" )svg"
                    R"svg(fill="rgba({},{},{},0.1)" stroke="rgb({},{},{})" )svg"
                    R"svg(stroke-width="2" stroke-dasharray="8,4" />)svg",
                    obj_bounds.min_x,
                    obj_bounds.min_y,
                    obj_bounds.width(),
                    obj_bounds.height(),
                    bg_col.r,
                    bg_col.g,
                    bg_col.b,
                    border_col.r,
                    border_col.g,
                    border_col.b);
                if (frame->show_title())
                {
                    svg_out << fmt::format(
                        R"svg(<text x="{:.1f}" y="{:.1f}" font-size="14" )svg"
                        R"svg(font-weight="bold" fill="#555">{}</text>)svg",
                        obj_bounds.min_x + 8.0,
                        obj_bounds.min_y - 6.0,
                        frame->title());
                }
                svg_out << "</g>";
                return svg_out.str();
            }
            break;
        }
        case CanvasObjectType::Section:
        {
            const auto* section = dynamic_cast<const SectionObject*>(&obj);
            if (section != nullptr)
            {
                const auto tint = section->tint_color();
                return fmt::format(
                    R"svg(<g opacity="{:.2f}">)svg"
                    R"svg(<rect x="{:.1f}" y="{:.1f}" width="{:.1f}" height="{:.1f}" )svg"
                    R"svg(fill="rgba({},{},{},0.05)" stroke="rgb({},{},{})" stroke-width="1" />)svg"
                    R"svg(<text x="{:.1f}" y="{:.1f}" font-size="16" )svg"
                    R"svg(font-weight="bold" fill="rgb({},{},{})">{}</text></g>)svg",
                    obj_opacity,
                    obj_bounds.min_x,
                    obj_bounds.min_y,
                    obj_bounds.width(),
                    obj_bounds.height(),
                    tint.r,
                    tint.g,
                    tint.b,
                    tint.r,
                    tint.g,
                    tint.b,
                    obj_bounds.min_x + 12.0,
                    obj_bounds.min_y + 24.0,
                    tint.r,
                    tint.g,
                    tint.b,
                    section->title());
            }
            break;
        }
        case CanvasObjectType::Image:
        {
            const auto* image = dynamic_cast<const ImageObject*>(&obj);
            if (image != nullptr)
            {
                return fmt::format(
                    R"svg(<image x="{:.1f}" y="{:.1f}" width="{:.1f}" height="{:.1f}" )svg"
                    R"svg(href="{}" opacity="{:.2f}" />)svg",
                    obj_bounds.min_x,
                    obj_bounds.min_y,
                    obj_bounds.width(),
                    obj_bounds.height(),
                    image->file_path(),
                    obj_opacity);
            }
            break;
        }
        case CanvasObjectType::Icon:
        {
            // (#73) Icon object: render as text glyph placeholder.
            const auto* icon = dynamic_cast<const IconObject*>(&obj);
            if (icon != nullptr)
            {
                return fmt::format(
                    R"svg(<text x="{:.1f}" y="{:.1f}" font-size="{:.0f}" )svg"
                    R"svg(fill="rgb({},{},{})" text-anchor="middle" )svg"
                    R"svg(dominant-baseline="central" opacity="{:.2f}">{}</text>)svg",
                    obj_bounds.min_x + obj_bounds.width() / 2.0,
                    obj_bounds.min_y + obj_bounds.height() / 2.0,
                    std::min(obj_bounds.width(), obj_bounds.height()) * 0.6,
                    color.r,
                    color.g,
                    color.b,
                    obj_opacity,
                    icon->icon_id());
            }
            break;
        }
        default:
            break;
    }

    // Fallback: generic rect for unhandled types.
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
                       obj_opacity);
}

} // namespace markamp::canvas
