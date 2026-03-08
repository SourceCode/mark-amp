// test_cancellation_token.cpp — 10 tests for CancellationToken & CancellationTokenSource
#include "core/CancellationToken.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("CancellationToken default is not cancelled", "[cancel]")
{
    CancellationToken token;
    CHECK_FALSE(token.is_cancelled());
}

TEST_CASE("CancellationToken::none is never cancelled", "[cancel]")
{
    auto token = CancellationToken::none();
    CHECK_FALSE(token.is_cancelled());
    CHECK_FALSE(token.is_valid());
}

TEST_CASE("CancellationTokenSource creates valid token", "[cancel]")
{
    CancellationTokenSource source;
    auto token = source.token();
    CHECK(token.is_valid());
    CHECK_FALSE(token.is_cancelled());
}

TEST_CASE("CancellationTokenSource cancel propagates to token", "[cancel]")
{
    CancellationTokenSource source;
    auto token = source.token();
    source.cancel();
    CHECK(token.is_cancelled());
    CHECK(source.is_cancelled());
}

TEST_CASE("CancellationToken throw_if_cancelled returns error when cancelled", "[cancel]")
{
    CancellationTokenSource source;
    auto token = source.token();
    auto result = token.throw_if_cancelled();
    CHECK(result.has_value());

    source.cancel();
    auto result2 = token.throw_if_cancelled();
    CHECK_FALSE(result2.has_value());
}

TEST_CASE("CancellationToken callback fires on cancel", "[cancel]")
{
    CancellationTokenSource source;
    auto token = source.token();
    bool fired = false;
    token.register_callback([&fired] { fired = true; });
    CHECK_FALSE(fired);
    source.cancel();
    CHECK(fired);
}

TEST_CASE("CancellationToken callback fires immediately if already cancelled", "[cancel]")
{
    CancellationTokenSource source;
    source.cancel();
    auto token = source.token();
    bool fired = false;
    token.register_callback([&fired] { fired = true; });
    CHECK(fired);
}

TEST_CASE("CancellationTokenSource double cancel is safe", "[cancel]")
{
    CancellationTokenSource source;
    auto token = source.token();
    int call_count = 0;
    token.register_callback([&call_count] { ++call_count; });
    source.cancel();
    source.cancel(); // should not fire callback again
    CHECK(call_count == 1);
}

TEST_CASE("CancellationToken multiple callbacks all fire", "[cancel]")
{
    CancellationTokenSource source;
    auto token = source.token();
    int count = 0;
    token.register_callback([&count] { ++count; });
    token.register_callback([&count] { ++count; });
    token.register_callback([&count] { ++count; });
    source.cancel();
    CHECK(count == 3);
}

TEST_CASE("CancellationToken register_callback on default token does nothing", "[cancel]")
{
    CancellationToken token; // no state
    bool fired = false;
    token.register_callback([&fired] { fired = true; });
    CHECK_FALSE(fired);
}
