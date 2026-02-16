/// test_lazy_service.cpp — LazyService<T> unit tests
///
/// Phase 12: Validates lazy construction, thread safety, and state queries.

#include "core/LazyService.h"

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <string>
#include <thread>
#include <vector>

namespace
{

/// Simple test service that tracks construction.
struct TestService
{
    static std::atomic<int> construction_count;

    int value{0};

    explicit TestService(int val)
        : value(val)
    {
        construction_count.fetch_add(1, std::memory_order_relaxed);
    }
};

std::atomic<int> TestService::construction_count{0};

/// Service with no arguments.
struct SimpleService
{
    bool initialized{true};
};

} // namespace

TEST_CASE("LazyService: not created until get_or_create called", "[lazy_service]")
{
    markamp::core::LazyService<SimpleService> lazy{[]()
                                                   { return std::make_unique<SimpleService>(); }};

    REQUIRE_FALSE(lazy.is_created());
    REQUIRE(lazy.get() == nullptr);

    auto* svc = lazy.get_or_create();
    REQUIRE(svc != nullptr);
    REQUIRE(svc->initialized);
    REQUIRE(lazy.is_created());
}

TEST_CASE("LazyService: subsequent calls return same instance", "[lazy_service]")
{
    markamp::core::LazyService<SimpleService> lazy{[]()
                                                   { return std::make_unique<SimpleService>(); }};

    auto* first = lazy.get_or_create();
    auto* second = lazy.get_or_create();
    auto* third = lazy.get_or_create();

    REQUIRE(first == second);
    REQUIRE(second == third);
}

TEST_CASE("LazyService: factory receives constructor arguments via capture", "[lazy_service]")
{
    TestService::construction_count.store(0);

    markamp::core::LazyService<TestService> lazy{[]()
                                                 { return std::make_unique<TestService>(42); }};

    auto* svc = lazy.get_or_create();
    REQUIRE(svc != nullptr);
    REQUIRE(svc->value == 42);
    REQUIRE(TestService::construction_count.load() == 1);

    // Second call should NOT construct again
    lazy.get_or_create();
    REQUIRE(TestService::construction_count.load() == 1);
}

TEST_CASE("LazyService: thread safety — concurrent get_or_create", "[lazy_service]")
{
    TestService::construction_count.store(0);

    markamp::core::LazyService<TestService> lazy{[]()
                                                 { return std::make_unique<TestService>(99); }};

    constexpr int kThreadCount = 8;
    std::vector<std::thread> threads;
    threads.reserve(kThreadCount);
    std::vector<TestService*> results(kThreadCount, nullptr);

    for (int idx = 0; idx < kThreadCount; ++idx)
    {
        threads.emplace_back([&lazy, &results, idx]()
                             { results[static_cast<size_t>(idx)] = lazy.get_or_create(); });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    // All threads should get the same pointer
    for (int idx = 1; idx < kThreadCount; ++idx)
    {
        REQUIRE(results[static_cast<size_t>(idx)] == results[0]);
    }

    // Constructed exactly once despite concurrent access
    REQUIRE(TestService::construction_count.load() == 1);
    REQUIRE(results[0]->value == 99);
}

TEST_CASE("LazyService: reset allows re-construction", "[lazy_service]")
{
    TestService::construction_count.store(0);

    markamp::core::LazyService<TestService> lazy{[]()
                                                 { return std::make_unique<TestService>(10); }};

    auto* first = lazy.get_or_create();
    REQUIRE(first->value == 10);
    REQUIRE(lazy.is_created());

    lazy.reset();
    REQUIRE_FALSE(lazy.is_created());

    auto* second = lazy.get_or_create();
    REQUIRE(second != nullptr);
    REQUIRE(second->value == 10);
    REQUIRE(TestService::construction_count.load() == 2);
}

TEST_CASE("LazyService: set_factory before first access", "[lazy_service]")
{
    markamp::core::LazyService<SimpleService> lazy;

    lazy.set_factory([]() { return std::make_unique<SimpleService>(); });

    auto* svc = lazy.get_or_create();
    REQUIRE(svc != nullptr);
    REQUIRE(svc->initialized);
}
