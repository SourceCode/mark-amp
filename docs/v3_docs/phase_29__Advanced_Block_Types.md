# Phase 29 -- Advanced Block Types (Callouts, Embeds, Audio, Video, IFrame)

## Objective

Implement advanced block types beyond standard Markdown, porting SiYuan's rich block support into MarkAmp's rendering pipeline. Standard Markdown (as parsed by md4c) covers paragraphs, headings, lists, code blocks, tables, and basic inline formatting. This phase extends the system with: callout/alert blocks (GitHub-style `> [!NOTE]` syntax), block embeds (transclusion of referenced block content inline), audio blocks (HTML5 audio player for attached recordings), video blocks (HTML5 video player for attached media), iframe blocks (sandboxed web content embedding), widget blocks (custom HTML/JS loaded from a widget path), and super blocks (horizontal or vertical layout containers that group child blocks).

These block types are critical for a knowledge management application. Callouts provide visual emphasis for important information. Embeds enable content reuse -- a single block of content can appear in multiple documents, always showing the latest version. Audio/video blocks support multimedia notes. IFrame blocks allow embedding external tools and dashboards. Widget blocks enable user-created interactive components. Super blocks provide layout flexibility by allowing side-by-side or vertically stacked arrangements of child blocks, similar to Notion's column layouts.

Each block type requires: (1) a detection mechanism in the Markdown parser or a custom syntax extension, (2) a new entry in the `MdNodeType` enum, (3) a dedicated renderer class that converts the block to HTML for the preview panel, and (4) integration with the existing HtmlRenderer dispatch. The rendering pipeline follows the existing pattern established by MermaidBlockRenderer -- each advanced block type gets its own renderer class implementing a common interface, registered with the HtmlRenderer for dispatch during AST traversal.

## Prerequisites

- No strict phase dependencies. Uses existing MarkAmp infrastructure: HtmlRenderer, MdNodeType, Md4cWrapper, EventBus.

## SiYuan Source Reference

- `kernel/treenode/node.go` -- Block type constants: NodeDocument, NodeHeading, NodeList, NodeBlockquote, NodeSuperBlock, NodeAudio, NodeVideo, NodeIFrame, NodeWidget, NodeBlockQueryEmbed
- `app/src/protyle/render/av/blockAttr.ts` -- Callout/alert block rendering with type icons and colors
- `app/src/protyle/render/blockRender.ts` -- Block embed rendering, fetching and displaying referenced content
- `app/src/protyle/render/av/asset.ts` -- Audio/video block rendering with HTML5 media players
- `app/src/protyle/wysiwyg/renderWidget.ts` -- Widget block rendering with sandboxed iframe
- `kernel/model/block.go` -- Super block handling, horizontal/vertical layout types
- GitHub Flavored Markdown spec -- Alert syntax: `> [!NOTE]`, `> [!TIP]`, `> [!IMPORTANT]`, `> [!WARNING]`, `> [!CAUTION]`

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|------|-----------|---------|
| `src/rendering/CalloutBlockRenderer.h` | `markamp::rendering` | Callout/alert block renderer |
| `src/rendering/CalloutBlockRenderer.cpp` | `markamp::rendering` | Callout rendering implementation |
| `src/rendering/EmbedBlockRenderer.h` | `markamp::rendering` | Block embed (transclusion) renderer |
| `src/rendering/EmbedBlockRenderer.cpp` | `markamp::rendering` | Embed rendering implementation |
| `src/rendering/AudioBlockRenderer.h` | `markamp::rendering` | Audio block renderer |
| `src/rendering/AudioBlockRenderer.cpp` | `markamp::rendering` | Audio rendering implementation |
| `src/rendering/VideoBlockRenderer.h` | `markamp::rendering` | Video block renderer |
| `src/rendering/VideoBlockRenderer.cpp` | `markamp::rendering` | Video rendering implementation |
| `src/rendering/IFrameBlockRenderer.h` | `markamp::rendering` | IFrame block renderer |
| `src/rendering/IFrameBlockRenderer.cpp` | `markamp::rendering` | IFrame rendering implementation |
| `src/rendering/SuperBlockRenderer.h` | `markamp::rendering` | Super block (layout container) renderer |
| `src/rendering/SuperBlockRenderer.cpp` | `markamp::rendering` | Super block implementation |
| `src/core/BlockTypeDetector.h` | `markamp::core` | Detects advanced block types in Markdown |
| `src/core/BlockTypeDetector.cpp` | `markamp::core` | BlockTypeDetector implementation |
| `tests/unit/test_advanced_blocks.cpp` | (test) | Catch2 test suite for all advanced block types |

### Existing Files to Modify

| File | Change |
|------|--------|
| `src/core/Types.h` | Add new MdNodeType entries |
| `src/core/Events.h` | Add embed/widget/audio events |
| `src/rendering/HtmlRenderer.h` | Add renderer pointers and dispatch for new block types |
| `src/rendering/HtmlRenderer.cpp` | Wire new block type rendering in render_node() |
| `src/core/Md4cWrapper.cpp` | Add callout detection in blockquote processing |
| `src/CMakeLists.txt` | Add new source files |
| `tests/CMakeLists.txt` | Add test_advanced_blocks target |

## Data Structures to Implement

```cpp
// ============================================================================
// Additions to src/core/Types.h -- new MdNodeType entries
// ============================================================================

// Add these to the MdNodeType enum:
//
// // Advanced block types
// CalloutBlock,      // > [!TYPE] alert block
// EmbedBlock,        // ((block-id)) transclusion
// AudioBlock,        // Audio player block
// VideoBlock,        // Video player block
// IFrameBlock,       // Sandboxed iframe block
// WidgetBlock,       // Custom HTML widget block
// SuperBlock,        // Layout container (horizontal/vertical)
```

```cpp
// ============================================================================
// File: src/rendering/CalloutBlockRenderer.h
// ============================================================================
#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace markamp::rendering
{

/// Type of callout/alert block.
enum class CalloutType : uint8_t
{
    Note,
    Tip,
    Important,
    Warning,
    Caution
};

/// Rendered callout with styling information.
struct CalloutRenderResult
{
    std::string html;
    CalloutType type{CalloutType::Note};
    std::string icon;       // SVG icon or Unicode glyph
    std::string css_class;  // CSS class for theming
};

/// Detects and renders GitHub-style callout/alert blocks.
/// Syntax: > [!NOTE], > [!TIP], > [!IMPORTANT], > [!WARNING], > [!CAUTION]
class CalloutBlockRenderer
{
public:
    CalloutBlockRenderer();

    /// Detect if a blockquote contains a callout marker on the first line.
    [[nodiscard]] auto detect_callout(std::string_view blockquote_content) const -> bool;

    /// Parse the callout type from the first line of a blockquote.
    [[nodiscard]] auto parse_callout_type(std::string_view first_line) const
        -> CalloutType;

    /// Render a callout block to HTML with appropriate styling.
    [[nodiscard]] auto render(std::string_view content, CalloutType type) const
        -> CalloutRenderResult;

    /// Get the display name for a callout type.
    [[nodiscard]] static auto type_name(CalloutType type) -> std::string_view;

    /// Get the icon for a callout type.
    [[nodiscard]] static auto type_icon(CalloutType type) -> std::string_view;

    /// Get the CSS color class for a callout type.
    [[nodiscard]] static auto type_css_class(CalloutType type) -> std::string_view;

private:
    std::unordered_map<std::string, CalloutType> type_map_;
};

} // namespace markamp::rendering
```

```cpp
// ============================================================================
// File: src/rendering/EmbedBlockRenderer.h
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
    std::string source_block_id; // The block being embedded
    bool resolved{false};        // Whether the block was found and rendered
};

/// Renders block embeds (transclusion). When a document contains ((block-id)),
/// this renderer fetches the referenced block's content, renders it to HTML,
/// and wraps it in a styled embed container with a source link.
class EmbedBlockRenderer
{
public:
    /// Callback type for resolving a block ID to its Markdown content.
    using BlockResolver = std::function<std::expected<std::string, std::string>(
        const std::string& block_id)>;

    /// Set the block resolver callback.
    void set_block_resolver(BlockResolver resolver);

    /// Render an embed block. Calls the resolver to fetch content, renders
    /// the content as Markdown, and wraps in an embed container.
    [[nodiscard]] auto render(const std::string& block_id) const -> EmbedRenderResult;

    /// Detect embed syntax in text: ((block-id)) pattern.
    [[nodiscard]] static auto detect_embed(std::string_view text) -> bool;

    /// Extract block ID from embed syntax.
    [[nodiscard]] static auto extract_block_id(std::string_view embed_text) -> std::string;

private:
    BlockResolver resolver_;
};

} // namespace markamp::rendering
```

```cpp
// ============================================================================
// File: src/rendering/AudioBlockRenderer.h
// ============================================================================
#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace markamp::rendering
{

/// Renders audio blocks as HTML5 <audio> elements with playback controls.
class AudioBlockRenderer
{
public:
    /// Render an audio block from a file path or URL.
    [[nodiscard]] auto render(std::string_view source_path,
                              const std::filesystem::path& base_path) const -> std::string;

    /// Detect if a block references an audio file (by extension).
    [[nodiscard]] static auto is_audio_file(std::string_view path) -> bool;

    /// Supported audio extensions.
    static constexpr std::string_view kSupportedExtensions[] = {
        ".mp3", ".wav", ".ogg", ".m4a", ".flac", ".aac", ".webm"
    };

    /// Get the MIME type for an audio file extension.
    [[nodiscard]] static auto mime_type(std::string_view extension) -> std::string_view;
};

} // namespace markamp::rendering
```

```cpp
// ============================================================================
// File: src/rendering/VideoBlockRenderer.h
// ============================================================================
#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace markamp::rendering
{

/// Renders video blocks as HTML5 <video> elements with playback controls.
class VideoBlockRenderer
{
public:
    /// Render a video block from a file path or URL.
    [[nodiscard]] auto render(std::string_view source_path,
                              const std::filesystem::path& base_path,
                              int width = 0,
                              int height = 0) const -> std::string;

    /// Detect if a block references a video file (by extension).
    [[nodiscard]] static auto is_video_file(std::string_view path) -> bool;

    /// Supported video extensions.
    static constexpr std::string_view kSupportedExtensions[] = {
        ".mp4", ".webm", ".ogv", ".mov", ".avi", ".mkv"
    };

    /// Get the MIME type for a video file extension.
    [[nodiscard]] static auto mime_type(std::string_view extension) -> std::string_view;
};

} // namespace markamp::rendering
```

```cpp
// ============================================================================
// File: src/rendering/IFrameBlockRenderer.h
// ============================================================================
#pragma once

#include <string>
#include <string_view>

namespace markamp::rendering
{

/// Configuration for an iframe block.
struct IFrameConfig
{
    std::string url;
    int width{640};
    int height{480};
    bool sandbox{true};       // Enable sandbox attribute
    bool allow_scripts{false}; // sandbox="allow-scripts"
    std::string border_style{"none"};
};

/// Renders iframe blocks as sandboxed <iframe> elements.
/// Applied strict CSP and sandbox attributes by default.
class IFrameBlockRenderer
{
public:
    /// Render an iframe block with the given configuration.
    [[nodiscard]] auto render(const IFrameConfig& config) const -> std::string;

    /// Validate a URL for iframe embedding (block dangerous schemes).
    [[nodiscard]] static auto validate_url(std::string_view url) -> bool;

    /// Parse iframe dimensions from a markdown code block info string.
    [[nodiscard]] static auto parse_dimensions(std::string_view info_string)
        -> std::pair<int, int>;
};

} // namespace markamp::rendering
```

```cpp
// ============================================================================
// File: src/rendering/SuperBlockRenderer.h
// ============================================================================
#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace markamp::rendering
{

/// Layout direction for a super block container.
enum class SuperBlockLayout : uint8_t
{
    Horizontal, // Children side-by-side (flex-direction: row)
    Vertical    // Children stacked (flex-direction: column)
};

/// Renders super blocks -- layout containers that group child blocks
/// in horizontal or vertical arrangements.
class SuperBlockRenderer
{
public:
    /// Render a super block containing pre-rendered child HTML fragments.
    [[nodiscard]] auto render(const std::vector<std::string>& child_html_fragments,
                              SuperBlockLayout layout) const -> std::string;

    /// Detect super block syntax markers.
    [[nodiscard]] static auto detect_super_block(std::string_view content) -> bool;

    /// Parse the layout direction from a super block marker.
    [[nodiscard]] static auto parse_layout(std::string_view marker) -> SuperBlockLayout;

    /// Generate CSS for super block containers.
    [[nodiscard]] static auto css() -> std::string;
};

} // namespace markamp::rendering
```

```cpp
// ============================================================================
// File: src/core/BlockTypeDetector.h
// ============================================================================
#pragma once

#include "Types.h"

#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Result of detecting an advanced block type.
struct BlockDetection
{
    MdNodeType type{MdNodeType::Paragraph};
    std::string content;         // The block's inner content
    std::string metadata;        // Type-specific metadata (URL, path, block_id, etc.)
    int start_line{0};
    int end_line{0};

    [[nodiscard]] auto is_advanced_type() const -> bool
    {
        return type == MdNodeType::CalloutBlock
            || type == MdNodeType::EmbedBlock
            || type == MdNodeType::AudioBlock
            || type == MdNodeType::VideoBlock
            || type == MdNodeType::IFrameBlock
            || type == MdNodeType::WidgetBlock
            || type == MdNodeType::SuperBlock;
    }
};

/// Pre-processes Markdown content to detect advanced block types
/// that are not natively supported by md4c.
class BlockTypeDetector
{
public:
    /// Scan Markdown content for advanced block types.
    /// Returns a list of detected blocks with their positions.
    [[nodiscard]] auto detect(std::string_view markdown) const
        -> std::vector<BlockDetection>;

    /// Detect callout blocks within blockquotes.
    [[nodiscard]] auto detect_callouts(std::string_view markdown) const
        -> std::vector<BlockDetection>;

    /// Detect embed blocks: ((block-id)) on its own line.
    [[nodiscard]] auto detect_embeds(std::string_view markdown) const
        -> std::vector<BlockDetection>;

    /// Detect audio/video blocks: !audio[](path) or !video[](path) syntax.
    [[nodiscard]] auto detect_media(std::string_view markdown) const
        -> std::vector<BlockDetection>;

    /// Detect iframe blocks: ```iframe syntax.
    [[nodiscard]] auto detect_iframes(std::string_view markdown) const
        -> std::vector<BlockDetection>;

    /// Detect super block markers: {{{ row/col ... }}}.
    [[nodiscard]] auto detect_super_blocks(std::string_view markdown) const
        -> std::vector<BlockDetection>;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`CalloutBlockRenderer::detect_callout(blockquote_content)`** -- Check if the first line of the blockquote matches the pattern `[!TYPE]` where TYPE is one of NOTE, TIP, IMPORTANT, WARNING, CAUTION (case-insensitive). Return true if matched.

2. **`CalloutBlockRenderer::render(content, type)`** -- Generate HTML: a `<div class="callout callout-{type}">` containing an icon element, a title element with the type name, and the content rendered as the blockquote body. Each type has distinct styling: NOTE=blue, TIP=green, IMPORTANT=purple, WARNING=yellow, CAUTION=red.

3. **`EmbedBlockRenderer::render(block_id)`** -- Call the `resolver_` callback to fetch the block's Markdown content. If resolved, render the content as HTML (by calling back into HtmlRenderer or a provided render function). Wrap in a `<div class="embed-block" data-block-id="{id}">` with a subtle border and a small source link. If unresolved, render a placeholder with "Block not found" message.

4. **`AudioBlockRenderer::render(source_path, base_path)`** -- Resolve the source path relative to base_path. Verify the file exists using `std::error_code` filesystem operations. Generate `<audio controls><source src="{resolved_path}" type="{mime}"></audio>` HTML. Add a fallback text for unsupported browsers.

5. **`VideoBlockRenderer::render(source_path, base_path, width, height)`** -- Similar to AudioBlockRenderer but generates `<video controls width="{w}" height="{h}">`. Include poster attribute if a .jpg thumbnail exists alongside the video file.

6. **`IFrameBlockRenderer::render(config)`** -- Validate the URL via `validate_url()`. Generate `<iframe src="{url}" width="{w}" height="{h}" sandbox="{attrs}" style="border:{border}">`. The sandbox attribute defaults to strict (no permissions) unless `allow_scripts` is true.

7. **`IFrameBlockRenderer::validate_url(url)`** -- Return false for `javascript:`, `data:`, `file:`, and `vbscript:` schemes. Return true for `https://`, `http://`. Block empty URLs.

8. **`SuperBlockRenderer::render(children, layout)`** -- Generate a `<div class="super-block super-block-{layout}">` with flexbox CSS. Each child fragment is wrapped in `<div class="super-block-child">`. Horizontal layout uses `flex-direction: row` with equal-width children. Vertical layout uses `flex-direction: column`.

9. **`BlockTypeDetector::detect_callouts(markdown)`** -- Scan for lines starting with `> [!` followed by a type keyword and `]`. Record the start and end lines of the enclosing blockquote. Return BlockDetection entries with `type = CalloutBlock`.

10. **`BlockTypeDetector::detect_embeds(markdown)`** -- Scan for lines containing only `((` + alphanumeric-with-hyphens + `))` (optionally surrounded by whitespace). Return BlockDetection with `type = EmbedBlock` and `metadata = block_id`.

11. **`HtmlRenderer::render_node()` -- advanced block dispatch** -- In the switch statement for `MdNodeType`, add cases for each advanced block type. Delegate to the corresponding renderer (callout_renderer_, embed_renderer_, etc.). Pass the node's content and metadata. Append the returned HTML to the output string.

12. **`BlockTypeDetector::detect_super_blocks(markdown)`** -- Scan for `{{{row` or `{{{col` markers. Pair with closing `}}}`. Extract child blocks between markers. Return BlockDetection with `type = SuperBlock` and metadata indicating layout direction.

## Events to Add

Add the following to `src/core/Events.h`:

```cpp
// ============================================================================
// Advanced block events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(EmbedBlockResolvedEvent)
std::string block_id;
bool resolved{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(WidgetLoadedEvent)
std::string widget_path;
bool success{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AudioRecordingStartedEvent)
std::string target_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AudioRecordingStoppedEvent)
std::string target_path;
double duration_seconds{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(MediaBlockInsertedEvent)
std::string file_path;
std::string block_type; // "audio", "video", "iframe"
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `knowledgebase.blocks.callouts_enabled` | bool | true | Enable callout/alert block rendering |
| `knowledgebase.blocks.embeds_enabled` | bool | true | Enable block embed transclusion |
| `knowledgebase.blocks.media_enabled` | bool | true | Enable audio/video block rendering |
| `knowledgebase.blocks.iframe_enabled` | bool | false | Enable iframe blocks (disabled by default for security) |
| `knowledgebase.blocks.widgets_enabled` | bool | false | Enable widget blocks (disabled by default for security) |
| `knowledgebase.blocks.iframe_max_width` | int | 800 | Maximum iframe width in pixels |
| `knowledgebase.blocks.iframe_max_height` | int | 600 | Maximum iframe height in pixels |

## Test Cases

All tests in `tests/unit/test_advanced_blocks.cpp` using Catch2.

1. **"CalloutBlockRenderer detects NOTE callout"** -- Input: `"> [!NOTE]\n> This is important"`. Verify `detect_callout()` returns true. Verify `parse_callout_type()` returns `CalloutType::Note`.

2. **"CalloutBlockRenderer renders all five callout types"** -- For each type (Note, Tip, Important, Warning, Caution), verify `render()` produces HTML containing the correct CSS class (`callout-note`, `callout-tip`, etc.) and the type name in the title.

3. **"CalloutBlockRenderer is case-insensitive"** -- Input: `"> [!note]"`, `"> [!NOTE]"`, `"> [!Note]"` -- all should detect as NOTE type.

4. **"EmbedBlockRenderer resolves and renders block content"** -- Set up a resolver that returns `"# Hello World"` for block_id "test-123". Call `render("test-123")`. Verify result contains rendered HTML with "Hello World" and `resolved == true`.

5. **"EmbedBlockRenderer shows placeholder for unresolved blocks"** -- Set up a resolver that returns an error. Call `render("missing-id")`. Verify `resolved == false` and HTML contains a "not found" message.

6. **"AudioBlockRenderer renders HTML5 audio element"** -- Call `render("recording.mp3", temp_dir)` where `recording.mp3` exists. Verify output contains `<audio controls>` and `<source src=` with correct MIME type `audio/mpeg`.

7. **"VideoBlockRenderer renders HTML5 video with dimensions"** -- Call `render("video.mp4", temp_dir, 640, 480)`. Verify output contains `<video controls width="640" height="480">`.

8. **"IFrameBlockRenderer validates URLs"** -- Verify `validate_url("https://example.com")` returns true. Verify `validate_url("javascript:alert(1)")` returns false. Verify `validate_url("")` returns false. Verify `validate_url("file:///etc/passwd")` returns false.

9. **"IFrameBlockRenderer applies sandbox by default"** -- Call `render({.url="https://example.com", .sandbox=true})`. Verify output contains `sandbox=""` attribute (empty sandbox = maximum restriction).

10. **"SuperBlockRenderer renders horizontal layout"** -- Call `render({"<p>A</p>", "<p>B</p>"}, SuperBlockLayout::Horizontal)`. Verify output contains `flex-direction: row` and two child divs.

11. **"BlockTypeDetector finds callout in blockquote"** -- Input with a callout blockquote and a normal blockquote. Verify `detect_callouts()` returns 1 detection with `type == CalloutBlock`.

12. **"BlockTypeDetector finds embed on standalone line"** -- Input: `"Some text\n((block-abc))\nMore text"`. Verify `detect_embeds()` returns 1 detection with metadata "block-abc". Verify `((block-abc))` inside inline text is NOT detected as a standalone embed.

## Acceptance Criteria

- [ ] Callout blocks render with correct icon, color, and title for all five types
- [ ] Callout detection is case-insensitive and handles multi-line content
- [ ] Embed blocks resolve and render referenced block content inline
- [ ] Unresolved embeds show a styled placeholder instead of breaking the document
- [ ] Audio blocks render as HTML5 audio players with correct MIME types
- [ ] Video blocks render as HTML5 video players with configurable dimensions
- [ ] IFrame blocks validate URLs and apply sandbox restrictions by default
- [ ] Super blocks render horizontal and vertical layouts using flexbox
- [ ] BlockTypeDetector correctly identifies all advanced types during pre-processing
- [ ] All new block types integrate with HtmlRenderer without breaking existing rendering

## Files to Create/Modify

```
CREATE  src/rendering/CalloutBlockRenderer.h
CREATE  src/rendering/CalloutBlockRenderer.cpp
CREATE  src/rendering/EmbedBlockRenderer.h
CREATE  src/rendering/EmbedBlockRenderer.cpp
CREATE  src/rendering/AudioBlockRenderer.h
CREATE  src/rendering/AudioBlockRenderer.cpp
CREATE  src/rendering/VideoBlockRenderer.h
CREATE  src/rendering/VideoBlockRenderer.cpp
CREATE  src/rendering/IFrameBlockRenderer.h
CREATE  src/rendering/IFrameBlockRenderer.cpp
CREATE  src/rendering/SuperBlockRenderer.h
CREATE  src/rendering/SuperBlockRenderer.cpp
CREATE  src/core/BlockTypeDetector.h
CREATE  src/core/BlockTypeDetector.cpp
CREATE  tests/unit/test_advanced_blocks.cpp
MODIFY  src/core/Types.h               -- add 7 new MdNodeType entries
MODIFY  src/core/Events.h              -- add advanced block events
MODIFY  src/rendering/HtmlRenderer.h   -- add renderer pointers for new block types
MODIFY  src/rendering/HtmlRenderer.cpp -- add dispatch cases in render_node()
MODIFY  src/core/Md4cWrapper.cpp       -- add callout detection in blockquote handler
MODIFY  src/CMakeLists.txt              -- add new source files
MODIFY  tests/CMakeLists.txt            -- add test_advanced_blocks target
MODIFY  resources/config_defaults.json  -- add knowledgebase.blocks.* defaults
```
