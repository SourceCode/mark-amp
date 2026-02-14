#include "core/EnvironmentService.h"
#include "core/ExtensionEvents.h"
#include "core/GrammarEngine.h"
#include "core/IPlugin.h"
#include "core/PluginContext.h"
#include "core/ProgressService.h"
#include "core/SnippetEngine.h"
#include "core/TaskRunnerService.h"
#include "core/TerminalService.h"
#include "core/TextEditorService.h"
#include "core/WorkspaceService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ══════════════════════════════════════════
// P1: Command Execution API (Gap 7)
// ══════════════════════════════════════════

TEST_CASE("CommandExecution: execute_command callback", "[p1][commands]")
{
    PluginContext ctx;

    bool command_ran = false;
    std::unordered_map<std::string, std::function<void()>> commands;

    ctx.register_command_handler =
        [&commands](const std::string& cmd_id, std::function<void()> handler)
    { commands[cmd_id] = std::move(handler); };

    ctx.execute_command = [&commands, &command_ran](const std::string& cmd_id) -> bool
    {
        auto iter = commands.find(cmd_id);
        if (iter != commands.end())
        {
            iter->second();
            command_ran = true;
            return true;
        }
        return false;
    };

    ctx.get_commands = [&commands]() -> std::vector<std::string>
    {
        std::vector<std::string> result;
        for (const auto& [cmd_id, handler] : commands)
        {
            result.push_back(cmd_id);
        }
        return result;
    };

    // Register a command
    ctx.register_command_handler("test.hello", []() {});

    // Execute it
    REQUIRE(ctx.execute_command("test.hello"));
    REQUIRE(command_ran);

    // Unknown command
    REQUIRE_FALSE(ctx.execute_command("test.unknown"));

    // List commands
    auto cmd_list = ctx.get_commands();
    REQUIRE(cmd_list.size() == 1);
    REQUIRE(cmd_list[0] == "test.hello");
}

// ══════════════════════════════════════════
// P1: Snippet Engine (Gap 4)
// ══════════════════════════════════════════

TEST_CASE("SnippetEngine: register and expand", "[p1][snippets]")
{
    SnippetEngine engine;

    engine.register_snippet({
        .name = "Callout",
        .prefix = "!callout",
        .body = "> [!NOTE]\n> $0",
        .description = "Insert a callout",
    });

    REQUIRE(engine.count() == 1);

    auto expanded = engine.expand("!callout");
    REQUIRE(expanded.has_value());
    REQUIRE(expanded.value() == "> [!NOTE]\n> ");

    auto missing = engine.expand("!unknown");
    REQUIRE_FALSE(missing.has_value());
}

TEST_CASE("SnippetEngine: placeholder expansion", "[p1][snippets]")
{
    auto result = SnippetEngine::expand_body("Hello ${1:World}!");
    REQUIRE(result == "Hello World!");
}

TEST_CASE("SnippetEngine: choice expansion", "[p1][snippets]")
{
    auto result = SnippetEngine::expand_body("> [!${1|NOTE,TIP,WARNING|}]");
    REQUIRE(result == "> [!NOTE]");
}

TEST_CASE("SnippetEngine: simple tab stop", "[p1][snippets]")
{
    auto result = SnippetEngine::expand_body("# $1\n\n$0");
    REQUIRE(result == "# \n\n");
}

TEST_CASE("SnippetEngine: escaped dollar sign", "[p1][snippets]")
{
    auto result = SnippetEngine::expand_body("Price: \\$10");
    REQUIRE(result == "Price: $10");
}

TEST_CASE("SnippetEngine: JSON parsing", "[p1][snippets]")
{
    SnippetEngine engine;
    const std::string json = R"({
        "Bold": {
            "prefix": "**",
            "body": ["**${1:text}**"],
            "description": "Bold text"
        },
        "Italic": {
            "prefix": "*",
            "body": "*${1:text}*"
        }
    })";

    auto count = engine.register_from_json(json);
    REQUIRE(count == 2);
    REQUIRE(engine.count() == 2);

    auto bold = engine.expand("**");
    REQUIRE(bold.has_value());
    REQUIRE(bold.value() == "**text**");

    auto italic = engine.expand("*");
    REQUIRE(italic.has_value());
    REQUIRE(italic.value() == "*text*");
}

TEST_CASE("SnippetEngine: prefix search", "[p1][snippets]")
{
    SnippetEngine engine;
    engine.register_snippet({.name = "Heading 1", .prefix = "#h1", .body = "# $0"});
    engine.register_snippet({.name = "Heading 2", .prefix = "#h2", .body = "## $0"});
    engine.register_snippet({.name = "Code Block", .prefix = "```", .body = "```$1\n$0\n```"});

    auto matches = engine.get_for_prefix("#h");
    REQUIRE(matches.size() == 2);

    auto all = engine.get_for_prefix("");
    REQUIRE(all.size() == 3);
}

TEST_CASE("SnippetEngine: contribution import", "[p1][snippets]")
{
    SnippetEngine engine;
    std::vector<SnippetContribution> contributions = {
        {.name = "Callout", .trigger = "!callout", .body = "> [!NOTE]\n> $0"},
        {.name = "Link", .trigger = "!link", .body = "[$1]($0)"},
    };
    engine.register_from_contributions(contributions);
    REQUIRE(engine.count() == 2);
    REQUIRE(engine.get_by_name("Callout") != nullptr);
}

TEST_CASE("SnippetEngine: clear", "[p1][snippets]")
{
    SnippetEngine engine;
    engine.register_snippet({.name = "Test", .prefix = "!t", .body = "test"});
    REQUIRE(engine.count() == 1);
    engine.clear();
    REQUIRE(engine.count() == 0);
}

// ══════════════════════════════════════════
// P2: WorkspaceService (Gap 2)
// ══════════════════════════════════════════

TEST_CASE("WorkspaceService: document lifecycle", "[p2][workspace]")
{
    WorkspaceService svc;

    // Open a document
    svc.open_document({
        .uri = "file:///test.md",
        .language_id = "markdown",
        .version = 1,
        .content = "# Hello",
    });

    auto* doc = svc.get_document("file:///test.md");
    REQUIRE(doc != nullptr);
    REQUIRE(doc->content == "# Hello");
    REQUIRE(doc->version == 1);

    // Change the document (full replacement)
    svc.change_document("file:///test.md", 2, {{.range = {}, .text = "# Updated"}});

    doc = svc.get_document("file:///test.md");
    REQUIRE(doc != nullptr);
    REQUIRE(doc->content == "# Updated");
    REQUIRE(doc->version == 2);

    // Close the document
    svc.close_document("file:///test.md");
    REQUIRE(svc.get_document("file:///test.md") == nullptr);
}

TEST_CASE("WorkspaceService: document events", "[p2][workspace]")
{
    WorkspaceService svc;

    bool opened = false;
    bool closed = false;
    bool changed = false;
    bool saved = false;

    svc.on_did_open([&opened](const TextDocumentItem&) { opened = true; });
    svc.on_did_close([&closed](const std::string&) { closed = true; });
    svc.on_did_change([&changed](const TextDocumentChangeEvent&) { changed = true; });
    svc.on_did_save([&saved](const std::string&) { saved = true; });

    svc.open_document({.uri = "file:///test.md", .language_id = "markdown"});
    REQUIRE(opened);

    svc.change_document("file:///test.md", 2, {{.range = {}, .text = "new"}});
    REQUIRE(changed);

    svc.save_document("file:///test.md");
    REQUIRE(saved);

    svc.close_document("file:///test.md");
    REQUIRE(closed);
}

TEST_CASE("WorkspaceService: workspace folders", "[p2][workspace]")
{
    WorkspaceService svc;

    svc.add_workspace_folder({.uri = "file:///project", .name = "My Project"});
    REQUIRE(svc.workspace_folders().size() == 1);
    REQUIRE(svc.workspace_folders()[0].name == "My Project");

    svc.remove_workspace_folder("file:///project");
    REQUIRE(svc.workspace_folders().empty());
}

TEST_CASE("WorkspaceService: text_documents", "[p2][workspace]")
{
    WorkspaceService svc;
    svc.open_document({.uri = "file:///a.md", .language_id = "markdown"});
    svc.open_document({.uri = "file:///b.md", .language_id = "markdown"});

    auto docs = svc.text_documents();
    REQUIRE(docs.size() == 2);
}

TEST_CASE("WorkspaceService: apply_edit", "[p2][workspace]")
{
    WorkspaceService svc;
    svc.open_document(
        {.uri = "file:///test.md", .language_id = "markdown", .version = 1, .content = "old"});

    WorkspaceEditBatch edit;
    edit.edits.push_back({.uri = "file:///test.md", .range = {}, .new_text = "new content"});

    REQUIRE(svc.apply_edit(edit));
    auto* doc = svc.get_document("file:///test.md");
    REQUIRE(doc != nullptr);
    REQUIRE(doc->content == "new content");
}

TEST_CASE("WorkspaceService: remove_listener", "[p2][workspace]")
{
    WorkspaceService svc;
    int call_count = 0;
    auto listener_id = svc.on_did_open([&call_count](const TextDocumentItem&) { ++call_count; });

    svc.open_document({.uri = "file:///a.md", .language_id = "markdown"});
    REQUIRE(call_count == 1);

    svc.remove_listener(listener_id);
    svc.open_document({.uri = "file:///b.md", .language_id = "markdown"});
    REQUIRE(call_count == 1); // Listener was removed
}

// ══════════════════════════════════════════
// P2: TextEditorService (Gap 3)
// ══════════════════════════════════════════

TEST_CASE("TextEditorService: active editor lifecycle", "[p2][editor]")
{
    TextEditorService svc;

    REQUIRE(svc.active_editor() == nullptr);

    TextEditor editor;
    editor.uri = "file:///test.md";
    editor.language_id = "markdown";
    svc.set_active_editor(std::move(editor));

    auto* active = svc.active_editor();
    REQUIRE(active != nullptr);
    REQUIRE(active->uri == "file:///test.md");

    svc.clear_active_editor();
    REQUIRE(svc.active_editor() == nullptr);
}

TEST_CASE("TextEditorService: active editor change events", "[p2][editor]")
{
    TextEditorService svc;

    int change_count = 0;
    bool got_nullptr = false;

    svc.on_did_change_active_editor(
        [&](const TextEditor* editor)
        {
            ++change_count;
            if (editor == nullptr)
                got_nullptr = true;
        });

    TextEditor editor;
    editor.uri = "file:///test.md";
    svc.set_active_editor(std::move(editor));
    REQUIRE(change_count == 1);

    svc.clear_active_editor();
    REQUIRE(change_count == 2);
    REQUIRE(got_nullptr);
}

TEST_CASE("TextEditorService: visible editors", "[p2][editor]")
{
    TextEditorService svc;

    TextEditor editor1;
    editor1.uri = "file:///a.md";
    TextEditor editor2;
    editor2.uri = "file:///b.md";

    svc.add_visible_editor(std::move(editor1));
    svc.add_visible_editor(std::move(editor2));
    REQUIRE(svc.visible_editors().size() == 2);

    svc.remove_visible_editor("file:///a.md");
    REQUIRE(svc.visible_editors().size() == 1);
    REQUIRE(svc.visible_editors()[0].uri == "file:///b.md");
}

TEST_CASE("TextEditorService: show_document", "[p2][editor]")
{
    TextEditorService svc;

    auto* editor = svc.show_document("file:///test.md");
    REQUIRE(editor != nullptr);
    REQUIRE(editor->uri == "file:///test.md");
    REQUIRE(svc.active_editor() != nullptr);
}

TEST_CASE("EditBuilder: accumulate edits", "[p2][editor]")
{
    EditBuilder builder;

    builder.insert({0, 0}, "# Title\n");
    builder.replace({{1, 0}, {1, 5}}, "World");
    builder.delete_range({{2, 0}, {2, 10}});

    REQUIRE(builder.edits().size() == 3);
    REQUIRE(builder.edits()[0].type == EditBuilder::Edit::Type::kInsert);
    REQUIRE(builder.edits()[1].type == EditBuilder::Edit::Type::kReplace);
    REQUIRE(builder.edits()[2].type == EditBuilder::Edit::Type::kDelete);

    builder.clear();
    REQUIRE(builder.edits().empty());
}

// ══════════════════════════════════════════
// P2: ProgressService
// ══════════════════════════════════════════

TEST_CASE("ProgressService: basic progress", "[p2][progress]")
{
    ProgressService svc;

    REQUIRE_FALSE(svc.is_active());

    bool task_ran = false;
    svc.with_progress({.title = "Processing...", .cancellable = true},
                      [&task_ran](ProgressReporter& progress)
                      {
                          progress.report(50, "Halfway...");
                          REQUIRE(progress.percentage() == 50);
                          REQUIRE(progress.message() == "Halfway...");
                          REQUIRE_FALSE(progress.is_cancelled());

                          progress.report(50, "Done!");
                          REQUIRE(progress.percentage() == 100);

                          task_ran = true;
                      });

    REQUIRE(task_ran);
    REQUIRE_FALSE(svc.is_active());
}

TEST_CASE("ProgressService: cancellation", "[p2][progress]")
{
    ProgressService svc;

    svc.with_progress({.title = "Cancellable", .cancellable = true},
                      [](ProgressReporter& progress)
                      {
                          progress.report(25);
                          progress.cancel();
                          REQUIRE(progress.is_cancelled());
                      });
}

// ══════════════════════════════════════════
// P2+P3: Extension Events (Gap 5)
// ══════════════════════════════════════════

TEST_CASE("ExtensionEventBus: will_save event", "[p2][events]")
{
    ExtensionEventBus bus;

    std::string saved_uri;
    bus.on_will_save([&saved_uri](const TextDocumentWillSaveEvent& event)
                     { saved_uri = event.uri; });

    bus.fire_will_save({.uri = "file:///test.md", .reason = TextDocumentSaveReason::kManual});
    REQUIRE(saved_uri == "file:///test.md");
}

TEST_CASE("ExtensionEventBus: config change event", "[p2][events]")
{
    ExtensionEventBus bus;

    std::vector<std::string> affected;
    bus.on_did_change_configuration([&affected](const ConfigurationChangeEvent& event)
                                    { affected = event.affected_sections; });

    bus.fire_configuration_change({.affected_sections = {"editor.fontSize", "editor.tabSize"}});
    REQUIRE(affected.size() == 2);
    REQUIRE(affected[0] == "editor.fontSize");
}

TEST_CASE("ExtensionEventBus: remove_listener", "[p2][events]")
{
    ExtensionEventBus bus;

    int count = 0;
    auto listener_id = bus.on_will_save([&count](const TextDocumentWillSaveEvent&) { ++count; });

    bus.fire_will_save({.uri = "file:///a.md"});
    REQUIRE(count == 1);

    bus.remove_listener(listener_id);
    bus.fire_will_save({.uri = "file:///b.md"});
    REQUIRE(count == 1); // Listener removed
}

// ══════════════════════════════════════════
// P3: EnvironmentService (Gap 6)
// ══════════════════════════════════════════

TEST_CASE("EnvironmentService: app identity", "[p3][environment]")
{
    EnvironmentService svc;

    REQUIRE(svc.app_name() == "MarkAmp");
    REQUIRE(svc.uri_scheme() == "markamp");
    REQUIRE_FALSE(svc.language().empty());
    REQUIRE_FALSE(svc.machine_id().empty());
}

TEST_CASE("EnvironmentService: clipboard round-trip", "[p3][environment]")
{
    EnvironmentService svc;

    svc.clipboard_write("Hello, clipboard!");
    REQUIRE(svc.clipboard_read() == "Hello, clipboard!");
}

TEST_CASE("EnvironmentService: open_external", "[p3][environment]")
{
    EnvironmentService svc;

    REQUIRE(svc.open_external("https://example.com"));
    REQUIRE_FALSE(svc.open_external("")); // Empty URI fails
}

TEST_CASE("EnvironmentService: overrides for testing", "[p3][environment]")
{
    EnvironmentService svc;

    svc.set_app_root("/usr/local/markamp");
    REQUIRE(svc.app_root() == "/usr/local/markamp");

    svc.set_language("fr");
    REQUIRE(svc.language() == "fr");

    svc.set_machine_id("test-machine-123");
    REQUIRE(svc.machine_id() == "test-machine-123");
}

// ══════════════════════════════════════════
// P3: GrammarEngine Stub (Gap 8)
// ══════════════════════════════════════════

TEST_CASE("GrammarEngine: stub returns defaults", "[p3][grammar]")
{
    GrammarEngine engine;

    REQUIRE_FALSE(engine.load_grammar("/path/to/grammar.json"));
    REQUIRE(engine.get_grammar("source.markdown") == nullptr);
    REQUIRE(engine.tokenize_line("source.markdown", "# Hello").empty());
    REQUIRE(engine.grammars().empty());
}

// ══════════════════════════════════════════
// P4: Terminal Stub (Gap 9)
// ══════════════════════════════════════════

TEST_CASE("TerminalService: stub returns empty", "[p4][terminal]")
{
    TerminalService svc;

    auto terminal = svc.create_terminal("bash", "/bin/bash");
    REQUIRE_FALSE(terminal.has_value());

    REQUIRE_FALSE(svc.send_text(0, "echo hello"));
    REQUIRE_FALSE(svc.close_terminal(0));
    REQUIRE(svc.terminals().empty());
}

// ══════════════════════════════════════════
// P4: Task Runner Stub (Gap 10)
// ══════════════════════════════════════════

TEST_CASE("TaskRunnerService: stub returns empty", "[p4][tasks]")
{
    TaskRunnerService svc;

    REQUIRE_FALSE(
        svc.register_task_provider("shell", []() -> std::vector<TaskDefinition> { return {}; }));
    REQUIRE_FALSE(svc.execute_task("build"));
    REQUIRE(svc.get_tasks().empty());
}

// ══════════════════════════════════════════
// Integration: PluginContext with all P1-P4 services
// ══════════════════════════════════════════

TEST_CASE("PluginContext: wire up all P1-P4 services", "[integration]")
{
    // Instantiate all services
    SnippetEngine snippet_engine;
    WorkspaceService workspace_svc;
    TextEditorService text_editor_svc;
    ProgressService progress_svc;
    ExtensionEventBus event_bus;
    EnvironmentService env_svc;
    GrammarEngine grammar_engine;
    TerminalService terminal_svc;
    TaskRunnerService task_runner_svc;

    // Wire into PluginContext
    PluginContext ctx;
    ctx.snippet_engine = &snippet_engine;
    ctx.workspace_service = &workspace_svc;
    ctx.text_editor_service = &text_editor_svc;
    ctx.progress_service = &progress_svc;
    ctx.extension_event_bus = &event_bus;
    ctx.environment_service = &env_svc;
    ctx.grammar_engine = &grammar_engine;
    ctx.terminal_service = &terminal_svc;
    ctx.task_runner_service = &task_runner_svc;

    // Verify all are wired
    REQUIRE(ctx.snippet_engine != nullptr);
    REQUIRE(ctx.workspace_service != nullptr);
    REQUIRE(ctx.text_editor_service != nullptr);
    REQUIRE(ctx.progress_service != nullptr);
    REQUIRE(ctx.extension_event_bus != nullptr);
    REQUIRE(ctx.environment_service != nullptr);
    REQUIRE(ctx.grammar_engine != nullptr);
    REQUIRE(ctx.terminal_service != nullptr);
    REQUIRE(ctx.task_runner_service != nullptr);

    // Quick smoke test: verify services are usable through context
    ctx.snippet_engine->register_snippet({.name = "Test", .prefix = "!test", .body = "test $0"});
    REQUIRE(ctx.snippet_engine->count() == 1);

    ctx.workspace_service->open_document({.uri = "file:///ctx.md", .language_id = "markdown"});
    REQUIRE(ctx.workspace_service->get_document("file:///ctx.md") != nullptr);

    REQUIRE(ctx.environment_service->app_name() == "MarkAmp");
}
