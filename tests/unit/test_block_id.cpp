#include "core/BlockID.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <set>
#include <thread>
#include <unordered_set>
#include <vector>

using namespace markamp::core;

TEST_CASE("ID generation produces valid format", "[blockid][phase02]")
{
    auto id = BlockIDGenerator::generate();
    CHECK(id.size() == 22);
    CHECK(id[14] == '-');

    // First 14 chars are digits
    for (int i = 0; i < 14; ++i)
    {
        CHECK((id[i] >= '0' && id[i] <= '9'));
    }
    // Last 7 chars are lowercase alphanumeric
    for (int i = 15; i < 22; ++i)
    {
        CHECK(((id[i] >= '0' && id[i] <= '9') || (id[i] >= 'a' && id[i] <= 'z')));
    }
}

TEST_CASE("ID uniqueness (sequential)", "[blockid][phase02]")
{
    std::unordered_set<std::string> ids;
    for (int i = 0; i < 100; ++i)
    {
        ids.insert(BlockIDGenerator::generate());
    }
    CHECK(ids.size() == 100);
}

TEST_CASE("ID uniqueness (concurrent)", "[blockid][phase02]")
{
    constexpr int threads = 8;
    constexpr int per_thread = 100;

    std::vector<std::vector<std::string>> results(threads);
    std::vector<std::thread> thread_pool;

    for (int t = 0; t < threads; ++t)
    {
        thread_pool.emplace_back(
            [&results, t]()
            {
                for (int i = 0; i < per_thread; ++i)
                {
                    results[static_cast<std::size_t>(t)].push_back(BlockIDGenerator::generate());
                }
            });
    }
    for (auto& t : thread_pool)
    {
        t.join();
    }

    std::unordered_set<std::string> all;
    for (const auto& vec : results)
    {
        for (const auto& id : vec)
        {
            all.insert(id);
        }
    }
    CHECK(all.size() == threads * per_thread);
}

TEST_CASE("Timestamp extraction matches current time", "[blockid][phase02]")
{
    auto before = std::chrono::system_clock::now();
    auto id = BlockIDGenerator::generate();
    auto after = std::chrono::system_clock::now();

    auto parsed = BlockIDGenerator::parse_timestamp(id);
    REQUIRE(parsed.has_value());

    // The parsed timestamp should be within 1 second of now
    auto diff_before =
        std::chrono::duration_cast<std::chrono::seconds>(parsed.value() - before).count();
    auto diff_after =
        std::chrono::duration_cast<std::chrono::seconds>(after - parsed.value()).count();

    CHECK(diff_before >= -1);
    CHECK(diff_after >= -1);
}

TEST_CASE("Custom timestamp generation", "[blockid][phase02]")
{
    // Create a specific time: 2024-01-15 10:30:45
    std::tm tm_val{};
    tm_val.tm_year = 2024 - 1900;
    tm_val.tm_mon = 0; // January
    tm_val.tm_mday = 15;
    tm_val.tm_hour = 10;
    tm_val.tm_min = 30;
    tm_val.tm_sec = 45;
    tm_val.tm_isdst = -1;

    auto time_t_val = std::mktime(&tm_val);
    auto tp = std::chrono::system_clock::from_time_t(time_t_val);

    auto id = BlockIDGenerator::generate(tp);
    CHECK(id.size() == 22);
    CHECK(BlockIDGenerator::timestamp_prefix(id) == "20240115103045");
}

TEST_CASE("Validation accepts valid IDs", "[blockid][phase02]")
{
    CHECK(BlockIDGenerator::is_valid("20210808180117-6v0mkxr"));
    CHECK(BlockIDGenerator::is_valid("20260214120000-abcdefg"));
    CHECK(BlockIDGenerator::is_valid("99991231235959-0000000"));
}

TEST_CASE("Validation rejects invalid IDs", "[blockid][phase02]")
{
    CHECK_FALSE(BlockIDGenerator::is_valid(""));
    CHECK_FALSE(BlockIDGenerator::is_valid("short"));
    CHECK_FALSE(BlockIDGenerator::is_valid("20210808180117_6v0mkxr"));  // underscore
    CHECK_FALSE(BlockIDGenerator::is_valid("2021080818011X-6v0mkxr"));  // X in timestamp
    CHECK_FALSE(BlockIDGenerator::is_valid("20210808180117-6v0mkXr"));  // uppercase X
    CHECK_FALSE(BlockIDGenerator::is_valid("20210808180117-6v0mkxr!")); // too long
}

TEST_CASE("Random suffix uniqueness with fixed timestamp", "[blockid][phase02]")
{
    std::tm tm_val{};
    tm_val.tm_year = 2024 - 1900;
    tm_val.tm_mon = 5;
    tm_val.tm_mday = 15;
    tm_val.tm_hour = 12;
    tm_val.tm_min = 0;
    tm_val.tm_sec = 0;
    tm_val.tm_isdst = -1;

    auto time_t_val = std::mktime(&tm_val);
    auto tp = std::chrono::system_clock::from_time_t(time_t_val);

    std::unordered_set<std::string> ids;
    for (int i = 0; i < 1000; ++i)
    {
        ids.insert(BlockIDGenerator::generate(tp));
    }
    CHECK(ids.size() == 1000);
}

TEST_CASE("Parse round-trip", "[blockid][phase02]")
{
    auto id = BlockIDGenerator::generate();
    auto parsed = BlockIDGenerator::parse_timestamp(id);
    REQUIRE(parsed.has_value());

    auto formatted = BlockIDGenerator::format_timestamp(parsed.value());
    CHECK(formatted == BlockIDGenerator::timestamp_prefix(id));
}

TEST_CASE("Lexicographic chronological sorting", "[blockid][phase02]")
{
    // Use timestamps 1 minute apart
    std::tm tm1{};
    tm1.tm_year = 2024 - 1900;
    tm1.tm_mon = 0;
    tm1.tm_mday = 1;
    tm1.tm_hour = 12;
    tm1.tm_min = 0;
    tm1.tm_sec = 0;
    tm1.tm_isdst = -1;

    std::tm tm2 = tm1;
    tm2.tm_min = 1;

    auto tp1 = std::chrono::system_clock::from_time_t(std::mktime(&tm1));
    auto tp2 = std::chrono::system_clock::from_time_t(std::mktime(&tm2));

    auto id1 = BlockIDGenerator::generate(tp1);
    auto id2 = BlockIDGenerator::generate(tp2);

    CHECK(id1 < id2);
}

TEST_CASE("Prefix and suffix extraction", "[blockid][phase02]")
{
    auto id = BlockIDGenerator::generate();
    auto prefix = BlockIDGenerator::timestamp_prefix(id);
    auto suffix = BlockIDGenerator::random_suffix(id);

    CHECK(prefix.size() == 14);
    CHECK(suffix.size() == 7);
    CHECK(prefix + "-" + suffix == id);
}

TEST_CASE("Invalid ID extraction returns empty/nullopt", "[blockid][phase02]")
{
    CHECK(BlockIDGenerator::timestamp_prefix("").empty());
    CHECK(BlockIDGenerator::random_suffix("").empty());
    CHECK_FALSE(BlockIDGenerator::parse_timestamp("invalid").has_value());
    CHECK(BlockIDGenerator::timestamp_prefix("not-valid-id").empty());
}
