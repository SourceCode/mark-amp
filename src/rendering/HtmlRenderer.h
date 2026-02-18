#pragma once

#include "CodeBlockRenderer.h"
#include "MarkdownRenderingFeatures.h"
#include "core/Types.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{
class IMermaidRenderer;
class IMathRenderer;
} // namespace markamp::core

namespace markamp::rendering
{

/// Result of footnote pre-processing.
struct FootnoteResult
{
    std::string processed_markdown;    // Markdown with [^N] replaced by HTML superscripts
    std::string footnote_section_html; // HTML <section> with footnote definitions
    bool has_footnotes{false};
};

/// Extracts footnote references and definitions from markdown text.
/// md4c does not natively support footnotes, so this pre-processes
/// the markdown before passing it to the parser.
class FootnotePreprocessor
{
public:
    [[nodiscard]] auto process(std::string_view markdown) -> FootnoteResult;

private:
    struct FootnoteDefinition
    {
        std::string id; // "1", "note", etc.
        std::string content;
        int number{0}; // Sequential display number
    };

    [[nodiscard]] auto extract_definitions(std::string_view markdown)
        -> std::pair<std::string, std::vector<FootnoteDefinition>>;

    [[nodiscard]] auto replace_references(const std::string& markdown,
                                          const std::vector<FootnoteDefinition>& definitions)
        -> std::string;

    [[nodiscard]] auto build_section(const std::vector<FootnoteDefinition>& definitions)
        -> std::string;
};

/// Converts a MarkdownDocument AST to HTML.
class HtmlRenderer
{
public:
    [[nodiscard]] auto render(const core::MarkdownDocument& doc) -> std::string;

    /// Render with footnote section appended.
    [[nodiscard]] auto render_with_footnotes(const core::MarkdownDocument& doc,
                                             const std::string& footnote_section) -> std::string;

    /// Set optional Mermaid renderer for diagram blocks.
    void set_mermaid_renderer(core::IMermaidRenderer* renderer);

    /// Enable or disable Mermaid diagram rendering (feature guard).
    void set_mermaid_enabled(bool enabled)
    {
        mermaid_enabled_ = enabled;
    }

    /// Set optional math renderer for LaTeX math expressions.
    void set_math_renderer(core::IMathRenderer* renderer);

    /// Enable or disable math rendering (feature guard).
    void set_math_enabled(bool enabled)
    {
        math_enabled_ = enabled;
    }

    /// Set base path for resolving relative image paths.
    void set_base_path(const std::filesystem::path& base_path);

    /// Access the code block renderer (e.g. for clipboard copy).
    [[nodiscard]] auto code_renderer() const -> const CodeBlockRenderer&
    {
        return code_renderer_;
    }

    // ── Phase 8: Source-Line Attributes ──────────────────

    /// Enable/disable data-source-line="N" attributes on block elements.
    void set_source_line_attributes(bool enabled)
    {
        source_line_attrs_enabled_ = enabled;
    }
    [[nodiscard]] auto source_line_attributes_enabled() const -> bool
    {
        return source_line_attrs_enabled_;
    }

    /// Collected heading anchors from the most recent render() call.
    [[nodiscard]] auto heading_anchors() const -> const std::vector<HeadingAnchor>&
    {
        return heading_anchors_;
    }

    /// Collected source-line mappings from the most recent render() call.
    [[nodiscard]] auto source_line_mappings() const -> const std::vector<SourceLineMapping>&
    {
        return source_line_mappings_;
    }

    // ── Phase 8: Code Block Controls ─────────────────────

    /// Set code block rendering configuration.
    void set_code_block_config(const CodeBlockConfig& config)
    {
        code_block_config_ = config;
    }
    [[nodiscard]] auto code_block_config() const -> const CodeBlockConfig&
    {
        return code_block_config_;
    }

    // ── Phase 8: Sortable Tables ─────────────────────────

    /// Set sortable table configuration.
    void set_sortable_table_config(const SortableTableConfig& config)
    {
        sortable_table_config_ = config;
    }

    /// Static utilities (public for cross-component use)
    [[nodiscard]] static auto escape_html(std::string_view text) -> std::string;
    [[nodiscard]] static auto slugify(std::string_view text) -> std::string;
    [[nodiscard]] static auto alignment_style(core::MdAlignment align) -> std::string_view;
    [[nodiscard]] static auto mime_for_extension(std::string_view ext) -> std::string_view;

private:
    void render_node(const core::MdNode& node, std::string& output, int depth = 0);
    void render_children(const core::MdNode& node, std::string& output, int depth = 0);

    /// Stability #31: max recursion depth for render_node
    static constexpr int kMaxRenderDepth = 100;

    /// Improvement #38: collect plain text content from node children.
    [[nodiscard]] static auto collect_plain_text(const core::MdNode& node) -> std::string;

    /// Resolve an image URL to an absolute path, validating security constraints.
    /// Returns empty path if the URL is remote, blocked, or the file doesn't exist.
    [[nodiscard]] auto resolve_image_path(std::string_view url) const -> std::filesystem::path;

    /// Encode a local image file as a base64 data URI.
    /// Returns empty string on failure.
    [[nodiscard]] static auto encode_image_as_data_uri(const std::filesystem::path& image_path)
        -> std::string;

    /// Render a missing-image placeholder.
    [[nodiscard]] static auto render_missing_image(std::string_view url, std::string_view alt_text)
        -> std::string;

    core::IMermaidRenderer* mermaid_renderer_{nullptr};
    bool mermaid_enabled_{true};
    core::IMathRenderer* math_renderer_{nullptr};
    bool math_enabled_{true};
    std::filesystem::path base_path_;
    mutable CodeBlockRenderer code_renderer_;

    /// Improvement #6: track heading slug usage for uniqueness
    std::unordered_map<std::string, int> heading_slug_counts_;

    /// Max image file size: 10 MB
    static constexpr size_t kMaxImageFileSize = static_cast<size_t>(10) * 1024 * 1024;

    // ── Phase 8 state ────────────────────────────────────
    bool source_line_attrs_enabled_{false};
    int current_source_line_{0};
    std::vector<HeadingAnchor> heading_anchors_;
    std::vector<SourceLineMapping> source_line_mappings_;
    CodeBlockConfig code_block_config_;
    SortableTableConfig sortable_table_config_;
};

} // namespace markamp::rendering
