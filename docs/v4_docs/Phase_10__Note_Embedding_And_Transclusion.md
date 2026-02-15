# Phase 10 -- Note Embedding and Transclusion

## Objective

Implement `![[Note]]` embed/transclusion rendering: when a document contains `![[Other Note]]`, the content of "Other Note" is rendered inline at that position. Supports embedding entire documents, specific headings (`![[Note#Heading]]`), specific blocks (`![[Note^block-id]]`), and media files (images, PDFs). Embeds are live -- updating the source note updates all transclusions.

## Prerequisites

- Phase 01 (WikiLink with is_embed, DocumentModel)
- Phase 02 (VaultService -- document access and link resolution)
- Existing HtmlRenderer, PreviewPanel

## Feature References (PRD)

- PRD #21: Note Embedding
- PRD #22: Transclusion
- PRD #26: External File Embedding (images, PDFs, audio)

## Data Structures to Implement

### File: `src/core/EmbedResolver.h`

```cpp
#pragma once

#include "WikiLink.h"

#include <expected>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class VaultService;
class EventBus;

enum class EmbedType : uint8_t
{
    FullDocument,    // ![[Note]] -- embed entire note
    Heading,         // ![[Note#Section]] -- embed a heading section
    Block,           // ![[Note^block-id]] -- embed a specific block
    Image,           // ![[image.png]] -- embed an image
    Pdf,             // ![[document.pdf]] -- embed a PDF
    Audio,           // ![[recording.mp3]] -- embed audio player
    Video,           // ![[clip.mp4]] -- embed video player
    Unknown
};

struct ResolvedEmbed
{
    WikiLink source_link;              // The original ![[...]] link
    EmbedType type{EmbedType::Unknown};
    std::string resolved_path;         // Absolute file path
    std::string content;               // Markdown content (for text embeds)
    std::string html;                  // Pre-rendered HTML (for media embeds)
    std::string document_id;           // Source document ID (for text embeds)
    std::string error;                 // Error message if resolution failed
    bool is_resolved{false};
    int depth{0};                      // Nesting depth (embeds within embeds)

    [[nodiscard]] auto is_text_embed() const -> bool;
    [[nodiscard]] auto is_media_embed() const -> bool;
};

/// Resolves ![[embed]] links to their content or media representation.
class EmbedResolver
{
public:
    EmbedResolver(EventBus& event_bus, VaultService& vault_service);

    /// Resolve a single embed link.
    [[nodiscard]] auto resolve(const WikiLink& link, int depth = 0) const
        -> ResolvedEmbed;

    /// Resolve all embeds in a document's markdown content.
    [[nodiscard]] auto resolve_all(const std::string& markdown,
                                    const std::string& source_doc_id) const
        -> std::vector<ResolvedEmbed>;

    /// Expand all embeds in markdown content, replacing ![[...]] with actual content.
    /// Handles recursive embeds up to max_depth.
    [[nodiscard]] auto expand_content(const std::string& markdown,
                                       const std::string& source_doc_id,
                                       int max_depth = 3) const
        -> std::string;

    /// Generate HTML for an embed (used by rendering pipeline).
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
    [[nodiscard]] auto resolve_block_embed(const WikiLink& link, int depth) const
        -> ResolvedEmbed;
    [[nodiscard]] auto resolve_media_embed(const WikiLink& link) const
        -> ResolvedEmbed;
    [[nodiscard]] auto extract_heading_section(const std::string& markdown,
                                                const std::string& heading) const
        -> std::optional<std::string>;
    [[nodiscard]] auto extract_block(const std::string& markdown,
                                      const std::string& block_id) const
        -> std::optional<std::string>;

    static constexpr int MAX_EMBED_DEPTH = 5;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`resolve(link, depth)`** -- Detect embed type via file extension. Dispatch to resolve_document_embed, resolve_heading_embed, resolve_block_embed, or resolve_media_embed. Set depth on result. Check depth < MAX_EMBED_DEPTH to prevent infinite recursion.

2. **`resolve_document_embed(link, depth)`** -- Resolve link target to document path via VaultService. Read the file content. Strip frontmatter (don't embed YAML). Return content as markdown. If depth > 0, recursively expand any embeds within.

3. **`resolve_heading_embed(link, depth)`** -- Resolve document. Find the heading section matching link.heading. Extract all content from that heading until the next heading of same or higher level. Return section content.

4. **`extract_heading_section(markdown, heading)`** -- Parse markdown line by line. Find `# heading` match (case insensitive). Collect lines until next heading of equal or higher level (fewer `#` marks). Return collected content.

5. **`resolve_block_embed(link, depth)`** -- Find a block identified by `^block-id` at the end of a paragraph. Extract just that paragraph/block.

6. **`resolve_media_embed(link)`** -- Detect file type from extension. For images: generate `<img>` tag. For PDFs: generate `<iframe>` or `<object>` tag. For audio: `<audio>` tag. For video: `<video>` tag. Set resolved_path to absolute file path.

7. **`expand_content(markdown, source_doc_id, max_depth)`** -- Find all `![[...]]` patterns. For each, resolve and replace with the embedded content (indented in a blockquote or `<div class="embed">`). Handle recursive embeds by incrementing depth. Stop at max_depth.

8. **`render_embed_html(embed)`** -- Generate HTML representation: for text embeds, wrap in `<div class="transclusion">` with a header showing source note name and a link back. For media, use appropriate HTML5 media tags.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(EmbedResolvedEvent)
std::string source_doc_id;
std::string target_path;
EmbedType embed_type;
bool success{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TransclusionUpdatedEvent)
std::string source_doc_id;   // The document being re-rendered
std::string embedded_doc_id; // The embedded document that changed
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_embed_resolver.cpp`

1. **Full document embed** -- `![[Note B]]` resolves to Note B's full content (sans frontmatter).
2. **Heading embed** -- `![[Note B#Section]]` resolves to only that section's content.
3. **Block embed** -- `![[Note B^abc123]]` resolves to the paragraph ending with `^abc123`.
4. **Image embed** -- `![[photo.png]]` resolves to `<img>` HTML.
5. **PDF embed** -- `![[paper.pdf]]` resolves to `<iframe>` HTML.
6. **Audio embed** -- `![[recording.mp3]]` resolves to `<audio>` HTML.
7. **Unresolved embed** -- `![[NonExistent]]` returns is_resolved=false with error message.
8. **Recursive embed** -- Note A embeds Note B which embeds Note C. expand_content(A) includes all three. Verify depth tracking.
9. **Circular embed prevention** -- Note A embeds Note B which embeds Note A. Verify max_depth stops recursion.
10. **Heading extraction** -- Document with H1, H2, H2, H1. Extract second H2 section. Verify stops at next H1.
11. **Expand replaces inline** -- Markdown with text, embed, more text. expand_content produces continuous content.
12. **Embed type detection** -- Verify .png=Image, .pdf=Pdf, .mp3=Audio, .mp4=Video, .md=FullDocument.

## Acceptance Criteria

- [ ] Full document embeds render the target note's content inline
- [ ] Heading embeds extract and render only the specified section
- [ ] Block embeds extract a single paragraph/block
- [ ] Media embeds render appropriate HTML5 tags
- [ ] Recursive embeds work up to max_depth with no infinite loops
- [ ] Unresolved embeds show an error message inline
- [ ] Frontmatter is stripped from embedded content
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/EmbedResolver.h` | EmbedResolver, ResolvedEmbed, EmbedType |
| CREATE | `src/core/EmbedResolver.cpp` | Full implementation |
| MODIFY | `src/rendering/HtmlRenderer.cpp` | Integrate embed resolution into rendering pipeline |
| MODIFY | `src/core/Events.h` | Add 2 embed events |
| MODIFY | `src/core/PluginContext.h` | Add `EmbedResolver* embed_resolver{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add EmbedResolver.cpp |
| CREATE | `tests/unit/test_embed_resolver.cpp` | 12 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_embed_resolver target |

## Architecture Notes

- EmbedResolver is stateless per-call; it reads from VaultService on each resolve
- The rendering pipeline (HtmlRenderer) calls EmbedResolver when it encounters `![[...]]`
- Live updates: when an embedded note changes (FileContentChangedEvent), the embedding note's preview re-renders
- Circular reference detection uses depth counter, not a visited set (simpler, sufficient for reasonable depths)
- Constructor injection: EmbedResolver(EventBus&, VaultService&)

## Estimated Complexity

**L** -- Multiple embed types, heading/block extraction, recursive expansion, media HTML generation, 12 tests.
