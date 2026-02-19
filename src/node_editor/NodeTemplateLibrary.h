#pragma once

// V11 Phase 45: Template Library Starter Graphs And Snippets
// Manages graph templates, starter graphs, and node snippets.

#include "NodeEditorTypes.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

/// Strong-typed template identifier.
struct TemplateId
{
    uint64_t value{0};

    constexpr TemplateId() = default;
    constexpr explicit TemplateId(uint64_t v_arg)
        : value(v_arg)
    {
    }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        return value != 0;
    }
    constexpr auto operator==(const TemplateId&) const noexcept -> bool = default;
    constexpr auto operator<=>(const TemplateId&) const noexcept = default;
};

/// Strong-typed snippet identifier.
struct SnippetId
{
    uint64_t value{0};

    constexpr SnippetId() = default;
    constexpr explicit SnippetId(uint64_t v_arg)
        : value(v_arg)
    {
    }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        return value != 0;
    }
    constexpr auto operator==(const SnippetId&) const noexcept -> bool = default;
    constexpr auto operator<=>(const SnippetId&) const noexcept = default;
};

/// A graph template: a reusable starting point for new graphs.
struct GraphTemplate
{
    TemplateId template_id;
    std::string name;
    std::string description;
    std::string category;
    std::vector<std::string> tags;
    std::string serialized_graph;
    std::string thumbnail_hint;
    std::string author;
    std::string version;
};

/// A node snippet: a small sub-graph that can be pasted.
struct NodeSnippet
{
    SnippetId snippet_id;
    std::string name;
    std::string description;
    std::string category;
    std::vector<std::string> tags;
    std::string serialized_nodes; // Sub-graph serialization
};

/// Manages graph templates and snippets.
class NodeTemplateLibrary
{
public:
    /// Constructor registers built-in templates.
    NodeTemplateLibrary();

    // --- Templates ---
    auto register_template(GraphTemplate tmpl) -> TemplateId;
    auto unregister_template(TemplateId template_id) -> bool;
    [[nodiscard]] auto find_template(TemplateId template_id) const -> const GraphTemplate*;
    [[nodiscard]] auto all_templates() const -> std::vector<TemplateId>;
    [[nodiscard]] auto templates_for_category(const std::string& category) const
        -> std::vector<TemplateId>;
    [[nodiscard]] auto search_templates(const std::string& query) const -> std::vector<TemplateId>;
    [[nodiscard]] auto template_count() const -> std::size_t;

    // --- Snippets ---
    auto register_snippet(NodeSnippet snippet) -> SnippetId;
    auto unregister_snippet(SnippetId snippet_id) -> bool;
    [[nodiscard]] auto find_snippet(SnippetId snippet_id) const -> const NodeSnippet*;
    [[nodiscard]] auto all_snippets() const -> std::vector<SnippetId>;
    [[nodiscard]] auto snippet_count() const -> std::size_t;

    // --- Bulk ---
    void clear();

private:
    uint64_t next_template_id_{1};
    uint64_t next_snippet_id_{1};
    std::unordered_map<uint64_t, GraphTemplate> templates_;
    std::unordered_map<uint64_t, NodeSnippet> snippets_;

    void register_builtin_templates();
};

} // namespace markamp::node_editor
