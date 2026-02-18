/// test_chunked_storage.cpp — Unit tests for ChunkedStorage

#include "core/ChunkedStorage.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace markamp::core;

TEST_CASE("ChunkedBuffer: default is empty", "[chunked_storage]")
{
    ChunkedBuffer<4096> buf;
    REQUIRE(buf.empty());
    REQUIRE(buf.size() == 0);
    REQUIRE(buf.chunk_count() == 0);
}

TEST_CASE("ChunkedBuffer: append and read", "[chunked_storage]")
{
    ChunkedBuffer<64> buf;
    buf.append("Hello World");
    REQUIRE(buf.size() == 11);
    REQUIRE(buf.read(0, 5) == "Hello");
    REQUIRE(buf.read(6, 5) == "World");
}

TEST_CASE("ChunkedBuffer: read entire content", "[chunked_storage]")
{
    ChunkedBuffer<64> buf;
    buf.append("Test content");
    REQUIRE(buf.read(0, 12) == "Test content");
}

TEST_CASE("ChunkedBuffer: cross-chunk read", "[chunked_storage]")
{
    ChunkedBuffer<8> buf;
    buf.append("12345678ABCD");
    REQUIRE(buf.chunk_count() == 2);
    REQUIRE(buf.read(6, 4) == "78AB");
}

TEST_CASE("ChunkedBuffer: read past end returns partial", "[chunked_storage]")
{
    ChunkedBuffer<64> buf;
    buf.append("abc");
    REQUIRE(buf.read(0, 100) == "abc");
}

TEST_CASE("ChunkedBuffer: read at invalid offset returns empty", "[chunked_storage]")
{
    ChunkedBuffer<64> buf;
    buf.append("abc");
    REQUIRE(buf.read(100, 10).empty());
}

TEST_CASE("ChunkedBuffer: clear resets state", "[chunked_storage]")
{
    ChunkedBuffer<64> buf;
    buf.append("hello");
    buf.clear();
    REQUIRE(buf.empty());
    REQUIRE(buf.chunk_count() == 0);
}

TEST_CASE("ChunkedBuffer: capacity tracks allocated space", "[chunked_storage]")
{
    ChunkedBuffer<16> buf;
    buf.append("data");
    REQUIRE(buf.capacity() == 16);
}

TEST_CASE("ByteCappedLRU: put and get", "[chunked_storage]")
{
    auto size_fn = [](const std::string& s) -> std::size_t { return s.size(); };
    ByteCappedLRU<int, std::string> cache(100, size_fn);
    cache.put(1, "hello");
    auto* val = cache.get(1);
    REQUIRE(val != nullptr);
    REQUIRE(*val == "hello");
}

TEST_CASE("ByteCappedLRU: get missing returns nullptr", "[chunked_storage]")
{
    auto size_fn = [](const std::string& s) -> std::size_t { return s.size(); };
    ByteCappedLRU<int, std::string> cache(100, size_fn);
    REQUIRE(cache.get(42) == nullptr);
}

TEST_CASE("ByteCappedLRU: eviction on byte cap", "[chunked_storage]")
{
    auto size_fn = [](const std::string& s) -> std::size_t { return s.size(); };
    ByteCappedLRU<int, std::string> cache(10, size_fn);
    cache.put(1, "12345");
    cache.put(2, "67890");
    REQUIRE(cache.size() == 2);
    cache.put(3, "ABCDE");
    REQUIRE(cache.size() == 2); // evicted oldest
}

TEST_CASE("ByteCappedLRU: clear resets", "[chunked_storage]")
{
    auto size_fn = [](const std::string& s) -> std::size_t { return s.size(); };
    ByteCappedLRU<int, std::string> cache(100, size_fn);
    cache.put(1, "hello");
    cache.clear();
    REQUIRE(cache.size() == 0);
    REQUIRE(cache.current_bytes() == 0);
}
