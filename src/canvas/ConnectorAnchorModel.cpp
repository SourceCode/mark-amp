#include "ConnectorAnchorModel.h"

#include <algorithm>

namespace markamp::canvas
{

auto ConnectorAnchor::position_name() const -> std::string
{
    switch (position)
    {
        case AnchorPosition::kTop:
            return "top";
        case AnchorPosition::kBottom:
            return "bottom";
        case AnchorPosition::kLeft:
            return "left";
        case AnchorPosition::kRight:
            return "right";
        case AnchorPosition::kCenter:
            return "center";
        case AnchorPosition::kAuto:
            return "auto";
    }
    return "unknown";
}

auto ConnectorAnchorModel::resolve_anchor(const ConnectorAnchor& anchor, const ObjectBounds& bounds)
    -> AnchorPoint
{
    AnchorPoint point;

    switch (anchor.position)
    {
        case AnchorPosition::kTop:
            point.pos_x = bounds.pos_x + bounds.width / 2.0;
            point.pos_y = bounds.pos_y;
            break;
        case AnchorPosition::kBottom:
            point.pos_x = bounds.pos_x + bounds.width / 2.0;
            point.pos_y = bounds.pos_y + bounds.height;
            break;
        case AnchorPosition::kLeft:
            point.pos_x = bounds.pos_x;
            point.pos_y = bounds.pos_y + bounds.height / 2.0;
            break;
        case AnchorPosition::kRight:
            point.pos_x = bounds.pos_x + bounds.width;
            point.pos_y = bounds.pos_y + bounds.height / 2.0;
            break;
        case AnchorPosition::kCenter:
            point.pos_x = bounds.pos_x + bounds.width / 2.0;
            point.pos_y = bounds.pos_y + bounds.height / 2.0;
            break;
        case AnchorPosition::kAuto:
            // Default to center for auto
            point.pos_x = bounds.pos_x + bounds.width / 2.0;
            point.pos_y = bounds.pos_y + bounds.height / 2.0;
            break;
    }

    point.pos_x += anchor.offset_x;
    point.pos_y += anchor.offset_y;

    return point;
}

void ConnectorAnchorModel::add_connection(const Connection& conn)
{
    connections_.push_back(conn);
}

void ConnectorAnchorModel::remove_connection(const std::string& connection_id)
{
    connections_.erase(std::remove_if(connections_.begin(),
                                      connections_.end(),
                                      [&connection_id](const Connection& conn)
                                      { return conn.connection_id == connection_id; }),
                       connections_.end());
}

auto ConnectorAnchorModel::find_connection(const std::string& connection_id) const
    -> const Connection*
{
    for (const auto& conn : connections_)
    {
        if (conn.connection_id == connection_id)
        {
            return &conn;
        }
    }
    return nullptr;
}

auto ConnectorAnchorModel::connection_count() const -> int
{
    return static_cast<int>(connections_.size());
}

auto ConnectorAnchorModel::connections_for_object(const std::string& object_id) const
    -> std::vector<const Connection*>
{
    std::vector<const Connection*> result;
    for (const auto& conn : connections_)
    {
        if (conn.source.object_id == object_id || conn.target.object_id == object_id)
        {
            result.push_back(&conn);
        }
    }
    return result;
}

auto ConnectorAnchorModel::is_anchor_valid(const ConnectorAnchor& anchor,
                                           const std::vector<ObjectBounds>& objects) const -> bool
{
    return std::any_of(objects.begin(),
                       objects.end(),
                       [&anchor](const ObjectBounds& obj)
                       { return obj.object_id == anchor.object_id; });
}

} // namespace markamp::canvas
