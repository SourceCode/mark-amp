#include "AsyncFileLoader.h"

#include <fstream>

namespace markamp::core
{

AsyncFileLoader::AsyncFileLoader(const std::filesystem::path& path,
                                 ChunkCallback on_chunk,
                                 ErrorCallback on_error)
    : worker_([this, path, cb = std::move(on_chunk), err = std::move(on_error)]()
              { load_file(path, cb, err); })
{
}

AsyncFileLoader::~AsyncFileLoader()
{
    cancel();
}

void AsyncFileLoader::cancel()
{
    cancelled_.store(true, std::memory_order_release);
    if (worker_.joinable())
    {
        worker_.join();
    }
    complete_.store(true, std::memory_order_release);
}

auto AsyncFileLoader::is_complete() const noexcept -> bool
{
    return complete_.load(std::memory_order_acquire);
}

void AsyncFileLoader::load_file(std::filesystem::path path,
                                ChunkCallback on_chunk,
                                ErrorCallback on_error)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        if (on_error)
        {
            on_error("Failed to open file: " + path.string());
        }
        complete_.store(true, std::memory_order_release);
        return;
    }

    std::string buffer(kChunkSize, '\0');
    std::size_t offset = 0;

    while (!cancelled_.load(std::memory_order_acquire))
    {
        file.read(buffer.data(), static_cast<std::streamsize>(kChunkSize));
        auto bytes_read = static_cast<std::size_t>(file.gcount());

        if (bytes_read == 0)
        {
            break;
        }

        bool is_last = file.eof() || bytes_read < kChunkSize;

        FileChunk chunk;
        chunk.offset = offset;
        chunk.data = buffer.substr(0, bytes_read);
        chunk.is_last = is_last;

        if (on_chunk && !cancelled_.load(std::memory_order_acquire))
        {
            on_chunk(std::move(chunk));
        }

        offset += bytes_read;

        if (is_last)
        {
            break;
        }
    }

    complete_.store(true, std::memory_order_release);
}

} // namespace markamp::core
