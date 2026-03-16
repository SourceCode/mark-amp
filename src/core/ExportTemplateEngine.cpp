// ============================================================================
// File: src/core/ExportTemplateEngine.cpp
// Phase 24: Export & Publishing — Customizable export template engine
// ============================================================================

#include "ExportTemplateEngine.h"

#include <algorithm>
#include <array>
#include <regex>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Construction
// ============================================================================

ExportTemplateEngine::ExportTemplateEngine() = default;

// ============================================================================
// Template registry
// ============================================================================

auto ExportTemplateEngine::register_template(ExportTemplate tpl) -> bool
{
    if (tpl.template_id.empty() || templates_.contains(tpl.template_id))
    {
        return false;
    }
    auto id = tpl.template_id;
    templates_.emplace(std::move(id), std::move(tpl));
    return true;
}

auto ExportTemplateEngine::get_template(const std::string& template_id) const
    -> const ExportTemplate*
{
    auto iter = templates_.find(template_id);
    return iter != templates_.end() ? &iter->second : nullptr;
}

auto ExportTemplateEngine::list_templates() const -> std::vector<ExportTemplate>
{
    std::vector<ExportTemplate> result;
    result.reserve(templates_.size());
    for (const auto& [id, tpl] : templates_)
    {
        result.push_back(tpl);
    }
    // Sort by name for consistent ordering.
    std::sort(result.begin(),
              result.end(),
              [](const ExportTemplate& lhs, const ExportTemplate& rhs)
              { return lhs.name < rhs.name; });
    return result;
}

auto ExportTemplateEngine::remove_template(const std::string& template_id) -> bool
{
    auto iter = templates_.find(template_id);
    if (iter == templates_.end() || iter->second.is_builtin)
    {
        return false;
    }
    templates_.erase(iter);
    return true;
}

// ============================================================================
// Built-in templates
// ============================================================================

auto ExportTemplateEngine::load_builtin_templates() -> void
{
    static const std::array<BuiltinTemplate, 5> kBuiltins = {
        BuiltinTemplate::kArticle,
        BuiltinTemplate::kReport,
        BuiltinTemplate::kSlides,
        BuiltinTemplate::kResume,
        BuiltinTemplate::kMinimal,
    };

    for (auto bt : kBuiltins)
    {
        ExportTemplate tpl;
        tpl.template_id = builtin_id(bt);
        tpl.is_builtin = true;
        tpl.html_template = builtin_html(bt);
        tpl.css = builtin_css(bt);

        // Common variables for all built-ins.
        tpl.variables = {
            {"title", "Untitled", "Document title", false},
            {"author", "", "Author name", false},
            {"date", "", "Publication date", false},
            {"content", "", "Rendered HTML body (auto-filled)", true},
        };

        switch (bt)
        {
            case BuiltinTemplate::kArticle:
                tpl.name = "Article";
                break;
            case BuiltinTemplate::kReport:
                tpl.name = "Report";
                tpl.variables.push_back({"toc", "", "Table of contents HTML", false});
                break;
            case BuiltinTemplate::kSlides:
                tpl.name = "Slides";
                break;
            case BuiltinTemplate::kResume:
                tpl.name = "Resume";
                tpl.variables.push_back({"email", "", "Contact email", false});
                tpl.variables.push_back({"phone", "", "Contact phone", false});
                break;
            case BuiltinTemplate::kMinimal:
                tpl.name = "Minimal";
                break;
        }

        templates_.emplace(tpl.template_id, std::move(tpl));
    }
}

auto ExportTemplateEngine::builtin_id(BuiltinTemplate bt) -> std::string
{
    switch (bt)
    {
        case BuiltinTemplate::kArticle:
            return "builtin-article";
        case BuiltinTemplate::kReport:
            return "builtin-report";
        case BuiltinTemplate::kSlides:
            return "builtin-slides";
        case BuiltinTemplate::kResume:
            return "builtin-resume";
        case BuiltinTemplate::kMinimal:
            return "builtin-minimal";
    }
    return "builtin-article"; // Fallback.
}

auto ExportTemplateEngine::builtin_template(BuiltinTemplate bt) const -> const ExportTemplate*
{
    return get_template(builtin_id(bt));
}

auto ExportTemplateEngine::builtin_css(BuiltinTemplate bt) -> std::string
{
    switch (bt)
    {
        case BuiltinTemplate::kArticle:
            return R"(body { font-family: Georgia, serif; max-width: 720px; margin: 2em auto; line-height: 1.6; color: #333; }
h1 { font-size: 2em; border-bottom: 2px solid #444; padding-bottom: 0.3em; }
h2 { font-size: 1.5em; margin-top: 1.5em; }
code { background: #f5f5f5; padding: 2px 6px; border-radius: 3px; }
pre { background: #f5f5f5; padding: 1em; overflow-x: auto; border-radius: 4px; }
blockquote { border-left: 4px solid #ccc; margin-left: 0; padding-left: 1em; color: #666; })";

        case BuiltinTemplate::kReport:
            return R"(body { font-family: 'Times New Roman', serif; max-width: 800px; margin: 2em auto; line-height: 1.8; }
.cover-page { text-align: center; padding: 4em 0; page-break-after: always; }
.cover-page h1 { font-size: 2.5em; margin-bottom: 0.5em; }
.cover-page .author { font-size: 1.2em; color: #555; }
.cover-page .date { font-size: 1em; color: #777; margin-top: 1em; }
.toc { page-break-after: always; }
.toc ul { list-style: none; padding-left: 0; }
.toc li { margin: 0.3em 0; }
h2 { page-break-before: always; }
table { border-collapse: collapse; width: 100%; }
th, td { border: 1px solid #ccc; padding: 8px; })";

        case BuiltinTemplate::kSlides:
            return R"(body { font-family: 'Helvetica Neue', Arial, sans-serif; margin: 0; padding: 0; }
.slide { width: 100%; min-height: 100vh; display: flex; flex-direction: column; justify-content: center; padding: 2em 4em; box-sizing: border-box; page-break-after: always; }
.slide h1 { font-size: 2.5em; }
.slide h2 { font-size: 2em; color: #444; }
.slide ul { font-size: 1.3em; line-height: 2; }
.slide code { font-size: 0.9em; })";

        case BuiltinTemplate::kResume:
            return R"(body { font-family: 'Helvetica Neue', Arial, sans-serif; max-width: 700px; margin: 1em auto; line-height: 1.4; color: #222; }
.header { text-align: center; border-bottom: 2px solid #333; padding-bottom: 0.5em; margin-bottom: 1em; }
.header h1 { margin-bottom: 0.2em; }
.contact { font-size: 0.9em; color: #555; }
h2 { font-size: 1.2em; text-transform: uppercase; border-bottom: 1px solid #999; padding-bottom: 0.2em; margin-top: 1.5em; }
ul { padding-left: 1.2em; })";

        case BuiltinTemplate::kMinimal:
            return R"(body { font-family: system-ui, sans-serif; max-width: 680px; margin: 1em auto; line-height: 1.5; })";
    }
    return "";
}

auto ExportTemplateEngine::builtin_html(BuiltinTemplate bt) -> std::string
{
    switch (bt)
    {
        case BuiltinTemplate::kArticle:
            return R"(<!DOCTYPE html>
<html lang="en">
<head><meta charset="utf-8"><title>{{title}}</title><style>{{css}}</style></head>
<body>
<article>
<h1>{{title}}</h1>
<p class="meta">{{author}} &mdash; {{date}}</p>
{{content}}
</article>
</body>
</html>)";

        case BuiltinTemplate::kReport:
            return R"(<!DOCTYPE html>
<html lang="en">
<head><meta charset="utf-8"><title>{{title}}</title><style>{{css}}</style></head>
<body>
<div class="cover-page">
<h1>{{title}}</h1>
<p class="author">{{author}}</p>
<p class="date">{{date}}</p>
</div>
<nav class="toc">
<h2>Table of Contents</h2>
{{toc}}
</nav>
<main>
{{content}}
</main>
</body>
</html>)";

        case BuiltinTemplate::kSlides:
            return R"(<!DOCTYPE html>
<html lang="en">
<head><meta charset="utf-8"><title>{{title}}</title><style>{{css}}</style></head>
<body>
<div class="slide"><h1>{{title}}</h1><p>{{author}}</p><p>{{date}}</p></div>
{{content}}
</body>
</html>)";

        case BuiltinTemplate::kResume:
            return R"(<!DOCTYPE html>
<html lang="en">
<head><meta charset="utf-8"><title>{{title}}</title><style>{{css}}</style></head>
<body>
<div class="header">
<h1>{{title}}</h1>
<p class="contact">{{email}} | {{phone}}</p>
</div>
{{content}}
</body>
</html>)";

        case BuiltinTemplate::kMinimal:
            return R"(<!DOCTYPE html>
<html lang="en">
<head><meta charset="utf-8"><title>{{title}}</title><style>{{css}}</style></head>
<body>
{{content}}
</body>
</html>)";
    }
    return "";
}

// ============================================================================
// Rendering
// ============================================================================

auto ExportTemplateEngine::render(const std::string& template_id,
                                  const std::string& content_html,
                                  const TemplateContext& context) const -> std::string
{
    auto iter = templates_.find(template_id);
    if (iter == templates_.end())
    {
        return "<!-- Error: template '" + template_id + "' not found -->" + content_html;
    }
    return render_template(iter->second, content_html, context);
}

auto ExportTemplateEngine::render_template(const ExportTemplate& tpl,
                                           const std::string& content_html,
                                           const TemplateContext& context) const -> std::string
{
    // Build the full context: start with variable defaults, overlay caller context,
    // then inject css and content.
    TemplateContext full_ctx;
    for (const auto& var : tpl.variables)
    {
        if (!var.default_value.empty())
        {
            full_ctx[var.name] = var.default_value;
        }
    }
    for (const auto& [key, value] : context)
    {
        full_ctx[key] = value;
    }
    full_ctx["content"] = content_html;
    full_ctx["css"] = tpl.css;

    return substitute(tpl.html_template, full_ctx);
}

auto ExportTemplateEngine::substitute(const std::string& text, const TemplateContext& context)
    -> std::string
{
    // Replace all {{key}} occurrences with their values.
    static const std::regex kVarPattern(R"(\{\{(\w+)\}\})");

    std::string result;
    result.reserve(text.size());

    auto begin = std::sregex_iterator(text.begin(), text.end(), kVarPattern);
    auto end = std::sregex_iterator();

    size_t last_pos = 0;
    for (auto iter = begin; iter != end; ++iter)
    {
        const auto& match = *iter;
        auto match_start = static_cast<size_t>(match.position());

        // Append text before this match.
        result.append(text, last_pos, match_start - last_pos);

        // Lookup the variable.
        const std::string var_name = match[1].str();
        auto ctx_iter = context.find(var_name);
        if (ctx_iter != context.end())
        {
            result.append(ctx_iter->second);
        }
        else
        {
            // Variable not found — insert a visible commented placeholder
            // so the user can see what was missing, rather than silently removing it.
            result.append("<!-- MISSING: {{" + var_name + "}} -->");
        }

        last_pos = match_start + static_cast<size_t>(match.length());
    }
    result.append(text, last_pos);
    return result;
}

// ============================================================================
// Validation
// ============================================================================

auto ExportTemplateEngine::validate_template(const ExportTemplate& tpl) const
    -> TemplateValidationResult
{
    TemplateValidationResult result;

    if (tpl.template_id.empty())
    {
        result.valid = false;
        result.errors.emplace_back("Template id is empty");
    }
    if (tpl.html_template.empty())
    {
        result.valid = false;
        result.errors.emplace_back("HTML template body is empty");
    }

    // Check for unresolved variables — variables referenced in the template
    // that are not declared in tpl.variables and are not the auto-injected
    // 'content' or 'css'.
    auto referenced = extract_variables(tpl.html_template);

    for (const auto& var_name : referenced)
    {
        if (var_name == "content" || var_name == "css")
        {
            continue; // Auto-injected.
        }
        bool declared = false;
        for (const auto& var : tpl.variables)
        {
            if (var.name == var_name)
            {
                declared = true;
                break;
            }
        }
        if (!declared)
        {
            result.unresolved_variables.push_back(var_name);
        }
    }

    if (!result.unresolved_variables.empty())
    {
        result.valid = false;
        for (const auto& name : result.unresolved_variables)
        {
            result.errors.push_back("Unresolved variable: {{" + name + "}}");
        }
    }

    return result;
}

auto ExportTemplateEngine::extract_variables(const std::string& html_template)
    -> std::vector<std::string>
{
    static const std::regex kVarPattern(R"(\{\{(\w+)\}\})");

    std::vector<std::string> vars;
    auto begin = std::sregex_iterator(html_template.begin(), html_template.end(), kVarPattern);
    auto end = std::sregex_iterator();

    for (auto iter = begin; iter != end; ++iter)
    {
        const std::string name = (*iter)[1].str();
        // Deduplicate.
        if (std::find(vars.begin(), vars.end(), name) == vars.end())
        {
            vars.push_back(name);
        }
    }
    return vars;
}

// (#96) Return the number of registered templates.
auto ExportTemplateEngine::template_count() const -> std::size_t
{
    return templates_.size();
}

} // namespace markamp::core
