#include "core/MarkdownParser.h"
#include "core/SyntaxHighlighter.h"
#include "rendering/CodeBlockRenderer.h"
#include "rendering/HtmlRenderer.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;
using namespace markamp::rendering;
using Catch::Matchers::ContainsSubstring;

// ═══════════════════════════════════════════════════════
// Language support & alias resolution
// ═══════════════════════════════════════════════════════

TEST_CASE("SyntaxHighlighter supports 15 built-in languages", "[syntax][languages]")
{
    SyntaxHighlighter hl;
    auto langs = hl.supported_languages();
    REQUIRE(langs.size() >= 15);

    // Tier 1 languages
    CHECK(hl.is_supported("javascript"));
    CHECK(hl.is_supported("typescript"));
    CHECK(hl.is_supported("python"));
    CHECK(hl.is_supported("c"));
    CHECK(hl.is_supported("cpp"));
    CHECK(hl.is_supported("rust"));
    CHECK(hl.is_supported("go"));
    CHECK(hl.is_supported("java"));
    CHECK(hl.is_supported("csharp"));
    CHECK(hl.is_supported("html"));
    CHECK(hl.is_supported("css"));
    CHECK(hl.is_supported("json"));
    CHECK(hl.is_supported("yaml"));
    CHECK(hl.is_supported("sql"));
    CHECK(hl.is_supported("bash"));
}

TEST_CASE("SyntaxHighlighter resolves language aliases", "[syntax][aliases]")
{
    SyntaxHighlighter hl;

    // JavaScript aliases
    CHECK(hl.is_supported("js"));
    CHECK(hl.is_supported("jsx"));
    CHECK(hl.is_supported("mjs"));

    // TypeScript aliases
    CHECK(hl.is_supported("ts"));
    CHECK(hl.is_supported("tsx"));

    // Python aliases
    CHECK(hl.is_supported("py"));

    // C++ aliases
    CHECK(hl.is_supported("c++"));
    CHECK(hl.is_supported("cxx"));
    CHECK(hl.is_supported("cc"));
    CHECK(hl.is_supported("hpp"));

    // Rust
    CHECK(hl.is_supported("rs"));

    // Go
    CHECK(hl.is_supported("golang"));

    // C#
    CHECK(hl.is_supported("cs"));

    // Shell aliases
    CHECK(hl.is_supported("sh"));
    CHECK(hl.is_supported("shell"));
    CHECK(hl.is_supported("zsh"));

    // SQL aliases
    CHECK(hl.is_supported("mysql"));
    CHECK(hl.is_supported("postgresql"));
    CHECK(hl.is_supported("sqlite"));
}

TEST_CASE("SyntaxHighlighter returns false for unsupported languages", "[syntax][unsupported]")
{
    SyntaxHighlighter hl;
    CHECK_FALSE(hl.is_supported("brainfuck"));
    CHECK_FALSE(hl.is_supported(""));
    CHECK_FALSE(hl.is_supported("fortran77"));
}

// ═══════════════════════════════════════════════════════
// JavaScript tokenization
// ═══════════════════════════════════════════════════════

TEST_CASE("JavaScript: keywords, functions, strings, comments", "[syntax][javascript]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize("function hello(name) {\n  return `Hi ${name}`;\n}\n// done", "js");

    // Find keywords
    bool found_function = false;
    bool found_return = false;
    bool found_comment = false;
    bool found_string = false;

    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Keyword && tok.text == "function")
            found_function = true;
        if (tok.type == TokenType::Keyword && tok.text == "return")
            found_return = true;
        if (tok.type == TokenType::Comment && tok.text == "// done")
            found_comment = true;
        if (tok.type == TokenType::String)
            found_string = true;
    }

    CHECK(found_function);
    CHECK(found_return);
    CHECK(found_comment);
    CHECK(found_string);
}

TEST_CASE("JavaScript: function detection (identifier followed by parens)", "[syntax][javascript]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize("console.log(x)", "js");

    bool found_func = false;
    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Function && tok.text == "log")
            found_func = true;
    }
    CHECK(found_func);
}

// ═══════════════════════════════════════════════════════
// Python tokenization
// ═══════════════════════════════════════════════════════

TEST_CASE("Python: class, def, decorators, strings", "[syntax][python]")
{
    SyntaxHighlighter hl;
    auto tokens =
        hl.tokenize("@dataclass\nclass Foo:\n    def bar(self):\n        return \"hello\"", "py");

    bool found_class = false;
    bool found_def = false;
    bool found_decorator = false;
    bool found_return = false;
    bool found_string = false;
    bool found_self = false;

    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Keyword && tok.text == "class")
            found_class = true;
        if (tok.type == TokenType::Keyword && tok.text == "def")
            found_def = true;
        if (tok.type == TokenType::Attribute && tok.text == "@dataclass")
            found_decorator = true;
        if (tok.type == TokenType::Keyword && tok.text == "return")
            found_return = true;
        if (tok.type == TokenType::String && tok.text == "\"hello\"")
            found_string = true;
        if (tok.type == TokenType::Constant && tok.text == "self")
            found_self = true;
    }

    CHECK(found_class);
    CHECK(found_def);
    CHECK(found_decorator);
    CHECK(found_return);
    CHECK(found_string);
    CHECK(found_self);
}

TEST_CASE("Python: line comment with #", "[syntax][python]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize("x = 1  # comment", "py");

    bool found_comment = false;
    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Comment && tok.text == "# comment")
            found_comment = true;
    }
    CHECK(found_comment);
}

// ═══════════════════════════════════════════════════════
// C++ tokenization
// ═══════════════════════════════════════════════════════

TEST_CASE("C++: preprocessor, keywords, types", "[syntax][cpp]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize("#include <iostream>\nint main() {\n    return 0;\n}", "cpp");

    bool found_preprocessor = false;
    bool found_int = false;
    bool found_return = false;
    bool found_number = false;

    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Preprocessor)
            found_preprocessor = true;
        if (tok.type == TokenType::Type && tok.text == "int")
            found_int = true;
        if (tok.type == TokenType::Keyword && tok.text == "return")
            found_return = true;
        if (tok.type == TokenType::Number && tok.text == "0")
            found_number = true;
    }

    CHECK(found_preprocessor);
    CHECK(found_int);
    CHECK(found_return);
    CHECK(found_number);
}

// ═══════════════════════════════════════════════════════
// HTML tokenization
// ═══════════════════════════════════════════════════════

TEST_CASE("HTML: block comments", "[syntax][html]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize("<!-- comment --><div>hello</div>", "html");

    bool found_comment = false;
    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Comment && tok.text == "<!-- comment -->")
            found_comment = true;
    }
    CHECK(found_comment);
}

// ═══════════════════════════════════════════════════════
// CSS tokenization
// ═══════════════════════════════════════════════════════

TEST_CASE("CSS: block comments and strings", "[syntax][css]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize("/* reset */\nbody { color: red; }", "css");

    bool found_comment = false;
    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Comment)
            found_comment = true;
    }
    CHECK(found_comment);
}

// ═══════════════════════════════════════════════════════
// JSON tokenization
// ═══════════════════════════════════════════════════════

TEST_CASE("JSON: strings, numbers, constants", "[syntax][json]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize(R"({"name": "Alice", "age": 30, "active": true})", "json");

    bool found_string = false;
    bool found_number = false;
    bool found_constant = false;

    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::String && tok.text == "\"name\"")
            found_string = true;
        if (tok.type == TokenType::Number && tok.text == "30")
            found_number = true;
        if (tok.type == TokenType::Constant && tok.text == "true")
            found_constant = true;
    }

    CHECK(found_string);
    CHECK(found_number);
    CHECK(found_constant);
}

// ═══════════════════════════════════════════════════════
// Bash tokenization
// ═══════════════════════════════════════════════════════

TEST_CASE("Bash: keywords, comments", "[syntax][bash]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize("#!/bin/bash\nif [ -f file ]; then\n  echo 'found'\nfi", "sh");

    bool found_if = false;
    bool found_then = false;
    bool found_fi = false;
    bool found_echo = false;
    bool found_comment = false;

    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Keyword && tok.text == "if")
            found_if = true;
        if (tok.type == TokenType::Keyword && tok.text == "then")
            found_then = true;
        if (tok.type == TokenType::Keyword && tok.text == "fi")
            found_fi = true;
        if (tok.type == TokenType::Keyword && tok.text == "echo")
            found_echo = true;
        if (tok.type == TokenType::Comment)
            found_comment = true;
    }

    CHECK(found_if);
    CHECK(found_then);
    CHECK(found_fi);
    CHECK(found_echo);
    CHECK(found_comment);
}

// ═══════════════════════════════════════════════════════
// SQL tokenization
// ═══════════════════════════════════════════════════════

TEST_CASE("SQL: keywords and types", "[syntax][sql]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize("SELECT name FROM users WHERE id = 1;", "sql");

    bool found_select = false;
    bool found_from = false;
    bool found_where = false;

    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Keyword && tok.text == "SELECT")
            found_select = true;
        if (tok.type == TokenType::Keyword && tok.text == "FROM")
            found_from = true;
        if (tok.type == TokenType::Keyword && tok.text == "WHERE")
            found_where = true;
    }

    CHECK(found_select);
    CHECK(found_from);
    CHECK(found_where);
}

// ═══════════════════════════════════════════════════════
// Edge cases
// ═══════════════════════════════════════════════════════

TEST_CASE("Unsupported language falls back to plain text", "[syntax][fallback]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize("hello world", "brainfuck");

    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::Text);
    CHECK(tokens[0].text == "hello world");
}

TEST_CASE("Empty source returns empty tokens", "[syntax][edge]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize("", "javascript");
    CHECK(tokens.empty());
}

TEST_CASE("Multi-line block comment", "[syntax][edge]")
{
    SyntaxHighlighter hl;
    std::string source = "/* line1\n   line2\n   line3 */\ncode";
    auto tokens = hl.tokenize(source, "c");

    REQUIRE(!tokens.empty());
    CHECK(tokens[0].type == TokenType::Comment);
    CHECK(tokens[0].text == "/* line1\n   line2\n   line3 */");
}

TEST_CASE("String with escape sequences", "[syntax][edge]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize(R"("hello \"world\"")", "javascript");

    bool found_string = false;
    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::String)
        {
            found_string = true;
            CHECK(tok.text == R"("hello \"world\"")");
        }
    }
    CHECK(found_string);
}

TEST_CASE("Hex number literal", "[syntax][edge]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize("0xFF", "c");

    bool found_hex = false;
    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Number && tok.text == "0xFF")
            found_hex = true;
    }
    CHECK(found_hex);
}

TEST_CASE("ALL_CAPS identifier classified as constant", "[syntax][edge]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize("MAX_SIZE", "c");

    bool found_const = false;
    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Constant && tok.text == "MAX_SIZE")
            found_const = true;
    }
    CHECK(found_const);
}

TEST_CASE("Token position accuracy", "[syntax][positions]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize("int x = 42;", "c");

    // Verify token positions are contiguous
    size_t expected_pos = 0;
    for (const auto& tok : tokens)
    {
        CHECK(tok.start == expected_pos);
        CHECK(tok.length == tok.text.size());
        expected_pos = tok.start + tok.length;
    }
    CHECK(expected_pos == 11); // Length of "int x = 42;"
}

// ═══════════════════════════════════════════════════════
// render_html
// ═══════════════════════════════════════════════════════

TEST_CASE("render_html produces well-formed span tags", "[syntax][render]")
{
    SyntaxHighlighter hl;
    auto html = hl.render_html("if (true) return;", "javascript");

    CHECK_THAT(html, ContainsSubstring("<span class=\"token-keyword\">if</span>"));
    CHECK_THAT(html, ContainsSubstring("<span class=\"token-constant\">true</span>"));
    CHECK_THAT(html, ContainsSubstring("<span class=\"token-keyword\">return</span>"));
}

TEST_CASE("render_html escapes HTML special characters", "[syntax][render]")
{
    SyntaxHighlighter hl;
    auto html = hl.render_html("x < y && z > w", "javascript");

    CHECK_THAT(html, ContainsSubstring("&lt;"));
    CHECK_THAT(html, ContainsSubstring("&amp;"));
    CHECK_THAT(html, ContainsSubstring("&gt;"));
}

TEST_CASE("token_class returns correct CSS class suffix", "[syntax][render]")
{
    CHECK(SyntaxHighlighter::token_class(TokenType::Keyword) == "keyword");
    CHECK(SyntaxHighlighter::token_class(TokenType::String) == "string");
    CHECK(SyntaxHighlighter::token_class(TokenType::Number) == "number");
    CHECK(SyntaxHighlighter::token_class(TokenType::Comment) == "comment");
    CHECK(SyntaxHighlighter::token_class(TokenType::Function) == "function");
    CHECK(SyntaxHighlighter::token_class(TokenType::Type) == "type");
    CHECK(SyntaxHighlighter::token_class(TokenType::Constant) == "constant");
    CHECK(SyntaxHighlighter::token_class(TokenType::Preprocessor) == "preprocessor");
    CHECK(SyntaxHighlighter::token_class(TokenType::Operator) == "operator");
    CHECK(SyntaxHighlighter::token_class(TokenType::Punctuation) == "punctuation");
    CHECK(SyntaxHighlighter::token_class(TokenType::Text) == "text");
}

// ═══════════════════════════════════════════════════════
// CodeBlockRenderer
// ═══════════════════════════════════════════════════════

TEST_CASE("CodeBlockRenderer produces PRD HTML structure", "[codeblock][structure]")
{
    CodeBlockRenderer renderer;
    auto html = renderer.render("int x = 1;", "cpp");

    CHECK_THAT(html, ContainsSubstring("<div class=\"code-block-wrapper\">"));
    CHECK_THAT(html, ContainsSubstring("<div class=\"code-block-header\">"));
    CHECK_THAT(html, ContainsSubstring("<span class=\"language-label\">cpp</span>"));
    CHECK_THAT(html, ContainsSubstring("<pre class=\"code-block\">"));
    CHECK_THAT(html, ContainsSubstring("<code class=\"language-cpp\">"));
    CHECK_THAT(html, ContainsSubstring("</code></pre>"));
    CHECK_THAT(html, ContainsSubstring("</div>"));
}

TEST_CASE("CodeBlockRenderer with syntax highlighting", "[codeblock][highlighting]")
{
    CodeBlockRenderer renderer;
    auto html = renderer.render("return true;", "javascript");

    CHECK_THAT(html, ContainsSubstring("<span class=\"token-keyword\">return</span>"));
    CHECK_THAT(html, ContainsSubstring("<span class=\"token-constant\">true</span>"));
}

TEST_CASE("CodeBlockRenderer: no language = no header", "[codeblock][plain]")
{
    CodeBlockRenderer renderer;
    auto html = renderer.render_plain("raw code");

    CHECK_THAT(html, ContainsSubstring("<div class=\"code-block-wrapper\">"));
    CHECK_THAT(html, ContainsSubstring("<pre class=\"code-block\"><code>"));
    CHECK_THAT(html, ContainsSubstring("raw code"));
    // No header div or language label
    CHECK_THAT(html, !ContainsSubstring("code-block-header"));
    CHECK_THAT(html, !ContainsSubstring("language-label"));
}

TEST_CASE("CodeBlockRenderer: empty language delegates to render_plain", "[codeblock][empty]")
{
    CodeBlockRenderer renderer;
    auto html = renderer.render("code here", "");

    CHECK_THAT(html, !ContainsSubstring("code-block-header"));
    CHECK_THAT(html, ContainsSubstring("code here"));
}

TEST_CASE("CodeBlockRenderer: unsupported language shows plain text with label",
          "[codeblock][unsupported]")
{
    CodeBlockRenderer renderer;
    auto html = renderer.render("some code", "brainfuck");

    CHECK_THAT(html, ContainsSubstring("<span class=\"language-label\">brainfuck</span>"));
    CHECK_THAT(html, ContainsSubstring("some code"));
    // No token-* spans for unsupported language
    CHECK_THAT(html, !ContainsSubstring("token-keyword"));
}

// ═══════════════════════════════════════════════════════
// Integration: MarkdownParser with code blocks
// ═══════════════════════════════════════════════════════

TEST_CASE("MarkdownParser renders fenced code blocks with syntax highlighting", "[integration]")
{
    MarkdownParser parser;
    auto doc = parser.parse("```javascript\nconst x = 42;\n```\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);

    CHECK_THAT(html, ContainsSubstring("code-block-wrapper"));
    CHECK_THAT(html, ContainsSubstring("language-label"));
    CHECK_THAT(html, ContainsSubstring("javascript"));
}

TEST_CASE("MarkdownParser renders code blocks without language", "[integration]")
{
    MarkdownParser parser;
    auto doc = parser.parse("```\nplain text\n```\n");
    REQUIRE(doc.has_value());

    auto html = parser.render_html(*doc);

    CHECK_THAT(html, ContainsSubstring("code-block-wrapper"));
    CHECK_THAT(html, ContainsSubstring("plain text"));
}

// ═══════════════════════════════════════════════════════
// Rust tokenization
// ═══════════════════════════════════════════════════════

TEST_CASE("Rust: keywords and types", "[syntax][rust]")
{
    SyntaxHighlighter hl;
    auto tokens = hl.tokenize(
        "fn main() -> i32 {\n    let x: String = String::new();\n    return 0;\n}", "rs");

    bool found_fn = false;
    bool found_let = false;
    bool found_i32 = false;
    bool found_string_type = false;

    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Keyword && tok.text == "fn")
            found_fn = true;
        if (tok.type == TokenType::Keyword && tok.text == "let")
            found_let = true;
        if (tok.type == TokenType::Type && tok.text == "i32")
            found_i32 = true;
        if (tok.type == TokenType::Type && tok.text == "String")
            found_string_type = true;
    }

    CHECK(found_fn);
    CHECK(found_let);
    CHECK(found_i32);
    CHECK(found_string_type);
}

// ═══════════════════════════════════════════════════════
// Go tokenization
// ═══════════════════════════════════════════════════════

TEST_CASE("Go: keywords and constants", "[syntax][go]")
{
    SyntaxHighlighter hl;
    auto tokens =
        hl.tokenize("func main() {\n    var x int = 42\n    if x == nil {\n    }\n}", "golang");

    bool found_func = false;
    bool found_var = false;
    bool found_nil = false;

    for (const auto& tok : tokens)
    {
        if (tok.type == TokenType::Keyword && tok.text == "func")
            found_func = true;
        if (tok.type == TokenType::Keyword && tok.text == "var")
            found_var = true;
        if (tok.type == TokenType::Constant && tok.text == "nil")
            found_nil = true;
    }

    CHECK(found_func);
    CHECK(found_var);
    CHECK(found_nil);
}
