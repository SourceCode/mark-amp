// test_chunked_storage.cpp — 10 tests for ChunkedBuffer and ByteCappedLRU
#include "core/ChunkedStorage.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// ChunkedBuffer tests
// ============================================================================

TEST_CASE("ChunkedBuffer starts empty", "[chunked][buffer]")
{
    ChunkedBuffer<1024> buf;
    CHECK(buf.empty());
    CHECK(buf.size() == 0);
    CHECK(buf.chunk_count() == 0);
}

TEST_CASE("ChunkedBuffer append and read back", "[chunked][buffer]")
{
    ChunkedBuffer<1024> buf;
    buf.append("Hello World");
    CHECK(buf.size() == 11);
    CHECK(buf.read(0, 11) == "Hello World");
}

TEST_CASE("ChunkedBuffer read with offset", "[chunked][buffer]")
{
    ChunkedBuffer<1024> buf;
    buf.append("ABCDEFGHIJ");
    CHECK(buf.read(3, 4) == "DEFG");
    CHECK(buf.read(0, 1) == "A");
    CHECK(buf.read(9, 1) == "J");
}

TEST_CASE("ChunkedBuffer read beyond size returns truncated", "[chunked][buffer]")
{
    ChunkedBuffer<1024> buf;
    buf.append("Short");
    CHECK(buf.read(0, 100) == "Short");
    CHECK(buf.read(10, 5).empty());
}

TEST_CASE("ChunkedBuffer spans multiple chunks", "[chunked][buffer]")
{
    ChunkedBuffer<16> buf; // tiny 16-byte chunks
    std::string data(50, 'X');
    buf.append(data);
    CHECK(buf.size() == 50);
    CHECK(buf.chunk_count() >= 3); // ceil(50/16) = 4
    CHECK(buf.read(0, 50) == data);
}

TEST_CASE("ChunkedBuffer clear resets all state", "[chunked][buffer]")
{
    ChunkedBuffer<1024> buf;
    buf.append("data");
    buf.clear();
    CHECK(buf.empty());
    CHECK(buf.size() == 0);
    CHECK(buf.chunk_count() == 0);
}

// ============================================================================
// ByteCappedLRU tests
// ============================================================================

static auto string_size(const std::string& s) -> std::size_t
{
    return s.size();
}

TEST_CASE("ByteCappedLRU put and get", "[chunked][lru]")
{
    ByteCappedLRU<std::string, std::string> cache(1000, string_size);
    cache.put("a", "alpha");
    auto* val = cache.get("a");
    REQUIRE(val != nullptr);
    CHECK(*val == "alpha");
}

TEST_CASE("ByteCappedLRU returns nullptr for missing key", "[chunked][lru]")
{
    ByteCappedLRU<std::string, std::string> cache(1000, string_size);
    CHECK(cache.get("missing") == nullptr);
}

TEST_CASE("ByteCappedLRU evicts by byte cap", "[chunked][lru]")
{
    ByteCappedLRU<std::string, std::string> cache(10, string_size);
    cache.put("a", "12345"); // 5 bytes
    cache.put("b", "67890"); // 5 bytes, total=10
    cache.put("c", "ABCDE"); // 5 bytes, must evict "a"
    CHECK(cache.get("a") == nullptr);
    CHECK(cache.get("c") != nullptr);
}

TEST_CASE("ByteCappedLRU clear resets bytes", "[chunked][lru]")
{
    ByteCappedLRU<std::string, std::string> cache(1000, string_size);
    cache.put("x", "data");
    CHECK(cache.current_bytes() > 0);
    cache.clear();
    CHECK(cache.current_bytes() == 0);
    CHECK(cache.size() == 0);
}
