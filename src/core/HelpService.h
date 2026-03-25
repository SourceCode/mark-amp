/// @file HelpService.h
/// @brief V9 Phase 35 — Central help coordination service.
///
/// Owns the `HelpArticleRegistry`, provides contextual help lookup,
/// tooltip information, and unified search across all help content.
/// Registers built-in articles for Markdown syntax, common errors,
/// and feature documentation.

#pragma once

#include "HelpArticle.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// TooltipInfo — tooltip content for UI elements
// ============================================================================

/// Tooltip information for a UI element.
struct TooltipInfo
{
    std::string description;   ///< Short action description
    std::string shortcut_hint; ///< Keyboard shortcut, e.g. "⌘+S"
    std::string extended_help; ///< Extended explanation shown on prolonged hover
};

// ============================================================================
// HelpService — central help coordination
// ============================================================================

/// Coordinates all help content: article registry, contextual help,
/// tooltips, and search.
///
/// Usage:
/// ```cpp
/// HelpService help;
/// auto results = help.search("heading");
/// auto ctx = help.get_contextual_help("editor");
/// auto tip = help.get_tooltip("toolbar.bold");
/// ```
class HelpService
{
public:
    HelpService()
    {
        register_built_in_articles();
        register_built_in_tooltips();
    }

    // --- Article registry delegation ---

    /// Register a custom help article.
    void register_article(HelpArticle article)
    {
        registry_.register_article(std::move(article));
    }

    /// Get an article by ID.
    [[nodiscard]] auto get_article(const std::string& article_id) const -> const HelpArticle*
    {
        return registry_.get_article(article_id);
    }

    /// Get articles by category.
    [[nodiscard]] auto get_articles_by_category(HelpCategory category) const
        -> std::vector<const HelpArticle*>
    {
        return registry_.get_articles_by_category(category);
    }

    /// Total article count.
    [[nodiscard]] auto article_count() const -> std::size_t
    {
        return registry_.article_count();
    }

    // --- Search ---

    /// Unified search across all help content.
    [[nodiscard]] auto search(const std::string& query) const -> std::vector<HelpSearchResult>
    {
        return registry_.search(query);
    }

    // --- Contextual help ---

    /// Get help article for the current context (mode).
    /// Context IDs: "editor", "graph", "settings"
    [[nodiscard]] auto get_contextual_help(const std::string& context_id) const
        -> const HelpArticle*
    {
        auto iter = context_map_.find(context_id);
        if (iter != context_map_.end())
        {
            return registry_.get_article(iter->second);
        }
        return registry_.get_article("general.overview");
    }

    /// Register a context → article mapping.
    void register_context_help(const std::string& context_id, const std::string& article_id)
    {
        context_map_[context_id] = article_id;
    }

    // --- Tooltips ---

    /// Get tooltip for a UI element.
    [[nodiscard]] auto get_tooltip(const std::string& element_id) const
        -> std::optional<TooltipInfo>
    {
        auto iter = tooltips_.find(element_id);
        if (iter != tooltips_.end())
        {
            return iter->second;
        }
        return std::nullopt;
    }

    /// Register a tooltip for a UI element.
    void register_tooltip(const std::string& element_id, TooltipInfo info)
    {
        tooltips_[element_id] = std::move(info);
    }

    /// Get the underlying registry for direct access.
    [[nodiscard]] auto registry() const -> const HelpArticleRegistry&
    {
        return registry_;
    }

private:
    HelpArticleRegistry registry_;
    std::unordered_map<std::string, std::string> context_map_;
    std::unordered_map<std::string, TooltipInfo> tooltips_;

    void register_built_in_articles()
    {
        // --- Markdown syntax reference ---
        registry_.register_article({.id = "syntax.headings",
                                    .title = "Headings",
                                    .body = "Use `#` symbols to create headings.\n\n"
                                            "# Heading 1\n## Heading 2\n### Heading 3\n"
                                            "#### Heading 4\n##### Heading 5\n###### Heading 6\n",
                                    .category = HelpCategory::Syntax,
                                    .tags = {"heading", "title", "h1", "h2", "h3"}});

        registry_.register_article({.id = "syntax.emphasis",
                                    .title = "Emphasis",
                                    .body = "Use `*` or `_` for emphasis.\n\n"
                                            "*italic* or _italic_\n"
                                            "**bold** or __bold__\n"
                                            "***bold italic*** or ___bold italic___\n"
                                            "~~strikethrough~~\n",
                                    .category = HelpCategory::Syntax,
                                    .tags = {"bold", "italic", "strikethrough", "emphasis"}});

        registry_.register_article({.id = "syntax.links",
                                    .title = "Links & Images",
                                    .body = "[Link text](url)\n"
                                            "![Alt text](image.png)\n"
                                            "[[wiki-link]]\n"
                                            "[[wiki-link|display text]]\n",
                                    .category = HelpCategory::Syntax,
                                    .tags = {"link", "image", "wiki-link", "url"}});

        registry_.register_article({.id = "syntax.lists",
                                    .title = "Lists",
                                    .body = "Unordered:\n- Item 1\n- Item 2\n  - Nested\n\n"
                                            "Ordered:\n1. First\n2. Second\n\n"
                                            "Task list:\n- [x] Done\n- [ ] Todo\n",
                                    .category = HelpCategory::Syntax,
                                    .tags = {"list", "bullet", "ordered", "task", "checkbox"}});

        registry_.register_article({.id = "syntax.code",
                                    .title = "Code",
                                    .body = "Inline code: `code`\n\n"
                                            "Fenced code block:\n```language\ncode here\n```\n\n"
                                            "Indented code block:\n    code here\n",
                                    .category = HelpCategory::Syntax,
                                    .tags = {"code", "fenced", "inline", "block"}});

        registry_.register_article({.id = "syntax.tables",
                                    .title = "Tables",
                                    .body = "| Header | Header |\n"
                                            "|--------|--------|\n"
                                            "| Cell   | Cell   |\n",
                                    .category = HelpCategory::Syntax,
                                    .tags = {"table", "row", "column", "header"}});

        registry_.register_article({.id = "syntax.blockquotes",
                                    .title = "Blockquotes",
                                    .body = "> Single-level quote\n\n"
                                            "> Nested:\n>> Inner quote\n",
                                    .category = HelpCategory::Syntax,
                                    .tags = {"blockquote", "quote", "callout"}});

        registry_.register_article(
            {.id = "syntax.extensions",
             .title = "MarkAmp Extensions",
             .body = "Wiki-links: [[page-name]]\n"
                     "Embed: `![[embedded-file]]`\n"
                     "Mermaid diagrams: ```mermaid ... ```\n",
             .category = HelpCategory::Syntax,
             .tags = {"wiki-link", "embed", "mermaid", "extension"}});

        // --- Contextual help articles ---
        registry_.register_article(
            {.id = "context.editor",
             .title = "Editor Mode",
             .body = "The editor supports full Markdown editing with live preview, "
                     "syntax highlighting, auto-completion, and keyboard shortcuts.\n\n"
                     "Use Cmd+P to quick-open files, Cmd+Shift+P for command palette.\n",
             .category = HelpCategory::General,
             .tags = {"editor", "markdown", "editing"}});

        // V29: Canvas and Notebook context help articles removed (features retired).

        registry_.register_article(
            {.id = "context.graph",
             .title = "Graph View",
             .body = "The knowledge graph visualises connections between your documents. "
                     "Nodes represent files, edges represent links.\n\n"
                     "Use filters to focus on specific connections.\n",
             .category = HelpCategory::General,
             .tags = {"graph", "backlink", "knowledge", "connection"}});

        registry_.register_article(
            {.id = "general.overview",
             .title = "MarkAmp Overview",
             .body = "MarkAmp is a focused Markdown workbench with editor, "
                     "knowledge graph, and powerful document tooling.\n\n"
                     "Press Cmd+Shift+P to open the command palette and explore features.\n",
             .category = HelpCategory::General,
             .tags = {"overview", "getting-started", "introduction"}});

        // --- Error help articles ---
        registry_.register_article(
            {.id = "error.file_not_found",
             .title = "File Not Found",
             .body = "The requested file could not be found at the specified path.\n\n"
                     "**How to fix:**\n"
                     "- Check the file path for typos\n"
                     "- Ensure the file exists in your workspace\n"
                     "- Check if the file was moved or renamed\n",
             .category = HelpCategory::Error,
             .tags = {"file", "not-found", "path", "missing"}});

        registry_.register_article(
            {.id = "error.invalid_markdown",
             .title = "Invalid Markdown",
             .body = "The document contains Markdown that could not be fully parsed.\n\n"
                     "**How to fix:**\n"
                     "- Check for unclosed code fences (missing closing ```)\n"
                     "- Verify table alignment separators\n"
                     "- Check for malformed link syntax\n",
             .category = HelpCategory::Error,
             .tags = {"markdown", "parse", "syntax", "invalid"}});

        // V29: Corrupt board error article removed (canvas feature retired).

        registry_.register_article({.id = "error.generic",
                                    .title = "General Troubleshooting",
                                    .body =
                                        "An unexpected error occurred.\n\n"
                                        "**General steps:**\n"
                                        "- Restart MarkAmp\n"
                                        "- Check the Output panel for detailed error messages\n"
                                        "- Try 'Help: Open Playground' to verify functionality\n"
                                        "- Report the issue via 'Help: Report Issue'\n",
                                    .category = HelpCategory::Error,
                                    .tags = {"error", "troubleshooting", "generic", "help"}});

        // --- Register context mappings ---
        context_map_["editor"] = "context.editor";
        context_map_["graph"] = "context.graph";
        context_map_["settings"] = "general.overview";
    }

    void register_built_in_tooltips()
    {
        tooltips_["toolbar.bold"] = {"Toggle bold text", "⌘+B", "Wraps selection in **bold**"};
        tooltips_["toolbar.italic"] = {"Toggle italic text", "⌘+I", "Wraps selection in *italic*"};
        tooltips_["toolbar.code"] = {"Toggle inline code", "⌘+`", "Wraps selection in `code`"};
        tooltips_["toolbar.link"] = {"Insert link", "⌘+K", "Creates a [text](url) link"};
        tooltips_["toolbar.heading"] = {
            "Cycle heading level", "⌘+H", "Cycles through heading levels 1-6"};
        tooltips_["toolbar.list"] = {"Toggle list", "⌘+L", "Creates an unordered list"};
        tooltips_["toolbar.task"] = {"Toggle task", "⌘+Shift+T", "Creates a task checkbox"};
        tooltips_["toolbar.save"] = {"Save file", "⌘+S", "Saves the current file to disk"};
        tooltips_["toolbar.preview"] = {
            "Toggle preview", "⌘+Shift+V", "Shows rendered Markdown preview"};
        tooltips_["toolbar.split"] = {"Split editor", "⌘+\\", "Opens a side-by-side split view"};
        tooltips_["sidebar.explorer"] = {
            "File Explorer", "⌘+Shift+E", "Browse workspace files and folders"};
        tooltips_["sidebar.search"] = {
            "Search", "⌘+Shift+F", "Search across all files in workspace"};
        tooltips_["sidebar.graph"] = {
            "Knowledge Graph", "⌘+Shift+G", "View connections between documents"};
    }
};

} // namespace markamp::core
