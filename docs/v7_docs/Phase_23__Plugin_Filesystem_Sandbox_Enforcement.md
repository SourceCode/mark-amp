# Phase 23: Plugin Filesystem Sandbox Enforcement

## Metadata

| Field | Value |
|---|---|
| Phase ID | 23 |
| Prerequisites | Phase 08 (SafePath), Phase 22 (plugin isolation) |
| Estimated Complexity | Medium |
| Estimated File Count | 0 created, 2 modified, 1 test |
| PRD Sections | PI-31 (sandbox filesystem restrictions), PI-33 (fail-closed policy) |

---

## Objective

Enforce that extensions can only access files within their allowed boundaries: their extension directory, workspace storage path, and global storage path. Extensions without the `kFilesystem` permission cannot access any filesystem API. Fail-closed: if permission validation fails for any reason, access is denied.

---

## Background

The PRD mandates sandbox filesystem restrictions (PI-31) and a fail-closed policy (PI-33). Currently `ExtensionSandbox` exists but may not enforce path boundaries at the filesystem level. A malicious extension could potentially read/write files outside its designated directories. This phase uses `SafePath` from Phase 08 to enforce strict path boundaries.

---

## Scope

### Tasks

1. **Modify `src/core/ExtensionSandbox.h` / `ExtensionSandbox.cpp`**:
   - Add filesystem boundary enforcement:
     ```cpp
     class ExtensionSandbox {
     public:
         // ... existing API ...

         // Validate a file access request from an extension
         [[nodiscard]] auto validate_file_access(
             const std::string& extension_id,
             const std::filesystem::path& requested_path,
             FileAccessMode mode  // Read, Write, Create, Delete
         ) -> Result<std::filesystem::path>;

         // Register allowed paths for an extension
         void register_extension_paths(
             const std::string& extension_id,
             const std::filesystem::path& extension_dir,
             const std::filesystem::path& workspace_storage,
             const std::filesystem::path& global_storage
         );

     private:
         struct ExtensionPaths {
             SafePath extension_dir;
             SafePath workspace_storage;
             SafePath global_storage;
         };
         std::unordered_map<std::string, ExtensionPaths> extension_paths_;
     };
     ```
   - `validate_file_access()` logic:
     1. Check if extension has `kFilesystem` permission → deny if not
     2. Resolve requested path using `SafePath::resolve_with_symlink_check()`
     3. Check if resolved path is within any of the three allowed boundaries
     4. Return resolved canonical path on success, error on failure
   - **Fail-closed**: Any error during validation (path resolution failure, permission check failure, unexpected exception) results in access denied
   - Log all denied access attempts with extension_id and requested path

2. **Modify extension filesystem API** (in PluginContext or equivalent):
   - All filesystem operations routed through `ExtensionSandbox::validate_file_access()` before actual I/O
   - This includes: `readFile`, `writeFile`, `deleteFile`, `listDirectory`, `stat`

3. **Create `tests/unit/test_plugin_sandbox.cpp`**:
   - TEST_CASE: "Extension can read within extension directory"
   - TEST_CASE: "Extension can write to workspace storage"
   - TEST_CASE: "Extension can write to global storage"
   - TEST_CASE: "Extension cannot read outside allowed paths"
   - TEST_CASE: "Extension cannot write outside allowed paths"
   - TEST_CASE: "Extension without kFilesystem permission denied all access"
   - TEST_CASE: "Symlink escape from extension directory detected and blocked"
   - TEST_CASE: "Path traversal (../../) blocked"
   - TEST_CASE: "Fail-closed: invalid path format results in denial"
   - TEST_CASE: "Denied access logged with extension_id"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `src/core/ExtensionSandbox.h` |
| Modify | `src/core/ExtensionSandbox.cpp` |
| Create | `tests/unit/test_plugin_sandbox.cpp` |

---

## Implementation Notes

- **SafePath integration**: Use `SafePath` from Phase 08 for all path resolution and boundary checking. Each extension gets three `SafePath` instances (one per allowed directory).
- **Permission check**: Extensions declare permissions in their manifest. The sandbox checks `manifest.permissions` for `kFilesystem` before allowing any file operation.
- **Fail-closed design**: The default response for any edge case is "deny." If `SafePath::resolve()` returns an error, return denial. If the extension is not registered, return denial. If the permission check throws, return denial.
- **Logging**: All denied accesses logged at WARN level with: extension_id, requested_path, denial_reason. This is critical for debugging extension issues.
- **Registration timing**: `register_extension_paths()` is called during extension activation, before any file operations are possible.
- **Performance**: Path validation involves filesystem calls (`weakly_canonical`, `is_symlink`). Cache results for frequently accessed paths.
- Update `tests/CMakeLists.txt` for new test.

---

## Acceptance Criteria

- [ ] Extension can read/write files within its extension_path
- [ ] Extension can read/write files within workspace_storage_path
- [ ] Extension can read/write files within global_storage_path
- [ ] Extension CANNOT access files outside these three boundaries
- [ ] Extension without `kFilesystem` permission cannot call any filesystem API
- [ ] Symlink escape from extension directory detected and blocked
- [ ] Path traversal (`../../`) blocked
- [ ] Fail-closed: any validation error results in access denial
- [ ] All denied access attempts logged with extension_id and path
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Create temp directories simulating extension/workspace/global storage
- Test valid access within each boundary
- Test invalid access outside boundaries (traversal, absolute paths, symlinks)
- Test permission denial for extensions without kFilesystem
- Test fail-closed behavior with malformed paths
- Verify log output for denied access
- Run under ASan to verify no path handling memory issues
