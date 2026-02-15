# Phase 24: Extension API Contract Versioning

## Metadata

| Field | Value |
|---|---|
| Phase ID | 24 |
| Prerequisites | Phase 12 (manifest validator) |
| Estimated Complexity | Low |
| Estimated File Count | 2 created, 1 modified, 1 test |
| PRD Sections | PI-32 (extension API contract versioning), PI-33 (fail-closed — reject incompatible) |

---

## Objective

Add API version negotiation so incompatible plugins are rejected at load time rather than crashing at runtime. Implement semver-based compatibility checking with clear, user-facing error messages for version mismatches.

---

## Background

The PRD mandates extension API contract versioning (PI-32) and fail-closed rejection of incompatible plugins (PI-33). Extensions declare their required API version in the manifest's `engines.markamp` field. If the host API version is incompatible, the extension should be rejected early with a clear error message rather than loading and crashing when it tries to call a missing API.

---

## Scope

### Tasks

1. **Create `src/core/ApiVersion.h` / `ApiVersion.cpp`**:
   ```cpp
   namespace markamp::core {

   struct SemVer {
       int major{0};
       int minor{0};
       int patch{0};
       std::string prerelease;
       std::string build;

       [[nodiscard]] auto to_string() const -> std::string;

       // Comparison operators
       auto operator<=>(const SemVer& other) const -> std::strong_ordering;
       auto operator==(const SemVer& other) const -> bool;
   };

   // Parse a semver string
   [[nodiscard]] auto parse_semver(std::string_view version) -> Result<SemVer>;

   // Check if an extension's required API version is compatible with the host
   // Compatible when: major matches AND extension.minor <= host.minor
   [[nodiscard]] auto is_api_compatible(const SemVer& required, const SemVer& host) -> bool;

   // Get the current host API version
   [[nodiscard]] auto host_api_version() -> SemVer;

   // Check compatibility and return descriptive error if incompatible
   [[nodiscard]] auto check_api_compatibility(
       std::string_view extension_id,
       std::string_view required_version
   ) -> Result<void>;

   } // namespace markamp::core
   ```
   - **Compatibility rules**:
     - Major version must match exactly (breaking changes = major bump)
     - Extension minor version must be <= host minor version (new features = minor bump)
     - Patch version is ignored for compatibility (bug fixes only)
     - Prerelease versions are compared per semver spec
   - **Host API version**: Defined as a compile-time constant (e.g., `1.0.0`)

2. **Modify `src/core/PluginManager.cpp`**:
   - Before activating any plugin, call `check_api_compatibility()`
   - On incompatibility: skip plugin, log error, emit `PluginFaultedEvent` with reason
   - User-facing error message: "Extension '{name}' requires MarkAmp API v{required}, but this version provides v{host}. Please update the extension or MarkAmp."

3. **Create `tests/unit/test_api_version.cpp`**:
   - TEST_CASE: "parse_semver parses valid version"
   - TEST_CASE: "parse_semver rejects invalid version"
   - TEST_CASE: "is_api_compatible: same version"
   - TEST_CASE: "is_api_compatible: lower minor version (compatible)"
   - TEST_CASE: "is_api_compatible: higher minor version (incompatible)"
   - TEST_CASE: "is_api_compatible: different major version (incompatible)"
   - TEST_CASE: "check_api_compatibility returns error with descriptive message"
   - TEST_CASE: "SemVer comparison operators"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ApiVersion.h` |
| Create | `src/core/ApiVersion.cpp` |
| Modify | `src/core/PluginManager.cpp` |
| Create | `tests/unit/test_api_version.cpp` |

---

## Implementation Notes

- **Semver parsing**: Parse `MAJOR.MINOR.PATCH` manually (split on `.`, convert to int). Handle optional `-prerelease` and `+build` suffixes.
- **Host API version**: Define as `constexpr` in `ApiVersion.h`. Update this when making API-breaking changes. Start at `1.0.0`.
- **Compatibility check timing**: Run BEFORE activation. This is more efficient than activating and then crashing.
- **User-facing message**: The error message should guide the user to a resolution (update extension or update MarkAmp). Use `ErrorReportingService::report_and_notify()` if available (Phase 30).
- **Edge cases**: Handle missing `engines.markamp` field (treat as compatible — legacy extensions). Handle malformed version strings (treat as incompatible — fail-closed).
- **SemVer comparison**: Use `<=>` (spaceship operator) for C++20/23 comparison. Compare major, minor, patch in order.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Plugin requiring API v2.0 rejected on host v1.x with clear error
- [ ] Plugin requiring API v1.2 accepted on host v1.5 (minor version compatible)
- [ ] Plugin requiring API v1.5 rejected on host v1.2 (minor too high)
- [ ] API version mismatch produces clear user-facing error message
- [ ] Missing `engines.markamp` field treated as compatible (legacy support)
- [ ] Malformed version string treated as incompatible (fail-closed)
- [ ] SemVer parsing handles prerelease and build metadata
- [ ] All 8+ test cases pass

---

## Testing Strategy

- Test compatibility matrix: all combinations of major/minor/patch relationships
- Test semver parsing with valid versions ("1.0.0", "2.3.4-beta.1+build.42")
- Test semver parsing with invalid versions ("", "1", "1.2", "abc", "1.2.banana")
- Test user-facing error message content
- Test backward compatibility with missing engines.markamp
- Integration test: verify incompatible plugin is skipped and others activate
