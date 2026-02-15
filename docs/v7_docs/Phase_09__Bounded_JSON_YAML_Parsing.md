# Phase 09: Bounded JSON/YAML Parsing

## Metadata

| Field | Value |
|---|---|
| Phase ID | 09 |
| Prerequisites | Phase 01 (error types), Phase 02 (validation) |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 2 modified, 1 test |
| PRD Sections | PI-11 (bounded JSON/YAML parsing) |

---

## Objective

Enforce size and depth limits on all JSON/YAML parsing to prevent OOM from malicious or corrupted config files. Provide safe parsing wrappers that check file size, nesting depth, key count, and string value length before full parsing.

---

## Background

The PRD mandates bounded JSON/YAML parsing (PI-11). MarkAmp loads YAML frontmatter from theme files, extension manifests, and config files. A malicious or corrupted file with extreme nesting depth, millions of keys, or gigabyte-sized values could exhaust memory during parsing. This phase wraps the existing YAML/JSON parsing with pre-validation limits.

---

## Scope

### Tasks

1. **Create `src/core/BoundedParser.h` / `BoundedParser.cpp`**:
   ```cpp
   namespace markamp::core {

   struct ParsingLimits {
       size_t max_file_size{10 * 1024 * 1024};    // 10 MB
       size_t max_yaml_depth{20};                   // Nesting depth
       size_t max_json_depth{50};                   // Nesting depth
       size_t max_key_count{10000};                  // Total keys
       size_t max_string_value_length{1 * 1024 * 1024};  // 1 MB per value
   };

   class BoundedParser {
   public:
       explicit BoundedParser(ParsingLimits limits = {});

       // Parse YAML string with bounds checking
       [[nodiscard]] auto parse_yaml(std::string_view input) -> Result<YAML::Node>;

       // Parse YAML file with size check first
       [[nodiscard]] auto parse_yaml_file(const std::filesystem::path& path) -> Result<YAML::Node>;

       // Parse JSON string with bounds checking
       [[nodiscard]] auto parse_json(std::string_view input) -> Result<nlohmann::json>;

       // Parse JSON file with size check first
       [[nodiscard]] auto parse_json_file(const std::filesystem::path& path) -> Result<nlohmann::json>;

       [[nodiscard]] auto limits() const -> const ParsingLimits&;

   private:
       ParsingLimits limits_;

       [[nodiscard]] auto check_file_size(const std::filesystem::path& path) -> Result<void>;
       [[nodiscard]] auto check_yaml_depth(const YAML::Node& node, size_t current_depth) -> Result<void>;
       [[nodiscard]] auto check_json_depth(const nlohmann::json& node, size_t current_depth) -> Result<void>;
       [[nodiscard]] auto count_keys(const YAML::Node& node) -> Result<size_t>;
   };

   } // namespace markamp::core
   ```
   - Pre-parsing: check file size via `std::filesystem::file_size()` with error_code
   - Post-parsing depth check: walk the parsed tree and count nesting depth
   - Post-parsing key count: walk the parsed tree and count total keys
   - Post-parsing value length: check each string value against max_string_value_length
   - On any limit exceeded: return error with specific limit name and actual value

2. **Modify `src/core/Config.cpp`**:
   - Replace direct YAML parsing calls with `BoundedParser::parse_yaml_file()`
   - Use default limits for config files
   - Handle error result (log and use default config on parse failure)

3. **Modify `src/core/ExtensionManifest.cpp`**:
   - Replace direct JSON/YAML parsing with `BoundedParser::parse_json()` or `BoundedParser::parse_yaml()`
   - Manifest-specific limits: `max_file_size = 1 MB`, `max_depth = 10`

4. **Create `tests/unit/test_bounded_parser.cpp`**:
   - TEST_CASE: "parse_yaml accepts valid YAML"
   - TEST_CASE: "parse_yaml_file rejects oversized file"
   - TEST_CASE: "parse_yaml rejects deeply nested YAML"
   - TEST_CASE: "parse_yaml rejects excessive key count"
   - TEST_CASE: "parse_yaml rejects oversized string value"
   - TEST_CASE: "parse_json accepts valid JSON"
   - TEST_CASE: "parse_json rejects deeply nested JSON"
   - TEST_CASE: "Error includes specific limit exceeded"
   - TEST_CASE: "Error includes actual value vs limit"
   - TEST_CASE: "Config loading works with valid config"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/BoundedParser.h` |
| Create | `src/core/BoundedParser.cpp` |
| Modify | `src/core/Config.cpp` |
| Modify | `src/core/ExtensionManifest.cpp` |
| Create | `tests/unit/test_bounded_parser.cpp` |

---

## Implementation Notes

- **Two-phase checking**: First check file size (cheap, O(1)), then parse, then walk tree for depth/key checks (linear in tree size). This ensures we never attempt to load a 1 GB file into memory.
- **YAML depth check**: YAML::Node traversal — recursively count depth for Map and Sequence nodes. Use iterative traversal with explicit stack to avoid stack overflow on deep documents.
- **JSON depth check**: nlohmann::json traversal — similar recursive/iterative depth check.
- **String value length**: During tree walk, check `.as<std::string>()` length for YAML scalar nodes. For JSON, check `.get<std::string>()` for string types.
- **Error messages**: Include both the limit name and the actual value. Example: "YAML nesting depth exceeded: limit=20, actual=150"
- **Exception handling**: YAML parsing itself can throw. Wrap in try/catch and convert to Result error.
- **Backward compatibility**: Config and ExtensionManifest must continue to work for all valid files. Only reject files that exceed safety limits.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] 100 MB YAML file rejected before fully loading (file size pre-check)
- [ ] Deeply nested YAML (100 levels) rejected with clear error
- [ ] YAML with 100,000+ keys rejected
- [ ] YAML with 10 MB string value rejected
- [ ] Error message includes specific limit that was exceeded and actual value
- [ ] Config loading continues to work for all valid config files
- [ ] ExtensionManifest loading continues to work for valid manifests
- [ ] YAML parse exceptions caught and converted to Result errors
- [ ] JSON depth checking works correctly
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Generate test YAML/JSON with specific characteristics (deep nesting, many keys, large values)
- Test boundary conditions (exactly at limit, one over limit)
- Test with real MarkAmp config files to verify no false rejections
- Test error message formatting for actionability
- Run under ASan to verify no memory issues during tree traversal
