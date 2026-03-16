#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasObject.h"
#include "canvas/CanvasObjectFactory.h"
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

    // ── Improvements (#27-28) ─────────────────────────────────

    /// Validate that a JSON string is a well-formed board file.
    [[nodiscard]] auto validate_json(const std::string& json_data) const -> bool;

    /// Migrate board data from an older format version to the current version.
    [[nodiscard]] auto migrate_format(const std::string& json_data, int from_version) const
        -> std::string;

    /// File format version.
    static constexpr int kFormatVersion = 2;

    /// Number of registered object factories.
    [[nodiscard]] auto factory_count() const noexcept -> std::size_t
    {
        return factories_.size();
    }

    /// Whether the given version matches the current format version.
    [[nodiscard]] static constexpr auto is_current_version(int version) noexcept -> bool
    {
        return version == kFormatVersion;
    }

    // ── Round 2 Batch 9 (#81-82) ──────────────────────────────────

    /// (#81) Whether any object factories are registered.
    [[nodiscard]] auto has_factories() const noexcept -> bool
    {
        return !factories_.empty();
    }

    /// (#82) Whether no factories are registered.
    [[nodiscard]] auto is_factory_empty() const noexcept -> bool
    {
        return factories_.empty();
    }

private:
    std::unordered_map<uint8_t, ObjectFactory> factories_;
    CanvasObjectFactory factory_;

    [[nodiscard]] auto serialize_metadata(const BoardMetadata& meta) const -> std::string;
};

} // namespace markamp::canvas
