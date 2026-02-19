#pragma once

// V11 Phase 42: Webview And Custom Widget Node Embeds
// Manages embedded custom widget descriptors per node.

#include "NodeEditorTypes.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

/// Strong-typed embed identifier.
struct EmbedId
{
    uint64_t value{0};

    constexpr EmbedId() = default;
    constexpr explicit EmbedId(uint64_t v_arg)
        : value(v_arg)
    {
    }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        return value != 0;
    }
    constexpr auto operator==(const EmbedId&) const noexcept -> bool = default;
    constexpr auto operator<=>(const EmbedId&) const noexcept = default;
};

/// Type of embedded widget.
enum class WidgetEmbedType : uint8_t
{
    kSlider,
    kColorPicker,
    kDropdown,
    kToggle,
    kTextInput,
    kCustomHTML
};

/// Descriptor for an embedded widget.
struct EmbedDescriptor
{
    EmbedId embed_id;
    NodeId node_id;
    WidgetEmbedType widget_type{WidgetEmbedType::kSlider};
    Rect bounds;
    bool is_interactive{true};
    bool sandboxed{true};
    std::string current_value;
    std::string label;
};

/// Manages embedded custom widgets within nodes.
class NodeWidgetEmbed
{
public:
    NodeWidgetEmbed() = default;

    // --- Registration ---
    auto register_embed(NodeId node_id, EmbedDescriptor descriptor) -> EmbedId;
    auto remove_embed(EmbedId embed_id) -> bool;

    // --- Queries ---
    [[nodiscard]] auto embeds_for(NodeId node_id) const -> std::vector<EmbedId>;
    [[nodiscard]] auto find_embed(EmbedId embed_id) const -> const EmbedDescriptor*;
    [[nodiscard]] auto embed_count() const -> std::size_t;
    [[nodiscard]] auto is_sandboxed(EmbedId embed_id) const -> bool;

    // --- Value management ---
    void set_value(EmbedId embed_id, const std::string& val);
    [[nodiscard]] auto value(EmbedId embed_id) const -> std::string;

    // --- Layout ---
    [[nodiscard]] auto embed_bounds(EmbedId embed_id) const -> Rect;
    void resize_embed(EmbedId embed_id, Rect new_bounds);

    // --- Bulk ---
    void clear_all();

private:
    uint64_t next_id_{1};
    std::unordered_map<uint64_t, EmbedDescriptor> embeds_;
};

} // namespace markamp::node_editor
