# Phase 12: Extension Manifest Schema Validation

## Metadata

| Field | Value |
|---|---|
| Phase ID | 12 |
| Prerequisites | Phase 02 (validation), Phase 09 (bounded parsing) |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 1 modified, 1 test |
| PRD Sections | PI-15 (strict schema validation for ExtensionManifest), PI-32 (extension API contract versioning) |

---

## Objective

Validate extension manifests against a strict schema before loading, rejecting malformed or incompatible extensions early. This catches errors at load time rather than at runtime when they would cause crashes or undefined behavior.

---

## Background

The PRD mandates strict schema validation for ExtensionManifest (PI-15). The existing `ExtensionManifest::parse()` does some field extraction but lacks comprehensive validation. Missing required fields, invalid semver strings, unknown permission values, or incompatible API versions can all cause downstream failures. This phase adds a validation layer between parsing and loading.

---

## Scope

### Tasks

1. **Create `src/core/ManifestValidator.h` / `ManifestValidator.cpp`**:
   ```cpp
   namespace markamp::core {

   struct ManifestValidationResult {
       bool valid{true};
       std::vector<std::string> errors;
       std::vector<std::string> warnings;
   };

   class ManifestValidator {
   public:
       [[nodiscard]] auto validate(const ExtensionManifest& manifest)
           -> ManifestValidationResult;

       // Set the host API version for compatibility checks
       void set_host_api_version(std::string_view version);

   private:
       std::string host_api_version_{"1.0.0"};

       [[nodiscard]] auto validate_required_fields(const ExtensionManifest& m) -> std::vector<std::string>;
       [[nodiscard]] auto validate_semver(std::string_view version, std::string_view field_name) -> std::optional<std::string>;
       [[nodiscard]] auto validate_activation_events(const ExtensionManifest& m) -> std::vector<std::string>;
       [[nodiscard]] auto validate_permissions(const ExtensionManifest& m) -> std::vector<std::string>;
       [[nodiscard]] auto validate_api_compatibility(const ExtensionManifest& m) -> std::vector<std::string>;
       [[nodiscard]] auto validate_contribution_points(const ExtensionManifest& m) -> std::vector<std::string>;
   };

   } // namespace markamp::core
   ```

   **Validation rules:**
   - **Required fields**: `name`, `publisher`, `version`, `engines.markamp` must be present and non-empty
   - **Semver format**: version, engines.markamp must match `MAJOR.MINOR.PATCH` (optional `-prerelease+build`)
   - **Activation events**: must match known patterns (`onLanguage:*`, `onCommand:*`, `onStartupFinished`, `*`)
   - **Permissions**: must match `ExtensionPermission` enum values (`kFilesystem`, `kNetwork`, `kClipboard`, etc.)
   - **API version compatibility**: `engines.markamp` must be compatible with host version (major version match, minor <= host minor)
   - **Contribution points**: if present, must be known types (`commands`, `themes`, `languages`, `menus`, `keybindings`, etc.)
   - **Warnings** (non-fatal): missing `displayName`, missing `description`, missing `icon`

2. **Modify `src/core/ExtensionManifest.cpp`**:
   - After `parse()` succeeds, run `ManifestValidator::validate()`
   - If validation has errors: reject manifest, log all errors, return `Result<ExtensionManifest>` error
   - If validation has only warnings: log warnings, continue loading
   - Use BoundedParser (Phase 09) for initial parsing

3. **Create `tests/unit/test_manifest_validator.cpp`**:
   - TEST_CASE: "Valid manifest passes validation"
   - TEST_CASE: "Missing name field rejected"
   - TEST_CASE: "Missing publisher field rejected"
   - TEST_CASE: "Missing version field rejected"
   - TEST_CASE: "Invalid semver rejected"
   - TEST_CASE: "Valid semver with prerelease accepted"
   - TEST_CASE: "Unknown permission rejected"
   - TEST_CASE: "Valid permissions accepted"
   - TEST_CASE: "Unknown activation event rejected"
   - TEST_CASE: "API version incompatibility detected"
   - TEST_CASE: "API version compatible (minor version lower)"
   - TEST_CASE: "Unknown contribution point type generates warning"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ManifestValidator.h` |
| Create | `src/core/ManifestValidator.cpp` |
| Modify | `src/core/ExtensionManifest.cpp` |
| Create | `tests/unit/test_manifest_validator.cpp` |

---

## Implementation Notes

- **Semver parsing**: Implement a simple semver parser. Match pattern `^(\d+)\.(\d+)\.(\d+)(-[\w.]+)?(\+[\w.]+)?$`. Use `safe_regex_compile` from Phase 10 if available, or manual parsing.
- **API version compatibility**: Major version must match exactly. Minor version of the extension must be <= host minor version. This follows the standard semver compatibility contract.
- **Known activation events**: Build a static list of known patterns. For wildcard patterns like `onLanguage:*`, validate the prefix.
- **Known permissions**: Build from the existing `ExtensionPermission` enum values.
- **Error aggregation**: Collect ALL validation errors before returning, don't stop at the first error. This gives extension developers a complete list of issues to fix.
- **Backward compatibility**: Existing valid manifests must continue to load. The validator only rejects manifests that would have caused runtime errors.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Manifest missing `name` field rejected with clear error message
- [ ] Invalid semver `"1.2.banana"` rejected
- [ ] Valid semver `"1.2.3-beta.1+build.42"` accepted
- [ ] Unknown permission `"hack_everything"` rejected
- [ ] API version incompatibility detected (e.g., extension requires v2.0, host is v1.x)
- [ ] API version compatible when extension minor <= host minor
- [ ] All validation errors collected and reported (not just first error)
- [ ] Warnings logged but don't prevent loading
- [ ] Valid manifests continue to load successfully (no false rejections)
- [ ] All 12+ test cases pass

---

## Testing Strategy

- Test with valid manifests (minimal and full) to verify no false rejections
- Test with each required field missing individually
- Test semver edge cases: "0.0.0", "999.999.999", "1.0.0-alpha", "1.0.0+build"
- Test permission validation with all known and unknown values
- Test API version compatibility matrix
- Verify error messages are actionable (include field name and expected format)
