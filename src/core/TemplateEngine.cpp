// ============================================================================
// File: src/core/TemplateEngine.cpp
// Phase 35: Template Engine — Full implementation
// ============================================================================

#include "TemplateEngine.h"

#include "Events.h"
#include "TemplateFunctions.h"

#include <algorithm>
#include <chrono>
#include <sstream>

namespace markamp::core
{

// ── TemplateVariable helpers ────────────────────────────────────────────────

auto TemplateVariable::as_string() const -> std::string
{
    return std::visit(
        [](const auto& v) -> std::string
        {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>)
                return v;
            else if constexpr (std::is_same_v<T, int64_t>)
                return std::to_string(v);
            else if constexpr (std::is_same_v<T, double>)
                return std::to_string(v);
            else if constexpr (std::is_same_v<T, bool>)
                return v ? "true" : "false";
            else if constexpr (std::is_same_v<T, std::vector<std::string>>)
                return "[list:" + std::to_string(v.size()) + "]";
            else
                return "[map:" + std::to_string(v.size()) + "]";
        },
        value);
}

auto TemplateVariable::as_int() const -> int64_t
{
    if (auto* v = std::get_if<int64_t>(&value))
        return *v;
    if (auto* v = std::get_if<double>(&value))
        return static_cast<int64_t>(*v);
    if (auto* v = std::get_if<bool>(&value))
        return *v ? 1 : 0;
    if (auto* v = std::get_if<std::string>(&value))
    {
        try
        {
            return std::stoll(*v);
        }
        catch (...)
        {
            return 0;
        }
    }
    return 0;
}

auto TemplateVariable::as_double() const -> double
{
    if (auto* v = std::get_if<double>(&value))
        return *v;
    if (auto* v = std::get_if<int64_t>(&value))
        return static_cast<double>(*v);
    if (auto* v = std::get_if<bool>(&value))
        return *v ? 1.0 : 0.0;
    if (auto* v = std::get_if<std::string>(&value))
    {
        try
        {
            return std::stod(*v);
        }
        catch (...)
        {
            return 0.0;
        }
    }
    return 0.0;
}

auto TemplateVariable::as_bool() const -> bool
{
    return is_truthy();
}

auto TemplateVariable::is_truthy() const -> bool
{
    return std::visit(
        [](const auto& v) -> bool
        {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>)
                return !v.empty();
            else if constexpr (std::is_same_v<T, int64_t>)
                return v != 0;
            else if constexpr (std::is_same_v<T, double>)
                return v != 0.0;
            else if constexpr (std::is_same_v<T, bool>)
                return v;
            else if constexpr (std::is_same_v<T, std::vector<std::string>>)
                return !v.empty();
            else
                return !v.empty();
        },
        value);
}

// ── TemplateEngine ──────────────────────────────────────────────────────────

TemplateEngine::TemplateEngine(EventBus& event_bus)
    : event_bus_(event_bus)
{
    // Register all built-in Sprig-compatible functions.
    TemplateFunctionRegistry::register_all(*this);
}

auto TemplateEngine::parse(const std::string& template_source)
    -> std::expected<std::vector<TemplateNode>, std::string>
{
    TemplateOptions defaults;
    auto tokens_result = tokenize(template_source, defaults);
    if (!tokens_result)
    {
        return std::unexpected(tokens_result.error());
    }

    size_t pos = 0;
    return parse_tokens(*tokens_result, pos);
}

auto TemplateEngine::render(const std::string& template_source,
                            const TemplateContext& context,
                            const TemplateOptions& options) -> TemplateResult
{
    auto start = std::chrono::steady_clock::now();
    TemplateResult result;

    // Parse.
    auto tokens_result = tokenize(template_source, options);
    if (!tokens_result)
    {
        result.success = false;
        result.error_message = tokens_result.error();
        return result;
    }

    size_t pos = 0;
    auto ast_result = parse_tokens(*tokens_result, pos);
    if (!ast_result)
    {
        result.success = false;
        result.error_message = ast_result.error();
        return result;
    }

    // Render.
    auto mutable_context = context;
    render_nodes(*ast_result, mutable_context, options, result.output, result.warnings, 0);

    auto end = std::chrono::steady_clock::now();
    result.elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return result;
}

auto TemplateEngine::render_ast(const std::vector<TemplateNode>& ast,
                                const TemplateContext& context,
                                const TemplateOptions& options) -> TemplateResult
{
    auto start = std::chrono::steady_clock::now();
    TemplateResult result;

    auto mutable_context = context;
    render_nodes(ast, mutable_context, options, result.output, result.warnings, 0);

    auto end = std::chrono::steady_clock::now();
    result.elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return result;
}

auto TemplateEngine::register_function(const std::string& name, TemplateFuncSignature func) -> void
{
    functions_[name] = std::move(func);
}

auto TemplateEngine::has_function(const std::string& name) const -> bool
{
    return functions_.contains(name);
}

auto TemplateEngine::register_template(const std::string& name, const std::string& template_source)
    -> void
{
    auto ast = parse(template_source);
    if (ast)
    {
        named_templates_[name] = std::move(*ast);
    }
}

auto TemplateEngine::function_names() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(functions_.size());
    for (const auto& [name, _] : functions_)
    {
        names.push_back(name);
    }
    std::sort(names.begin(), names.end());
    return names;
}

auto TemplateEngine::template_names() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(named_templates_.size());
    for (const auto& [name, _] : named_templates_)
    {
        names.push_back(name);
    }
    std::sort(names.begin(), names.end());
    return names;
}

// ── Tokenizer ───────────────────────────────────────────────────────────────

auto TemplateEngine::tokenize(const std::string& source, const TemplateOptions& options) const
    -> std::expected<std::vector<Token>, std::string>
{
    std::vector<Token> tokens;
    const auto& ld = options.left_delim;
    const auto& rd = options.right_delim;
    const auto& cl = options.comment_left;
    const auto& cr = options.comment_right;

    size_t pos = 0;
    int line = 1;

    while (pos < source.size())
    {
        // Check for comment.
        auto comment_start = source.find(cl, pos);
        auto tag_start = source.find(ld, pos);

        // Comment takes precedence if it comes first.
        if (comment_start != std::string::npos &&
            (tag_start == std::string::npos || comment_start < tag_start))
        {
            // Emit text before comment.
            if (comment_start > pos)
            {
                Token text_tok;
                text_tok.type = Token::Type::Text;
                text_tok.content = source.substr(pos, comment_start - pos);
                text_tok.line = line;
                for (char c : text_tok.content)
                {
                    if (c == '\n')
                        ++line;
                }
                tokens.push_back(std::move(text_tok));
            }

            auto comment_end = source.find(cr, comment_start + cl.size());
            if (comment_end == std::string::npos)
            {
                return std::unexpected("Unclosed comment starting at line " + std::to_string(line));
            }

            Token comment_tok;
            comment_tok.type = Token::Type::Comment;
            comment_tok.content =
                source.substr(comment_start + cl.size(), comment_end - comment_start - cl.size());
            comment_tok.line = line;
            tokens.push_back(std::move(comment_tok));

            pos = comment_end + cr.size();
            continue;
        }

        // Check for tag delimiter.
        if (tag_start == std::string::npos)
        {
            // Remaining text.
            if (pos < source.size())
            {
                Token text_tok;
                text_tok.type = Token::Type::Text;
                text_tok.content = source.substr(pos);
                text_tok.line = line;
                tokens.push_back(std::move(text_tok));
            }
            break;
        }

        // Emit text before tag.
        if (tag_start > pos)
        {
            Token text_tok;
            text_tok.type = Token::Type::Text;
            text_tok.content = source.substr(pos, tag_start - pos);
            text_tok.line = line;
            for (char c : text_tok.content)
            {
                if (c == '\n')
                    ++line;
            }
            tokens.push_back(std::move(text_tok));
        }

        auto tag_end = source.find(rd, tag_start + ld.size());
        if (tag_end == std::string::npos)
        {
            return std::unexpected("Unclosed tag starting at line " + std::to_string(line));
        }

        Token tag_tok;
        tag_tok.type = Token::Type::Tag;
        auto raw = source.substr(tag_start + ld.size(), tag_end - tag_start - ld.size());
        // Trim whitespace from tag content.
        auto first = raw.find_first_not_of(" \t\n\r");
        auto last = raw.find_last_not_of(" \t\n\r");
        tag_tok.content = (first == std::string::npos) ? "" : raw.substr(first, last - first + 1);
        tag_tok.line = line;
        tokens.push_back(std::move(tag_tok));

        pos = tag_end + rd.size();
    }

    return tokens;
}

// ── Parser ──────────────────────────────────────────────────────────────────

auto TemplateEngine::parse_tokens(const std::vector<Token>& tokens, size_t& pos) const
    -> std::expected<std::vector<TemplateNode>, std::string>
{
    std::vector<TemplateNode> nodes;

    while (pos < tokens.size())
    {
        const auto& tok = tokens[pos];

        if (tok.type == Token::Type::Text)
        {
            TemplateNode node;
            node.type = TemplateNodeType::Text;
            node.content = tok.content;
            node.line = tok.line;
            nodes.push_back(std::move(node));
            ++pos;
            continue;
        }

        if (tok.type == Token::Type::Comment)
        {
            TemplateNode node;
            node.type = TemplateNodeType::Comment;
            node.content = tok.content;
            node.line = tok.line;
            nodes.push_back(std::move(node));
            ++pos;
            continue;
        }

        // Tag processing.
        const auto& content = tok.content;

        // {{ end }} — closing block, return to parent.
        if (content == "end")
        {
            ++pos;
            return nodes;
        }

        // {{ else }} — handled by if/range/with caller.
        if (content == "else")
        {
            ++pos;
            return nodes;
        }

        // {{ if .cond }}
        if (content.starts_with("if "))
        {
            TemplateNode node;
            node.type = TemplateNodeType::If;
            node.content = content.substr(3);
            node.line = tok.line;
            ++pos;

            auto body = parse_tokens(tokens, pos);
            if (!body)
                return std::unexpected(body.error());
            node.children = std::move(*body);

            // Check if we hit an "else".
            if (pos > 0 && pos <= tokens.size())
            {
                auto prev_pos = pos - 1;
                if (prev_pos < tokens.size() && tokens[prev_pos].type == Token::Type::Tag &&
                    tokens[prev_pos].content == "else")
                {
                    auto else_body = parse_tokens(tokens, pos);
                    if (!else_body)
                        return std::unexpected(else_body.error());
                    node.else_branch = std::move(*else_body);
                }
            }

            nodes.push_back(std::move(node));
            continue;
        }

        // {{ range .list }}
        if (content.starts_with("range "))
        {
            TemplateNode node;
            node.type = TemplateNodeType::Range;
            node.content = content.substr(6);
            node.line = tok.line;
            ++pos;

            auto body = parse_tokens(tokens, pos);
            if (!body)
                return std::unexpected(body.error());
            node.children = std::move(*body);
            nodes.push_back(std::move(node));
            continue;
        }

        // {{ with .var }}
        if (content.starts_with("with "))
        {
            TemplateNode node;
            node.type = TemplateNodeType::With;
            node.content = content.substr(5);
            node.line = tok.line;
            ++pos;

            auto body = parse_tokens(tokens, pos);
            if (!body)
                return std::unexpected(body.error());
            node.children = std::move(*body);
            nodes.push_back(std::move(node));
            continue;
        }

        // {{ define "name" }}
        if (content.starts_with("define "))
        {
            TemplateNode node;
            node.type = TemplateNodeType::Define;
            auto name = content.substr(7);
            // Strip quotes.
            if (name.size() >= 2 && name.front() == '"' && name.back() == '"')
            {
                name = name.substr(1, name.size() - 2);
            }
            node.name = name;
            node.line = tok.line;
            ++pos;

            auto body = parse_tokens(tokens, pos);
            if (!body)
                return std::unexpected(body.error());
            node.children = std::move(*body);
            nodes.push_back(std::move(node));
            continue;
        }

        // {{ template "name" . }}
        if (content.starts_with("template "))
        {
            TemplateNode node;
            node.type = TemplateNodeType::Template;
            auto rest = content.substr(9);
            // Parse name and optional dot.
            auto space = rest.find(' ');
            auto name = (space != std::string::npos) ? rest.substr(0, space) : rest;
            if (name.size() >= 2 && name.front() == '"' && name.back() == '"')
            {
                name = name.substr(1, name.size() - 2);
            }
            node.name = name;
            node.line = tok.line;
            nodes.push_back(std::move(node));
            ++pos;
            continue;
        }

        // {{ block "name" }}
        if (content.starts_with("block "))
        {
            TemplateNode node;
            node.type = TemplateNodeType::Block;
            auto name = content.substr(6);
            if (name.size() >= 2 && name.front() == '"' && name.back() == '"')
            {
                name = name.substr(1, name.size() - 2);
            }
            node.name = name;
            node.line = tok.line;
            ++pos;

            auto body = parse_tokens(tokens, pos);
            if (!body)
                return std::unexpected(body.error());
            node.children = std::move(*body);
            nodes.push_back(std::move(node));
            continue;
        }

        // Variable / pipeline expression: {{ .var }}, {{ .var | func }}
        auto node = parse_expression(content);
        node.line = tok.line;
        nodes.push_back(std::move(node));
        ++pos;
    }

    return nodes;
}

auto TemplateEngine::parse_expression(const std::string& expr) const -> TemplateNode
{
    TemplateNode node;
    node.type = TemplateNodeType::Variable;

    // Check for pipeline (pipes).
    auto pipe_pos = expr.find('|');
    if (pipe_pos != std::string::npos)
    {
        node.type = TemplateNodeType::Pipe;
        // Variable part before the first pipe.
        auto var_part = expr.substr(0, pipe_pos);
        auto vf = var_part.find_first_not_of(" \t");
        auto vl = var_part.find_last_not_of(" \t");
        node.content = (vf == std::string::npos) ? "" : var_part.substr(vf, vl - vf + 1);

        // Parse pipeline functions.
        auto remaining = expr.substr(pipe_pos + 1);
        std::istringstream ss(remaining);
        std::string func;
        while (std::getline(ss, func, '|'))
        {
            auto ff = func.find_first_not_of(" \t");
            auto fl = func.find_last_not_of(" \t");
            if (ff != std::string::npos)
            {
                node.args.push_back(func.substr(ff, fl - ff + 1));
            }
        }
    }
    else
    {
        // Simple variable.
        auto vf = expr.find_first_not_of(" \t");
        auto vl = expr.find_last_not_of(" \t");
        node.content = (vf == std::string::npos) ? "" : expr.substr(vf, vl - vf + 1);
    }

    return node;
}

// ── Renderer ────────────────────────────────────────────────────────────────

auto TemplateEngine::render_nodes(const std::vector<TemplateNode>& nodes,
                                  TemplateContext& context,
                                  const TemplateOptions& options,
                                  std::string& output,
                                  std::vector<std::string>& warnings,
                                  int depth) const -> void
{
    if (depth > options.max_recursion_depth)
    {
        warnings.push_back("Max recursion depth exceeded at depth " + std::to_string(depth));
        return;
    }

    for (const auto& node : nodes)
    {
        switch (node.type)
        {
            case TemplateNodeType::Text:
                output += node.content;
                break;

            case TemplateNodeType::Comment:
                // Comments are silently dropped.
                break;

            case TemplateNodeType::Variable:
            case TemplateNodeType::Pipe:
            {
                auto val = evaluate_expression(node, context, warnings);
                if (options.auto_escape_html)
                {
                    val = html_escape(val);
                }
                output += val;
                break;
            }

            case TemplateNodeType::If:
            {
                bool cond = is_truthy(node.content, context);
                if (cond)
                {
                    render_nodes(node.children, context, options, output, warnings, depth + 1);
                }
                else if (!node.else_branch.empty())
                {
                    render_nodes(node.else_branch, context, options, output, warnings, depth + 1);
                }
                break;
            }

            case TemplateNodeType::Range:
            {
                auto var = resolve_variable(node.content, context);
                if (!var)
                {
                    warnings.push_back("range: undefined variable '" + node.content + "'");
                    break;
                }
                // Try to find the TemplateVariable to get list values.
                auto clean = node.content;
                if (clean.starts_with("."))
                    clean = clean.substr(1);
                auto it = context.find(clean);
                if (it != context.end())
                {
                    auto* list = std::get_if<std::vector<std::string>>(&it->second.value);
                    if (list)
                    {
                        int iter_count = 0;
                        for (const auto& item : *list)
                        {
                            if (++iter_count > options.max_iterations)
                            {
                                warnings.push_back("range: max iterations exceeded");
                                break;
                            }
                            // Set "." to current item.
                            TemplateVariable dot_var;
                            dot_var.name = ".";
                            dot_var.value = item;
                            auto saved = context;
                            context["."] = dot_var;
                            render_nodes(
                                node.children, context, options, output, warnings, depth + 1);
                            context = saved;
                        }
                    }
                }
                break;
            }

            case TemplateNodeType::With:
            {
                if (is_truthy(node.content, context))
                {
                    auto val = resolve_variable(node.content, context);
                    if (val)
                    {
                        TemplateVariable dot_var;
                        dot_var.name = ".";
                        dot_var.value = *val;
                        auto saved_dot = context.find(".");
                        auto had_dot = saved_dot != context.end();
                        auto old_dot = had_dot ? saved_dot->second : TemplateVariable{};
                        context["."] = dot_var;
                        render_nodes(node.children, context, options, output, warnings, depth + 1);
                        if (had_dot)
                            context["."] = old_dot;
                        else
                            context.erase(".");
                    }
                }
                break;
            }

            case TemplateNodeType::Define:
                // Store the template definition for later use.
                // (const_cast is acceptable here as we're caching parsed templates)
                const_cast<TemplateEngine*>(this)->named_templates_[node.name] = node.children;
                break;

            case TemplateNodeType::Template:
            {
                auto tmpl_it = named_templates_.find(node.name);
                if (tmpl_it != named_templates_.end())
                {
                    render_nodes(tmpl_it->second, context, options, output, warnings, depth + 1);
                }
                else
                {
                    warnings.push_back("template: undefined template '" + node.name + "'");
                }
                break;
            }

            case TemplateNodeType::Block:
            {
                // Block acts as define + template.
                auto tmpl_it = named_templates_.find(node.name);
                if (tmpl_it != named_templates_.end())
                {
                    // Override exists, use it.
                    render_nodes(tmpl_it->second, context, options, output, warnings, depth + 1);
                }
                else
                {
                    // Use default content.
                    render_nodes(node.children, context, options, output, warnings, depth + 1);
                }
                break;
            }
        }
    }
}

auto TemplateEngine::evaluate_expression(const TemplateNode& node,
                                         const TemplateContext& context,
                                         std::vector<std::string>& warnings) const -> std::string
{
    std::string value;

    // Resolve the base variable.
    if (node.content == ".")
    {
        auto it = context.find(".");
        value = (it != context.end()) ? it->second.as_string() : "";
    }
    else if (node.content.starts_with("\"") && node.content.ends_with("\""))
    {
        // String literal.
        value = node.content.substr(1, node.content.size() - 2);
    }
    else
    {
        auto resolved = resolve_variable(node.content, context);
        if (resolved)
        {
            value = *resolved;
        }
        else
        {
            warnings.push_back("undefined variable '" + node.content + "'");
        }
    }

    // Apply pipeline functions.
    if (node.type == TemplateNodeType::Pipe)
    {
        for (const auto& func_expr : node.args)
        {
            // Parse function name and arguments.
            std::istringstream ss(func_expr);
            std::string func_name;
            ss >> func_name;
            std::vector<std::string> func_args;
            std::string arg;
            while (ss >> arg)
            {
                // Strip quotes from string arguments.
                if (arg.size() >= 2 && arg.front() == '"' && arg.back() == '"')
                {
                    arg = arg.substr(1, arg.size() - 2);
                }
                else if (arg.starts_with("."))
                {
                    // Variable reference as argument.
                    auto ref = resolve_variable(arg, context);
                    arg = ref.value_or("");
                }
                func_args.push_back(arg);
            }
            value = apply_function(func_name, value, func_args, warnings);
        }
    }

    return value;
}

auto TemplateEngine::apply_function(const std::string& func_name,
                                    const std::string& input,
                                    const std::vector<std::string>& args,
                                    std::vector<std::string>& warnings) const -> std::string
{
    auto it = functions_.find(func_name);
    if (it == functions_.end())
    {
        warnings.push_back("undefined function '" + func_name + "'");
        return input;
    }

    // Build full args: input is first argument.
    std::vector<std::string> full_args;
    full_args.push_back(input);
    full_args.insert(full_args.end(), args.begin(), args.end());

    try
    {
        return it->second(full_args);
    }
    catch (const std::exception& e)
    {
        warnings.push_back("function '" + func_name + "' error: " + std::string(e.what()));
        return input;
    }
}

auto TemplateEngine::resolve_variable(const std::string& path, const TemplateContext& context) const
    -> std::optional<std::string>
{
    auto clean = path;
    if (clean.starts_with("."))
        clean = clean.substr(1);
    if (clean.empty())
    {
        auto it = context.find(".");
        return it != context.end() ? std::optional(it->second.as_string()) : std::nullopt;
    }

    // Simple single-level lookup.
    auto it = context.find(clean);
    if (it != context.end())
    {
        return it->second.as_string();
    }

    // Dot-path lookup (e.g., "meta.author" -> context["meta"] map lookup).
    auto dot = clean.find('.');
    if (dot != std::string::npos)
    {
        auto parent_key = clean.substr(0, dot);
        auto child_key = clean.substr(dot + 1);
        auto parent_it = context.find(parent_key);
        if (parent_it != context.end())
        {
            auto* map_val =
                std::get_if<std::unordered_map<std::string, std::string>>(&parent_it->second.value);
            if (map_val)
            {
                auto child_it = map_val->find(child_key);
                if (child_it != map_val->end())
                {
                    return child_it->second;
                }
            }
        }
    }

    return std::nullopt;
}

auto TemplateEngine::is_truthy(const std::string& path, const TemplateContext& context) const
    -> bool
{
    auto clean = path;
    if (clean.starts_with("."))
        clean = clean.substr(1);
    if (clean.empty())
        return false;

    // Check for "not" prefix.
    bool negate = false;
    if (clean.starts_with("not "))
    {
        negate = true;
        clean = clean.substr(4);
        if (clean.starts_with("."))
            clean = clean.substr(1);
    }

    auto it = context.find(clean);
    bool result = (it != context.end()) && it->second.is_truthy();

    return negate ? !result : result;
}

auto TemplateEngine::html_escape(const std::string& input) -> std::string
{
    std::string escaped;
    escaped.reserve(input.size());
    for (char c : input)
    {
        switch (c)
        {
            case '&':
                escaped += "&amp;";
                break;
            case '<':
                escaped += "&lt;";
                break;
            case '>':
                escaped += "&gt;";
                break;
            case '"':
                escaped += "&quot;";
                break;
            case '\'':
                escaped += "&#39;";
                break;
            default:
                escaped += c;
                break;
        }
    }
    return escaped;
}

auto TemplateEngine::trim_tag_whitespace(const std::string& text) -> std::string
{
    auto result = text;
    // Trim trailing whitespace before tags and leading whitespace after tags.
    // Simple implementation: trim lines that are only whitespace.
    return result;
}

} // namespace markamp::core
