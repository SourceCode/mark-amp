# Phase 13: MIME Validation and Markdown Parsing Limits

## Metadata

| Field | Value |
|---|---|
| Phase ID | 13 |
| Prerequisites | Phase 01 (Result types), Phase 10 (safe regex) |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 2 modified, 1 test |
| PRD Sections | PII-21 (strict MIME validation), PII-22 (markdown parsing hard limits), PII-23 (sandboxed HTML rendering), PII-24 (time-limited parsing) |

---

## Objective

Add content-type validation for embedded content, enforce hard limits on markdown parsing (nesting depth, table size, Mermaid node count), block inline script execution in HTML preview, and add time-limited parsing with deadline enforcement.

---

## Background

The PRD requires strict MIME validation for embedded content (PII-21), markdown parsing hard limits (PII-22), sandboxed HTML rendering with script blocking (PII-23), and time-limited parsing (PII-24). A malicious markdown file with 200 levels of nested blockquotes, a table with 1 million rows, or embedded `<script>` tags could crash the application or create security vulnerabilities.

---

## Scope

### Tasks

1. **Create `src/core/ContentValidator.h` / `ContentValidator.cpp`**:
   ```cpp
   namespace markamp::core {

   // MIME type validation
   namespace mime {
       constexpr std::array<std::string_view, 6> kAllowedEmbedTypes = {
           "image/png", "image/jpeg", "image/gif", "image/svg+xml",
           "text/plain", "application/pdf"
       };

       [[nodiscard]] auto is_allowed_embed_type(std::string_view mime_type) -> bool;
       [[nodiscard]] auto validate_mime_type(std::string_view mime_type) -> Result<std::string_view>;
       [[nodiscard]] auto detect_mime_from_extension(std::string_view file_extension) -> std::string_view;
   }

   // Markdown parsing limits
   struct MarkdownLimits {
       size_t max_nesting_depth{100};
       size_t max_table_columns{200};
       size_t max_table_rows{50000};
       size_t max_mermaid_nodes{500};
       std::chrono::seconds max_parse_time{5};
       size_t max_document_size{50 * 1024 * 1024};  // 50 MB
   };

   class ContentValidator {
   public:
       explicit ContentValidator(MarkdownLimits limits = {});

       // Validate markdown content before full parsing
       [[nodiscard]] auto pre_validate_markdown(std::string_view content) -> Result<void>;

       // Check nesting depth (quick scan)
       [[nodiscard]] auto check_nesting_depth(std::string_view content) -> Result<void>;

       // Check table dimensions (quick scan)
       [[nodiscard]] auto check_table_size(std::string_view content) -> Result<void>;

       // Sanitize HTML output (remove scripts)
       [[nodiscard]] auto sanitize_html(std::string_view html) -> std::string;

       [[nodiscard]] auto limits() const -> const MarkdownLimits&;

   private:
       MarkdownLimits limits_;
   };

   } // namespace markamp::core
   ```
   - **MIME validation**: Allowlist-based. Only permit known-safe embedded content types.
   - **Nesting depth check**: Count consecutive `>` (blockquote), `- ` (list), indentation levels. Approximate but catches pathological cases.
   - **Table size check**: Count `|` characters per line to estimate columns, count lines starting with `|` for rows.
   - **HTML sanitization**: Remove all `<script>`, `<iframe>`, `<object>`, `<embed>`, `<applet>` tags and `on*` event attributes.
   - **Parse timeout**: Provide deadline checking utility that markdown parser can call periodically.

2. **Modify `src/core/MarkdownParser.cpp`** (or equivalent markdown processing):
   - Call `ContentValidator::pre_validate_markdown()` before parsing
   - If pre-validation fails: return partial result with warning, do not crash
   - Apply `MarkdownLimits::max_parse_time` by checking elapsed time at document milestones

3. **Modify `src/core/HtmlSanitizer.cpp`** (or equivalent HTML output):
   - Use `ContentValidator::sanitize_html()` on all HTML output
   - Ensure `<script>` tags are permanently removed from preview output
   - Remove `on*` event handler attributes (onclick, onerror, onload, etc.)

4. **Create `tests/unit/test_content_validator.cpp`**:
   - TEST_CASE: "MIME validation accepts image/png"
   - TEST_CASE: "MIME validation rejects application/javascript"
   - TEST_CASE: "MIME detection from .png extension"
   - TEST_CASE: "Nesting depth check rejects 200-level nesting"
   - TEST_CASE: "Nesting depth check accepts normal nesting"
   - TEST_CASE: "Table size check rejects 1 million rows"
   - TEST_CASE: "Table size check accepts normal table"
   - TEST_CASE: "HTML sanitize removes script tags"
   - TEST_CASE: "HTML sanitize removes onclick attributes"
   - TEST_CASE: "HTML sanitize preserves safe HTML"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ContentValidator.h` |
| Create | `src/core/ContentValidator.cpp` |
| Modify | `src/core/MarkdownParser.cpp` |
| Modify | `src/core/HtmlSanitizer.cpp` |
| Create | `tests/unit/test_content_validator.cpp` |

---

## Implementation Notes

- **Nesting depth estimation**: Count consecutive `>` at line start for blockquotes. Count indent levels (4 spaces = 1 level) for lists. This is an approximation — the full parser will do accurate counting. The pre-validator catches obviously pathological cases.
- **Table row estimation**: Count lines that start with `|` (after trimming whitespace). This is fast (single pass over content) and catches 1M-row tables.
- **HTML sanitization**: Use regex (from Phase 10 `safe_regex_replace`) or manual string scanning to remove script tags. For `on*` attributes, scan for ` on` followed by alphanumeric characters and `=`.
- **MIME allowlist**: Be conservative. Only allow types that the rendering engine can safely handle. `image/svg+xml` is allowed but SVG content should also be sanitized (SVG can contain scripts).
- **Parse timeout**: Provide a `DeadlineChecker` utility that the parser calls at regular intervals (e.g., every 1000 lines). If deadline exceeded, parser returns partial result.
- **Backward compatibility**: Normal markdown files (even large ones) should parse without issues. Only pathological files are rejected.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] 200-level nested blockquotes rejected safely (not crash)
- [ ] Table with 1 million rows rejected before OOM
- [ ] Mermaid diagram with 1000+ nodes limited to 500
- [ ] Parsing timeout produces partial result + warning (not crash)
- [ ] `<script>` tags removed from HTML preview output
- [ ] `onclick` and other `on*` attributes removed
- [ ] MIME validation accepts allowed types, rejects script types
- [ ] Normal markdown documents parse without any change in behavior
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Generate test markdown with specific nesting depths (50, 100, 150, 200)
- Generate test markdown with large tables (100 rows, 50000 rows, 100000 rows)
- Test HTML sanitization with various script injection patterns
- Test MIME validation with comprehensive type list
- Verify partial parse result on timeout is usable (not empty)
- Run under ASan to verify no memory issues with large documents
