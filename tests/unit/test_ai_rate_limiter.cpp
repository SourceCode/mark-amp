// test_ai_rate_limiter.cpp — 10 tests for AIRateLimiter
#include "core/AIRateLimiter.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("AIRateLimiter session usage starts at zero", "[ai][ratelimit]")
{
    EventBus bus;
    AIRateLimiter limiter(bus);
    auto stats = limiter.session_usage();
    CHECK(stats.prompt_tokens == 0);
    CHECK(stats.completion_tokens == 0);
    CHECK(stats.request_count == 0);
}

TEST_CASE("AIRateLimiter record_usage tracks tokens", "[ai][ratelimit]")
{
    EventBus bus;
    AIRateLimiter limiter(bus);
    limiter.record_usage(100, 50, AIProvider::OpenAI);
    auto stats = limiter.session_usage();
    CHECK(stats.prompt_tokens == 100);
    CHECK(stats.completion_tokens == 50);
    CHECK(stats.request_count == 1);
}

TEST_CASE("AIRateLimiter estimated_cost starts at zero", "[ai][ratelimit]")
{
    EventBus bus;
    AIRateLimiter limiter(bus);
    CHECK(limiter.estimated_cost() == 0.0);
}

TEST_CASE("AIRateLimiter monthly_limit default is unlimited", "[ai][ratelimit]")
{
    EventBus bus;
    AIRateLimiter limiter(bus);
    CHECK(limiter.monthly_limit() == 0.0);
}

TEST_CASE("AIRateLimiter set monthly limit", "[ai][ratelimit]")
{
    EventBus bus;
    AIRateLimiter limiter(bus);
    limiter.set_monthly_limit(50.0);
    CHECK(limiter.monthly_limit() == 50.0);
}

TEST_CASE("AIRateLimiter check_limit allows when no limit set", "[ai][ratelimit]")
{
    EventBus bus;
    AIRateLimiter limiter(bus);
    CHECK(limiter.check_limit());
}

TEST_CASE("AIRateLimiter hard_stop default is true", "[ai][ratelimit]")
{
    EventBus bus;
    AIRateLimiter limiter(bus);
    CHECK(limiter.hard_stop_enabled());
}

TEST_CASE("AIRateLimiter set_hard_stop toggles", "[ai][ratelimit]")
{
    EventBus bus;
    AIRateLimiter limiter(bus);
    limiter.set_hard_stop(false);
    CHECK_FALSE(limiter.hard_stop_enabled());
    limiter.set_hard_stop(true);
    CHECK(limiter.hard_stop_enabled());
}

TEST_CASE("AIRateLimiter reset_session clears session stats", "[ai][ratelimit]")
{
    EventBus bus;
    AIRateLimiter limiter(bus);
    limiter.record_usage(200, 100, AIProvider::Anthropic);
    limiter.reset_session();
    auto stats = limiter.session_usage();
    CHECK(stats.prompt_tokens == 0);
    CHECK(stats.request_count == 0);
}

TEST_CASE("AIRateLimiter reset_all clears everything", "[ai][ratelimit]")
{
    EventBus bus;
    AIRateLimiter limiter(bus);
    limiter.record_usage(200, 100, AIProvider::OpenAI);
    limiter.reset_all();
    auto session = limiter.session_usage();
    auto total = limiter.total_usage();
    CHECK(session.prompt_tokens == 0);
    CHECK(total.prompt_tokens == 0);
}
