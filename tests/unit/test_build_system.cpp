/// @file test_build_system.cpp
/// @brief Phase 38 — Unit tests for BuildOutputParser, CMakeIntegration,
///        MakeIntegration, BuildTaskManager, BuildDependencyGraph,
///        IncrementalBuildTracker, BuildLogManager.

#include "core/BuildDependencyGraph.h"
#include "core/BuildLogManager.h"
#include "core/BuildOutputParser.h"
#include "core/BuildTaskManager.h"
#include "core/CMakeIntegration.h"
#include "core/IncrementalBuildTracker.h"
#include "core/MakeIntegration.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;

// ============================================================================
// BuildOutputParser
// ============================================================================

TEST_CASE("BuildOutputParser: GCC/Clang error format", "[build_parser]")
{
    BuildOutputParser parser;
    auto diags =
        parser.parse_line("/src/main.cpp:42:10: error: use of undeclared identifier 'foo'");
    REQUIRE(diags.size() == 1);
    REQUIRE(diags[0].file == "/src/main.cpp");
    REQUIRE(diags[0].line == 42);
    REQUIRE(diags[0].column == 10);
    REQUIRE(diags[0].severity == BuildDiagnosticSeverity::kError);
    REQUIRE(diags[0].message.find("undeclared identifier") != std::string::npos);
}

TEST_CASE("BuildOutputParser: GCC/Clang warning format", "[build_parser]")
{
    BuildOutputParser parser;
    auto diags =
        parser.parse_line("/src/util.cpp:10:5: warning: unused variable 'x' [-Wunused-variable]");
    REQUIRE(diags.size() == 1);
    REQUIRE(diags[0].severity == BuildDiagnosticSeverity::kWarning);
    REQUIRE(diags[0].error_code == "-Wunused-variable");
}

TEST_CASE("BuildOutputParser: GCC format without column", "[build_parser]")
{
    BuildOutputParser parser;
    auto diags = parser.parse_line("/src/main.cpp:42: error: something failed");
    REQUIRE(diags.size() == 1);
    REQUIRE(diags[0].file == "/src/main.cpp");
    REQUIRE(diags[0].line == 42);
    REQUIRE(diags[0].column == 0);
}

TEST_CASE("BuildOutputParser: MSVC error format", "[build_parser]")
{
    BuildOutputParser parser;
    auto diags = parser.parse_line("main.cpp(42,10): error C2065: 'foo': undeclared identifier");
    REQUIRE(diags.size() == 1);
    REQUIRE(diags[0].file == "main.cpp");
    REQUIRE(diags[0].line == 42);
    REQUIRE(diags[0].column == 10);
    REQUIRE(diags[0].error_code == "C2065");
}

TEST_CASE("BuildOutputParser: CMake error format", "[build_parser]")
{
    BuildOutputParser parser;
    auto diags =
        parser.parse_line("CMake Error at CMakeLists.txt:15 (find_package): Could not find Boost");
    REQUIRE(diags.size() == 1);
    REQUIRE(diags[0].file == "CMakeLists.txt");
    REQUIRE(diags[0].line == 15);
    REQUIRE(diags[0].severity == BuildDiagnosticSeverity::kError);
}

TEST_CASE("BuildOutputParser: non-diagnostic line", "[build_parser]")
{
    BuildOutputParser parser;
    auto diags = parser.parse_line("Scanning dependencies of target myapp");
    REQUIRE(diags.empty());
}

TEST_CASE("BuildOutputParser: multi-line parse", "[build_parser]")
{
    BuildOutputParser parser;
    std::string output = "/a.cpp:1:1: error: missing include\n"
                         "Building target...\n"
                         "/b.cpp:5:3: warning: unused [-Wunused]";
    auto diags = parser.parse(output);
    REQUIRE(diags.size() == 2);
    REQUIRE(BuildOutputParser::error_count(diags) == 1);
    REQUIRE(BuildOutputParser::warning_count(diags) == 1);
}

TEST_CASE("BuildOutputParser: format_diagnostic", "[build_parser]")
{
    BuildDiagnostic diag;
    diag.file = "main.cpp";
    diag.line = 10;
    diag.column = 5;
    diag.severity = BuildDiagnosticSeverity::kError;
    diag.message = "undeclared";
    diag.error_code = "C2065";
    auto formatted = BuildOutputParser::format_diagnostic(diag);
    REQUIRE(formatted.find("main.cpp:10:5") != std::string::npos);
    REQUIRE(formatted.find("error") != std::string::npos);
    REQUIRE(formatted.find("C2065") != std::string::npos);
}

TEST_CASE("BuildOutputParser: fatal error", "[build_parser]")
{
    BuildOutputParser parser;
    auto diags = parser.parse_line("/src/main.cpp:1:10: fatal error: 'missing.h' file not found");
    REQUIRE(diags.size() == 1);
    REQUIRE(diags[0].severity == BuildDiagnosticSeverity::kError);
}

// ============================================================================
// CMakeIntegration
// ============================================================================

TEST_CASE("CMakeIntegration: configure_command", "[cmake_integration]")
{
    CMakeIntegration cmake;
    auto cmd = cmake.configure_command("/project", "build", "Release");
    REQUIRE(cmd.find("-S /project") != std::string::npos);
    REQUIRE(cmd.find("-B build") != std::string::npos);
    REQUIRE(cmd.find("CMAKE_BUILD_TYPE=Release") != std::string::npos);
}

TEST_CASE("CMakeIntegration: build_command", "[cmake_integration]")
{
    CMakeIntegration cmake;
    auto cmd = cmake.build_command("build", "myapp", 4);
    REQUIRE(cmd.find("--build build") != std::string::npos);
    REQUIRE(cmd.find("--target myapp") != std::string::npos);
    REQUIRE(cmd.find("-j4") != std::string::npos);
}

TEST_CASE("CMakeIntegration: clean_command", "[cmake_integration]")
{
    CMakeIntegration cmake;
    auto cmd = cmake.clean_command("build");
    REQUIRE(cmd.find("--target clean") != std::string::npos);
}

TEST_CASE("CMakeIntegration: configure_with_preset", "[cmake_integration]")
{
    CMakeIntegration cmake;
    auto cmd = cmake.configure_with_preset("/project", "debug");
    REQUIRE(cmd.find("--preset debug") != std::string::npos);
}

TEST_CASE("CMakeIntegration: detect_project non-existent", "[cmake_integration]")
{
    CMakeIntegration cmake;
    REQUIRE_FALSE(cmake.detect_project("/nonexistent_path_12345"));
}

// ============================================================================
// MakeIntegration
// ============================================================================

TEST_CASE("MakeIntegration: build_command", "[make_integration]")
{
    MakeIntegration make;
    auto cmd = make.build_command("all", "/project", 8);
    REQUIRE(cmd.find("-C /project") != std::string::npos);
    REQUIRE(cmd.find("all") != std::string::npos);
    REQUIRE(cmd.find("-j8") != std::string::npos);
}

TEST_CASE("MakeIntegration: clean_command", "[make_integration]")
{
    MakeIntegration make;
    auto cmd = make.clean_command("/project");
    REQUIRE(cmd.find("clean") != std::string::npos);
}

TEST_CASE("MakeIntegration: detect_makefile non-existent", "[make_integration]")
{
    MakeIntegration make;
    REQUIRE_FALSE(make.detect_makefile("/nonexistent_path_12345"));
}

// ============================================================================
// BuildTaskManager
// ============================================================================

TEST_CASE("BuildTaskManager: add and find task", "[build_tasks]")
{
    BuildTaskManager mgr;
    BuildTask task;
    task.label = "Build All";
    task.command = "cmake";
    task.group = "build";
    task.is_default = true;
    mgr.add_task(task);

    REQUIRE(mgr.task_count() == 1);
    auto* found = mgr.find_task("Build All");
    REQUIRE(found != nullptr);
    REQUIRE(found->command == "cmake");
}

TEST_CASE("BuildTaskManager: remove task", "[build_tasks]")
{
    BuildTaskManager mgr;
    BuildTask task;
    task.label = "Test";
    task.command = "ctest";
    mgr.add_task(task);
    REQUIRE(mgr.remove_task("Test"));
    REQUIRE(mgr.task_count() == 0);
    REQUIRE_FALSE(mgr.remove_task("NonExistent"));
}

TEST_CASE("BuildTaskManager: tasks_for_group", "[build_tasks]")
{
    BuildTaskManager mgr;
    BuildTask t1{.label = "Build", .command = "make", .group = "build"};
    BuildTask t2{.label = "Test", .command = "ctest", .group = "test"};
    BuildTask t3{.label = "Lint", .command = "clang-tidy", .group = "build"};
    mgr.add_task(t1);
    mgr.add_task(t2);
    mgr.add_task(t3);
    auto build_tasks = mgr.tasks_for_group("build");
    REQUIRE(build_tasks.size() == 2);
}

TEST_CASE("BuildTaskManager: default task", "[build_tasks]")
{
    BuildTaskManager mgr;
    BuildTask t1{.label = "Build", .command = "make", .group = "build", .is_default = true};
    BuildTask t2{.label = "Lint", .command = "tidy", .group = "build"};
    mgr.add_task(t1);
    mgr.add_task(t2);
    auto* def = mgr.default_task("build");
    REQUIRE(def != nullptr);
    REQUIRE(def->label == "Build");
}

TEST_CASE("BuildTaskManager: expand_variables", "[build_tasks]")
{
    BuildTaskManager mgr;
    auto result =
        mgr.expand_variables("${workspaceFolder}/build", "/project", "/project/src/main.cpp");
    REQUIRE(result == "/project/build");

    result = mgr.expand_variables("${file}", "/project", "/project/src/main.cpp");
    REQUIRE(result == "/project/src/main.cpp");

    result = mgr.expand_variables("${fileBasename}", "/project", "/project/src/main.cpp");
    REQUIRE(result == "main.cpp");
}

TEST_CASE("BuildTaskManager: replace existing task", "[build_tasks]")
{
    BuildTaskManager mgr;
    BuildTask t1{.label = "Build", .command = "make"};
    mgr.add_task(t1);
    BuildTask t2{.label = "Build", .command = "ninja"};
    mgr.add_task(t2);
    REQUIRE(mgr.task_count() == 1);
    REQUIRE(mgr.find_task("Build")->command == "ninja");
}

// ============================================================================
// BuildDependencyGraph
// ============================================================================

TEST_CASE("BuildDependencyGraph: add nodes and edges", "[build_graph]")
{
    BuildDependencyGraph graph;
    graph.add_node("app");
    graph.add_node("libcore");
    graph.add_edge("app", "libcore");
    REQUIRE(graph.node_count() == 2);
    REQUIRE(graph.edge_count() == 1);
}

TEST_CASE("BuildDependencyGraph: dependencies_of", "[build_graph]")
{
    BuildDependencyGraph graph;
    graph.add_edge("app", "libcore");
    graph.add_edge("app", "libutil");
    auto deps = graph.dependencies_of("app");
    REQUIRE(deps.size() == 2);
}

TEST_CASE("BuildDependencyGraph: transitive_dependencies", "[build_graph]")
{
    BuildDependencyGraph graph;
    graph.add_edge("app", "libcore");
    graph.add_edge("libcore", "libbase");
    auto transitive = graph.transitive_dependencies("app");
    REQUIRE(transitive.size() == 2);
}

TEST_CASE("BuildDependencyGraph: dependents_of", "[build_graph]")
{
    BuildDependencyGraph graph;
    graph.add_edge("app", "libcore");
    graph.add_edge("tests", "libcore");
    auto dependents = graph.dependents_of("libcore");
    REQUIRE(dependents.size() == 2);
}

TEST_CASE("BuildDependencyGraph: topological_sort", "[build_graph]")
{
    BuildDependencyGraph graph;
    graph.add_edge("app", "libcore");
    graph.add_edge("libcore", "libbase");
    auto sorted = graph.topological_sort();
    REQUIRE(sorted.size() == 3);
}

TEST_CASE("BuildDependencyGraph: has_cycles false", "[build_graph]")
{
    BuildDependencyGraph graph;
    graph.add_edge("a", "b");
    graph.add_edge("b", "c");
    REQUIRE_FALSE(graph.has_cycles());
}

TEST_CASE("BuildDependencyGraph: has_cycles true", "[build_graph]")
{
    BuildDependencyGraph graph;
    graph.add_edge("a", "b");
    graph.add_edge("b", "c");
    graph.add_edge("c", "a");
    REQUIRE(graph.has_cycles());
}

TEST_CASE("BuildDependencyGraph: clear", "[build_graph]")
{
    BuildDependencyGraph graph;
    graph.add_edge("a", "b");
    graph.clear();
    REQUIRE(graph.node_count() == 0);
    REQUIRE(graph.edge_count() == 0);
}

TEST_CASE("BuildDependencyGraph: parse_dot", "[build_graph]")
{
    BuildDependencyGraph graph;
    std::string dot = R"(digraph {
        "app" -> "libcore";
        "app" -> "libutil";
        "libcore" -> "libbase";
    })";
    graph.parse_dot(dot);
    REQUIRE(graph.node_count() == 4);
    REQUIRE(graph.edge_count() == 3);
}

// ============================================================================
// IncrementalBuildTracker
// ============================================================================

TEST_CASE("IncrementalBuildTracker: mark states", "[incremental_build]")
{
    IncrementalBuildTracker tracker;
    tracker.mark_modified("main.cpp");
    REQUIRE(tracker.tracked_count() == 1);
    REQUIRE(tracker.dirty_count() == 1);

    tracker.mark_building("main.cpp");
    REQUIRE(tracker.dirty_count() == 0);

    tracker.mark_built("main.cpp");
    auto* info = tracker.get_file("main.cpp");
    REQUIRE(info != nullptr);
    REQUIRE(info->status == FileBuildStatus::kUpToDate);
}

TEST_CASE("IncrementalBuildTracker: mark_failed", "[incremental_build]")
{
    IncrementalBuildTracker tracker;
    tracker.mark_failed("bad.cpp", 3);
    REQUIRE(tracker.failed_count() == 1);
    auto* info = tracker.get_file("bad.cpp");
    REQUIRE(info != nullptr);
    REQUIRE(info->error_count == 3);
}

TEST_CASE("IncrementalBuildTracker: files_with_status", "[incremental_build]")
{
    IncrementalBuildTracker tracker;
    tracker.mark_modified("a.cpp");
    tracker.mark_modified("b.cpp");
    tracker.mark_built("c.cpp");
    auto modified = tracker.files_with_status(FileBuildStatus::kModified);
    REQUIRE(modified.size() == 2);
}

TEST_CASE("IncrementalBuildTracker: reset_all", "[incremental_build]")
{
    IncrementalBuildTracker tracker;
    tracker.mark_built("a.cpp");
    tracker.mark_failed("b.cpp");
    tracker.reset_all();
    REQUIRE(tracker.dirty_count() == 0);
    REQUIRE(tracker.failed_count() == 0);
}

// ============================================================================
// BuildLogManager
// ============================================================================

TEST_CASE("BuildLogManager: add and retrieve entries", "[build_log]")
{
    BuildLogManager mgr;
    BuildLogEntry entry;
    entry.timestamp = "2026-01-01T00:00:00Z";
    entry.build_config = "Debug";
    entry.success = true;
    entry.error_count = 0;
    entry.warning_count = 2;
    mgr.add_entry(entry);

    REQUIRE(mgr.entry_count() == 1);
    auto* latest = mgr.latest();
    REQUIRE(latest != nullptr);
    REQUIRE(latest->success);
    REQUIRE(latest->warning_count == 2);
}

TEST_CASE("BuildLogManager: export_text", "[build_log]")
{
    BuildLogEntry entry;
    entry.timestamp = "2026-01-01";
    entry.build_config = "Release";
    entry.success = true;
    auto text = BuildLogManager::export_text(entry);
    REQUIRE(text.find("Release") != std::string::npos);
    REQUIRE(text.find("SUCCESS") != std::string::npos);
}

TEST_CASE("BuildLogManager: compare", "[build_log]")
{
    BuildLogEntry a, b;
    a.timestamp = "Build A";
    a.error_count = 5;
    a.warning_count = 10;
    b.timestamp = "Build B";
    b.error_count = 3;
    b.warning_count = 8;
    auto diff = BuildLogManager::compare(a, b);
    REQUIRE(diff.find("Errors") != std::string::npos);
}
