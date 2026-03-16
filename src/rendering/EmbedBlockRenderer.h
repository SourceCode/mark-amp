// ============================================================================
// File: src/rendering/EmbedBlockRenderer.h
// Phase 29: Advanced Block Types — Block embed (transclusion) renderer
// ============================================================================
#pragma once

#include <expected>
#include <functional>
#include <string>
#include <string_view>

namespace markamp::rendering
{

/// Result of rendering an embedded block.
struct EmbedRenderResult
{
    std::string html;
    std::string source_block_id;
    bool resolved{false};
};

/// Renders block embeds (transclusion). When a document contains ((block-id)),
/// this renderer fetches the referenced block's content, renders it to HTML,
/// and wraps it in a styled embed container with a source link.
class EmbedBlockRenderer
{
public:
    /// Callback type for resolving a block ID to its Markdown content.
    using BlockResolver =
        std::function<std::expected<std::string, std::string>(const std::string& block_id)>;

    /// Set the block resolver callback.
    void set_block_resolver(BlockResolver resolver);

    /// Render an embed block. Calls the resolver to fetch content.
    [[nodiscard]] auto render(const std::string& block_id) const -> EmbedRenderResult;

    /// Detect embed syntax in text: ((block-id)) pattern.
    [[nodiscard]] static auto detect_embed(std::string_view text) -> bool;

    /// Extract block ID from embed syntax.
    [[nodiscard]] static auto extract_block_id(std::string_view embed_text) -> std::string;

    /// Whether a block resolver callback has been set.
    [[nodiscard]] auto has_resolver() const -> bool
    {
        return resolver_ != nullptr;
    }

private:
    BlockResolver resolver_;
};

} // namespace markamp::rendering
