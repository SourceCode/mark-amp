#pragma once

/// @file EmbedResolver.h
/// @brief V4 Phase 10 – Note Embedding and Transclusion.

#include "WikiLink.h"

#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class VaultService;
class EventBus;

// ============================================================================
// Embed Type enum
// ============================================================================

enum class EmbedType : uint8_t
{
    kFullDocument,
    kHeading,
    kBlock,
    kImage,
    kPdf,
    kAudio,
    kVideo,
    kUnknown
};

// ============================================================================
// Resolved Embed result
// ============================================================================

struct ResolvedEmbed
{
    WikiLink source_link;
    EmbedType type{EmbedType::kUnknown};
    std::string resolved_path;
    std::string content;
    std::string html;
    std::string document_id;
    std::string error;
    bool is_resolved{false};
    int depth{0};

    [[nodiscard]] auto is_text_embed() const -> bool;
    [[nodiscard]] auto is_media_embed() const -> bool;
};

// ============================================================================
// Embed Resolver
// ============================================================================

class EmbedResolver
{
public:
    EmbedResolver(EventBus& event_bus, VaultService& vault_service);

    /// Resolve a single embed link.
    [[nodiscard]] auto resolve(const WikiLink& link, int depth = 0) const -> ResolvedEmbed;

    /// Resolve all embeds in a document's markdown content.
    [[nodiscard]] auto resolve_all(const std::string& markdown,
                                   const std::string& source_doc_id) const
        -> std::vector<ResolvedEmbed>;

    /// Expand all embeds in markdown content, replacing ![[...]] with actual content.
    [[nodiscard]] auto expand_content(const std::string& markdown,
                                      const std::string& source_doc_id,
                                      int max_depth = 3) const -> std::string;

    /// Generate HTML for an embed.
    [[nodiscard]] auto render_embed_html(const ResolvedEmbed& embed) const -> std::string;

    /// Detect the embed type from a file extension or link target.
    [[nodiscard]] static auto detect_type(const WikiLink& link) -> EmbedType;

private:
    EventBus& event_bus_;
    VaultService& vault_service_;

    [[nodiscard]] auto resolve_document_embed(const WikiLink& link, int depth) const
        -> ResolvedEmbed;
    [[nodiscard]] auto resolve_heading_embed(const WikiLink& link, int depth) const
        -> ResolvedEmbed;
    [[nodiscard]] auto resolve_block_embed(const WikiLink& link, int depth) const -> ResolvedEmbed;
    [[nodiscard]] auto resolve_media_embed(const WikiLink& link) const -> ResolvedEmbed;
    [[nodiscard]] auto extract_heading_section(const std::string& markdown,
                                               const std::string& heading) const
        -> std::optional<std::string>;
    [[nodiscard]] auto extract_block(const std::string& markdown, const std::string& block_id) const
        -> std::optional<std::string>;
    [[nodiscard]] auto strip_frontmatter(const std::string& markdown) const -> std::string;

    static constexpr int kMaxEmbedDepth = 5;
};

} // namespace markamp::core
