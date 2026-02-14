#include "core/ContextKeyService.h"
#include "core/WhenClause.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ══════════════════════════════════════════
// ContextKeyService Tests
// ══════════════════════════════════════════

TEST_CASE("ContextKeyService: set and get bool", "[context]")
{
    ContextKeyService ctx;
    ctx.set_context("editorFocus", true);

    REQUIRE(ctx.has_context("editorFocus"));
    REQUIRE(ctx.get_bool("editorFocus") == true);
}

TEST_CASE("ContextKeyService: set and get string", "[context]")
{
    ContextKeyService ctx;
    ctx.set_context("resourceScheme", std::string("file"));
    REQUIRE(ctx.get_string("resourceScheme") == "file");
}

TEST_CASE("ContextKeyService: set and get int", "[context]")
{
    ContextKeyService ctx;
    ctx.set_context("editorTabSize", 4);
    REQUIRE(ctx.get_int("editorTabSize") == 4);
}

TEST_CASE("ContextKeyService: missing key returns defaults", "[context]")
{
    ContextKeyService ctx;
    REQUIRE(ctx.get_bool("nonexistent") == false);
    REQUIRE(ctx.get_string("nonexistent").empty());
    REQUIRE(ctx.get_int("nonexistent") == 0);
    REQUIRE_FALSE(ctx.has_context("nonexistent"));
}

TEST_CASE("ContextKeyService: remove key", "[context]")
{
    ContextKeyService ctx;
    ctx.set_context("key1", true);
    REQUIRE(ctx.has_context("key1"));
    ctx.remove_context("key1");
    REQUIRE_FALSE(ctx.has_context("key1"));
}

TEST_CASE("ContextKeyService: scoped keys", "[context]")
{
    ContextKeyService ctx;
    ctx.set_context("global", std::string("g"));
    REQUIRE(ctx.scope_depth() == 0);

    ctx.push_scope();
    REQUIRE(ctx.scope_depth() == 1);
    ctx.set_context("local", std::string("l"));

    // Both visible
    REQUIRE(ctx.get_string("global") == "g");
    REQUIRE(ctx.get_string("local") == "l");

    ctx.pop_scope();
    REQUIRE(ctx.scope_depth() == 0);

    // Only global visible
    REQUIRE(ctx.get_string("global") == "g");
    REQUIRE_FALSE(ctx.has_context("local"));
}

TEST_CASE("ContextKeyService: scope override", "[context]")
{
    ContextKeyService ctx;
    ctx.set_context("theme", std::string("dark"));

    ctx.push_scope();
    ctx.set_context("theme", std::string("light"));

    // Inner scope takes precedence
    REQUIRE(ctx.get_string("theme") == "light");

    ctx.pop_scope();

    // Outer scope restored
    REQUIRE(ctx.get_string("theme") == "dark");
}

TEST_CASE("ContextKeyService: truthiness", "[context]")
{
    ContextKeyService ctx;

    ctx.set_context("boolTrue", true);
    ctx.set_context("boolFalse", false);
    ctx.set_context("intNonZero", 42);
    ctx.set_context("intZero", 0);
    ctx.set_context("strNonEmpty", std::string("hello"));
    ctx.set_context("strEmpty", std::string(""));
    ctx.set_context("dblNonZero", 3.14);
    ctx.set_context("dblZero", 0.0);

    REQUIRE(ctx.is_truthy("boolTrue"));
    REQUIRE_FALSE(ctx.is_truthy("boolFalse"));
    REQUIRE(ctx.is_truthy("intNonZero"));
    REQUIRE_FALSE(ctx.is_truthy("intZero"));
    REQUIRE(ctx.is_truthy("strNonEmpty"));
    REQUIRE_FALSE(ctx.is_truthy("strEmpty"));
    REQUIRE(ctx.is_truthy("dblNonZero"));
    REQUIRE_FALSE(ctx.is_truthy("dblZero"));
    REQUIRE_FALSE(ctx.is_truthy("nonexistent"));
}

TEST_CASE("ContextKeyService: change listener", "[context]")
{
    ContextKeyService ctx;
    std::string changed_key;

    auto sub_id = ctx.on_did_change([&changed_key](const std::string& key) { changed_key = key; });

    ctx.set_context("test", true);
    REQUIRE(changed_key == "test");

    ctx.remove_change_listener(sub_id);
    ctx.set_context("test2", true);
    REQUIRE(changed_key == "test"); // Not updated
}

// ══════════════════════════════════════════
// WhenClause Parser Tests
// ══════════════════════════════════════════

TEST_CASE("WhenClause: empty expression returns null", "[when]")
{
    auto node = WhenClauseParser::parse("");
    REQUIRE(node == nullptr);

    auto node2 = WhenClauseParser::parse("   ");
    REQUIRE(node2 == nullptr);
}

TEST_CASE("WhenClause: bare context key", "[when]")
{
    auto node = WhenClauseParser::parse("editorTextFocus");
    REQUIRE(node != nullptr);
    REQUIRE(node->kind == WhenClauseNodeKind::kHasKey);
    REQUIRE(node->key == "editorTextFocus");
}

TEST_CASE("WhenClause: negation", "[when]")
{
    auto node = WhenClauseParser::parse("!editorReadonly");
    REQUIRE(node != nullptr);
    REQUIRE(node->kind == WhenClauseNodeKind::kNot);
    REQUIRE(node->left != nullptr);
    REQUIRE(node->left->kind == WhenClauseNodeKind::kHasKey);
    REQUIRE(node->left->key == "editorReadonly");
}

TEST_CASE("WhenClause: equality", "[when]")
{
    auto node = WhenClauseParser::parse("resourceScheme == file");
    REQUIRE(node != nullptr);
    REQUIRE(node->kind == WhenClauseNodeKind::kEquals);
    REQUIRE(node->key == "resourceScheme");
    REQUIRE(node->value == "file");
}

TEST_CASE("WhenClause: inequality", "[when]")
{
    auto node = WhenClauseParser::parse("resourceScheme != untitled");
    REQUIRE(node != nullptr);
    REQUIRE(node->kind == WhenClauseNodeKind::kNotEquals);
    REQUIRE(node->key == "resourceScheme");
    REQUIRE(node->value == "untitled");
}

TEST_CASE("WhenClause: AND expression", "[when]")
{
    auto node = WhenClauseParser::parse("editorFocus && editorTextFocus");
    REQUIRE(node != nullptr);
    REQUIRE(node->kind == WhenClauseNodeKind::kAnd);
    REQUIRE(node->left->kind == WhenClauseNodeKind::kHasKey);
    REQUIRE(node->left->key == "editorFocus");
    REQUIRE(node->right->kind == WhenClauseNodeKind::kHasKey);
    REQUIRE(node->right->key == "editorTextFocus");
}

TEST_CASE("WhenClause: OR expression", "[when]")
{
    auto node = WhenClauseParser::parse("isWindows || isMac");
    REQUIRE(node != nullptr);
    REQUIRE(node->kind == WhenClauseNodeKind::kOr);
    REQUIRE(node->left->key == "isWindows");
    REQUIRE(node->right->key == "isMac");
}

TEST_CASE("WhenClause: precedence AND before OR", "[when]")
{
    // a || b && c => a || (b && c)
    auto node = WhenClauseParser::parse("a || b && c");
    REQUIRE(node != nullptr);
    REQUIRE(node->kind == WhenClauseNodeKind::kOr);
    REQUIRE(node->left->kind == WhenClauseNodeKind::kHasKey);
    REQUIRE(node->left->key == "a");
    REQUIRE(node->right->kind == WhenClauseNodeKind::kAnd);
}

TEST_CASE("WhenClause: parentheses", "[when]")
{
    auto node = WhenClauseParser::parse("(a || b) && c");
    REQUIRE(node != nullptr);
    REQUIRE(node->kind == WhenClauseNodeKind::kAnd);
    REQUIRE(node->left->kind == WhenClauseNodeKind::kOr);
    REQUIRE(node->right->kind == WhenClauseNodeKind::kHasKey);
    REQUIRE(node->right->key == "c");
}

TEST_CASE("WhenClause: literal true/false", "[when]")
{
    auto node_true = WhenClauseParser::parse("true");
    REQUIRE(node_true->kind == WhenClauseNodeKind::kLiteralTrue);

    auto node_false = WhenClauseParser::parse("false");
    REQUIRE(node_false->kind == WhenClauseNodeKind::kLiteralFalse);
}

TEST_CASE("WhenClause: regex match", "[when]")
{
    auto node = WhenClauseParser::parse("resourceFilename =~ /\\.md$/");
    REQUIRE(node != nullptr);
    REQUIRE(node->kind == WhenClauseNodeKind::kRegexMatch);
    REQUIRE(node->key == "resourceFilename");
    REQUIRE(node->value == "\\.md$");
}

// ══════════════════════════════════════════
// WhenClause Evaluator Tests
// ══════════════════════════════════════════

TEST_CASE("WhenClause evaluate: bare key truthy", "[when]")
{
    ContextKeyService ctx;
    ctx.set_context("editorFocus", true);

    REQUIRE(WhenClauseEvaluator::matches("editorFocus", ctx));
    REQUIRE_FALSE(WhenClauseEvaluator::matches("editorReadonly", ctx));
}

TEST_CASE("WhenClause evaluate: negation", "[when]")
{
    ContextKeyService ctx;
    ctx.set_context("editorReadonly", false);

    REQUIRE(WhenClauseEvaluator::matches("!editorReadonly", ctx));
}

TEST_CASE("WhenClause evaluate: equality string", "[when]")
{
    ContextKeyService ctx;
    ctx.set_context("resourceScheme", std::string("file"));

    REQUIRE(WhenClauseEvaluator::matches("resourceScheme == file", ctx));
    REQUIRE_FALSE(WhenClauseEvaluator::matches("resourceScheme == untitled", ctx));
}

TEST_CASE("WhenClause evaluate: inequality", "[when]")
{
    ContextKeyService ctx;
    ctx.set_context("resourceScheme", std::string("file"));

    REQUIRE(WhenClauseEvaluator::matches("resourceScheme != untitled", ctx));
    REQUIRE_FALSE(WhenClauseEvaluator::matches("resourceScheme != file", ctx));
}

TEST_CASE("WhenClause evaluate: AND", "[when]")
{
    ContextKeyService ctx;
    ctx.set_context("editorFocus", true);
    ctx.set_context("editorTextFocus", true);

    REQUIRE(WhenClauseEvaluator::matches("editorFocus && editorTextFocus", ctx));

    ctx.set_context("editorTextFocus", false);
    REQUIRE_FALSE(WhenClauseEvaluator::matches("editorFocus && editorTextFocus", ctx));
}

TEST_CASE("WhenClause evaluate: OR", "[when]")
{
    ContextKeyService ctx;
    ctx.set_context("isWindows", false);
    ctx.set_context("isMac", true);

    REQUIRE(WhenClauseEvaluator::matches("isWindows || isMac", ctx));
}

TEST_CASE("WhenClause evaluate: complex nested", "[when]")
{
    ContextKeyService ctx;
    ctx.set_context("editorFocus", true);
    ctx.set_context("resourceScheme", std::string("file"));
    ctx.set_context("editorReadonly", false);

    REQUIRE(WhenClauseEvaluator::matches("editorFocus && resourceScheme == file && !editorReadonly",
                                         ctx));
}

TEST_CASE("WhenClause evaluate: empty expression is true", "[when]")
{
    ContextKeyService ctx;
    REQUIRE(WhenClauseEvaluator::matches("", ctx));
    REQUIRE(WhenClauseEvaluator::matches("  ", ctx));
}

TEST_CASE("WhenClause evaluate: regex match", "[when]")
{
    ContextKeyService ctx;
    ctx.set_context("resourceFilename", std::string("readme.md"));

    REQUIRE(WhenClauseEvaluator::matches("resourceFilename =~ /\\.md$/", ctx));
    REQUIRE_FALSE(WhenClauseEvaluator::matches("resourceFilename =~ /\\.txt$/", ctx));
}

TEST_CASE("WhenClause evaluate: bool equality", "[when]")
{
    ContextKeyService ctx;
    ctx.set_context("debugMode", true);

    REQUIRE(WhenClauseEvaluator::matches("debugMode == true", ctx));
    REQUIRE_FALSE(WhenClauseEvaluator::matches("debugMode == false", ctx));
}

TEST_CASE("WhenClause evaluate: literal true/false", "[when]")
{
    ContextKeyService ctx;
    REQUIRE(WhenClauseEvaluator::matches("true", ctx));
    REQUIRE_FALSE(WhenClauseEvaluator::matches("false", ctx));
}
