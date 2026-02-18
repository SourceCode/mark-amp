/// AtomicWriter.h — V7 Phase 31: Atomic config writes & corruption detection
///
/// Write-to-temp + fsync + rename pattern with CRC32 checksums for
/// corruption detection. Backup before overwrite.

#pragma once

#include "Result.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// CRC32 Checksum
// ══════════════════════════════════════════════════════════════════════════════

/// Compute CRC32 checksum for data integrity verification.
[[nodiscard]] auto crc32_checksum(std::string_view data) -> uint32_t;

// ══════════════════════════════════════════════════════════════════════════════
// Atomic Writer
// ══════════════════════════════════════════════════════════════════════════════

/// Safely writes files using the write-to-temp + rename pattern.
class AtomicWriter
{
public:
    AtomicWriter() = default;

    /// Atomically write data to a file.
    /// Steps: write to temp file → fsync → rename over target.
    [[nodiscard]] auto write(const std::filesystem::path& target, std::string_view data)
        -> Result<void>;

    /// Atomically write data with a CRC32 checksum appended as a trailer.
    /// The trailer format is: \n__CRC32__=XXXXXXXX\n
    [[nodiscard]] auto write_with_checksum(const std::filesystem::path& target,
                                           std::string_view data) -> Result<void>;

    /// Read a file and verify its CRC32 checksum trailer.
    [[nodiscard]] auto read_verified(const std::filesystem::path& path) -> Result<std::string>;

    /// Create a backup of a file before overwriting.
    [[nodiscard]] static auto backup(const std::filesystem::path& path)
        -> Result<std::filesystem::path>;

    /// Whether to create backups before overwriting. Default: true.
    void set_backup_enabled(bool enabled)
    {
        backup_enabled_ = enabled;
    }
    [[nodiscard]] auto backup_enabled() const noexcept -> bool
    {
        return backup_enabled_;
    }

private:
    bool backup_enabled_{true};
};

/// Extract the CRC32 checksum trailer from file content.
/// Returns the data (without trailer) and the expected checksum.
[[nodiscard]] auto extract_checksum_trailer(std::string_view content)
    -> Result<std::pair<std::string, uint32_t>>;

} // namespace markamp::core
