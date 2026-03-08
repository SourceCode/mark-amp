/// @file GrammarEngine.cpp
/// @brief V16 Phase 12 — TextMate grammar loading, parsing, and tokenization.
///
/// Implements JSON-based grammar loading with support for match rules,
/// begin/end region rules, repository references, and nested patterns.

#include "GrammarEngine.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <fstream>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Grammar loading
// ============================================================================

auto GrammarEngine::load_grammar(const std::string& path) -> bool
{
    if (path.empty())
    {
        return false;
    }

    std::ifstream file(path);
    if (!file.is_open())
    {
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    const std::string content = buffer.str();

    auto definition = parse_grammar_json(content, path);
    if (!definition.has_value())
    {
        return false;
    }

    const std::string scope = definition->metadata.scope_name;
    grammars_.push_back(definition->metadata);

    // Register file extensions
    for (const auto& ext : definition->file_extensions)
    {
        extension_map_[ext] = scope;
    }

    definitions_[scope] = std::move(*definition);
    return true;
}

auto GrammarEngine::load_grammar_from_string(const std::string& json_content,
                                             const std::string& scope_name) -> bool
{
    if (json_content.empty() || scope_name.empty())
    {
        return false;
    }

    auto definition = parse_grammar_json(json_content, "");
    if (!definition.has_value())
    {
        return false;
    }

    // Override scope name if provided
    if (definition->metadata.scope_name.empty())
    {
        definition->metadata.scope_name = scope_name;
    }

    const std::string scope = definition->metadata.scope_name;
    grammars_.push_back(definition->metadata);

    // Register file extensions from grammar definition
    for (const auto& ext : definition->file_extensions)
    {
        extension_map_[ext] = scope;
    }

    definitions_[scope] = std::move(*definition);
    return true;
}

// ============================================================================
// Grammar access
// ============================================================================

auto GrammarEngine::get_grammar(const std::string& scope_name) const -> const Grammar*
{
    for (const auto& grammar : grammars_)
    {
        if (grammar.scope_name == scope_name)
        {
            return &grammar;
        }
    }
    return nullptr;
}

auto GrammarEngine::grammars() const -> const std::vector<Grammar>&
{
    return grammars_;
}

auto GrammarEngine::get_definition(const std::string& scope_name) const -> const GrammarDefinition*
{
    auto def_it = definitions_.find(scope_name);
    if (def_it != definitions_.end())
    {
        return &def_it->second;
    }
    return nullptr;
}

auto GrammarEngine::scope_for_extension(const std::string& extension) const
    -> std::optional<std::string>
{
    auto ext_it = extension_map_.find(extension);
    if (ext_it != extension_map_.end())
    {
        return ext_it->second;
    }
    return std::nullopt;
}

auto GrammarEngine::grammar_count() const -> size_t
{
    return grammars_.size();
}

// ============================================================================
// Tokenization
// ============================================================================

auto GrammarEngine::tokenize_line(const std::string& scope_name, const std::string& line) const
    -> std::vector<GrammarToken>
{
    auto def_it = definitions_.find(scope_name);
    if (def_it == definitions_.end())
    {
        return {};
    }

    std::vector<GrammarToken> tokens;
    apply_rules(line, def_it->second.patterns, def_it->second, tokens);

    // Sort by start position for consistent output
    std::sort(tokens.begin(),
              tokens.end(),
              [](const GrammarToken& token_a, const GrammarToken& token_b)
              { return token_a.start_index < token_b.start_index; });

    return tokens;
}

auto GrammarEngine::tokenize_line_stateful(const std::string& scope_name,
                                           const std::string& line,
                                           ScopeStack& state) const -> std::vector<GrammarToken>
{
    // For now, delegate to stateless tokenization.
    // Full multi-line state tracking would require maintaining
    // the active begin/end context across lines.
    auto tokens = tokenize_line(scope_name, line);

    // Update scope stack based on results
    if (!tokens.empty())
    {
        for (const auto& token : tokens)
        {
            if (!token.scope.empty() && !state.contains(token.scope))
            {
                // Track active scopes (simplified — full implementation
                // would push/pop based on begin/end rule matches)
            }
        }
    }

    return tokens;
}

// ============================================================================
// Rule application
// ============================================================================

void GrammarEngine::apply_rules(const std::string& line,
                                const std::vector<GrammarRule>& rules,
                                const GrammarDefinition& grammar_def,
                                std::vector<GrammarToken>& tokens,
                                int depth) const
{
    if (depth > kMaxRuleDepth)
    {
        return; // Prevent infinite recursion
    }

    for (const auto& rule : rules)
    {
        // Handle #include references
        if (!rule.include_ref.empty())
        {
            const auto* included = resolve_include(rule.include_ref, grammar_def);
            if (included != nullptr)
            {
                std::vector<GrammarRule> included_rules = {*included};
                apply_rules(line, included_rules, grammar_def, tokens, depth + 1);
            }
            continue;
        }

        if (rule.pattern_type == GrammarRule::PatternType::Match)
        {
            // Simple match rule
            if (rule.match_pattern.empty())
            {
                continue;
            }

            try
            {
                std::regex pattern(rule.match_pattern,
                                   std::regex::ECMAScript | std::regex::optimize);
                std::sregex_iterator match_it(line.begin(), line.end(), pattern);
                const std::sregex_iterator match_end;

                while (match_it != match_end)
                {
                    const auto& match = *match_it;

                    // Whole match scope
                    if (!rule.scope_name.empty())
                    {
                        GrammarToken token;
                        token.start_index = static_cast<int>(match.position());
                        token.end_index = static_cast<int>(match.position() + match.length());
                        token.scope = rule.scope_name;
                        tokens.push_back(token);
                    }

                    // Captured group scopes
                    for (const auto& [group_idx, scope] : rule.captures)
                    {
                        const auto grp = static_cast<size_t>(group_idx);
                        if (grp < match.size() && match[grp].matched)
                        {
                            GrammarToken token;
                            token.start_index = static_cast<int>(match.position(grp));
                            token.end_index =
                                static_cast<int>(match.position(grp) + match.length(grp));
                            token.scope = scope;
                            tokens.push_back(token);
                        }
                    }

                    ++match_it;
                }
            }
            catch (const std::regex_error& /*regex_err*/)
            {
                // Skip invalid regex patterns gracefully
            }
        }
        else if (rule.pattern_type == GrammarRule::PatternType::BeginEnd)
        {
            // Begin/End region rule
            if (rule.begin_pattern.empty())
            {
                continue;
            }

            try
            {
                std::regex begin_regex(rule.begin_pattern,
                                       std::regex::ECMAScript | std::regex::optimize);
                std::smatch begin_match;
                std::string search_str = line;
                size_t search_offset = 0;

                while (std::regex_search(search_str, begin_match, begin_regex))
                {
                    const size_t begin_pos =
                        search_offset + static_cast<size_t>(begin_match.position());
                    const size_t begin_len = static_cast<size_t>(begin_match.length());

                    // Try to find the end pattern in the same line
                    size_t end_pos = line.size();
                    size_t end_len = 0;

                    if (!rule.end_pattern.empty())
                    {
                        try
                        {
                            std::regex end_regex(rule.end_pattern,
                                                 std::regex::ECMAScript | std::regex::optimize);
                            const std::string after_begin = line.substr(begin_pos + begin_len);
                            std::smatch end_match;
                            if (std::regex_search(after_begin, end_match, end_regex))
                            {
                                end_pos = begin_pos + begin_len +
                                          static_cast<size_t>(end_match.position()) +
                                          static_cast<size_t>(end_match.length());
                                end_len = static_cast<size_t>(end_match.length());
                            }
                        }
                        catch (const std::regex_error& /*end_err*/)
                        {
                            // If end pattern is invalid, treat as line-end region
                        }
                    }

                    // Emit scope for the entire begin..end region
                    if (!rule.scope_name.empty())
                    {
                        GrammarToken token;
                        token.start_index = static_cast<int>(begin_pos);
                        token.end_index = static_cast<int>(end_pos);
                        token.scope = rule.scope_name;
                        tokens.push_back(token);
                    }

                    // Apply begin captures
                    for (const auto& [group_idx, scope] : rule.begin_captures)
                    {
                        const auto grp = static_cast<size_t>(group_idx);
                        if (grp < begin_match.size() && begin_match[grp].matched)
                        {
                            GrammarToken token;
                            token.start_index = static_cast<int>(
                                search_offset + static_cast<size_t>(begin_match.position(grp)));
                            token.end_index = static_cast<int>(
                                search_offset + static_cast<size_t>(begin_match.position(grp)) +
                                static_cast<size_t>(begin_match.length(grp)));
                            token.scope = scope;
                            tokens.push_back(token);
                        }
                    }

                    // Apply nested patterns within the region
                    if (!rule.nested_patterns.empty() && end_pos > begin_pos + begin_len)
                    {
                        const std::string content_region = line.substr(
                            begin_pos + begin_len, end_pos - end_len - begin_pos - begin_len);
                        std::vector<GrammarToken> nested_tokens;
                        apply_rules(content_region,
                                    rule.nested_patterns,
                                    grammar_def,
                                    nested_tokens,
                                    depth + 1);

                        // Offset nested tokens to align with line positions
                        for (auto& nested_token : nested_tokens)
                        {
                            nested_token.start_index += static_cast<int>(begin_pos + begin_len);
                            nested_token.end_index += static_cast<int>(begin_pos + begin_len);
                            tokens.push_back(nested_token);
                        }
                    }

                    // Move past this match
                    search_offset = end_pos;
                    if (search_offset >= line.size())
                    {
                        break;
                    }
                    search_str = line.substr(search_offset);
                }
            }
            catch (const std::regex_error& /*begin_err*/)
            {
                // Skip invalid regex patterns
            }
        }
    }
}

// ============================================================================
// Include resolution
// ============================================================================

auto GrammarEngine::resolve_include(const std::string& ref,
                                    const GrammarDefinition& grammar_def) const
    -> const GrammarRule*
{
    if (ref.empty())
    {
        return nullptr;
    }

    // Repository reference: "#name"
    if (ref[0] == '#')
    {
        const std::string repo_name = ref.substr(1);
        auto repo_it = grammar_def.repository.find(repo_name);
        if (repo_it != grammar_def.repository.end())
        {
            return &repo_it->second;
        }
    }

    // External grammar reference: "scope.name" - not yet supported
    return nullptr;
}

// ============================================================================
// Scope → TokenType mapping
// ============================================================================

auto GrammarEngine::scope_to_token_type(const std::string& scope) -> TokenType
{
    // Match from most specific to least specific
    if (scope.find("comment.block.documentation") != std::string::npos ||
        scope.find("comment.line.documentation") != std::string::npos)
    {
        return TokenType::DocComment;
    }
    if (scope.find("comment") != std::string::npos)
    {
        return TokenType::Comment;
    }
    if (scope.find("string.regexp") != std::string::npos)
    {
        return TokenType::Regex;
    }
    if (scope.find("constant.character.escape") != std::string::npos)
    {
        return TokenType::Escape;
    }
    if (scope.find("string") != std::string::npos)
    {
        return TokenType::String;
    }
    if (scope.find("constant.numeric") != std::string::npos)
    {
        return TokenType::Number;
    }
    if (scope.find("constant.language") != std::string::npos)
    {
        return TokenType::Constant;
    }
    if (scope.find("keyword.control") != std::string::npos ||
        scope.find("keyword.operator.new") != std::string::npos ||
        scope.find("keyword.other") != std::string::npos ||
        scope.find("storage.type") != std::string::npos ||
        scope.find("storage.modifier") != std::string::npos)
    {
        return TokenType::Keyword;
    }
    if (scope.find("entity.name.function") != std::string::npos ||
        scope.find("support.function") != std::string::npos)
    {
        return TokenType::Function;
    }
    if (scope.find("entity.name.type.class") != std::string::npos ||
        scope.find("entity.name.type.struct") != std::string::npos ||
        scope.find("support.class") != std::string::npos)
    {
        return TokenType::Type;
    }
    if (scope.find("entity.name.type.interface") != std::string::npos)
    {
        return TokenType::Interface;
    }
    if (scope.find("entity.name.type.enum") != std::string::npos)
    {
        return TokenType::Enum;
    }
    if (scope.find("entity.name.type.namespace") != std::string::npos ||
        scope.find("entity.name.namespace") != std::string::npos)
    {
        return TokenType::Namespace;
    }
    if (scope.find("entity.name.tag") != std::string::npos)
    {
        return TokenType::Tag;
    }
    if (scope.find("entity.other.attribute-name") != std::string::npos)
    {
        return TokenType::Attribute;
    }
    if (scope.find("variable.parameter") != std::string::npos)
    {
        return TokenType::Parameter;
    }
    if (scope.find("variable.other.property") != std::string::npos ||
        scope.find("entity.name.property") != std::string::npos)
    {
        return TokenType::Property;
    }
    if (scope.find("variable") != std::string::npos)
    {
        return TokenType::Variable;
    }
    if (scope.find("keyword.operator") != std::string::npos)
    {
        return TokenType::Operator;
    }
    if (scope.find("punctuation") != std::string::npos)
    {
        return TokenType::Punctuation;
    }
    if (scope.find("meta.preprocessor") != std::string::npos ||
        scope.find("keyword.control.directive") != std::string::npos)
    {
        return TokenType::Preprocessor;
    }
    if (scope.find("meta.decorator") != std::string::npos ||
        scope.find("entity.name.decorator") != std::string::npos)
    {
        return TokenType::Decorator;
    }
    if (scope.find("meta.embedded") != std::string::npos)
    {
        return TokenType::Embedded;
    }

    return TokenType::Text;
}

// ============================================================================
// JSON parsing
// ============================================================================

namespace
{

auto parse_captures_map(const nlohmann::json& captures_json) -> std::unordered_map<int, std::string>
{
    std::unordered_map<int, std::string> captures;
    if (!captures_json.is_object())
    {
        return captures;
    }

    for (auto& [key, value] : captures_json.items())
    {
        try
        {
            const int group_index = std::stoi(key);
            if (value.is_object() && value.contains("name"))
            {
                captures[group_index] = value["name"].get<std::string>();
            }
        }
        catch (const std::exception& /*parse_err*/)
        {
            // Skip invalid capture keys
        }
    }
    return captures;
}

auto parse_rule(const nlohmann::json& rule_json) -> GrammarRule;

auto parse_patterns_array(const nlohmann::json& patterns_json) -> std::vector<GrammarRule>
{
    std::vector<GrammarRule> rules;
    if (!patterns_json.is_array())
    {
        return rules;
    }

    for (const auto& pattern_json : patterns_json)
    {
        rules.push_back(parse_rule(pattern_json));
    }
    return rules;
}

auto parse_rule(const nlohmann::json& rule_json) -> GrammarRule
{
    GrammarRule rule;

    if (rule_json.contains("include"))
    {
        rule.include_ref = rule_json["include"].get<std::string>();
        return rule;
    }

    if (rule_json.contains("name"))
    {
        rule.scope_name = rule_json["name"].get<std::string>();
    }

    if (rule_json.contains("contentName"))
    {
        rule.content_name = rule_json["contentName"].get<std::string>();
    }

    if (rule_json.contains("match"))
    {
        rule.pattern_type = GrammarRule::PatternType::Match;
        rule.match_pattern = rule_json["match"].get<std::string>();

        if (rule_json.contains("captures"))
        {
            rule.captures = parse_captures_map(rule_json["captures"]);
        }
    }
    else if (rule_json.contains("begin"))
    {
        rule.pattern_type = GrammarRule::PatternType::BeginEnd;
        rule.begin_pattern = rule_json["begin"].get<std::string>();

        if (rule_json.contains("end"))
        {
            rule.end_pattern = rule_json["end"].get<std::string>();
        }

        if (rule_json.contains("beginCaptures"))
        {
            rule.begin_captures = parse_captures_map(rule_json["beginCaptures"]);
        }
        if (rule_json.contains("endCaptures"))
        {
            rule.end_captures = parse_captures_map(rule_json["endCaptures"]);
        }
        if (rule_json.contains("captures"))
        {
            rule.captures = parse_captures_map(rule_json["captures"]);
        }

        if (rule_json.contains("patterns"))
        {
            rule.nested_patterns = parse_patterns_array(rule_json["patterns"]);
        }
    }

    return rule;
}

} // namespace

auto GrammarEngine::parse_grammar_json(const std::string& json_content,
                                       const std::string& file_path)
    -> std::optional<GrammarDefinition>
{
    try
    {
        auto json_doc = nlohmann::json::parse(json_content);

        GrammarDefinition definition;
        definition.metadata.path = file_path;

        if (json_doc.contains("scopeName"))
        {
            definition.metadata.scope_name = json_doc["scopeName"].get<std::string>();
        }
        if (json_doc.contains("name"))
        {
            definition.metadata.name = json_doc["name"].get<std::string>();
        }

        // Parse file extensions
        if (json_doc.contains("fileTypes") && json_doc["fileTypes"].is_array())
        {
            for (const auto& ext_json : json_doc["fileTypes"])
            {
                if (ext_json.is_string())
                {
                    definition.file_extensions.push_back(ext_json.get<std::string>());
                }
            }
        }

        // Parse top-level patterns
        if (json_doc.contains("patterns"))
        {
            definition.patterns = parse_patterns_array(json_doc["patterns"]);
        }

        // Parse repository
        if (json_doc.contains("repository") && json_doc["repository"].is_object())
        {
            for (auto& [repo_key, repo_value] : json_doc["repository"].items())
            {
                if (repo_value.is_object())
                {
                    // If the repo entry has "patterns", wrap it as a container rule
                    if (repo_value.contains("patterns"))
                    {
                        GrammarRule repo_rule;
                        repo_rule.nested_patterns = parse_patterns_array(repo_value["patterns"]);
                        if (repo_value.contains("name"))
                        {
                            repo_rule.scope_name = repo_value["name"].get<std::string>();
                        }
                        definition.repository[repo_key] = repo_rule;
                    }
                    else
                    {
                        definition.repository[repo_key] = parse_rule(repo_value);
                    }
                }
            }
        }

        return definition;
    }
    catch (const nlohmann::json::exception& /*json_err*/)
    {
        return std::nullopt;
    }
}

auto GrammarEngine::parse_patterns(const std::string& /*json_content*/,
                                   const std::string& /*patterns_key*/) -> std::vector<GrammarRule>
{
    // Not used — parsing is done through parse_grammar_json
    return {};
}

} // namespace markamp::core
