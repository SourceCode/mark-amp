// ============================================================================
// File: src/rendering/IFrameBlockRenderer.h
// Phase 29: Advanced Block Types — IFrame block renderer
// ============================================================================
#pragma once

#include <string>
#include <string_view>
#include <utility>

namespace markamp::rendering
{

/// Configuration for an iframe block.
struct IFrameConfig
{
    std::string url;
    int width{640};
    int height{480};
    bool sandbox{true};
    bool allow_scripts{false};
    std::string border_style{"none"};
};

/// Renders iframe blocks as sandboxed <iframe> elements.
/// Applies strict CSP and sandbox attributes by default.
class IFrameBlockRenderer
{
public:
    /// Render an iframe block with the given configuration.
    [[nodiscard]] auto render(const IFrameConfig& config) const -> std::string;

    /// Validate a URL for iframe embedding (block dangerous schemes).
    [[nodiscard]] static auto validate_url(std::string_view url) -> bool;

    /// Parse iframe dimensions from a markdown code block info string.
    [[nodiscard]] static auto parse_dimensions(std::string_view info_string) -> std::pair<int, int>;

    /// Whether iframes are sandboxed by default.
    [[nodiscard]] static constexpr auto is_sandboxed_by_default() noexcept -> bool
    {
        return true;
    }
};

} // namespace markamp::rendering
