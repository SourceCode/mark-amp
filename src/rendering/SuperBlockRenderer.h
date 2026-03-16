// ============================================================================
// File: src/rendering/SuperBlockRenderer.h
// Phase 29: Advanced Block Types — Super block (layout container) renderer
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::rendering
{

/// Layout direction for a super block container.
enum class SuperBlockLayout : uint8_t
{
    kHorizontal, // Children side-by-side (flex-direction: row)
    kVertical    // Children stacked (flex-direction: column)
};

/// Renders super blocks — layout containers that group child blocks
/// in horizontal or vertical arrangements.
class SuperBlockRenderer
{
public:
    /// Render a super block containing pre-rendered child HTML fragments.
    [[nodiscard]] auto render(const std::vector<std::string>& child_html_fragments,
                              SuperBlockLayout layout) const -> std::string;

    /// Detect super block syntax markers: {{{row or {{{col.
    [[nodiscard]] static auto detect_super_block(std::string_view content) -> bool;

    /// Parse the layout direction from a super block marker.
    [[nodiscard]] static auto parse_layout(std::string_view marker) -> SuperBlockLayout;

    /// Generate CSS for super block containers.
    [[nodiscard]] static auto css() -> std::string;

    /// Number of layout directions (horizontal and vertical).
    [[nodiscard]] static constexpr auto total_layouts() noexcept -> int
    {
        return 2;
    }
};

} // namespace markamp::rendering
