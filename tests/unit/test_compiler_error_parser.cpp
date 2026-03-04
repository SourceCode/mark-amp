#include "core/BuildService.h"
#include "core/CompilerErrorParser.h"
#include "core/TaskConfig.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ── CompilerErrorParser tests ──

TEST_CASE("Parse GCC error format", "[compiler_error_parser]")
{
    CompilerErrorParser parser(CompilerType::kGcc);

    auto errors =
        parser.parse_line("/home/user/src/main.cpp:42:10: error: expected ';' after expression");

    REQUIRE(errors.size() == 1);
    REQUIRE(errors[0].file == "/home/user/src/main.cpp");
    REQUIRE(errors[0].line == 42);
    REQUIRE(errors[0].column == 10);
    REQUIRE(errors[0].severity == DiagnosticSeverity::kError);
    REQUIRE(errors[0].message == "expected ';' after expression");
}

TEST_CASE("Parse GCC warning format", "[compiler_error_parser]")
{
    CompilerErrorParser parser(CompilerType::kGcc);

    auto errors = parser.parse_line("/src/foo.cpp:100:5: warning: unused variable 'x'");

    REQUIRE(errors.size() == 1);
    REQUIRE(errors[0].severity == DiagnosticSeverity::kWarning);
    REQUIRE(errors[0].line == 100);
}

TEST_CASE("Parse GCC note format", "[compiler_error_parser]")
{
    CompilerErrorParser parser(CompilerType::kGcc);

    auto errors = parser.parse_line("/src/foo.cpp:50:10: note: candidate function not viable");

    REQUIRE(errors.size() == 1);
    REQUIRE(errors[0].severity == DiagnosticSeverity::kInformation);
}

TEST_CASE("Parse Clang error - same as GCC", "[compiler_error_parser]")
{
    CompilerErrorParser parser(CompilerType::kClang);

    auto errors =
        parser.parse_line("src/widget.cpp:15:20: error: use of undeclared identifier 'foo'");

    REQUIRE(errors.size() == 1);
    REQUIRE(errors[0].file == "src/widget.cpp");
    REQUIRE(errors[0].line == 15);
    REQUIRE(errors[0].column == 20);
}

TEST_CASE("Parse MSVC error format", "[compiler_error_parser]")
{
    CompilerErrorParser parser(CompilerType::kMsvc);

    auto errors = parser.parse_line(
        R"(C:\Users\dev\src\main.cpp(42): error C2065: 'foo': undeclared identifier)");

    REQUIRE(errors.size() == 1);
    REQUIRE(errors[0].file == R"(C:\Users\dev\src\main.cpp)");
    REQUIRE(errors[0].line == 42);
    REQUIRE(errors[0].code == "C2065");
    REQUIRE(errors[0].severity == DiagnosticSeverity::kError);
}

TEST_CASE("Parse CMake error format", "[compiler_error_parser]")
{
    CompilerErrorParser parser(CompilerType::kCMake);

    auto errors = parser.parse_line(
        "CMake Error at CMakeLists.txt:15 (find_package): Could not find package Foo");

    REQUIRE(errors.size() == 1);
    REQUIRE(errors[0].file == "CMakeLists.txt");
    REQUIRE(errors[0].line == 15);
    REQUIRE(errors[0].severity == DiagnosticSeverity::kError);
}

TEST_CASE("Auto-detect compiler from GCC output", "[compiler_error_parser]")
{
    REQUIRE(CompilerErrorParser::detect_compiler("foo.cpp:10: error: msg") == CompilerType::kGcc);
}

TEST_CASE("Auto-detect compiler from MSVC output", "[compiler_error_parser]")
{
    REQUIRE(CompilerErrorParser::detect_compiler("foo.cpp(10): error C2000: msg") ==
            CompilerType::kMsvc);
}

TEST_CASE("Auto-detect compiler from CMake output", "[compiler_error_parser]")
{
    REQUIRE(CompilerErrorParser::detect_compiler("CMake Error at file.cmake:1:") ==
            CompilerType::kCMake);
}

TEST_CASE("Parse non-error lines returns empty", "[compiler_error_parser]")
{
    CompilerErrorParser parser;
    auto errors = parser.parse_line("Building CXX object src/main.cpp.o");
    REQUIRE(errors.empty());
}

TEST_CASE("to_diagnostics converts correctly", "[compiler_error_parser]")
{
    ParsedCompilerError error;
    error.file = "test.cpp";
    error.line = 10;
    error.column = 5;
    error.severity = DiagnosticSeverity::kError;
    error.message = "test error";
    error.code = "E001";

    auto diags = CompilerErrorParser::to_diagnostics({error});
    REQUIRE(diags.size() == 1);
    REQUIRE(diags[0].range.start.line == 9); // 0-indexed
    REQUIRE(diags[0].range.start.character == 4);
    REQUIRE(diags[0].message == "test error");
    REQUIRE(diags[0].source == "compiler");
}

TEST_CASE("Multi-line parse", "[compiler_error_parser]")
{
    CompilerErrorParser parser;

    auto errors = parser.parse("foo.cpp:1:1: error: first error\n"
                               "bar.cpp:2:1: warning: a warning\n"
                               "Just some normal output\n"
                               "baz.cpp:3:1: error: second error\n");

    REQUIRE(errors.size() == 3);
}

// ── TaskConfig tests ──

TEST_CASE("TaskConfig CMake build template", "[task_config]")
{
    auto task = TaskConfig::cmake_build_template("build/debug", "Debug");

    REQUIRE(task.name == "cmake-build");
    REQUIRE(task.label == "CMake: Build");
    REQUIRE(task.command == "cmake");
    REQUIRE(task.group == TaskGroup::kBuild);
    REQUIRE(task.is_default == true);
    REQUIRE(task.args.size() == 4);
}

TEST_CASE("TaskConfig Make build template", "[task_config]")
{
    auto task = TaskConfig::make_build_template("all");

    REQUIRE(task.name == "make-build");
    REQUIRE(task.command == "make");
    REQUIRE(task.group == TaskGroup::kBuild);
}

TEST_CASE("TaskConfig CTest template", "[task_config]")
{
    auto task = TaskConfig::ctest_template("build");

    REQUIRE(task.name == "ctest");
    REQUIRE(task.command == "ctest");
    REQUIRE(task.group == TaskGroup::kTest);
    REQUIRE(task.is_default == true);
}

TEST_CASE("TaskConfig add and find tasks", "[task_config]")
{
    TaskConfig config;

    config.add_task(TaskConfig::cmake_build_template());
    config.add_task(TaskConfig::ctest_template());

    REQUIRE(config.tasks().size() == 2);

    auto* found = config.find_task("cmake-build");
    REQUIRE(found != nullptr);
    REQUIRE(found->command == "cmake");

    auto* not_found = config.find_task("nonexistent");
    REQUIRE(not_found == nullptr);
}

TEST_CASE("TaskConfig tasks_by_group", "[task_config]")
{
    TaskConfig config;
    config.add_task(TaskConfig::cmake_build_template());
    config.add_task(TaskConfig::make_build_template());
    config.add_task(TaskConfig::ctest_template());

    auto build_tasks = config.tasks_by_group(TaskGroup::kBuild);
    REQUIRE(build_tasks.size() == 2);

    auto test_tasks = config.tasks_by_group(TaskGroup::kTest);
    REQUIRE(test_tasks.size() == 1);
}

TEST_CASE("TaskConfig remove task", "[task_config]")
{
    TaskConfig config;
    config.add_task(TaskConfig::cmake_build_template());
    REQUIRE(config.tasks().size() == 1);

    config.remove_task("cmake-build");
    REQUIRE(config.tasks().empty());
}

TEST_CASE("TaskConfig JSON serialization", "[task_config]")
{
    TaskConfig config;
    config.add_task(TaskConfig::cmake_build_template());

    auto json = config.to_json();
    REQUIRE(json.find("\"version\": \"2.0.0\"") != std::string::npos);
    REQUIRE(json.find("CMake: Build") != std::string::npos);
    REQUIRE(json.find("cmake") != std::string::npos);
}

TEST_CASE("TaskDefinition full_command", "[task_config]")
{
    TaskDefinition task;
    task.command = "cmake";
    task.args = {"--build", "build", "--config", "Debug"};

    REQUIRE(task.full_command() == "cmake --build build --config Debug");
}

TEST_CASE("task_group_name and from_string", "[task_config]")
{
    REQUIRE(std::string(task_group_name(TaskGroup::kBuild)) == "build");
    REQUIRE(std::string(task_group_name(TaskGroup::kTest)) == "test");
    REQUIRE(std::string(task_group_name(TaskGroup::kDeploy)) == "deploy");
    REQUIRE(std::string(task_group_name(TaskGroup::kOther)) == "other");

    REQUIRE(task_group_from_string("build") == TaskGroup::kBuild);
    REQUIRE(task_group_from_string("test") == TaskGroup::kTest);
    REQUIRE(task_group_from_string("deploy") == TaskGroup::kDeploy);
    REQUIRE(task_group_from_string("unknown") == TaskGroup::kOther);
}

// ── BuildService tests ──

TEST_CASE("BuildService initial state", "[build_service]")
{
    BuildService service;

    REQUIRE(service.status() == BuildStatus::kIdle);
    REQUIRE(service.is_building() == false);
    REQUIRE(service.build_system() == BuildSystem::kCMake);
    REQUIRE(service.active_configuration() == "Debug");
}

TEST_CASE("BuildService start and stop build", "[build_service]")
{
    BuildService service;

    REQUIRE(service.start_build("all", "Debug") == true);
    REQUIRE(service.status() == BuildStatus::kBuilding);
    REQUIRE(service.is_building() == true);

    // Cannot start another build while one is running
    REQUIRE(service.start_build() == false);

    service.stop_build();
    REQUIRE(service.status() == BuildStatus::kIdle);
}

TEST_CASE("BuildService Ninja progress parsing", "[build_service]")
{
    BuildService service;
    service.start_build();

    service.process_output_line("[1/10] Building CXX object src/main.cpp.o");
    REQUIRE(service.progress().completed == 1);
    REQUIRE(service.progress().total == 10);
    REQUIRE(service.progress().percentage > 0.0F);

    service.process_output_line("[5/10] Building CXX object src/util.cpp.o");
    REQUIRE(service.progress().completed == 5);
}

TEST_CASE("BuildService error counting", "[build_service]")
{
    BuildService service;
    service.start_build();

    service.process_output_line("src/main.cpp:10:5: error: undeclared identifier");
    service.process_output_line("src/main.cpp:20:5: warning: unused variable");

    REQUIRE(service.last_result().error_count == 1);
    REQUIRE(service.last_result().warning_count == 1);
    REQUIRE(service.parsed_errors().size() == 2);
}

TEST_CASE("BuildService default configurations", "[build_service]")
{
    BuildService service;
    auto configs = service.configurations();

    REQUIRE(configs.size() == 4);
    REQUIRE(configs[0].name == "Debug");
    REQUIRE(configs[1].name == "Release");
}

TEST_CASE("BuildService discover_targets", "[build_service]")
{
    BuildService service;
    service.discover_targets();

    auto targets = service.targets();
    REQUIRE(targets.size() == 3);
    REQUIRE(targets[0].name == "all");
    REQUIRE(targets[0].is_default == true);
}
