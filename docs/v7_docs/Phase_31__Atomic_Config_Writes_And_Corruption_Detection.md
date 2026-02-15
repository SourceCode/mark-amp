# Phase 31: Atomic Config Writes and Corruption Detection

## Metadata

| Field | Value |
|---|---|
| Phase ID | 31 |
| Prerequisites | Phase 01 (error types) |
| Estimated Complexity | Medium |
| Estimated File Count | 4 created, 1 modified, 1 test |
| PRD Sections | PI-46 (atomic config writes), PI-47 (workspace state snapshot versioning), PI-48 (corruption detection — checksum), PI-49 (transactional save operations) |

---

## Objective

Make all config and state persistence atomic using the write-to-temp + fsync + rename pattern. Add checksum-based corruption detection using CRC32 so corrupted files are detected at load time. Include schema version numbers in all persisted files.

---

## Background

The PRD mandates atomic config writes (PI-46), corruption detection via checksum (PI-48), transactional save operations (PI-49), and workspace state snapshot versioning (PI-47). Currently a crash during config save can produce a half-written file. On next startup, the corrupted config causes parse errors. Atomic writes ensure that either the old file or the new file exists — never a partial write.

---

## Scope

### Tasks

1. **Create `src/core/AtomicWriter.h` / `AtomicWriter.cpp`**:
   ```cpp
   namespace markamp::core {

   class AtomicWriter {
   public:
       // Write content to file atomically
       // 1. Write to temp file (same directory, ".tmp" suffix)
       // 2. fsync the temp file
       // 3. Rename temp to target (atomic on POSIX)
       [[nodiscard]] static auto write(const std::filesystem::path& target,
                                        std::string_view content) -> Result<void>;

       // Write with checksum
       [[nodiscard]] static auto write_with_checksum(
           const std::filesystem::path& target,
           std::string_view content
       ) -> Result<void>;

       // Write with schema version header
       [[nodiscard]] static auto write_versioned(
           const std::filesystem::path& target,
           std::string_view content,
           uint32_t schema_version
       ) -> Result<void>;

   private:
       static auto fsync_file(int fd) -> Result<void>;
       static auto fsync_directory(const std::filesystem::path& dir) -> Result<void>;
   };

   } // namespace markamp::core
   ```
   - **Temp file**: Write to `target.tmp` in the same directory. Same-directory is required for atomic rename on the same filesystem.
   - **fsync**: Call `fsync()` on the temp file before rename. On macOS, use `fcntl(fd, F_FULLFSYNC)` for guaranteed durability.
   - **Rename**: Use `std::filesystem::rename()` which is atomic on POSIX when source and target are on the same filesystem.
   - **Windows**: Use `MoveFileEx()` with `MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH`.
   - **Directory fsync**: After rename, fsync the directory to ensure the directory entry is durable.

2. **Create `src/core/ChecksumValidator.h` / `ChecksumValidator.cpp`**:
   ```cpp
   namespace markamp::core {

   class ChecksumValidator {
   public:
       // Compute CRC32 checksum of content
       [[nodiscard]] static auto compute_crc32(std::string_view content) -> uint32_t;

       // Write content with embedded checksum header
       // Format: "# checksum:CRC32:ABCD1234\n" + content
       [[nodiscard]] static auto embed_checksum(std::string_view content) -> std::string;

       // Validate content against embedded checksum
       [[nodiscard]] static auto validate(std::string_view content_with_header) -> Result<std::string_view>;

       // Extract content from checksummed file (strips header)
       [[nodiscard]] static auto extract_content(std::string_view content_with_header) -> Result<std::string_view>;
   };

   } // namespace markamp::core
   ```
   - **CRC32**: Use zlib's `crc32()` (already available via vcpkg) or implement a simple CRC32 lookup table.
   - **Header format**: First line of file: `# checksum:CRC32:ABCD1234` where ABCD1234 is the hex CRC32 of the remaining content.
   - **Validation**: On load, extract checksum from header, compute CRC32 of remaining content, compare.

3. **Modify `src/core/Config.cpp`**:
   - Replace direct file writes with `AtomicWriter::write_with_checksum()`
   - On load: validate checksum via `ChecksumValidator::validate()`
   - On checksum mismatch: log error, fall back to default config, notify user
   - Add schema version to config file (increment when config format changes)

4. **Create `tests/unit/test_atomic_writer.cpp`**:
   - TEST_CASE: "AtomicWriter creates file successfully"
   - TEST_CASE: "AtomicWriter overwrites existing file atomically"
   - TEST_CASE: "AtomicWriter temp file cleaned up on success"
   - TEST_CASE: "AtomicWriter old file preserved if write fails"
   - TEST_CASE: "ChecksumValidator embeds and validates correctly"
   - TEST_CASE: "ChecksumValidator detects content modification"
   - TEST_CASE: "ChecksumValidator detects truncated file"
   - TEST_CASE: "ChecksumValidator handles file without header"
   - TEST_CASE: "Schema version embedded in file"
   - TEST_CASE: "Config save uses atomic write"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/AtomicWriter.h` |
| Create | `src/core/AtomicWriter.cpp` |
| Create | `src/core/ChecksumValidator.h` |
| Create | `src/core/ChecksumValidator.cpp` |
| Modify | `src/core/Config.cpp` |
| Create | `tests/unit/test_atomic_writer.cpp` |

---

## Implementation Notes

- **Atomic rename guarantee**: On POSIX, `rename()` is atomic when source and target are on the same filesystem (same directory satisfies this). On Windows, `MoveFileEx` with `MOVEFILE_REPLACE_EXISTING` is atomic on NTFS.
- **fsync importance**: Without fsync, the data may be in the OS page cache but not on disk. A power failure could lose the data. `fsync()` ensures durability.
- **macOS F_FULLFSYNC**: macOS `fsync()` only guarantees write to the drive cache, not to the platters. Use `fcntl(fd, F_FULLFSYNC)` for true durability.
- **Directory fsync**: After rename, the directory entry needs to be synced. Open the directory, fsync it. This ensures the rename is durable.
- **CRC32 implementation**: If zlib is available, use `crc32()`. Otherwise, implement a 256-entry lookup table CRC32 (public domain algorithm, ~50 lines).
- **Config migration**: The existing `Config::migrate_from_json()` should also use atomic write and add checksum.
- **Error recovery**: If checksum validation fails on load, log the corruption, use default values, and optionally backup the corrupted file for diagnosis.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Config save never produces a half-written file (atomic write)
- [ ] Crash mid-write leaves old file intact (temp file pattern)
- [ ] Temp file cleaned up on successful rename
- [ ] Corrupted config detected by checksum mismatch
- [ ] Checksum stored in first line of file
- [ ] Corrupted config falls back to defaults with user notification
- [ ] Workspace state includes schema version number
- [ ] fsync called before rename (durability guarantee)
- [ ] Works on macOS, Linux, and Windows
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test atomic write by verifying file contents after write
- Test crash simulation: write temp file but don't rename, verify original intact
- Test checksum validation with: valid file, modified file, truncated file, missing header
- Test CRC32 computation against known test vectors
- Test concurrent write attempts (same file from two threads)
- Verify temp file cleanup after success
- Platform-specific: test on POSIX filesystem for atomic rename guarantee
