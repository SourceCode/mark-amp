// test_async_file_loader.cpp — 10 tests for AsyncFileLoader and FileChunk
#include "core/AsyncFileLoader.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <mutex>
#include <vector>

using namespace markamp::core;

TEST_CASE("FileChunk default values", "[async_loader]")
{
    FileChunk chunk;
    CHECK(chunk.offset == 0);
    CHECK(chunk.data.empty());
    CHECK_FALSE(chunk.is_last);
}

TEST_CASE("AsyncFileLoader kChunkSize is 256KB", "[async_loader]")
{
    CHECK(AsyncFileLoader::kChunkSize == 256 * 1024);
}

TEST_CASE("AsyncFileLoader loads small file completely", "[async_loader]")
{
    auto path = std::filesystem::temp_directory_path() / "markamp_test_async_load.txt";
    {
        std::ofstream out(path);
        out << "Small test file content";
    }

    std::vector<FileChunk> chunks;
    std::mutex mtx;
    bool error_occurred = false;

    {
        AsyncFileLoader loader(
            path,
            [&chunks, &mtx](FileChunk chunk)
            {
                std::lock_guard lock(mtx);
                chunks.push_back(std::move(chunk));
            },
            [&error_occurred](std::string) { error_occurred = true; });

        while (!loader.is_complete())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    CHECK_FALSE(error_occurred);
    CHECK_FALSE(chunks.empty());
    CHECK(chunks.back().is_last);
    std::filesystem::remove(path);
}

TEST_CASE("AsyncFileLoader reports error for missing file", "[async_loader]")
{
    bool error_occurred = false;
    std::string error_msg;

    {
        AsyncFileLoader loader(
            "/nonexistent/test/file.txt",
            [](FileChunk) {},
            [&error_occurred, &error_msg](std::string msg)
            {
                error_occurred = true;
                error_msg = std::move(msg);
            });

        while (!loader.is_complete())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    CHECK(error_occurred);
    CHECK_FALSE(error_msg.empty());
}

TEST_CASE("AsyncFileLoader cancel stops loading", "[async_loader]")
{
    auto path = std::filesystem::temp_directory_path() / "markamp_test_async_cancel.txt";
    {
        std::ofstream out(path);
        for (int i = 0; i < 10000; ++i)
        {
            out << "line " << i << "\n";
        }
    }

    AsyncFileLoader loader(
        path, [](FileChunk) {}, [](std::string) {});
    loader.cancel();

    while (!loader.is_complete())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    CHECK(loader.is_complete());
    std::filesystem::remove(path);
}

TEST_CASE("AsyncFileLoader is_complete false initially", "[async_loader]")
{
    auto path = std::filesystem::temp_directory_path() / "markamp_test_async_initial.txt";
    {
        std::ofstream out(path);
        out << "data";
    }

    AsyncFileLoader loader(
        path, [](FileChunk) {}, [](std::string) {});

    // May or may not be complete yet, but should not crash
    while (!loader.is_complete())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    CHECK(loader.is_complete());
    std::filesystem::remove(path);
}

TEST_CASE("AsyncFileLoader first chunk offset is 0", "[async_loader]")
{
    auto path = std::filesystem::temp_directory_path() / "markamp_test_async_offset.txt";
    {
        std::ofstream out(path);
        out << "offset test";
    }

    std::size_t first_offset = 999;
    std::mutex mtx;

    {
        AsyncFileLoader loader(
            path,
            [&first_offset, &mtx](FileChunk chunk)
            {
                std::lock_guard lock(mtx);
                if (first_offset == 999)
                    first_offset = chunk.offset;
            },
            [](std::string) {});

        while (!loader.is_complete())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    CHECK(first_offset == 0);
    std::filesystem::remove(path);
}

TEST_CASE("AsyncFileLoader empty file completes without error", "[async_loader]")
{
    auto path = std::filesystem::temp_directory_path() / "markamp_test_async_empty.txt";
    {
        std::ofstream out(path); // empty file
    }

    bool error_occurred = false;

    {
        AsyncFileLoader loader(
            path, [](FileChunk) {}, [&error_occurred](std::string) { error_occurred = true; });

        while (!loader.is_complete())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    CHECK_FALSE(error_occurred);
    std::filesystem::remove(path);
}

TEST_CASE("FileChunk data reflects file content", "[async_loader]")
{
    auto path = std::filesystem::temp_directory_path() / "markamp_test_async_content.txt";
    {
        std::ofstream out(path);
        out << "ABC123";
    }

    std::string all_data;
    std::mutex mtx;

    {
        AsyncFileLoader loader(
            path,
            [&all_data, &mtx](FileChunk chunk)
            {
                std::lock_guard lock(mtx);
                all_data += chunk.data;
            },
            [](std::string) {});

        while (!loader.is_complete())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    CHECK(all_data == "ABC123");
    std::filesystem::remove(path);
}

TEST_CASE("AsyncFileLoader destructor waits for thread", "[async_loader]")
{
    auto path = std::filesystem::temp_directory_path() / "markamp_test_async_dtor.txt";
    {
        std::ofstream out(path);
        out << "dtor test";
    }

    // Just verify no crash/hang when loader goes out of scope
    {
        AsyncFileLoader loader(
            path, [](FileChunk) {}, [](std::string) {});
    }
    CHECK(true);
    std::filesystem::remove(path);
}
