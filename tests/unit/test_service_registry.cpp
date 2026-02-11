#include "core/ServiceRegistry.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// Test interfaces and implementations
class ITestService
{
public:
    virtual ~ITestService() = default;
    [[nodiscard]] virtual auto name() const -> std::string = 0;
};

class TestServiceImpl : public ITestService
{
public:
    [[nodiscard]] auto name() const -> std::string override
    {
        return "TestServiceImpl";
    }
};

class IAnotherService
{
public:
    virtual ~IAnotherService() = default;
    [[nodiscard]] virtual auto value() const -> int = 0;
};

class AnotherServiceImpl : public IAnotherService
{
public:
    [[nodiscard]] auto value() const -> int override
    {
        return 42;
    }
};

TEST_CASE("ServiceRegistry: register and retrieve", "[core][registry]")
{
    ServiceRegistry registry;

    auto impl = std::make_shared<TestServiceImpl>();
    registry.register_service<ITestService, TestServiceImpl>(impl);

    auto svc = registry.get<ITestService>();
    REQUIRE(svc != nullptr);
    REQUIRE(svc->name() == "TestServiceImpl");
}

TEST_CASE("ServiceRegistry: get unregistered returns nullptr", "[core][registry]")
{
    ServiceRegistry registry;

    auto svc = registry.get<ITestService>();
    REQUIRE(svc == nullptr);
}

TEST_CASE("ServiceRegistry: has checks registration", "[core][registry]")
{
    ServiceRegistry registry;

    REQUIRE(!registry.has<ITestService>());

    auto impl = std::make_shared<TestServiceImpl>();
    registry.register_service<ITestService, TestServiceImpl>(impl);

    REQUIRE(registry.has<ITestService>());
}

TEST_CASE("ServiceRegistry: require throws on missing service", "[core][registry]")
{
    ServiceRegistry registry;

    REQUIRE_THROWS_AS(registry.require<ITestService>(), std::runtime_error);
}

TEST_CASE("ServiceRegistry: multiple services independent", "[core][registry]")
{
    ServiceRegistry registry;

    registry.register_service<ITestService, TestServiceImpl>(std::make_shared<TestServiceImpl>());
    registry.register_service<IAnotherService, AnotherServiceImpl>(
        std::make_shared<AnotherServiceImpl>());

    REQUIRE(registry.get<ITestService>()->name() == "TestServiceImpl");
    REQUIRE(registry.get<IAnotherService>()->value() == 42);
}
