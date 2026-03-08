/// test_generation_counter.cpp — Comprehensive tests for GenerationCounter
#include "core/GenerationCounter.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

// ── Positive Tests ──

TEST_CASE("GenerationCounter: initial value", "[generation_counter][positive]")
{
    GenerationCounter counter;
    auto initial = counter.current();
    REQUIRE(initial >= 0);
}

TEST_CASE("GenerationCounter: bump increments", "[generation_counter][positive]")
{
    GenerationCounter counter;
    auto before = counter.current();
    counter.bump();
    REQUIRE(counter.current() == before + 1);
}

TEST_CASE("GenerationCounter: multiple bumps accumulate", "[generation_counter][positive]")
{
    GenerationCounter counter;
    auto before = counter.current();
    counter.bump();
    counter.bump();
    counter.bump();
    REQUIRE(counter.current() == before + 3);
}

TEST_CASE("GenerationCounter: is_stale detects old generation", "[generation_counter][positive]")
{
    GenerationCounter counter;
    auto gen = counter.current();
    counter.bump();
    REQUIRE(counter.is_stale(gen));
}

TEST_CASE("GenerationCounter: is_stale false for current", "[generation_counter][positive]")
{
    GenerationCounter counter;
    counter.bump();
    auto gen = counter.current();
    REQUIRE_FALSE(counter.is_stale(gen));
}

// ── Negative Tests ──

TEST_CASE("GenerationCounter: is_stale for future generation", "[generation_counter][negative]")
{
    GenerationCounter counter;
    auto future = counter.current() + 100;
    // Any gen != current is stale (including future)
    REQUIRE(counter.is_stale(future));
}

// ── Edge Cases ──

TEST_CASE("GenerationCounter: bump many times", "[generation_counter][edge]")
{
    GenerationCounter counter;
    for (int i = 0; i < 1000; ++i)
    {
        counter.bump();
    }
    REQUIRE(counter.current() >= 1000);
}

TEST_CASE("GenerationCounter: is_stale with zero", "[generation_counter][edge]")
{
    GenerationCounter counter;
    counter.bump();
    REQUIRE(counter.is_stale(0));
}
