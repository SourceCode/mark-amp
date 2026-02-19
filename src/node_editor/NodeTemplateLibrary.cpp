#include "NodeTemplateLibrary.h"

#include <algorithm>

namespace markamp::node_editor
{

NodeTemplateLibrary::NodeTemplateLibrary()
{
    register_builtin_templates();
}

void NodeTemplateLibrary::register_builtin_templates()
{
    // Basic empty graph template
    {
        GraphTemplate tmpl;
        tmpl.name = "Blank Graph";
        tmpl.description = "An empty graph with no nodes";
        tmpl.category = "Basic";
        tmpl.tags = {"empty", "starter"};
        tmpl.serialized_graph = R"({"nodes":[],"links":[]})";
        tmpl.author = "MarkAmp";
        tmpl.version = "1.0";
        register_template(std::move(tmpl));
    }

    // Simple chain template
    {
        GraphTemplate tmpl;
        tmpl.name = "Simple Chain";
        tmpl.description = "Three nodes connected in a linear chain";
        tmpl.category = "Basic";
        tmpl.tags = {"chain", "linear", "starter"};
        tmpl.serialized_graph =
            R"({"nodes":["Source","Process","Output"],"links":[{"from":0,"to":1},{"from":1,"to":2}]})";
        tmpl.author = "MarkAmp";
        tmpl.version = "1.0";
        register_template(std::move(tmpl));
    }

    // Math operations template
    {
        GraphTemplate tmpl;
        tmpl.name = "Math Operations";
        tmpl.description = "Common math operations: Add, Multiply, Clamp";
        tmpl.category = "Math";
        tmpl.tags = {"math", "arithmetic"};
        tmpl.serialized_graph =
            R"({"nodes":["InputA","InputB","Add","Multiply","Clamp","Output"]})";
        tmpl.author = "MarkAmp";
        tmpl.version = "1.0";
        register_template(std::move(tmpl));
    }
}

auto NodeTemplateLibrary::register_template(GraphTemplate tmpl) -> TemplateId
{
    TemplateId tid{next_template_id_++};
    tmpl.template_id = tid;
    templates_.emplace(tid.value, std::move(tmpl));
    return tid;
}

auto NodeTemplateLibrary::unregister_template(TemplateId template_id) -> bool
{
    return templates_.erase(template_id.value) > 0;
}

auto NodeTemplateLibrary::find_template(TemplateId template_id) const -> const GraphTemplate*
{
    auto iter = templates_.find(template_id.value);
    if (iter == templates_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

auto NodeTemplateLibrary::all_templates() const -> std::vector<TemplateId>
{
    std::vector<TemplateId> result;
    result.reserve(templates_.size());
    for (const auto& [key, _] : templates_)
    {
        result.emplace_back(TemplateId{key});
    }
    return result;
}

auto NodeTemplateLibrary::templates_for_category(const std::string& category) const
    -> std::vector<TemplateId>
{
    std::vector<TemplateId> result;
    for (const auto& [key, tmpl] : templates_)
    {
        if (tmpl.category == category)
        {
            result.emplace_back(TemplateId{key});
        }
    }
    return result;
}

auto NodeTemplateLibrary::search_templates(const std::string& query) const
    -> std::vector<TemplateId>
{
    std::vector<TemplateId> result;
    std::string lower_query = query;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

    for (const auto& [key, tmpl] : templates_)
    {
        std::string lower_name = tmpl.name;
        std::transform(lower_name.begin(),
                       lower_name.end(),
                       lower_name.begin(),
                       [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

        std::string lower_desc = tmpl.description;
        std::transform(lower_desc.begin(),
                       lower_desc.end(),
                       lower_desc.begin(),
                       [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

        bool matches = lower_name.find(lower_query) != std::string::npos ||
                       lower_desc.find(lower_query) != std::string::npos;

        if (!matches)
        {
            for (const auto& tag : tmpl.tags)
            {
                std::string lower_tag = tag;
                std::transform(lower_tag.begin(),
                               lower_tag.end(),
                               lower_tag.begin(),
                               [](unsigned char ch)
                               { return static_cast<char>(std::tolower(ch)); });
                if (lower_tag.find(lower_query) != std::string::npos)
                {
                    matches = true;
                    break;
                }
            }
        }

        if (matches)
        {
            result.emplace_back(TemplateId{key});
        }
    }
    return result;
}

auto NodeTemplateLibrary::template_count() const -> std::size_t
{
    return templates_.size();
}

auto NodeTemplateLibrary::register_snippet(NodeSnippet snippet) -> SnippetId
{
    SnippetId sid{next_snippet_id_++};
    snippet.snippet_id = sid;
    snippets_.emplace(sid.value, std::move(snippet));
    return sid;
}

auto NodeTemplateLibrary::unregister_snippet(SnippetId snippet_id) -> bool
{
    return snippets_.erase(snippet_id.value) > 0;
}

auto NodeTemplateLibrary::find_snippet(SnippetId snippet_id) const -> const NodeSnippet*
{
    auto iter = snippets_.find(snippet_id.value);
    if (iter == snippets_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

auto NodeTemplateLibrary::all_snippets() const -> std::vector<SnippetId>
{
    std::vector<SnippetId> result;
    result.reserve(snippets_.size());
    for (const auto& [key, _] : snippets_)
    {
        result.emplace_back(SnippetId{key});
    }
    return result;
}

auto NodeTemplateLibrary::snippet_count() const -> std::size_t
{
    return snippets_.size();
}

void NodeTemplateLibrary::clear()
{
    templates_.clear();
    snippets_.clear();
}

} // namespace markamp::node_editor
