#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace markamp::canvas
{

/// Factory function that creates a CanvasObject from JSON data.
using ObjectFactory = std::function<std::unique_ptr<CanvasObject>(const std::string& json_data)>;

/// Serializes and deserializes Board data to/from JSON.
class BoardSerializer
{
public:
    /// Register a factory for a specific object type.
    auto register_factory(CanvasObjectType type, ObjectFactory factory) -> void;

    /// Check if a factory is registered for a type.
    [[nodiscard]] auto has_factory(CanvasObjectType type) const -> bool;

    /// Serialize a board to JSON string.
    [[nodiscard]] auto serialize(const Board& board) const -> std::string;

    /// Deserialize a board from JSON string.
    [[nodiscard]] auto deserialize(const std::string& json_data) const -> Board;

    /// File format version.
    static constexpr int kFormatVersion = 1;

private:
    std::unordered_map<uint8_t, ObjectFactory> factories_;

    [[nodiscard]] auto serialize_metadata(const BoardMetadata& meta) const -> std::string;
};

} // namespace markamp::canvas
