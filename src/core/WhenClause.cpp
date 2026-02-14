#include "WhenClause.h"

#include <cctype>
#include <regex>
#include <stdexcept>

namespace markamp::core
{

// ── WhenClauseParser ──

WhenClauseParser::WhenClauseParser(std::string expression)
    : input_(std::move(expression))
{
}

auto WhenClauseParser::parse(const std::string& expression) -> std::shared_ptr<WhenClauseNode>
{
    // Trim + check for empty
    std::size_t start = 0;
    std::size_t end = expression.size();
    while (start < end && (std::isspace(static_cast<unsigned char>(expression[start])) != 0))
    {
        ++start;
    }
    while (end > start && (std::isspace(static_cast<unsigned char>(expression[end - 1])) != 0))
    {
        --end;
    }
    if (start >= end)
    {
        return nullptr; // empty expression = always true
    }

    WhenClauseParser parser(expression);
    auto result = parser.parse_or();

    parser.skip_whitespace();
    if (!parser.at_end())
    {
        throw std::runtime_error("Unexpected characters after when-clause expression at position " +
                                 std::to_string(parser.pos_));
    }

    return result;
}

auto WhenClauseParser::parse_or() -> std::shared_ptr<WhenClauseNode>
{
    auto left = parse_and();

    while (true)
    {
        skip_whitespace();
        if (pos_ + 1 < input_.size() && input_[pos_] == '|' && input_[pos_ + 1] == '|')
        {
            pos_ += 2;
            auto right = parse_and();
            auto node = std::make_shared<WhenClauseNode>();
            node->kind = WhenClauseNodeKind::kOr;
            node->left = std::move(left);
            node->right = std::move(right);
            left = std::move(node);
        }
        else
        {
            break;
        }
    }

    return left;
}

auto WhenClauseParser::parse_and() -> std::shared_ptr<WhenClauseNode>
{
    auto left = parse_unary();

    while (true)
    {
        skip_whitespace();
        if (pos_ + 1 < input_.size() && input_[pos_] == '&' && input_[pos_ + 1] == '&')
        {
            pos_ += 2;
            auto right = parse_unary();
            auto node = std::make_shared<WhenClauseNode>();
            node->kind = WhenClauseNodeKind::kAnd;
            node->left = std::move(left);
            node->right = std::move(right);
            left = std::move(node);
        }
        else
        {
            break;
        }
    }

    return left;
}

auto WhenClauseParser::parse_unary() -> std::shared_ptr<WhenClauseNode>
{
    skip_whitespace();
    if (!at_end() && peek() == '!')
    {
        advance();
        auto operand = parse_unary();
        auto node = std::make_shared<WhenClauseNode>();
        node->kind = WhenClauseNodeKind::kNot;
        node->left = std::move(operand);
        return node;
    }
    return parse_primary();
}

auto WhenClauseParser::parse_primary() -> std::shared_ptr<WhenClauseNode>
{
    skip_whitespace();

    if (at_end())
    {
        throw std::runtime_error("Unexpected end of when-clause expression");
    }

    // Parenthesized sub-expression
    if (peek() == '(')
    {
        advance(); // consume '('
        auto inner = parse_or();
        skip_whitespace();
        if (at_end() || peek() != ')')
        {
            throw std::runtime_error("Missing closing ')' in when-clause");
        }
        advance(); // consume ')'
        return inner;
    }

    // Read a token (context key name or literal)
    auto token = read_token();
    if (token.empty())
    {
        throw std::runtime_error("Expected context key or value at position " +
                                 std::to_string(pos_));
    }

    // Check for literals
    if (token == "true")
    {
        auto node = std::make_shared<WhenClauseNode>();
        node->kind = WhenClauseNodeKind::kLiteralTrue;
        return node;
    }
    if (token == "false")
    {
        auto node = std::make_shared<WhenClauseNode>();
        node->kind = WhenClauseNodeKind::kLiteralFalse;
        return node;
    }

    // Check for comparison operators
    skip_whitespace();
    if (!at_end())
    {
        // ==
        if (pos_ + 1 < input_.size() && input_[pos_] == '=' && input_[pos_ + 1] == '=')
        {
            pos_ += 2;
            skip_whitespace();
            auto rhs = read_token();
            auto node = std::make_shared<WhenClauseNode>();
            node->kind = WhenClauseNodeKind::kEquals;
            node->key = std::move(token);
            node->value = std::move(rhs);
            return node;
        }

        // !=
        if (pos_ + 1 < input_.size() && input_[pos_] == '!' && input_[pos_ + 1] == '=')
        {
            pos_ += 2;
            skip_whitespace();
            auto rhs = read_token();
            auto node = std::make_shared<WhenClauseNode>();
            node->kind = WhenClauseNodeKind::kNotEquals;
            node->key = std::move(token);
            node->value = std::move(rhs);
            return node;
        }

        // =~  (regex)
        if (pos_ + 1 < input_.size() && input_[pos_] == '=' && input_[pos_ + 1] == '~')
        {
            pos_ += 2;
            skip_whitespace();
            auto pattern = read_regex_pattern();
            auto node = std::make_shared<WhenClauseNode>();
            node->kind = WhenClauseNodeKind::kRegexMatch;
            node->key = std::move(token);
            node->value = std::move(pattern);
            return node;
        }
    }

    // Otherwise: bare context key (truthy check)
    auto node = std::make_shared<WhenClauseNode>();
    node->kind = WhenClauseNodeKind::kHasKey;
    node->key = std::move(token);
    return node;
}

void WhenClauseParser::skip_whitespace()
{
    while (pos_ < input_.size() && (std::isspace(static_cast<unsigned char>(input_[pos_])) != 0))
    {
        ++pos_;
    }
}

auto WhenClauseParser::peek() const -> char
{
    return input_[pos_];
}

auto WhenClauseParser::advance() -> char
{
    return input_[pos_++];
}

auto WhenClauseParser::at_end() const -> bool
{
    return pos_ >= input_.size();
}

auto WhenClauseParser::match(char expected) -> bool
{
    if (!at_end() && peek() == expected)
    {
        advance();
        return true;
    }
    return false;
}

auto WhenClauseParser::read_token() -> std::string
{
    skip_whitespace();
    std::string token;

    // Handle quoted strings
    if (!at_end() && (peek() == '\'' || peek() == '"'))
    {
        const char quote_char = advance();
        while (!at_end() && peek() != quote_char)
        {
            token += advance();
        }
        if (!at_end())
        {
            advance(); // consume closing quote
        }
        return token;
    }

    // Regular token: alphanumeric, dot, colon, hyphen, underscore
    while (!at_end())
    {
        const char current = peek();
        if ((std::isalnum(static_cast<unsigned char>(current)) != 0) || current == '.' ||
            current == ':' || current == '-' || current == '_' || current == '/')
        {
            token += advance();
        }
        else
        {
            break;
        }
    }

    return token;
}

auto WhenClauseParser::read_regex_pattern() -> std::string
{
    skip_whitespace();
    std::string pattern;

    if (!at_end() && peek() == '/')
    {
        advance(); // consume opening /
        while (!at_end() && peek() != '/')
        {
            if (peek() == '\\' && pos_ + 1 < input_.size())
            {
                pattern += advance(); // backslash
                pattern += advance(); // escaped char
            }
            else
            {
                pattern += advance();
            }
        }
        if (!at_end())
        {
            advance(); // consume closing /
        }
        // Read flags (e.g., 'i' for case-insensitive)
        while (!at_end() && (std::isalpha(static_cast<unsigned char>(peek())) != 0))
        {
            pattern += advance();
        }
    }
    else
    {
        // No delimiters, just read as token
        pattern = read_token();
    }

    return pattern;
}

// ── WhenClauseEvaluator ──

auto WhenClauseEvaluator::evaluate(const std::shared_ptr<WhenClauseNode>& node,
                                   const ContextKeyService& context) -> bool
{
    if (!node)
    {
        return true; // null expression = always true
    }

    switch (node->kind)
    {
        case WhenClauseNodeKind::kLiteralTrue:
            return true;

        case WhenClauseNodeKind::kLiteralFalse:
            return false;

        case WhenClauseNodeKind::kHasKey:
            return context.is_truthy(node->key);

        case WhenClauseNodeKind::kNot:
            return !evaluate(node->left, context);

        case WhenClauseNodeKind::kAnd:
            return evaluate(node->left, context) && evaluate(node->right, context);

        case WhenClauseNodeKind::kOr:
            return evaluate(node->left, context) || evaluate(node->right, context);

        case WhenClauseNodeKind::kEquals:
        {
            auto ctx_val = context_value_as_string(context, node->key);
            return ctx_val == node->value;
        }

        case WhenClauseNodeKind::kNotEquals:
        {
            auto ctx_val = context_value_as_string(context, node->key);
            return ctx_val != node->value;
        }

        case WhenClauseNodeKind::kRegexMatch:
        {
            auto ctx_val = context_value_as_string(context, node->key);
            return evaluate_regex(ctx_val, node->value);
        }
    }

    return false; // unreachable
}

auto WhenClauseEvaluator::matches(const std::string& expression, const ContextKeyService& context)
    -> bool
{
    auto ast = WhenClauseParser::parse(expression);
    return evaluate(ast, context);
}

auto WhenClauseEvaluator::evaluate_regex(const std::string& text, const std::string& pattern)
    -> bool
{
    if (pattern.empty())
    {
        return false;
    }

    // Check for flags at end (pattern may end with 'i' for case-insensitive)
    std::string regex_pattern = pattern;
    auto flags = std::regex::ECMAScript;

    if (!regex_pattern.empty() && regex_pattern.back() == 'i')
    {
        regex_pattern.pop_back();
        flags |= std::regex::icase;
    }

    try
    {
        const std::regex regex_obj(regex_pattern, flags);
        return std::regex_search(text, regex_obj);
    }
    catch (const std::regex_error&)
    {
        return false; // Invalid regex = no match
    }
}

auto WhenClauseEvaluator::context_value_as_string(const ContextKeyService& context,
                                                  const std::string& key) -> std::string
{
    const auto* val = context.get_context(key);
    if (val == nullptr)
    {
        return {};
    }

    return std::visit(
        [](const auto& visited_value) -> std::string
        {
            using ValueType = std::decay_t<decltype(visited_value)>;
            if constexpr (std::is_same_v<ValueType, std::string>)
            {
                return visited_value;
            }
            else if constexpr (std::is_same_v<ValueType, bool>)
            {
                return visited_value ? "true" : "false";
            }
            else if constexpr (std::is_same_v<ValueType, int>)
            {
                return std::to_string(visited_value);
            }
            else if constexpr (std::is_same_v<ValueType, double>)
            {
                return std::to_string(visited_value);
            }
            else
            {
                return {};
            }
        },
        *val);
}

} // namespace markamp::core
