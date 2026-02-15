# Phase 08: Filesystem Canonicalization Wrapper

## Metadata

| Field | Value |
|---|---|
| Phase ID | 08 |
| Prerequisites | Phase 02 (PathValidator) |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 0 modified, 1 test |
| PRD Sections | PI-10 (filesystem canonicalization), PI-31 (sandbox filesystem restrictions) |

---

## Objective

Create a `SafePath` wrapper class that provides safe path resolution, prevents directory traversal attacks, detects symlink escapes, and uses `std::error_code` overloads exclusively (no filesystem exceptions). This becomes the single path resolution layer for all security-sensitive file access.

---

## Background

The PRD mandates a filesystem canonicalization wrapper (PI-10) that uses error_code overloads, rejects traversal outside workspace, and detects symlink escapes. While Phase 02 provides low-level `PathValidator` functions, this phase wraps them into a higher-level `SafePath` class that enforces boundaries automatically and is used by Phase 23 (plugin filesystem sandbox).

---

## Scope

### Tasks

1. **Create `src/core/SafePath.h` / `SafePath.cpp`**:
   ```cpp
   namespace markamp::core {

   class SafePath {
   public:
       // Create a SafePath resolver for a specific boundary
       explicit SafePath(std::filesystem::path allowed_root);

       // Resolve a requested path within the boundary
       [[nodiscard]] auto resolve(const std::filesystem::path& requested) const
           -> Result<std::filesystem::path>;

       // Check if a path is within the boundary (no resolution)
       [[nodiscard]] auto is_within_boundary(const std::filesystem::path& path) const -> bool;

       // Resolve with symlink detection
       [[nodiscard]] auto resolve_with_symlink_check(const std::filesystem::path& requested) const
           -> Result<std::filesystem::path>;

       // Join paths safely (prevents traversal via concatenation)
       [[nodiscard]] auto join(const std::filesystem::path& relative) const
           -> Result<std::filesystem::path>;

       // Get the allowed root
       [[nodiscard]] auto root() const -> const std::filesystem::path&;

   private:
       std::filesystem::path allowed_root_;
   };

   // Convenience functions for common boundaries
   [[nodiscard]] auto workspace_safe_path(const std::filesystem::path& workspace_root)
       -> SafePath;

   [[nodiscard]] auto extension_safe_path(const std::filesystem::path& extension_dir)
       -> SafePath;

   } // namespace markamp::core
   ```
   - `resolve()`:
     1. Reject paths containing null bytes
     2. Normalize the path (resolve `.` and `..`)
     3. Canonicalize using `std::filesystem::weakly_canonical()` with `std::error_code`
     4. Verify result starts with `allowed_root_`
     5. Return error with `ErrorCode::PathTraversal` if outside boundary
   - `resolve_with_symlink_check()`:
     1. Perform `resolve()` checks
     2. Check if any path component is a symlink
     3. If symlink target resolves outside boundary, return `ErrorCode::SymlinkEscape`
   - All filesystem operations use `std::error_code` overloads — zero exceptions
   - Platform considerations:
     - macOS: case-insensitive comparison for HFS+/APFS
     - Linux: case-sensitive comparison for ext4
     - Windows: case-insensitive, handle UNC paths and drive letters

2. **Create `tests/unit/test_safe_path.cpp`**:
   - TEST_CASE: "resolve accepts valid relative path"
   - TEST_CASE: "resolve accepts valid absolute path within boundary"
   - TEST_CASE: "resolve rejects ../../../etc/passwd"
   - TEST_CASE: "resolve rejects ../../ traversal"
   - TEST_CASE: "resolve rejects path with null bytes"
   - TEST_CASE: "resolve rejects empty path"
   - TEST_CASE: "is_within_boundary returns true for child path"
   - TEST_CASE: "is_within_boundary returns false for parent path"
   - TEST_CASE: "resolve_with_symlink_check detects escape" (create temp symlink in test)
   - TEST_CASE: "resolve_with_symlink_check accepts internal symlink"
   - TEST_CASE: "join prevents traversal via relative path"
   - TEST_CASE: "join works with simple filename"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/SafePath.h` |
| Create | `src/core/SafePath.cpp` |
| Create | `tests/unit/test_safe_path.cpp` |

---

## Implementation Notes

- **`weakly_canonical()`**: Use this instead of `canonical()` because `canonical()` throws if the file doesn't exist. `weakly_canonical()` resolves what it can and leaves the rest.
- **Path comparison**: After canonicalization, use string prefix comparison on the path. Ensure the comparison checks a full path component boundary (e.g., `/workspace/notes` should not match `/workspace/notes-backup`). Compare using `path.string().starts_with(root.string() + "/")` or by iterating path components.
- **Platform case sensitivity**: On macOS, convert both paths to lowercase before comparison (HFS+ is case-insensitive by default). Use `#ifdef __APPLE__` guard.
- **Symlink detection**: Use `std::filesystem::is_symlink()` with `std::error_code` to check each path component. Resolve the symlink target and verify it's within boundary.
- **Null byte rejection**: Check `path.string().find('\0') != npos` before any filesystem operation.
- **Error codes**: Use `ErrorCode::PathTraversal` for `..` attacks, `ErrorCode::SymlinkEscape` for symlink escapes, `ErrorCode::InvalidArgument` for empty/null paths.
- This class is used by Phase 23 (Plugin Filesystem Sandbox) as the enforcement mechanism.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] `../../../etc/passwd` traversal rejected with `ErrorCode::PathTraversal`
- [ ] Symlink pointing outside workspace detected with `ErrorCode::SymlinkEscape`
- [ ] Null bytes in path rejected
- [ ] All filesystem operations use `std::error_code` overloads (no exceptions)
- [ ] Works correctly on macOS (case-insensitive HFS+/APFS)
- [ ] Works correctly on Linux (case-sensitive ext4)
- [ ] Path component boundary correctly enforced (no prefix false positives)
- [ ] `join()` prevents traversal via malicious relative paths
- [ ] All 12+ test cases pass

---

## Testing Strategy

- Unit tests with adversarial path strings (traversal, null bytes, very long paths)
- Symlink test: create temp directory with symlink pointing outside, verify detection
- Test on actual filesystem (not mocked) for symlink and canonicalization accuracy
- Test path component boundary: verify `/workspace/notes` does not match `/workspace/notes-backup`
- Run under ASan to verify no buffer overflows in path string handling
