// ============================================================================
// File: src/canvas/CanvasObjectFactory.cpp
// Phase 11: Canvas Workbench Shell — object factory implementation
// ============================================================================
#include "CanvasObjectFactory.h"

#include "canvas/CommentObject.h"
#include "canvas/DiagramShapeObject.h"
#include "canvas/FrameObject.h"
#include "canvas/IconObject.h"
#include "canvas/ImageObject.h"
#include "canvas/SectionObject.h"
#include "canvas/StickyNote.h"
#include "canvas/TableObject.h"
#include "canvas/TextBox.h"

#include <cmath>

namespace markamp::canvas
{

auto CanvasObjectFactory::create(CanvasObjectType type) const -> ObjectCreationResult
{
    ObjectCreationParams params;
    params.size = default_size(type);
    return create(type, params);
}

auto CanvasObjectFactory::create(CanvasObjectType type, const ObjectCreationParams& params) const
    -> ObjectCreationResult
{
    ObjectCreationResult result;

    switch (type)
    {
        case CanvasObjectType::StickyNote:
            result.object = create_sticky_note(params);
            break;
        case CanvasObjectType::TextBox:
            result.object = create_text_box(params);
            break;
        case CanvasObjectType::Shape:
        case CanvasObjectType::DiagramShape:
            result.object = create_diagram_shape(params);
            break;
        case CanvasObjectType::Frame:
            result.object = create_frame(params);
            break;
        case CanvasObjectType::Section:
            result.object = create_section(params);
            break;
        case CanvasObjectType::Image:
            result.object = create_image(params);
            break;
        case CanvasObjectType::Comment:
            result.object = create_comment(params);
            break;
        case CanvasObjectType::Icon:
            result.object = create_icon(params);
            break;
        case CanvasObjectType::Table:
            result.object = create_table(params);
            break;
        default:
            result.error_message = "Unsupported object type for factory creation";
            return result;
    }

    if (result.object != nullptr)
    {
        // Apply common properties
        Transform2D xform;
        xform.tx = params.position.x;
        xform.ty = params.position.y;
        result.object->set_transform(xform);
        result.object->set_custom_color(params.fill_color);
        if (!params.name.empty())
        {
            result.object->set_name(params.name);
        }
        result.object->set_layer(params.layer_index);
        result.success = true;
    }
    else
    {
        result.error_message = "Failed to create object";
    }

    return result;
}

auto CanvasObjectFactory::default_size(CanvasObjectType type) -> Size2D
{
    switch (type)
    {
        case CanvasObjectType::StickyNote:
            return {200.0, 200.0};
        case CanvasObjectType::TextBox:
            return {250.0, 100.0};
        case CanvasObjectType::Shape:
        case CanvasObjectType::DiagramShape:
            return {120.0, 80.0};
        case CanvasObjectType::Frame:
            return {600.0, 400.0};
        case CanvasObjectType::Section:
            return {400.0, 300.0};
        case CanvasObjectType::Image:
            return {300.0, 200.0};
        case CanvasObjectType::Comment:
            return {250.0, 150.0};
        case CanvasObjectType::Icon:
            return {48.0, 48.0};
        case CanvasObjectType::Table:
            return {400.0, 250.0};
        default:
            return {200.0, 150.0};
    }
}

auto CanvasObjectFactory::type_display_name(CanvasObjectType type) -> std::string
{
    switch (type)
    {
        case CanvasObjectType::StickyNote:
            return "Sticky Note";
        case CanvasObjectType::TextBox:
            return "Text Box";
        case CanvasObjectType::Shape:
            return "Shape";
        case CanvasObjectType::Connector:
            return "Connector";
        case CanvasObjectType::FreehandPath:
            return "Freehand Path";
        case CanvasObjectType::Image:
            return "Image";
        case CanvasObjectType::Frame:
            return "Frame";
        case CanvasObjectType::Section:
            return "Section";
        case CanvasObjectType::Group:
            return "Group";
        case CanvasObjectType::Table:
            return "Table";
        case CanvasObjectType::MindMapNode:
            return "Mind Map Node";
        case CanvasObjectType::KanbanColumn:
            return "Kanban Column";
        case CanvasObjectType::KanbanCard:
            return "Kanban Card";
        case CanvasObjectType::BookmarkCard:
            return "Bookmark Card";
        case CanvasObjectType::VideoEmbed:
            return "Video Embed";
        case CanvasObjectType::PDFPage:
            return "PDF Page";
        case CanvasObjectType::Icon:
            return "Icon";
        case CanvasObjectType::Comment:
            return "Comment";
        case CanvasObjectType::DiagramShape:
            return "Diagram Shape";
        case CanvasObjectType::CrossBoardLink:
            return "Cross Board Link";
        case CanvasObjectType::AppWidget:
            return "App Widget";
    }
    return "Unknown";
}

auto CanvasObjectFactory::type_icon_name(CanvasObjectType type) -> std::string
{
    switch (type)
    {
        case CanvasObjectType::StickyNote:
            return "sticky-note";
        case CanvasObjectType::TextBox:
            return "text-box";
        case CanvasObjectType::Shape:
            return "shapes";
        case CanvasObjectType::Connector:
            return "git-merge";
        case CanvasObjectType::FreehandPath:
            return "pen-tool";
        case CanvasObjectType::Image:
            return "image";
        case CanvasObjectType::Frame:
            return "frame";
        case CanvasObjectType::Section:
            return "layout";
        case CanvasObjectType::Group:
            return "group";
        case CanvasObjectType::Table:
            return "table";
        case CanvasObjectType::Comment:
            return "message-square";
        case CanvasObjectType::DiagramShape:
            return "diamond";
        case CanvasObjectType::Icon:
            return "star";
        default:
            return "square";
    }
}

auto CanvasObjectFactory::creatable_types() -> std::vector<CanvasObjectType>
{
    return {
        CanvasObjectType::StickyNote,
        CanvasObjectType::TextBox,
        CanvasObjectType::Shape,
        CanvasObjectType::DiagramShape,
        CanvasObjectType::Frame,
        CanvasObjectType::Section,
        CanvasObjectType::Image,
        CanvasObjectType::Table,
        CanvasObjectType::Comment,
        CanvasObjectType::Icon,
    };
}

auto CanvasObjectFactory::duplicate(const CanvasObject& source, Point2D offset)
    -> std::unique_ptr<CanvasObject>
{
    auto clone = source.clone();
    if (clone != nullptr)
    {
        auto xform = clone->transform();
        xform.tx += offset.x;
        xform.ty += offset.y;
        clone->set_transform(xform);
    }
    return clone;
}

auto CanvasObjectFactory::create_grid(CanvasObjectType type,
                                      size_t count,
                                      Point2D origin,
                                      double spacing) const
    -> std::vector<std::unique_ptr<CanvasObject>>
{
    std::vector<std::unique_ptr<CanvasObject>> results;
    results.reserve(count);

    const auto sz = default_size(type);
    const auto cols =
        static_cast<size_t>(std::max(1.0, std::ceil(std::sqrt(static_cast<double>(count)))));

    for (size_t idx = 0; idx < count; ++idx)
    {
        const auto col = idx % cols;
        const auto row = idx / cols;

        ObjectCreationParams params;
        params.position.x = origin.x + static_cast<double>(col) * (sz.width + spacing);
        params.position.y = origin.y + static_cast<double>(row) * (sz.height + spacing);
        params.size = sz;

        auto result = create(type, params);
        if (result.success)
        {
            results.push_back(std::move(result.object));
        }
    }

    return results;
}

// ── Private Factory Methods ────────────────────────────────────────

auto CanvasObjectFactory::create_sticky_note(const ObjectCreationParams& params) const
    -> std::unique_ptr<CanvasObject>
{
    auto obj = std::make_unique<StickyNote>();
    obj->resize(params.size.width, params.size.height);
    if (!params.text_content.empty())
    {
        obj->set_text(params.text_content);
    }
    return obj;
}

auto CanvasObjectFactory::create_text_box(const ObjectCreationParams& params) const
    -> std::unique_ptr<CanvasObject>
{
    auto obj = std::make_unique<TextBox>();
    obj->resize(params.size.width, params.size.height);
    if (!params.text_content.empty())
    {
        obj->set_text(params.text_content);
    }
    return obj;
}

auto CanvasObjectFactory::create_shape(const ObjectCreationParams& params) const
    -> std::unique_ptr<CanvasObject>
{
    auto obj = std::make_unique<DiagramShapeObject>();
    obj->set_dimensions(params.size.width, params.size.height);
    return obj;
}

auto CanvasObjectFactory::create_frame(const ObjectCreationParams& params) const
    -> std::unique_ptr<CanvasObject>
{
    auto obj = std::make_unique<FrameObject>();
    obj->resize(params.size.width, params.size.height);
    return obj;
}

auto CanvasObjectFactory::create_section(const ObjectCreationParams& params) const
    -> std::unique_ptr<CanvasObject>
{
    auto obj = std::make_unique<SectionObject>();
    obj->resize(params.size.width, params.size.height);
    return obj;
}

auto CanvasObjectFactory::create_image(const ObjectCreationParams& params) const
    -> std::unique_ptr<CanvasObject>
{
    auto obj = std::make_unique<ImageObject>();
    obj->resize(params.size.width, params.size.height);
    return obj;
}

auto CanvasObjectFactory::create_diagram_shape(const ObjectCreationParams& params) const
    -> std::unique_ptr<CanvasObject>
{
    auto obj = std::make_unique<DiagramShapeObject>();
    obj->set_dimensions(params.size.width, params.size.height);
    return obj;
}

auto CanvasObjectFactory::create_comment(const ObjectCreationParams& params) const
    -> std::unique_ptr<CanvasObject>
{
    auto obj = std::make_unique<CommentObject>();
    // CommentObject has fixed bounds (24×24 icon), no resize needed
    if (!params.text_content.empty())
    {
        Comment root;
        root.body = params.text_content;
        obj->add_comment(root);
    }
    return obj;
}

auto CanvasObjectFactory::create_icon(const ObjectCreationParams& params) const
    -> std::unique_ptr<CanvasObject>
{
    auto obj = std::make_unique<IconObject>();
    obj->set_icon_size(params.size.width);
    return obj;
}

auto CanvasObjectFactory::create_table(const ObjectCreationParams& params) const
    -> std::unique_ptr<CanvasObject>
{
    auto obj = std::make_unique<TableObject>();
    // TableObject manages its own dimensions via columns/rows; no direct resize
    (void)params;
    return obj;
}

} // namespace markamp::canvas
