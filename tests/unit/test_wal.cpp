/// test_wal.cpp — V7 Phase 32: Write-ahead log tests

#include "core/WriteAheadLog.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>

using namespace markamp::core;

static auto temp_wal(const std::string& name) -> std::filesystem::path
{
    return std::filesystem::temp_directory_path() / ("markamp_test_" + name + ".wal");
}

static void cleanup(const std::filesystem::path& path)
{
    std::filesystem::remove(path);
}

// ══════════════════════════════════════════════════════════════════════════════
// Basic operations
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("WAL: open creates new log", "[wal]")
{
    auto path = temp_wal("new");
    WriteAheadLog wal;
    auto result = wal.open(path);
    REQUIRE(result.has_value());
    REQUIRE(wal.entry_count() == 0);
    REQUIRE(wal.sequence() == 0);
    cleanup(path);
}

TEST_CASE("WAL: append increments sequence", "[wal]")
{
    auto path = temp_wal("append");
    WriteAheadLog wal;
    (void)wal.open(path);

    auto seq1 = wal.append(WalOperation::Set, "key1", "value1");
    REQUIRE(seq1.has_value());
    REQUIRE(seq1.value() == 1);

    auto seq2 = wal.append(WalOperation::Set, "key2", "value2");
    REQUIRE(seq2.has_value());
    REQUIRE(seq2.value() == 2);

    REQUIRE(wal.entry_count() == 2);
    cleanup(path);
}

TEST_CASE("WAL: replay returns entries in order", "[wal]")
{
    auto path = temp_wal("replay");
    WriteAheadLog wal;
    (void)wal.open(path);

    (void)wal.append(WalOperation::Set, "key_a", "val_a");
    (void)wal.append(WalOperation::Set, "key_b", "val_b");
    (void)wal.append(WalOperation::Delete, "key_a");

    std::vector<std::string> keys;
    auto replayed = wal.replay([&keys](const WalEntry& entry) { keys.push_back(entry.key); });

    REQUIRE(replayed.has_value());
    REQUIRE(replayed.value() == 3);
    REQUIRE(keys.size() == 3);
    REQUIRE(keys[0] == "key_a");
    REQUIRE(keys[1] == "key_b");
    REQUIRE(keys[2] == "key_a");

    cleanup(path);
}

TEST_CASE("WAL: persistence across reopen", "[wal]")
{
    auto path = temp_wal("persist");

    {
        WriteAheadLog wal;
        (void)wal.open(path);
        (void)wal.append(WalOperation::Set, "persist_key", "persist_val");
    }

    {
        WriteAheadLog wal;
        (void)wal.open(path);
        REQUIRE(wal.entry_count() == 1);

        std::string value;
        (void)wal.replay(
            [&value](const WalEntry& entry)
            {
                if (entry.key == "persist_key")
                {
                    value = entry.value;
                }
            });
        REQUIRE(value == "persist_val");
    }

    cleanup(path);
}

// ══════════════════════════════════════════════════════════════════════════════
// Compaction
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("WAL: compact keeps only latest per key", "[wal]")
{
    auto path = temp_wal("compact");
    WriteAheadLog wal;
    (void)wal.open(path);

    (void)wal.append(WalOperation::Set, "key", "v1");
    (void)wal.append(WalOperation::Set, "key", "v2");
    (void)wal.append(WalOperation::Set, "key", "v3");

    REQUIRE(wal.entry_count() == 3);

    auto removed = wal.compact();
    REQUIRE(removed.has_value());
    REQUIRE(removed.value() == 2);
    REQUIRE(wal.entry_count() == 1);

    cleanup(path);
}

TEST_CASE("WAL: compact removes deleted keys", "[wal]")
{
    auto path = temp_wal("compact_del");
    WriteAheadLog wal;
    (void)wal.open(path);

    (void)wal.append(WalOperation::Set, "key", "value");
    (void)wal.append(WalOperation::Delete, "key");

    auto removed = wal.compact();
    REQUIRE(removed.has_value());
    REQUIRE(wal.entry_count() == 0);

    cleanup(path);
}

// ══════════════════════════════════════════════════════════════════════════════
// Error handling
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("WAL: append fails when not open", "[wal]")
{
    WriteAheadLog wal;
    auto result = wal.append(WalOperation::Set, "key", "value");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("WAL: close resets state", "[wal]")
{
    auto path = temp_wal("close");
    WriteAheadLog wal;
    (void)wal.open(path);
    (void)wal.append(WalOperation::Set, "key", "val");

    wal.close();
    REQUIRE(wal.entry_count() == 0);
    REQUIRE(wal.sequence() == 0);

    cleanup(path);
}
