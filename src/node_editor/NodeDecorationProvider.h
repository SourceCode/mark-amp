#pragma once

// V11 Phase 43: FileSystem Language And Decoration Provider Integration
// Decoration provider: applies visual decorations to nodes from external providers.

#include "NodeEditorTypes.h"

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

/// Strong-typed decoration identifier.
struct DecorationId
{
    uint64_t value{0};

    constexpr DecorationId() = default;
    constexpr explicit DecorationId(uint64_t v_arg)
        : value(v_arg)
    {
    }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        return value != 0;
    }
    constexpr auto operator==(const DecorationId&) const noexcept -> bool = default;
    constexpr auto operator<=>(const DecorationId&) const noexcept = default;
};

/// Type of visual decoration.
enum class DecorationType : uint8_t
{
    kBadge,
    kBorder,
    kBackground,
    kIcon,
    kTooltip
};

/// A visual decoration applied to a node.
struct NodeDecoration
{
    DecorationId decoration_id;
    NodeId target_node;
    DecorationType type{DecorationType::kBadge};
    std::string display_text;
    std::string color_hex{"#FFFFFF"};
    int priority{0};
    std::string provider_id;
};

/// Registered decoration provider info.
struct DecorationProviderInfo
{
    std::string provider_id;
    std::string description;
};

/// Manages visual decorations applied to nodes from external providers.
class NodeDecorationProvider
{
public:
    NodeDecorationProvider() = default;

    // --- Provider registration ---
    void register_provider(const std::string& provider_id, const std::string& description);
    auto unregister_provider(const std::string& provider_id) -> bool;
    [[nodiscard]] auto provider_count() const -> std::size_t;

    // --- Decoration management ---
    auto add_decoration(NodeId node_id, NodeDecoration decoration) -> DecorationId;
    auto remove_decoration(DecorationId decoration_id) -> bool;
    [[nodiscard]] auto decorations_for(NodeId node_id) const -> std::vector<NodeDecoration>;
    [[nodiscard]] auto decorations_by_provider(const std::string& provider_id) const
        -> std::vector<NodeDecoration>;
    void clear_provider(const std::string& provider_id);
    [[nodiscard]] auto decoration_count() const -> std::size_t;

    // --- Language hints ---
    void set_language_hint(NodeId node_id, const std::string& language_id);
    [[nodiscard]] auto language_hint(NodeId node_id) const -> std::optional<std::string>;

    // --- Bulk ---
    void clear_all();

private:
    uint64_t next_id_{1};
    std::unordered_map<uint64_t, NodeDecoration> decorations_;
    std::unordered_map<std::string, DecorationProviderInfo> providers_;
    std::unordered_map<uint64_t, std::string> language_hints_; // NodeId.value -> language
};

} // namespace markamp::node_editor
