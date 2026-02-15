/// @file test_magic_commands.cpp
/// @brief V4 Phase 38 – MagicCommandEngine & EnvironmentManager tests.

#include "core/EventBus.h"
#include "core/MagicCommandEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct MagicFixture
{
    EventBus event_bus;
    MagicCommandEngine engine{event_bus};
    EnvironmentManager env_manager{event_bus};
};

TEST_CASE("MagicCommandEngine: detect line magic", "[magic_commands]")
{
    MagicFixture fixture;

    REQUIRE(fixture.engine.is_magic("%time x = 1"));
    REQUIRE(fixture.engine.is_magic("  %env HOME"));
    REQUIRE(!fixture.engine.is_magic("x = 1"));
    REQUIRE(!fixture.engine.is_magic(""));
}

TEST_CASE("MagicCommandEngine: parse line magic", "[magic_commands]")
{
    MagicFixture fixture;
    auto cmd = fixture.engine.parse("%time x = 1 + 2");

    REQUIRE(cmd.has_value());
    REQUIRE(cmd->type == MagicType::kLine);
    REQUIRE(cmd->name == "time");
    REQUIRE(cmd->args == "x = 1 + 2");
}

TEST_CASE("MagicCommandEngine: parse cell magic", "[magic_commands]")
{
    MagicFixture fixture;
    auto cmd = fixture.engine.parse("%%bash\necho hello\necho world");

    REQUIRE(cmd.has_value());
    REQUIRE(cmd->type == MagicType::kCell);
    REQUIRE(cmd->name == "bash");
    REQUIRE(cmd->body == "echo hello\necho world");
}

TEST_CASE("MagicCommandEngine: execute %time", "[magic_commands]")
{
    MagicFixture fixture;
    auto result = fixture.engine.execute_time("x = 1 + 2");

    REQUIRE(result.success);
    REQUIRE(result.output.find("Wall time:") != std::string::npos);
}

TEST_CASE("MagicCommandEngine: execute %env set and get", "[magic_commands]")
{
    MagicFixture fixture;
    fixture.engine.set_env_vars({{"HOME", "/home/user"}});

    auto get_result = fixture.engine.execute_env("HOME");
    REQUIRE(get_result.success);
    REQUIRE(get_result.output == "/home/user");

    auto set_result = fixture.engine.execute_env("FOO=bar");
    REQUIRE(set_result.success);

    auto get_new = fixture.engine.execute_env("FOO");
    REQUIRE(get_new.output == "bar");
}

TEST_CASE("MagicCommandEngine: execute %who and %whos", "[magic_commands]")
{
    MagicFixture fixture;
    fixture.engine.set_variables({{"x", "int"}, {"df", "DataFrame"}});

    auto who_result = fixture.engine.execute_who("who");
    REQUIRE(who_result.success);
    REQUIRE(who_result.output.find("x") != std::string::npos);

    auto whos_result = fixture.engine.execute_who("whos");
    REQUIRE(whos_result.success);
    REQUIRE(whos_result.output.find("int") != std::string::npos);
    REQUIRE(whos_result.output.find("DataFrame") != std::string::npos);
}

TEST_CASE("MagicCommandEngine: custom magic", "[magic_commands]")
{
    MagicFixture fixture;
    fixture.engine.register_custom("greet",
                                   [](const MagicCommand& cmd) -> MagicResult
                                   {
                                       MagicResult result;
                                       result.success = true;
                                       result.output = "Hello, " + cmd.args + "!";
                                       return result;
                                   });

    auto cmd = fixture.engine.parse("%greet World");
    REQUIRE(cmd.has_value());

    auto result = fixture.engine.execute_magic(*cmd);
    REQUIRE(result.success);
    REQUIRE(result.output == "Hello, World!");
}

TEST_CASE("MagicCommandEngine: available magics includes builtins", "[magic_commands]")
{
    MagicFixture fixture;
    auto magics = fixture.engine.available_magics();

    REQUIRE(magics.size() >= 8);
    // Check a few known builtins.
    bool has_time = false;
    bool has_env = false;
    for (const auto& magic_name : magics)
    {
        if (magic_name == "time")
        {
            has_time = true;
        }
        if (magic_name == "env")
        {
            has_env = true;
        }
    }
    REQUIRE(has_time);
    REQUIRE(has_env);
}

TEST_CASE("EnvironmentManager: add and detect", "[magic_commands]")
{
    MagicFixture fixture;

    EnvironmentInfo env_info;
    env_info.name = "venv";
    env_info.python_version = "3.11";
    env_info.path = "/home/user/.venvs/venv";
    env_info.is_active = true;
    env_info.packages = {"numpy", "pandas"};
    fixture.env_manager.add_environment(env_info);

    auto envs = fixture.env_manager.detect_environments();
    REQUIRE(envs.size() == 1);
    REQUIRE(envs[0].name == "venv");

    auto active = fixture.env_manager.active_environment();
    REQUIRE(active.has_value());
    REQUIRE(active->name == "venv");
}

TEST_CASE("EnvironmentManager: switch environment", "[magic_commands]")
{
    MagicFixture fixture;

    EnvironmentInfo env1;
    env1.name = "base";
    env1.python_version = "3.10";
    env1.is_active = true;
    fixture.env_manager.add_environment(env1);

    EnvironmentInfo env2;
    env2.name = "ml-env";
    env2.python_version = "3.11";
    fixture.env_manager.add_environment(env2);

    REQUIRE(fixture.env_manager.active_environment()->name == "base");

    bool switched = fixture.env_manager.switch_environment("ml-env");
    REQUIRE(switched);
    REQUIRE(fixture.env_manager.active_environment()->name == "ml-env");

    bool bad_switch = fixture.env_manager.switch_environment("nonexistent");
    REQUIRE(!bad_switch);
}
