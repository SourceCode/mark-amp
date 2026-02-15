/// @file test_kernel_adapters.cpp
/// @brief V4 Phase 30 – Kernel Adapters unit tests.

#include "core/KernelAdapters.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("KernelAdapters: Python adapter language", "[adapters]")
{
    PythonAdapter adapter;
    REQUIRE(adapter.language() == "python");
}

TEST_CASE("KernelAdapters: Python magic preprocessing %time", "[adapters]")
{
    PythonAdapter adapter;
    std::string code = "%time x = 42";
    std::string result = adapter.preprocess_code(code);

    REQUIRE(result.find("import time") != std::string::npos);
    REQUIRE(result.find("x = 42") != std::string::npos);
}

TEST_CASE("KernelAdapters: Python pip magic", "[adapters]")
{
    PythonAdapter adapter;
    std::string code = "%pip install pandas";
    std::string result = adapter.preprocess_code(code);

    REQUIRE(result.find("subprocess") != std::string::npos);
    REQUIRE(result.find("pip") != std::string::npos);
}

TEST_CASE("KernelAdapters: SQL adapter preprocessing", "[adapters]")
{
    SqlAdapter adapter;
    std::string code = "SELECT * FROM users";
    std::string result = adapter.preprocess_code(code);

    REQUIRE(result.find("pandas") != std::string::npos);
    REQUIRE(result.find("read_sql") != std::string::npos);
    REQUIRE(result.find("SELECT * FROM users") != std::string::npos);
}

TEST_CASE("KernelAdapters: registry lookup", "[adapters]")
{
    KernelAdapterRegistry registry;

    auto python_adapter = registry.get_adapter("python");
    REQUIRE(python_adapter != nullptr);
    REQUIRE(python_adapter->language() == "python");

    auto r_adapter = registry.get_adapter("r");
    REQUIRE(r_adapter != nullptr);
    REQUIRE(r_adapter->language() == "r");
}

TEST_CASE("KernelAdapters: supported languages", "[adapters]")
{
    KernelAdapterRegistry registry;
    auto languages = registry.supported_languages();

    REQUIRE(languages.size() == 5);

    // Check all expected languages are present.
    bool has_python = false;
    bool has_r = false;
    bool has_julia = false;
    bool has_js = false;
    bool has_sql = false;

    for (const auto& lang : languages)
    {
        if (lang == "python")
        {
            has_python = true;
        }
        if (lang == "r")
        {
            has_r = true;
        }
        if (lang == "julia")
        {
            has_julia = true;
        }
        if (lang == "javascript")
        {
            has_js = true;
        }
        if (lang == "sql")
        {
            has_sql = true;
        }
    }

    REQUIRE(has_python);
    REQUIRE(has_r);
    REQUIRE(has_julia);
    REQUIRE(has_js);
    REQUIRE(has_sql);
}

TEST_CASE("KernelAdapters: unknown language returns nullptr", "[adapters]")
{
    KernelAdapterRegistry registry;
    REQUIRE(registry.get_adapter("unknown") == nullptr);
    REQUIRE(registry.get_adapter("fortran") == nullptr);
}

TEST_CASE("KernelAdapters: all kernel specs", "[adapters]")
{
    KernelAdapterRegistry registry;
    auto specs = registry.all_kernel_specs();

    // 5 adapters, each contributing 1 spec.
    REQUIRE(specs.size() == 5);

    bool has_python_spec = false;
    for (const auto& spec : specs)
    {
        if (spec.language == "python")
        {
            has_python_spec = true;
            REQUIRE(spec.name == "python3");
        }
    }
    REQUIRE(has_python_spec);
}

TEST_CASE("KernelAdapters: magic command listing", "[adapters]")
{
    PythonAdapter adapter;
    auto magics = adapter.supported_magic_commands();

    REQUIRE(magics.size() >= 5);

    bool has_time = false;
    bool has_pip = false;
    for (const auto& magic : magics)
    {
        if (magic == "%time")
        {
            has_time = true;
        }
        if (magic == "%pip")
        {
            has_pip = true;
        }
    }
    REQUIRE(has_time);
    REQUIRE(has_pip);
}

TEST_CASE("KernelAdapters: default kernel spec fields", "[adapters]")
{
    PythonAdapter adapter;
    auto spec = adapter.default_kernel_spec();

    REQUIRE(spec.name == "python3");
    REQUIRE(spec.display_name == "Python 3");
    REQUIRE(spec.language == "python");
    REQUIRE(spec.executable == "python3");
    REQUIRE(!spec.argv.empty());
}
