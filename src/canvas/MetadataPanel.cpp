#include "MetadataPanel.h"

#include <algorithm>
#include <sstream>

namespace markamp::canvas
{

// ── Helpers ────────────────────────────────────────────────────────────

namespace
{

auto type_to_string(CanvasObjectType obj_type) -> std::string
{
    switch (obj_type)
    {
        case CanvasObjectType::StickyNote:
            return "StickyNote";
        case CanvasObjectType::TextBox:
            return "TextBox";
        case CanvasObjectType::Shape:
            return "Shape";
        case CanvasObjectType::Connector:
            return "Connector";
        case CanvasObjectType::FreehandPath:
            return "FreehandPath";
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
            return "MindMapNode";
        case CanvasObjectType::KanbanColumn:
            return "KanbanColumn";
        case CanvasObjectType::KanbanCard:
            return "KanbanCard";
        case CanvasObjectType::BookmarkCard:
            return "BookmarkCard";
        case CanvasObjectType::VideoEmbed:
            return "VideoEmbed";
        case CanvasObjectType::PDFPage:
            return "PDFPage";
        case CanvasObjectType::Icon:
            return "Icon";
        case CanvasObjectType::Comment:
            return "Comment";
        case CanvasObjectType::DiagramShape:
            return "DiagramShape";
        case CanvasObjectType::CrossBoardLink:
            return "CrossBoardLink";
    }
    return "Unknown";
}

} // anonymous namespace

// ── Public API ─────────────────────────────────────────────────────────

auto MetadataPanel::set_object(ObjectId obj_id,
                               CanvasObjectType obj_type,
                               int z_index,
                               const std::unordered_map<std::string, std::string>& metadata,
                               const std::vector<std::string>& tags) -> void
{
    current_id_ = obj_id;
    rebuild_fields(obj_type, z_index, metadata, tags);
}

auto MetadataPanel::clear() -> void
{
    current_id_ = kInvalidObjectId;
    fields_.clear();
}

auto MetadataPanel::field_count() const -> size_t
{
    return fields_.size();
}

auto MetadataPanel::field_at(size_t index) const -> const MetadataField&
{
    return fields_.at(index);
}

auto MetadataPanel::add_custom_field(const std::string& key, const std::string& value) -> void
{
    // Prevent duplicate keys.
    for (const auto& field : fields_)
    {
        if (field.key == key)
        {
            return;
        }
    }

    MetadataField field;
    field.key = key;
    field.value = value;
    field.is_system = false;
    field.is_predefined = false;
    field.type = MetadataFieldType::kText;
    fields_.push_back(std::move(field));

    if (on_field_changed_)
    {
        on_field_changed_(current_id_, key, value);
    }
}

auto MetadataPanel::remove_field(const std::string& key) -> bool
{
    auto iter = std::find_if(fields_.begin(),
                             fields_.end(),
                             [&key](const MetadataField& field) { return field.key == key; });
    if (iter == fields_.end() || iter->is_system)
    {
        return false;
    }

    fields_.erase(iter);

    if (on_field_removed_)
    {
        on_field_removed_(current_id_, key);
    }
    return true;
}

auto MetadataPanel::set_field_value(const std::string& key, const std::string& value) -> bool
{
    auto iter = std::find_if(fields_.begin(),
                             fields_.end(),
                             [&key](const MetadataField& field) { return field.key == key; });
    if (iter == fields_.end() || iter->is_system)
    {
        return false;
    }

    iter->value = value;

    if (on_field_changed_)
    {
        on_field_changed_(current_id_, key, value);
    }
    return true;
}

auto MetadataPanel::set_field_type(const std::string& key, MetadataFieldType field_type) -> bool
{
    auto iter = std::find_if(fields_.begin(),
                             fields_.end(),
                             [&key](const MetadataField& field) { return field.key == key; });
    if (iter == fields_.end() || iter->is_system)
    {
        return false;
    }
    iter->type = field_type;
    return true;
}

auto MetadataPanel::set_on_field_changed(OnFieldChanged callback) -> void
{
    on_field_changed_ = std::move(callback);
}

auto MetadataPanel::set_on_field_removed(OnFieldRemoved callback) -> void
{
    on_field_removed_ = std::move(callback);
}

auto MetadataPanel::is_visible() const -> bool
{
    return visible_;
}

auto MetadataPanel::set_visible(bool visible) -> void
{
    visible_ = visible;
}

auto MetadataPanel::current_object_id() const -> ObjectId
{
    return current_id_;
}

// ── Private ────────────────────────────────────────────────────────────

auto MetadataPanel::rebuild_fields(CanvasObjectType obj_type,
                                   int z_index,
                                   const std::unordered_map<std::string, std::string>& metadata,
                                   const std::vector<std::string>& tags) -> void
{
    fields_.clear();

    // System fields (read-only).
    {
        MetadataField field;
        field.key = "id";
        field.value = std::to_string(current_id_);
        field.is_system = true;
        fields_.push_back(std::move(field));
    }
    {
        MetadataField field;
        field.key = "type";
        field.value = type_to_string(obj_type);
        field.is_system = true;
        fields_.push_back(std::move(field));
    }
    {
        MetadataField field;
        field.key = "z_index";
        field.value = std::to_string(z_index);
        field.is_system = true;
        fields_.push_back(std::move(field));
    }

    // Predefined fields.
    static const std::vector<std::string> kPredefinedKeys = {
        "status", "priority", "author", "created", "modified"};

    for (const auto& predefined_key : kPredefinedKeys)
    {
        MetadataField field;
        field.key = predefined_key;
        field.is_predefined = true;

        auto iter = metadata.find(predefined_key);
        if (iter != metadata.end())
        {
            field.value = iter->second;
        }

        // Assign type hints for predefined fields.
        if (predefined_key == "created" || predefined_key == "modified")
        {
            field.type = MetadataFieldType::kDate;
        }
        else if (predefined_key == "status")
        {
            field.type = MetadataFieldType::kSelect;
            field.options = {"todo", "in-progress", "done", "blocked"};
        }
        else if (predefined_key == "priority")
        {
            field.type = MetadataFieldType::kSelect;
            field.options = {"low", "medium", "high", "critical"};
        }

        fields_.push_back(std::move(field));
    }

    // Tags (as a single comma-separated system field).
    {
        MetadataField field;
        field.key = "tags";
        std::ostringstream tag_stream;
        for (size_t idx = 0; idx < tags.size(); ++idx)
        {
            if (idx > 0)
            {
                tag_stream << ", ";
            }
            tag_stream << tags[idx];
        }
        field.value = tag_stream.str();
        field.is_system = true;
        fields_.push_back(std::move(field));
    }

    // Custom fields from metadata (excluding predefined keys).
    for (const auto& [meta_key, meta_value] : metadata)
    {
        const bool is_predefined =
            std::find(kPredefinedKeys.begin(), kPredefinedKeys.end(), meta_key) !=
            kPredefinedKeys.end();
        if (!is_predefined)
        {
            MetadataField field;
            field.key = meta_key;
            field.value = meta_value;
            fields_.push_back(std::move(field));
        }
    }
}

} // namespace markamp::canvas
