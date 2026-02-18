/// test_cancellation_token.cpp — V7 Phase 17: Cancellation token tests

#include "core/CancellationToken.h"

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <thread>

using namespace markamp::core;

TEST_CASE("CancellationToken: not cancelled by default", "[cancellation]")
{
    CancellationTokenSource source;
    auto tok = source.token();
    REQUIRE_FALSE(tok.is_cancelled());
    REQUIRE(tok.is_valid());
}

TEST_CASE("CancellationToken: cancellation propagates", "[cancellation]")
{
    CancellationTokenSource source;
    auto tok = source.token();
    REQUIRE_FALSE(tok.is_cancelled());
    source.cancel();
    REQUIRE(tok.is_cancelled());
}

TEST_CASE("CancellationToken: throw_if_cancelled returns ok when not cancelled", "[cancellation]")
{
    CancellationTokenSource source;
    auto tok = source.token();
    auto result = tok.throw_if_cancelled();
    REQUIRE(result.has_value());
}

TEST_CASE("CancellationToken: throw_if_cancelled returns error when cancelled", "[cancellation]")
{
    CancellationTokenSource source;
    auto tok = source.token();
    source.cancel();
    auto result = tok.throw_if_cancelled();
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::Cancelled);
}

TEST_CASE("CancellationToken: callbacks fire on cancel", "[cancellation]")
{
    CancellationTokenSource source;
    auto tok = source.token();
    bool called = false;
    tok.register_callback([&called]() { called = true; });
    REQUIRE_FALSE(called);
    source.cancel();
    REQUIRE(called);
}

TEST_CASE("CancellationToken: callback fires immediately if already cancelled", "[cancellation]")
{
    CancellationTokenSource source;
    source.cancel();
    auto tok = source.token();
    bool called = false;
    tok.register_callback([&called]() { called = true; });
    REQUIRE(called);
}

TEST_CASE("CancellationToken: multiple tokens from same source", "[cancellation]")
{
    CancellationTokenSource source;
    auto tok1 = source.token();
    auto tok2 = source.token();
    source.cancel();
    REQUIRE(tok1.is_cancelled());
    REQUIRE(tok2.is_cancelled());
}

TEST_CASE("CancellationToken: none() token is never cancelled", "[cancellation]")
{
    auto tok = CancellationToken::none();
    REQUIRE_FALSE(tok.is_cancelled());
    REQUIRE_FALSE(tok.is_valid());
}

TEST_CASE("CancellationToken: source reports is_cancelled", "[cancellation]")
{
    CancellationTokenSource source;
    REQUIRE_FALSE(source.is_cancelled());
    source.cancel();
    REQUIRE(source.is_cancelled());
}

TEST_CASE("CancellationToken: double cancel is safe", "[cancellation]")
{
    CancellationTokenSource source;
    int call_count = 0;
    auto tok = source.token();
    tok.register_callback([&call_count]() { call_count++; });
    source.cancel();
    source.cancel(); // Should not re-fire callbacks
    REQUIRE(call_count == 1);
}

TEST_CASE("CancellationToken: works across threads", "[cancellation]")
{
    CancellationTokenSource source;
    auto tok = source.token();
    std::atomic<bool> seen{false};

    std::thread worker(
        [&tok, &seen]()
        {
            while (!tok.is_cancelled())
            {
                std::this_thread::yield();
            }
            seen.store(true);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    source.cancel();
    worker.join();
    REQUIRE(seen.load());
}
