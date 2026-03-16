#include "BoardSerializer.h"

#include <sstream>

namespace markamp::canvas
{

auto BoardSerializer::register_factory(CanvasObjectType type, ObjectFactory factory) -> void
{
    factories_[static_cast<uint8_t>(type)] = std::move(factory);
}

auto BoardSerializer::has_factory(CanvasObjectType type) const -> bool
{
    return factories_.contains(static_cast<uint8_t>(type));
}

auto BoardSerializer::serialize(const Board& board) const -> std::string
{
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"format_version\": " << kFormatVersion << ",\n";
    oss << "  \"metadata\": " << serialize_metadata(board.metadata()) << ",\n";
    oss << "  \"object_count\": " << board.object_count() << ",\n";

    // Improvement 48: Serialize objects array via each object's to_json()
    oss << "  \"objects\": [\n";
    const auto& objects = board.objects();
    for (size_t i = 0; i < objects.size(); ++i)
    {
        if (objects[i] != nullptr)
        {
            oss << "    " << objects[i]->to_json();
            if (i + 1 < objects.size())
            {
                oss << ",";
            }
            oss << "\n";
        }
    }
    oss << "  ]\n";
    oss << "}\n";
    return oss.str();
}

auto BoardSerializer::deserialize(const std::string& json_data) const -> Board
{
    // Improvement 49: Parse JSON and reconstruct objects via factories.
    Board board;

    if (!validate_json(json_data))
    {
        return board;
    }

    // Parse object count for pre-allocation
    const auto obj_count_pos = json_data.find("\"object_count\"");
    if (obj_count_pos != std::string::npos)
    {
        const auto colon_pos = json_data.find(':', obj_count_pos);
        if (colon_pos != std::string::npos)
        {
            auto num_start = colon_pos + 1;
            while (num_start < json_data.size() &&
                   (json_data[num_start] == ' ' || json_data[num_start] == '\t'))
            {
                ++num_start;
            }
            // Object count parsed for metadata purposes
        }
    }

    // Parse objects array — find each object's type and delegate to factory.
    const auto objects_pos = json_data.find("\"objects\"");
    if (objects_pos != std::string::npos)
    {
        const auto array_start = json_data.find('[', objects_pos);
        if (array_start != std::string::npos)
        {
            // Find each object block delimited by { }
            size_t pos = array_start + 1;
            while (pos < json_data.size())
            {
                const auto obj_start = json_data.find('{', pos);
                if (obj_start == std::string::npos || obj_start > json_data.find(']', pos))
                {
                    break;
                }

                // Find matching closing brace (handles nesting).
                int brace_depth = 1;
                auto obj_end = obj_start + 1;
                while (obj_end < json_data.size() && brace_depth > 0)
                {
                    if (json_data[obj_end] == '{') ++brace_depth;
                    if (json_data[obj_end] == '}') --brace_depth;
                    ++obj_end;
                }

                const auto obj_json = json_data.substr(obj_start, obj_end - obj_start);

                // Use CanvasObjectFactory to reconstruct the object from JSON.
                auto obj = factory_.from_json(obj_json);
                if (obj)
                {
                    board.add_object(std::move(obj));
                }

                pos = obj_end;
            }
        }
    }

    return board;
}

auto BoardSerializer::serialize_metadata(const BoardMetadata& meta) const -> std::string
{
    std::ostringstream oss;
    oss << "{\n";
    oss << "    \"name\": \"" << meta.name << "\",\n";
    oss << "    \"description\": \"" << meta.description << "\",\n";
    oss << "    \"author\": \"" << meta.author << "\",\n";
    oss << "    \"archived\": " << (meta.archived ? "true" : "false") << ",\n";
    oss << "    \"grid_visible\": " << (meta.grid_visible ? "true" : "false") << ",\n";
    oss << "    \"grid_spacing\": " << meta.grid_spacing << ",\n";
    oss << "    \"default_zoom\": " << meta.default_zoom << ",\n";
    oss << "    \"tag_count\": " << meta.tags.size() << "\n";
    oss << "  }";
    return oss.str();
}

// ── Improvements (#27-28) ───────────────────────────────────────

auto BoardSerializer::validate_json(const std::string& json_data) const -> bool
{
    // Basic validation: check that it looks like a JSON object with the expected keys.
    if (json_data.empty())
    {
        return false;
    }
    // Must start with '{' and contain "format_version".
    const auto first_brace = json_data.find('{');
    if (first_brace == std::string::npos)
    {
        return false;
    }
    return json_data.find("format_version") != std::string::npos &&
           json_data.find("metadata") != std::string::npos;
}

auto BoardSerializer::migrate_format(const std::string& json_data, int from_version) const
    -> std::string
{
    if (from_version >= kFormatVersion)
    {
        return json_data; // Already current or newer — no migration needed.
    }

    // Version 1 → 2: add "favorite" and "version" fields to metadata.
    std::string migrated = json_data;
    if (from_version < 2)
    {
        // Insert default values for new metadata fields.
        const auto meta_pos = migrated.find("\"metadata\"");
        if (meta_pos != std::string::npos)
        {
            const auto obj_start = migrated.find('{', meta_pos);
            if (obj_start != std::string::npos)
            {
                migrated.insert(obj_start + 1, "\n    \"favorite\": false,\n    \"version\": 1,");
            }
        }
    }
    return migrated;
}

} // namespace markamp::canvas
