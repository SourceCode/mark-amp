# Phase 32: Write-Ahead Logging for Workspace State

## Metadata

| Field | Value |
|---|---|
| Phase ID | 32 |
| Prerequisites | Phase 31 (atomic writer, checksum) |
| Estimated Complexity | High |
| Estimated File Count | 4 created, 0 modified, 1 test |
| PRD Sections | PII-16 (write-ahead logging), PII-17 (crash-safe recovery replay), PII-18 (state snapshot versioning), PII-19 (incremental state validation), PII-20 (corruption isolation mode) |

---

## Objective

Implement write-ahead logging (WAL) for workspace state mutations so incomplete operations can be recovered after a crash. Build a crash-safe recovery replay engine that processes uncommitted WAL entries on startup.

---

## Background

The PRD mandates WAL for workspace state (PII-16), crash-safe recovery replay (PII-17), state snapshot versioning (PII-18), incremental state validation (PII-19), and corruption isolation mode (PII-20). Currently, if MarkAmp crashes mid-operation (e.g., during a multi-file rename or workspace restructuring), the workspace can be left in an inconsistent state. WAL ensures that all state mutations are logged before being applied, enabling recovery on restart.

---

## Scope

### Tasks

1. **Create `src/core/WriteAheadLog.h` / `WriteAheadLog.cpp`**:
   ```cpp
   namespace markamp::core {

   struct WALEntry {
       uint64_t sequence_number;
       SubsystemId subsystem;
       std::string operation;        // "set_config", "save_document", "rename_file", etc.
       std::string payload_json;     // Serialized operation data
       uint32_t checksum;            // CRC32 of payload
       bool committed{false};
       std::chrono::system_clock::time_point timestamp;
   };

   class WriteAheadLog {
   public:
       explicit WriteAheadLog(std::filesystem::path wal_file);

       // Append a WAL entry (before performing the operation)
       [[nodiscard]] auto append(SubsystemId subsystem,
                                  std::string_view operation,
                                  std::string_view payload_json) -> Result<uint64_t>;

       // Mark an entry as committed (after operation succeeds)
       [[nodiscard]] auto commit(uint64_t sequence_number) -> Result<void>;

       // Get all uncommitted entries (for recovery)
       [[nodiscard]] auto uncommitted_entries() -> Result<std::vector<WALEntry>>;

       // Prune committed entries older than threshold
       [[nodiscard]] auto prune(std::chrono::hours max_age = std::chrono::hours{24}) -> Result<void>;

       // Get WAL file size
       [[nodiscard]] auto file_size() const -> size_t;

       // Truncate WAL if it exceeds max size
       [[nodiscard]] auto truncate_if_needed(size_t max_bytes = 10 * 1024 * 1024) -> Result<void>;

   private:
       std::filesystem::path wal_file_;
       uint64_t next_sequence_{1};
       std::mutex mutex_;

       [[nodiscard]] auto write_entry(const WALEntry& entry) -> Result<void>;
       [[nodiscard]] auto read_entries() -> Result<std::vector<WALEntry>>;
   };

   } // namespace markamp::core
   ```
   - **Append**: Write WAL entry to disk (with fsync) before returning. This guarantees the entry survives crashes.
   - **Commit**: Update the entry's committed flag in the WAL file. Committed entries can be pruned.
   - **File format**: One JSON entry per line (JSONL format) for easy parsing and append-only writes:
     ```json
     {"seq":1,"subsystem":"Config","op":"set_config","payload":"{\"key\":\"font_size\",\"value\":14}","crc":"ABCD1234","committed":false,"ts":"2026-02-15T10:30:00Z"}
     ```
   - **Pruning**: Remove committed entries and entries older than max_age. Rewrite the file (using AtomicWriter from Phase 31).

2. **Create `src/core/StateRecovery.h` / `StateRecovery.cpp`**:
   ```cpp
   namespace markamp::core {

   class StateRecovery {
   public:
       explicit StateRecovery(WriteAheadLog& wal);

       // Replay uncommitted WAL entries on startup
       [[nodiscard]] auto replay() -> Result<size_t>;  // Returns count of replayed entries

       // Register a recovery handler for an operation type
       void register_handler(std::string_view operation,
                             std::function<Result<void>(std::string_view payload_json)> handler);

       // Check if recovery is needed
       [[nodiscard]] auto needs_recovery() -> bool;

       // Enter corruption isolation mode (read-only workspace)
       void enter_isolation_mode();

       [[nodiscard]] auto is_isolated() const -> bool;

   private:
       WriteAheadLog& wal_;
       std::unordered_map<std::string,
           std::function<Result<void>(std::string_view)>> handlers_;
       bool isolated_{false};
   };

   } // namespace markamp::core
   ```
   - **Recovery replay**: On startup, check for uncommitted WAL entries. Replay each one using registered handlers.
   - **Corrupted entries**: If a WAL entry has a checksum mismatch, skip it with a log warning. Don't crash.
   - **Isolation mode**: If too many corrupted entries are found, enter isolation mode (read-only workspace). User can export data but cannot modify.
   - **Handler registration**: Each subsystem registers handlers for its operation types (e.g., Config registers "set_config", DocumentFileSystem registers "save_document").

3. **Create `tests/unit/test_wal.cpp`**:
   - TEST_CASE: "WAL append writes entry to file"
   - TEST_CASE: "WAL commit marks entry as committed"
   - TEST_CASE: "WAL uncommitted_entries returns only uncommitted"
   - TEST_CASE: "WAL survives process restart (read from file)"
   - TEST_CASE: "WAL corrupted entry skipped with warning"
   - TEST_CASE: "WAL prune removes old committed entries"
   - TEST_CASE: "WAL truncate at max file size"
   - TEST_CASE: "StateRecovery replays uncommitted entries"
   - TEST_CASE: "StateRecovery handles corrupted entries gracefully"
   - TEST_CASE: "StateRecovery enters isolation on many corruptions"
   - TEST_CASE: "WAL checksum validates payload integrity"
   - TEST_CASE: "WAL sequence numbers are monotonically increasing"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/WriteAheadLog.h` |
| Create | `src/core/WriteAheadLog.cpp` |
| Create | `src/core/StateRecovery.h` |
| Create | `src/core/StateRecovery.cpp` |
| Create | `tests/unit/test_wal.cpp` |

---

## Implementation Notes

- **JSONL format**: One JSON object per line. This is append-only friendly — no need to parse the entire file to add a new entry.
- **fsync on append**: Every WAL entry must be fsynced to disk before the operation proceeds. This is the durability guarantee.
- **Commit mechanism**: Two options: (a) Rewrite the line with committed=true, or (b) Append a separate "commit" marker line. Option (b) is simpler for append-only files:
  ```json
  {"commit":1}  // Marks sequence 1 as committed
  ```
- **Pruning**: When the WAL file exceeds max size (10 MB) or on explicit prune, read all entries, filter out committed/old, rewrite using AtomicWriter.
- **Thread safety**: WAL access serialized via mutex. Only one append at a time.
- **Isolation mode**: When `enter_isolation_mode()` is called, all write operations on the workspace return errors. The user can still read files and export, but cannot modify. A UI banner indicates isolation mode.
- **WAL location**: `~/.markamp/workspace_name.wal` (one WAL per workspace).
- **Performance**: fsync on every write is slow. For throughput, consider batching commits (fsync every N entries or every 100ms). For v7, prioritize correctness over speed.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] WAL entries written before state mutation (write-ahead guarantee)
- [ ] Uncommitted WAL entries replayed on startup
- [ ] Corrupted WAL entries skipped with log warning (not crash)
- [ ] Committed entries can be pruned
- [ ] WAL file bounded to 10 MB (auto-truncation)
- [ ] Corruption isolation mode opens workspace read-only
- [ ] WAL checksum validates payload integrity
- [ ] WAL survives process restart (file-based persistence)
- [ ] Sequence numbers monotonically increasing
- [ ] All 12+ test cases pass

---

## Testing Strategy

- Test WAL append/commit/read lifecycle
- Test crash simulation: append without commit, verify recovery finds uncommitted entry
- Test corrupted WAL entry (modify a line in the file, verify it's skipped)
- Test pruning with mix of committed and uncommitted entries
- Test truncation when file exceeds max size
- Test StateRecovery replay with registered handlers
- Test isolation mode activation threshold
- Run under ASan/TSan to verify memory and thread safety
