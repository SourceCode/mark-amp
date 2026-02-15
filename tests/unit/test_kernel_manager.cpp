/// @file test_kernel_manager.cpp
/// @brief V4 Phase 29 – Kernel Manager unit tests.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/KernelManager.h"

#include <catch2/catch_test_macros.hpp>

#include <set>

using namespace markamp::core;

struct KernelFixture
{
    EventBus event_bus;
    Config config;
    KernelManager manager{event_bus, config};
};

TEST_CASE("KernelManager: MimeBundle has/get/best_format", "[kernel]")
{
    MimeBundle bundle;
    bundle.data["text/plain"] = "Hello";
    bundle.data["text/html"] = "<b>Hello</b>";

    REQUIRE(bundle.has("text/plain"));
    REQUIRE(bundle.has("text/html"));
    REQUIRE_FALSE(bundle.has("image/png"));

    REQUIRE(bundle.get("text/plain") == "Hello");
    REQUIRE(bundle.get("text/html") == "<b>Hello</b>");
    REQUIRE(bundle.get("image/png").empty());

    REQUIRE(bundle.best_format() == "text/html"); // html > plain
}

TEST_CASE("KernelManager: ExecutionRequest construction", "[kernel]")
{
    ExecutionRequest request;
    request.code = "print('hello')";
    request.cell_id = "cell_1";
    request.silent = false;
    request.store_history = true;

    REQUIRE(request.code == "print('hello')");
    REQUIRE(request.cell_id == "cell_1");
    REQUIRE_FALSE(request.silent);
    REQUIRE(request.store_history);
}

TEST_CASE("KernelManager: ExecutionResult success and error", "[kernel]")
{
    ExecutionResult success;
    success.success = true;
    success.execution_count = 1;
    success.output.data["text/plain"] = "42";
    REQUIRE(success.success);
    REQUIRE(success.execution_count == 1);

    ExecutionResult error;
    error.success = false;
    error.error_name = "NameError";
    error.error_value = "name 'x' is not defined";
    error.traceback = {"line 1", "line 2"};
    REQUIRE_FALSE(error.success);
    REQUIRE(error.error_name == "NameError");
    REQUIRE(error.traceback.size() == 2);
}

TEST_CASE("KernelManager: kernel spec fields", "[kernel]")
{
    KernelSpec spec;
    spec.name = "python3";
    spec.display_name = "Python 3.11";
    spec.language = "python";
    spec.executable = "/usr/bin/python3";
    spec.argv = {"-m", "ipykernel"};

    REQUIRE(spec.name == "python3");
    REQUIRE(spec.language == "python");
    REQUIRE(spec.argv.size() == 2);
}

TEST_CASE("KernelManager: kernel ID uniqueness", "[kernel]")
{
    KernelFixture fixture;
    auto specs = fixture.manager.discover_kernels();
    REQUIRE(!specs.empty());

    // Start 10 kernels and verify all have unique IDs.
    std::set<std::string> ids;
    for (int idx = 0; idx < 10; ++idx)
    {
        auto result = fixture.manager.start_kernel(specs[0]);
        REQUIRE(result.has_value());
        ids.insert(*result);
    }
    REQUIRE(ids.size() == 10);
}

TEST_CASE("KernelManager: discover kernels", "[kernel]")
{
    KernelFixture fixture;
    auto specs = fixture.manager.discover_kernels();

    // Should have at least Python, R, Julia, Node.
    REQUIRE(specs.size() >= 4);

    bool has_python = false;
    for (const auto& spec : specs)
    {
        if (spec.language == "python")
        {
            has_python = true;
        }
    }
    REQUIRE(has_python);
}

TEST_CASE("KernelManager: start/stop kernel lifecycle", "[kernel]")
{
    KernelFixture fixture;
    KernelSpec spec;
    spec.name = "test_kernel";
    spec.language = "python";

    auto start_result = fixture.manager.start_kernel(spec);
    REQUIRE(start_result.has_value());

    auto kernel_id = *start_result;
    auto info = fixture.manager.get_kernel_info(kernel_id);
    REQUIRE(info.has_value());
    REQUIRE(info->state == KernelState::kIdle);

    auto stop_result = fixture.manager.stop_kernel(kernel_id);
    REQUIRE(stop_result.has_value());

    auto info_after = fixture.manager.get_kernel_info(kernel_id);
    REQUIRE_FALSE(info_after.has_value());
}

TEST_CASE("KernelManager: kernel state transitions", "[kernel]")
{
    KernelFixture fixture;
    KernelSpec spec;
    spec.name = "state_test";
    spec.language = "python";

    auto kernel_id = *fixture.manager.start_kernel(spec);

    // Should start as idle.
    REQUIRE(fixture.manager.kernel_state(kernel_id) == KernelState::kIdle);

    // Execute sets to busy then back to idle (in stub).
    ExecutionRequest request;
    request.code = "x = 1";
    request.cell_id = "c1";
    auto exec_result = fixture.manager.execute(kernel_id, request);
    REQUIRE(exec_result.has_value());

    // After execution completes (stub = synchronous), back to idle.
    REQUIRE(fixture.manager.kernel_state(kernel_id) == KernelState::kIdle);
}

TEST_CASE("KernelManager: active kernels listing", "[kernel]")
{
    KernelFixture fixture;
    KernelSpec spec1;
    spec1.name = "k1";
    spec1.language = "python";

    KernelSpec spec2;
    spec2.name = "k2";
    spec2.language = "r";

    fixture.manager.start_kernel(spec1);
    fixture.manager.start_kernel(spec2);

    auto active = fixture.manager.active_kernels();
    REQUIRE(active.size() == 2);
}

TEST_CASE("KernelManager: execution request queueing", "[kernel]")
{
    KernelFixture fixture;
    KernelSpec spec;
    spec.name = "exec_test";
    spec.language = "python";

    auto kernel_id = *fixture.manager.start_kernel(spec);

    // Execute multiple requests.
    ExecutionRequest req1;
    req1.code = "x = 1";
    req1.cell_id = "c1";

    ExecutionRequest req2;
    req2.code = "y = 2";
    req2.cell_id = "c2";

    auto msg1 = fixture.manager.execute(kernel_id, req1);
    auto msg2 = fixture.manager.execute(kernel_id, req2);

    REQUIRE(msg1.has_value());
    REQUIRE(msg2.has_value());
    REQUIRE(*msg1 != *msg2); // Unique message IDs.
}
