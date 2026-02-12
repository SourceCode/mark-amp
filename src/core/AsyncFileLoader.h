#pragma once

#include <atomic>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <string>
#include <thread>

namespace markamp::core
{

/// A single chunk delivered during async file loading.
struct FileChunk
{
    std::size_t offset{0}; // byte offset in the file
    std::string data;      // the chunk data
    bool is_last{false};   // true if this is the final chunk
};

/// Chunked asynchronous file loader.
///
/// Reads a file in 256 KB chunks on a background thread.
/// Each chunk is delivered via a callback. Supports cancellation
/// via an atomic bool — if the file load is cancelled, the
/// worker stops reading and no further callbacks are made.
///
/// Pattern implemented: #18 Predictable I/O never on the hot path
class AsyncFileLoader
{
public:
    static constexpr std::size_t kChunkSize = 256 * 1024; // 256 KB

    using ChunkCallback = std::function<void(FileChunk)>;
    using ErrorCallback = std::function<void(std::string)>;

    /// Start loading a file asynchronously.
    AsyncFileLoader(const std::filesystem::path& path,
                    ChunkCallback on_chunk,
                    ErrorCallback on_error);

    ~AsyncFileLoader();

    // Non-copyable, non-movable
    AsyncFileLoader(const AsyncFileLoader&) = delete;
    auto operator=(const AsyncFileLoader&) -> AsyncFileLoader& = delete;
    AsyncFileLoader(AsyncFileLoader&&) = delete;
    auto operator=(AsyncFileLoader&&) -> AsyncFileLoader& = delete;

    /// Cancel the loading operation.
    void cancel();

    /// Check if loading is complete (either success or cancelled).
    [[nodiscard]] auto is_complete() const noexcept -> bool;

private:
    std::thread worker_;
    std::atomic<bool> cancelled_{false};
    std::atomic<bool> complete_{false};

    void load_file(std::filesystem::path path, ChunkCallback on_chunk, ErrorCallback on_error);
};

} // namespace markamp::core
