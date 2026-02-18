/// @file CssSnippetLibrary.cpp
/// @brief V9 Phase 38 — CssSnippetLibrary implementation.

#include "CssSnippetLibrary.h"

#include <algorithm>
#include <set>
#include <sstream>

namespace markamp::core
{

auto snippet_category_name(SnippetCategory cat) -> std::string
{
    switch (cat)
    {
        case SnippetCategory::kTypography:
            return "typography";
        case SnippetCategory::kColors:
            return "colors";
        case SnippetCategory::kSpacing:
            return "spacing";
        case SnippetCategory::kAnimations:
            return "animations";
        case SnippetCategory::kLayout:
            return "layout";
        case SnippetCategory::kCustom:
            return "custom";
    }
    return "custom";
}

// ── CRUD ──────────────────────────────────────────────────────────────────────

void CssSnippetLibrary::add_snippet(CssSnippet snippet)
{
    // Replace if name already exists
    for (auto& existing : snippets_)
    {
        if (existing.name == snippet.name)
        {
            existing = std::move(snippet);
            return;
        }
    }
    snippets_.push_back(std::move(snippet));
}

auto CssSnippetLibrary::remove_snippet(const std::string& name) -> bool
{
    auto iter = std::find_if(snippets_.begin(),
                             snippets_.end(),
                             [&name](const CssSnippet& snip) { return snip.name == name; });
    if (iter == snippets_.end())
    {
        return false;
    }
    snippets_.erase(iter);
    return true;
}

auto CssSnippetLibrary::find_snippet(const std::string& name) const -> const CssSnippet*
{
    for (const auto& snippet : snippets_)
    {
        if (snippet.name == name)
        {
            return &snippet;
        }
    }
    return nullptr;
}

auto CssSnippetLibrary::all_snippets() const -> const std::vector<CssSnippet>&
{
    return snippets_;
}

auto CssSnippetLibrary::snippets_by_category(const std::string& category) const
    -> std::vector<const CssSnippet*>
{
    std::vector<const CssSnippet*> result;
    for (const auto& snippet : snippets_)
    {
        if (snippet.category == category)
        {
            result.push_back(&snippet);
        }
    }
    return result;
}

auto CssSnippetLibrary::snippet_count() const -> std::size_t
{
    return snippets_.size();
}

// ── Application ───────────────────────────────────────────────────────────────

auto CssSnippetLibrary::apply_snippet(const CssSnippet& snippet, const std::string& existing_css)
    -> std::string
{
    std::string merged = existing_css;
    if (!merged.empty() && merged.back() != '\n')
    {
        merged += '\n';
    }
    merged += "\n/* === Snippet: " + snippet.name + " === */\n";
    merged += snippet.css_content;
    merged += "\n/* === End: " + snippet.name + " === */\n";
    return merged;
}

auto CssSnippetLibrary::generate_preview(const CssSnippet& snippet, const std::string& base_css)
    -> std::string
{
    return apply_snippet(snippet, base_css);
}

// ── Export / Import ───────────────────────────────────────────────────────────

auto CssSnippetLibrary::export_library() const -> std::string
{
    std::ostringstream oss;
    oss << R"({"snippets":[)";
    for (std::size_t idx = 0; idx < snippets_.size(); ++idx)
    {
        const auto& snip = snippets_[idx];
        if (idx > 0)
        {
            oss << ",";
        }
        oss << R"({"name":")" << snip.name << R"(","description":")" << snip.description
            << R"(","category":")" << snip.category << R"(","css_content":")" << snip.css_content
            << R"(","is_builtin":)" << (snip.is_builtin ? "true" : "false") << R"(,"variables":[)";
        for (std::size_t var_idx = 0; var_idx < snip.variables.size(); ++var_idx)
        {
            if (var_idx > 0)
            {
                oss << ",";
            }
            oss << R"(")" << snip.variables[var_idx] << R"(")";
        }
        oss << "]}";
    }
    oss << "]}";
    return oss.str();
}

auto CssSnippetLibrary::import_library(const std::string& json_data) -> int
{
    int count = 0;
    // Simple JSON parsing for snippet entries
    std::string::size_type pos = 0;
    while ((pos = json_data.find(R"("name":")", pos)) != std::string::npos)
    {
        CssSnippet snippet;
        pos += 8; // skip past "name":"
        auto end = json_data.find('"', pos);
        if (end == std::string::npos)
        {
            break;
        }
        snippet.name = json_data.substr(pos, end - pos);

        // Find description
        auto desc_pos = json_data.find(R"("description":")", end);
        if (desc_pos != std::string::npos && desc_pos < end + 200)
        {
            desc_pos += 15;
            auto desc_end = json_data.find('"', desc_pos);
            if (desc_end != std::string::npos)
            {
                snippet.description = json_data.substr(desc_pos, desc_end - desc_pos);
            }
        }

        // Find category
        auto cat_pos = json_data.find(R"("category":")", end);
        if (cat_pos != std::string::npos && cat_pos < end + 300)
        {
            cat_pos += 12;
            auto cat_end = json_data.find('"', cat_pos);
            if (cat_end != std::string::npos)
            {
                snippet.category = json_data.substr(cat_pos, cat_end - cat_pos);
            }
        }

        // Find css_content
        auto css_pos = json_data.find(R"("css_content":")", end);
        if (css_pos != std::string::npos && css_pos < end + 500)
        {
            css_pos += 15;
            auto css_end = json_data.find('"', css_pos);
            if (css_end != std::string::npos)
            {
                snippet.css_content = json_data.substr(css_pos, css_end - css_pos);
            }
        }

        if (!snippet.name.empty())
        {
            add_snippet(std::move(snippet));
            ++count;
        }
    }
    return count;
}

// ── Builtins ──────────────────────────────────────────────────────────────────

void CssSnippetLibrary::load_builtins()
{
    {
        CssSnippet snip;
        snip.name = "Dark Headers";
        snip.description = "Dark background with light text for headings";
        snip.category = "typography";
        snip.css_content = "h1, h2, h3 { background: #1a1a2e; color: #e6e6fa; padding: 8px 12px; "
                           "border-radius: 4px; }";
        snip.variables = {"--header-bg", "--header-color"};
        snip.is_builtin = true;
        snippets_.push_back(std::move(snip));
    }
    {
        CssSnippet snip;
        snip.name = "Soft Pastels";
        snip.description = "Soft pastel color palette for backgrounds";
        snip.category = "colors";
        snip.css_content =
            ":root { --bg-primary: #fef9ef; --bg-secondary: #f0e6ef; --accent: #d4a5a5; }";
        snip.variables = {"--bg-primary", "--bg-secondary", "--accent"};
        snip.is_builtin = true;
        snippets_.push_back(std::move(snip));
    }
    {
        CssSnippet snip;
        snip.name = "Comfortable Spacing";
        snip.description = "Increased paragraph spacing for readability";
        snip.category = "spacing";
        snip.css_content = "p { margin-bottom: 1.5em; line-height: 1.8; } blockquote { margin: "
                           "1.5em 0; padding: 0.5em 1em; }";
        snip.variables = {};
        snip.is_builtin = true;
        snippets_.push_back(std::move(snip));
    }
    {
        CssSnippet snip;
        snip.name = "Smooth Transitions";
        snip.description = "Smooth transitions for interactive elements";
        snip.category = "animations";
        snip.css_content = "a, button, .interactive { transition: all 0.3s ease; } a:hover { "
                           "transform: translateY(-1px); }";
        snip.variables = {};
        snip.is_builtin = true;
        snippets_.push_back(std::move(snip));
    }
}

auto CssSnippetLibrary::categories() const -> std::vector<std::string>
{
    std::set<std::string> cats;
    for (const auto& snippet : snippets_)
    {
        if (!snippet.category.empty())
        {
            cats.insert(snippet.category);
        }
    }
    return {cats.begin(), cats.end()};
}

} // namespace markamp::core
