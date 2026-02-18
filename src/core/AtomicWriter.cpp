/// AtomicWriter.cpp — V7 Phase 31: Atomic config writes & corruption detection

#include "AtomicWriter.h"

#include <array>
#include <charconv>
#include <cstdio>
#include <fstream>
#include <sstream>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// CRC32 — Basic table-based implementation
// ══════════════════════════════════════════════════════════════════════════════

static constexpr auto build_crc_table() -> std::array<uint32_t, 256>
{
    std::array<uint32_t, 256> table{};
    for (uint32_t idx = 0; idx < 256; ++idx)
    {
        uint32_t crc = idx;
        for (int bit = 0; bit < 8; ++bit)
        {
            if ((crc & 1U) != 0)
            {
                crc = (crc >> 1U) ^ 0xEDB8'8320U;
            }
            else
            {
                crc >>= 1U;
            }
        }
        table[idx] = crc;
    }
    return table;
}

static constexpr auto kCrcTable = build_crc_table();

auto crc32_checksum(std::string_view data) -> uint32_t
{
    uint32_t crc = 0xFFFF'FFFFU;
    for (auto byte : data)
    {
        auto index = static_cast<uint8_t>(crc ^ static_cast<uint8_t>(byte));
        crc = kCrcTable[index] ^ (crc >> 8U);
    }
    return crc ^ 0xFFFF'FFFFU;
}

// ══════════════════════════════════════════════════════════════════════════════
// Atomic Writer
// ══════════════════════════════════════════════════════════════════════════════

static constexpr std::string_view kChecksumPrefix = "\n__CRC32__=";
static constexpr size_t kChecksumHexLen = 8;

auto AtomicWriter::write(const std::filesystem::path& target, std::string_view data) -> Result<void>
{
    // Create backup if file exists and backups enabled
    if (backup_enabled_ && std::filesystem::exists(target))
    {
        auto backup_result = backup(target);
        if (!backup_result.has_value())
        {
            return std::unexpected(backup_result.error());
        }
    }

    // Write to temp file
    auto temp_path = target;
    temp_path += ".tmp";

    {
        std::ofstream file(temp_path, std::ios::binary);
        if (!file.is_open())
        {
            return std::unexpected(make_io_error("Cannot create temp file: " + temp_path.string(),
                                                 ErrorCode::FileWriteError));
        }
        file.write(data.data(), static_cast<std::streamsize>(data.size()));
        file.flush();

        if (!file.good())
        {
            return std::unexpected(make_io_error("Failed to write temp file: " + temp_path.string(),
                                                 ErrorCode::FileWriteError));
        }
    }

    // Rename temp over target (atomic on POSIX)
    std::error_code error_code;
    std::filesystem::rename(temp_path, target, error_code);
    if (error_code)
    {
        std::filesystem::remove(temp_path, error_code);
        return std::unexpected(make_io_error(
            "Failed to rename temp to target: " + error_code.message(), ErrorCode::FileWriteError));
    }

    return {};
}

auto AtomicWriter::write_with_checksum(const std::filesystem::path& target, std::string_view data)
    -> Result<void>
{
    auto checksum = crc32_checksum(data);

    // Format checksum as hex
    std::array<char, kChecksumHexLen + 1> hex_buf{};
    auto [end_ptr, ec] =
        std::to_chars(hex_buf.data(), hex_buf.data() + kChecksumHexLen, checksum, 16);

    // Pad with leading zeros
    auto hex_len = static_cast<size_t>(end_ptr - hex_buf.data());
    std::string hex_str(kChecksumHexLen - hex_len, '0');
    hex_str.append(hex_buf.data(), hex_len);

    // Append trailer
    std::string full_content;
    full_content.reserve(data.size() + kChecksumPrefix.size() + kChecksumHexLen + 1);
    full_content.append(data);
    full_content.append(kChecksumPrefix);
    full_content.append(hex_str);
    full_content.push_back('\n');

    return write(target, full_content);
}

auto AtomicWriter::read_verified(const std::filesystem::path& path) -> Result<std::string>
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        return std::unexpected(
            make_io_error("Cannot open file: " + path.string(), ErrorCode::FileReadError));
    }

    std::ostringstream stream;
    stream << file.rdbuf();
    auto content = stream.str();

    auto result = extract_checksum_trailer(content);
    if (!result.has_value())
    {
        return std::unexpected(result.error());
    }

    auto& [payload, expected_checksum] = result.value();
    auto actual_checksum = crc32_checksum(payload);

    if (actual_checksum != expected_checksum)
    {
        return std::unexpected(make_io_error("Checksum mismatch: expected " +
                                                 std::to_string(expected_checksum) + ", got " +
                                                 std::to_string(actual_checksum),
                                             ErrorCode::ChecksumMismatch));
    }

    return payload;
}

auto AtomicWriter::backup(const std::filesystem::path& path) -> Result<std::filesystem::path>
{
    if (!std::filesystem::exists(path))
    {
        return std::unexpected(make_io_error("Cannot backup non-existent file: " + path.string(),
                                             ErrorCode::FileNotFound));
    }

    auto backup_path = path;
    backup_path += ".bak";

    std::error_code error_code;
    std::filesystem::copy_file(
        path, backup_path, std::filesystem::copy_options::overwrite_existing, error_code);
    if (error_code)
    {
        return std::unexpected(make_io_error("Failed to create backup: " + error_code.message(),
                                             ErrorCode::FileWriteError));
    }

    return backup_path;
}

auto extract_checksum_trailer(std::string_view content) -> Result<std::pair<std::string, uint32_t>>
{
    // Look for trailer: \n__CRC32__=XXXXXXXX\n
    auto trailer_pos = content.rfind(kChecksumPrefix);
    if (trailer_pos == std::string_view::npos)
    {
        return std::unexpected(
            make_io_error("No CRC32 checksum trailer found", ErrorCode::CorruptedFile));
    }

    auto hex_start = trailer_pos + kChecksumPrefix.size();
    if (hex_start + kChecksumHexLen > content.size())
    {
        return std::unexpected(
            make_io_error("Truncated checksum trailer", ErrorCode::CorruptedFile));
    }

    auto hex_str = content.substr(hex_start, kChecksumHexLen);
    uint32_t checksum = 0;
    auto [ptr, parse_ec] =
        std::from_chars(hex_str.data(), hex_str.data() + hex_str.size(), checksum, 16);
    if (parse_ec != std::errc{})
    {
        return std::unexpected(
            make_io_error("Invalid checksum hex value", ErrorCode::CorruptedFile));
    }

    auto payload = std::string(content.substr(0, trailer_pos));
    return std::pair{std::move(payload), checksum};
}

} // namespace markamp::core
