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
auto MindMapNode::set_dimensions(double node_width, double node_height) -> void
{
    width_ = node_width;
    height_ = node_height;
    mark_dirty();
}

// ── Extended properties (#19-23) ────────────────────────────

auto MindMapNode::icon() const -> const std::string&
{
    return icon_;
}
auto MindMapNode::set_icon(const std::string& icon_name) -> void
{
    icon_ = icon_name;
    mark_dirty();
}

auto MindMapNode::notes() const -> const std::string&
{
    return notes_;
}
auto MindMapNode::set_notes(const std::string& notes_text) -> void
{
    notes_ = notes_text;
    mark_dirty();
}

auto MindMapNode::is_collapsed() const -> bool
{
    return collapsed_;
}
auto MindMapNode::set_collapsed(bool collapsed) -> void
{
    collapsed_ = collapsed;
    mark_dirty();
}

auto MindMapNode::priority() const -> int
{
    return priority_;
}
auto MindMapNode::set_priority(int level) -> void
{
    priority_ = std::clamp(level, 0, 3);
    mark_dirty();
}

auto MindMapNode::progress() const -> int
{
    return progress_;
}
auto MindMapNode::set_progress(int percent) -> void
{
    progress_ = std::clamp(percent, 0, 100);
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
        << ",\"icon\":\"" << icon_ << "\""
        << ",\"notes\":\"" << notes_ << "\""
        << ",\"collapsed\":" << (collapsed_ ? "true" : "false") << ",\"priority\":" << priority_
        << ",\"progress\":" << progress_ << ",\"children\":[";
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

auto MindMapNode::from_json(const std::string& json) -> void
{
    // Helper to extract a string value.
    auto get_str = [&](const std::string& key) -> std::string
    {
        const std::string needle = "\"" + key + "\":\"";
        auto pos = json.find(needle);
        if (pos == std::string::npos)
        {
            return "";
        }
        pos += needle.size();
        const auto end_pos = json.find('"', pos);
        if (end_pos == std::string::npos)
        {
            return "";
        }
        return json.substr(pos, end_pos - pos);
    };

    // Helper to extract a numeric value.
    auto get_num = [&](const std::string& key) -> double
    {
        const std::string needle = "\"" + key + "\":";
        auto pos = json.find(needle);
        if (pos == std::string::npos)
        {
            return 0.0;
        }
        pos += needle.size();
        return std::stod(json.substr(pos));
    };

    // Helper to extract a bool value.
    auto get_bool = [&](const std::string& key) -> bool
    {
        const std::string needle = "\"" + key + "\":";
        auto pos = json.find(needle);
        if (pos == std::string::npos)
        {
            return false;
        }
        pos += needle.size();
        return json.substr(pos, 4) == "true";
    };

    text_ = get_str("text");
    parent_node_id_ = static_cast<ObjectId>(get_num("parent_id"));
    connector_id_ = static_cast<ObjectId>(get_num("connector_id"));
    depth_ = static_cast<int>(get_num("depth"));
    width_ = get_num("width");
    height_ = get_num("height");
    icon_ = get_str("icon");
    notes_ = get_str("notes");
    collapsed_ = get_bool("collapsed");
    priority_ = static_cast<int>(get_num("priority"));
    progress_ = static_cast<int>(get_num("progress"));

    // Parse children array.
    child_node_ids_.clear();
    auto children_pos = json.find("\"children\":[");
    if (children_pos != std::string::npos)
    {
        children_pos += 12;
        const auto arr_end = json.find(']', children_pos);
        if (arr_end != std::string::npos)
        {
            const std::string arr_str = json.substr(children_pos, arr_end - children_pos);
            size_t pos = 0;
            while (pos < arr_str.size())
            {
                while (pos < arr_str.size() && (arr_str[pos] == ',' || arr_str[pos] == ' '))
                {
                    ++pos;
                }
                if (pos >= arr_str.size())
                {
                    break;
                }
                child_node_ids_.push_back(
                    static_cast<ObjectId>(std::stoi(arr_str.substr(pos))));
                while (pos < arr_str.size() && arr_str[pos] != ',')
                {
                    ++pos;
                }
            }
        }
    }
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
    copy->set_icon(icon_);
    copy->set_notes(notes_);
    copy->set_collapsed(collapsed_);
    copy->set_priority(priority_);
    copy->set_progress(progress_);
    copy->set_name(name());
    return copy;
}

// ── Batch 9 (#54) ─────────────────────────────────────────────────

auto MindMapNode::is_leaf() const -> bool
{
    return child_node_ids_.empty();
}

} // namespace markamp::canvas
