#include "core/DebugConsoleService.h"
#include "core/ExpressionEvaluator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ── DebugConsoleService tests ──

TEST_CASE("DebugConsoleService add output", "[debug_console]")
{
    DebugConsoleService service;

    service.add_output("Hello, world!");
    REQUIRE(service.entry_count() == 1);
    REQUIRE(service.entries()[0].text == "Hello, world!");
    REQUIRE(service.entries()[0].type == ConsoleEntryType::kOutput);
}

TEST_CASE("DebugConsoleService add error output", "[debug_console]")
{
    DebugConsoleService service;

    service.add_output("Something failed", ConsoleEntryType::kError);
    REQUIRE(service.entries()[0].type == ConsoleEntryType::kError);
    REQUIRE(service.entries()[0].level == LogLevel::kError);
}

TEST_CASE("DebugConsoleService add output with log level", "[debug_console]")
{
    DebugConsoleService service;

    service.add_output("debug info", LogLevel::kDebug);
    REQUIRE(service.entries()[0].level == LogLevel::kDebug);
}

TEST_CASE("DebugConsoleService entries_filtered", "[debug_console]")
{
    DebugConsoleService service;

    service.add_output("trace msg", LogLevel::kTrace);
    service.add_output("info msg", LogLevel::kInfo);
    service.add_output("error msg", LogLevel::kError);

    auto warnings_up = service.entries_filtered(LogLevel::kWarning);
    REQUIRE(warnings_up.size() == 1);
    REQUIRE(warnings_up[0].text == "error msg");

    auto all = service.entries_filtered(LogLevel::kTrace);
    REQUIRE(all.size() == 3);
}

TEST_CASE("DebugConsoleService clear", "[debug_console]")
{
    DebugConsoleService service;

    service.add_output("msg1");
    service.add_output("msg2");
    REQUIRE(service.entry_count() == 2);

    service.clear();
    REQUIRE(service.entry_count() == 0);
}

TEST_CASE("DebugConsoleService command history", "[debug_console]")
{
    DebugConsoleService service;

    service.push_history("first");
    service.push_history("second");
    service.push_history("third");

    REQUIRE(service.history().size() == 3);

    // Navigate up (previous)
    auto cmd = service.navigate_history(-1);
    REQUIRE(cmd == "third");

    cmd = service.navigate_history(-1);
    REQUIRE(cmd == "second");

    // Navigate down (next)
    cmd = service.navigate_history(1);
    REQUIRE(cmd == "third");

    // Navigate past end returns empty
    service.navigate_history(1);
    cmd = service.navigate_history(1);
    REQUIRE(cmd.empty());
}

TEST_CASE("DebugConsoleService history deduplication", "[debug_console]")
{
    DebugConsoleService service;

    service.push_history("same");
    service.push_history("same"); // Should not duplicate
    REQUIRE(service.history().size() == 1);

    service.push_history("different");
    service.push_history("same"); // Different from last, should add
    REQUIRE(service.history().size() == 3);
}

TEST_CASE("DebugConsoleService empty command not added", "[debug_console]")
{
    DebugConsoleService service;
    service.push_history("");
    REQUIRE(service.history().empty());
}

TEST_CASE("DebugConsoleService change listener", "[debug_console]")
{
    DebugConsoleService service;

    int change_count = 0;
    auto listener_id = service.on_change([&change_count]() { change_count++; });

    service.add_output("msg1");
    REQUIRE(change_count == 1);

    service.clear();
    REQUIRE(change_count == 2);

    service.remove_listener(listener_id);
    service.add_output("msg2");
    REQUIRE(change_count == 2); // Not incremented after removal
}

TEST_CASE("DebugConsoleService add_object_result", "[debug_console]")
{
    DebugConsoleService service;

    ConsoleObjectValue obj_val;
    obj_val.name = "result";
    obj_val.value_text = "{x: 1, y: 2}";
    obj_val.type_name = "Point";
    obj_val.expandable = true;

    service.add_object_result("myVar", std::move(obj_val));
    REQUIRE(service.entry_count() == 1);
    REQUIRE(service.entries()[0].type == ConsoleEntryType::kResult);
    REQUIRE(service.entries()[0].has_object == true);
    REQUIRE(service.entries()[0].object_value.type_name == "Point");
}

// ── ExpressionEvaluator tests ──

TEST_CASE("ExpressionEvaluator .help command", "[expression_evaluator]")
{
    ExpressionEvaluator evaluator;

    auto result = evaluator.evaluate(".help");
    REQUIRE(!result.is_error);
    REQUIRE(result.output.find("Available Commands") != std::string::npos);
}

TEST_CASE("ExpressionEvaluator .version command", "[expression_evaluator]")
{
    ExpressionEvaluator evaluator;

    auto result = evaluator.evaluate(".version");
    REQUIRE(!result.is_error);
    REQUIRE(result.output.find("MarkAmp") != std::string::npos);
}

TEST_CASE("ExpressionEvaluator .env command", "[expression_evaluator]")
{
    ExpressionEvaluator evaluator;

    auto result = evaluator.evaluate(".env");
    REQUIRE(!result.is_error);
    REQUIRE(result.output.find("Platform") != std::string::npos);
}

TEST_CASE("ExpressionEvaluator .clear command", "[expression_evaluator]")
{
    ExpressionEvaluator evaluator;

    auto result = evaluator.evaluate(".clear");
    REQUIRE(!result.is_error);
    REQUIRE(result.output == "__CLEAR__");
}

TEST_CASE("ExpressionEvaluator unknown command", "[expression_evaluator]")
{
    ExpressionEvaluator evaluator;

    auto result = evaluator.evaluate(".unknown");
    REQUIRE(result.is_error);
    REQUIRE(result.output.find("Unknown command") != std::string::npos);
}

TEST_CASE("ExpressionEvaluator non-command expression", "[expression_evaluator]")
{
    ExpressionEvaluator evaluator;

    auto result = evaluator.evaluate("2 + 3");
    REQUIRE(!result.is_error);
    REQUIRE(result.output == "2 + 3"); // Echo for now
}

TEST_CASE("ExpressionEvaluator empty expression", "[expression_evaluator]")
{
    ExpressionEvaluator evaluator;

    auto result = evaluator.evaluate("");
    REQUIRE(!result.is_error);
    REQUIRE(result.output.empty());
}

TEST_CASE("ExpressionEvaluator is_command", "[expression_evaluator]")
{
    ExpressionEvaluator evaluator;

    REQUIRE(evaluator.is_command(".help") == true);
    REQUIRE(evaluator.is_command(".clear") == true);
    REQUIRE(evaluator.is_command(".unknown") == false);
    REQUIRE(evaluator.is_command("not_a_command") == false);
    REQUIRE(evaluator.is_command("") == false);
}

TEST_CASE("ExpressionEvaluator custom handler", "[expression_evaluator]")
{
    ExpressionEvaluator evaluator;

    evaluator.register_handler(".custom",
                               [](const std::string& args) -> EvalResult
                               { return {.output = "custom: " + args, .is_error = false}; });

    auto result = evaluator.evaluate(".custom hello world");
    REQUIRE(!result.is_error);
    REQUIRE(result.output == "custom: hello world");

    REQUIRE(evaluator.is_command(".custom") == true);

    evaluator.remove_handler(".custom");
    REQUIRE(evaluator.is_command(".custom") == false);
}

TEST_CASE("ExpressionEvaluator registered_commands", "[expression_evaluator]")
{
    ExpressionEvaluator evaluator;

    auto commands = evaluator.registered_commands();
    REQUIRE(commands.size() >= 6); // .help, .clear, .version, .env, .config, .theme
}

// ── Stack trace detection tests ──

TEST_CASE("detect_stack_traces GCC/Clang pattern", "[stack_trace]")
{
    auto links = DebugConsoleService::detect_stack_traces(
        "/usr/src/main.cpp:42:10: error: undeclared identifier");
    REQUIRE(links.size() == 1);
    REQUIRE(links[0].file == "/usr/src/main.cpp");
    REQUIRE(links[0].line == 42);
    REQUIRE(links[0].column == 10);
}

TEST_CASE("detect_stack_traces GCC without column", "[stack_trace]")
{
    auto links = DebugConsoleService::detect_stack_traces(
        "/home/user/project/test.h:123: warning: unused variable");
    REQUIRE(links.size() == 1);
    REQUIRE(links[0].file == "/home/user/project/test.h");
    REQUIRE(links[0].line == 123);
    REQUIRE(links[0].column == 0);
}

TEST_CASE("detect_stack_traces Python pattern", "[stack_trace]")
{
    auto links =
        DebugConsoleService::detect_stack_traces(R"(  File "/app/server.py", line 55, in main)");
    REQUIRE(links.size() == 1);
    REQUIRE(links[0].file == "/app/server.py");
    REQUIRE(links[0].line == 55);
}

TEST_CASE("detect_stack_traces Node.js pattern", "[stack_trace]")
{
    auto links =
        DebugConsoleService::detect_stack_traces("    at Object.run (/home/user/app.js:99:15)");
    // Both GCC and Node patterns match this text
    REQUIRE(links.size() >= 1);
    // Find the Node.js match (will have the full path)
    bool found = false;
    for (const auto& link : links)
    {
        if (link.file == "/home/user/app.js" && link.line == 99 && link.column == 15)
        {
            found = true;
            break;
        }
    }
    REQUIRE(found);
}

TEST_CASE("detect_stack_traces multiple matches", "[stack_trace]")
{
    std::string text = "/src/a.cpp:1:2: error\n"
                       "/src/b.cpp:3:4: warning\n";
    auto links = DebugConsoleService::detect_stack_traces(text);
    REQUIRE(links.size() == 2);
    REQUIRE(links[0].file == "/src/a.cpp");
    REQUIRE(links[1].file == "/src/b.cpp");
}

TEST_CASE("detect_stack_traces no matches", "[stack_trace]")
{
    auto links = DebugConsoleService::detect_stack_traces("No stack trace here");
    REQUIRE(links.empty());
}

TEST_CASE("detect_stack_traces match position tracking", "[stack_trace]")
{
    auto links = DebugConsoleService::detect_stack_traces("prefix /usr/src/file.cpp:10:5 suffix");
    REQUIRE(links.size() == 1);
    REQUIRE(links[0].match_start == 7);
    REQUIRE(links[0].match_length > 0);
}

// ── Auto-complete tests ──

TEST_CASE("auto_complete returns matching history entries", "[auto_complete]")
{
    DebugConsoleService service;
    service.push_history("console.log('hello')");
    service.push_history("console.error('fail')");
    service.push_history("print('world')");

    auto matches = service.auto_complete("console");
    REQUIRE(matches.size() == 2);
    // Most recent first
    REQUIRE(matches[0] == "console.error('fail')");
    REQUIRE(matches[1] == "console.log('hello')");
}

TEST_CASE("auto_complete returns empty for empty prefix", "[auto_complete]")
{
    DebugConsoleService service;
    service.push_history("hello");
    auto matches = service.auto_complete("");
    REQUIRE(matches.empty());
}

TEST_CASE("auto_complete returns empty when no matches", "[auto_complete]")
{
    DebugConsoleService service;
    service.push_history("hello");
    auto matches = service.auto_complete("xyz");
    REQUIRE(matches.empty());
}

TEST_CASE("auto_complete deduplicates history", "[auto_complete]")
{
    DebugConsoleService service;
    service.push_history("help()");
    service.push_history("hello()");
    service.push_history("help()");

    auto matches = service.auto_complete("hel");
    REQUIRE(matches.size() == 2);
    REQUIRE(matches[0] == "help()"); // most recent
    REQUIRE(matches[1] == "hello()");
}

// ── Object expansion tests ──

TEST_CASE("toggle_expansion toggles object expanded state", "[expansion]")
{
    DebugConsoleService service;
    ConsoleObjectValue obj;
    obj.name = "result";
    obj.value_text = "{...}";
    obj.expandable = true;
    obj.expanded = false;
    service.add_object_result("myVar", std::move(obj));

    REQUIRE(service.entry_count() == 1);
    REQUIRE(service.entries()[0].object_value.expanded == false);

    REQUIRE(service.toggle_expansion(0) == true);
    REQUIRE(service.entries()[0].object_value.expanded == true);

    REQUIRE(service.toggle_expansion(0) == true);
    REQUIRE(service.entries()[0].object_value.expanded == false);
}

TEST_CASE("toggle_expansion returns false for invalid index", "[expansion]")
{
    DebugConsoleService service;
    REQUIRE(service.toggle_expansion(99) == false);
}

// ── Console.log interception tests ──

TEST_CASE("intercept_console_output adds entry with source", "[interception]")
{
    DebugConsoleService service;
    service.intercept_console_output("Hello from script", LogLevel::kWarning);

    REQUIRE(service.entry_count() == 1);
    REQUIRE(service.entries()[0].text == "Hello from script");
    REQUIRE(service.entries()[0].source == "console.log");
    REQUIRE(service.entries()[0].level == LogLevel::kWarning);
}
