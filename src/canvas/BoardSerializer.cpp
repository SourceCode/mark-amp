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
    oss << "  \"objects\": []\n"; // Placeholder — real serialization via CanvasObject::to_json().
    oss << "}\n";
    return oss.str();
}

auto BoardSerializer::deserialize(const std::string& /*json_data*/) const -> Board
{
    // Stub: real implementation will parse JSON and reconstruct objects via factories.
    Board board;
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
