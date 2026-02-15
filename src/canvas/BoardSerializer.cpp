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

} // namespace markamp::canvas
