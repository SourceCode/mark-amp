#include "MindMapNode.h"

#include <algorithm>
#include <sstream>

namespace markamp::canvas
{

MindMapNode::MindMapNode()
    : CanvasObject(CanvasObjectType::MindMapNode)
{
    set_name("Mind Map Node");
}

auto MindMapNode::text() const -> const std::string&
{
    return text_;
}
auto MindMapNode::set_text(const std::string& text) -> void
{
    text_ = text;
    mark_dirty();
}

auto MindMapNode::parent_node_id() const -> ObjectId
{
    return parent_node_id_;
}
auto MindMapNode::set_parent_node_id(ObjectId id) -> void
{
    parent_node_id_ = id;
    mark_dirty();
}

auto MindMapNode::child_node_ids() const -> const std::vector<ObjectId>&
{
    return child_node_ids_;
}
auto MindMapNode::add_child_node_id(ObjectId id) -> void
{
    child_node_ids_.push_back(id);
    mark_dirty();
}
auto MindMapNode::remove_child_node_id(ObjectId id) -> void
{
    child_node_ids_.erase(std::remove(child_node_ids_.begin(), child_node_ids_.end(), id),
                          child_node_ids_.end());
    mark_dirty();
}

auto MindMapNode::connector_id() const -> ObjectId
{
    return connector_id_;
}
auto MindMapNode::set_connector_id(ObjectId id) -> void
{
    connector_id_ = id;
    mark_dirty();
}

auto MindMapNode::node_color() const -> const CanvasColor&
{
    return node_color_;
}
auto MindMapNode::set_node_color(const CanvasColor& color) -> void
{
    node_color_ = color;
    mark_dirty();
}

auto MindMapNode::depth() const -> int
{
    return depth_;
}
auto MindMapNode::set_depth(int d) -> void
{
    depth_ = d;
    mark_dirty();
}

auto MindMapNode::width() const -> double
{
    return width_;
}
auto MindMapNode::height() const -> double
{
    return height_;
}
auto MindMapNode::set_dimensions(double w, double h) -> void
{
    width_ = w;
    height_ = h;
    mark_dirty();
}

auto MindMapNode::local_bounds() const -> AABB
{
    return {0.0, 0.0, width_, height_};
}

auto MindMapNode::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"MindMapNode\""
        << ",\"text\":\"" << text_ << "\""
        << ",\"parent_id\":" << parent_node_id_ << ",\"connector_id\":" << connector_id_
        << ",\"depth\":" << depth_ << ",\"width\":" << width_ << ",\"height\":" << height_
        << ",\"children\":[";
    for (size_t idx = 0; idx < child_node_ids_.size(); ++idx)
    {
        if (idx > 0)
        {
            oss << ",";
        }
        oss << child_node_ids_[idx];
    }
    oss << "]}";
    return oss.str();
}

auto MindMapNode::from_json(const std::string& /*json*/) -> void
{
    // Stub — real JSON parsing deferred.
}

auto MindMapNode::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<MindMapNode>();
    copy->set_text(text_);
    copy->set_parent_node_id(parent_node_id_);
    for (const auto& child_id : child_node_ids_)
    {
        copy->add_child_node_id(child_id);
    }
    copy->set_connector_id(connector_id_);
    copy->set_node_color(node_color_);
    copy->set_depth(depth_);
    copy->set_dimensions(width_, height_);
    copy->set_name(name());
    return copy;
}

} // namespace markamp::canvas
