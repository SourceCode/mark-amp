# Phase 27: Security Hardening

## Objective

Harden MarkAmp against security threats: sanitize all rendered HTML to prevent XSS, sandbox rendering engines, restrict file access to local-only, validate all external input (theme files, markdown content), and ensure no remote code execution is possible.

## Prerequisites

- Phase 14 (Markdown-to-Rendered-View Pipeline)
- Phase 17 (Mermaid Diagram Integration)
- Phase 11 (File System Integration)

## Deliverables

1. HTML sanitization for all rendered content
2. Sandboxed Mermaid rendering
3. File access restrictions (no remote loading)
4. Input validation for all external data
5. Content Security Policy enforcement
6. Security audit report

## Tasks

### Task 27.1: Implement HTML sanitization

Create `/Users/ryanrentfro/code/markamp/src/core/HtmlSanitizer.h` and `HtmlSanitizer.cpp`:

All HTML output must be sanitized before display to prevent XSS attacks via malicious markdown files.

```cpp
namespace markamp::core {

class HtmlSanitizer
{
public:
    // Sanitize HTML content, removing dangerous elements and attributes
    auto sanitize(std::string_view html) -> std::string;

    // Configuration
    void allow_tag(const std::string& tag);
    void allow_attribute(const std::string& tag, const std::string& attribute);
    void block_tag(const std::string& tag);

private:
    // Allowed tags (whitelist approach)
    std::set<std::string> allowed_tags_;
    std::map<std::string, std::set<std::string>> allowed_attributes_;

    void initialize_defaults();
    auto sanitize_tag(std::string_view tag_content) -> std::string;
    auto sanitize_attribute(const std::string& tag, const std::string& attr,
                           const std::string& value) -> bool;
};

} // namespace markamp::core
```

**Allowed tags (whitelist):**
```
h1, h2, h3, h4, h5, h6, p, br, hr,
em, strong, del, code, pre,
ul, ol, li, input (checkbox only),
blockquote, table, thead, tbody, tr, th, td,
a, img, div, span, sup, section
```

**Blocked tags (always removed):**
```
script, style, iframe, object, embed, form,
input (non-checkbox), button, textarea, select,
link, meta, base, applet, frame, frameset
```

**Allowed attributes per tag:**
| Tag | Allowed Attributes |
|---|---|
| a | href, title (no javascript: URIs) |
| img | src (no data: URIs for SVG, local file: only), alt, title, width, height |
| input | type (checkbox only), checked, disabled |
| td, th | style (alignment only), class |
| code | class (language- prefix only) |
| div | class |
| * | id (for anchors) |

**Blocked attribute patterns:**
- `on*` event handlers (onclick, onerror, onload, etc.)
- `javascript:` URIs
- `data:` URIs for `<img>` with SVG content type (can contain scripts)
- `style` containing `expression()`, `url()`, or `import`

**Acceptance criteria:**
- All dangerous HTML is stripped
- Safe HTML passes through unchanged
- XSS attack vectors from OWASP list are neutralized
- Sanitizer does not break valid markdown-generated HTML

### Task 27.2: Apply sanitization to the rendering pipeline

Integrate the sanitizer into the rendering flow:

```cpp
void PreviewPanel::RenderContent(const std::string& markdown)
{
    // 1. Parse markdown
    auto doc = parser_->parse(markdown);

    // 2. Render to HTML
    auto body_html = renderer.render(*doc);

    // 3. SANITIZE the HTML output
    auto safe_html = sanitizer_.sanitize(body_html);

    // 4. Display
    DisplayHtml(safe_html);
}
```

**When to sanitize:**
- Always, even for internally generated HTML
- Defense in depth: even if md4c produces safe HTML, the sanitizer is a safety net
- Performance: sanitization should not add significant overhead (target: under 5ms for 10K-line documents)

**Acceptance criteria:**
- All displayed HTML passes through the sanitizer
- Sanitization does not noticeably slow rendering
- Valid markdown content is not altered by sanitization

### Task 27.3: Sandbox Mermaid rendering

If using an embedded JavaScript engine (QuickJS):

**Sandbox measures:**
1. **No filesystem access:** QuickJS instance cannot read/write files
2. **No network access:** No HTTP requests from within the JS engine
3. **Memory limit:** Cap QuickJS memory at 64MB
4. **CPU limit:** Set execution timeout to 10 seconds
5. **No DOM access:** Mermaid runs in a headless mode (SVG string output only)
6. **Restricted globals:** Only expose the minimum needed globals (no `eval()` bypasses)

**Implementation:**
```cpp
// QuickJS security configuration
JS_SetMemoryLimit(runtime, 64 * 1024 * 1024);  // 64MB
JS_SetMaxStackSize(runtime, 1 * 1024 * 1024);   // 1MB stack
// Set interrupt handler for timeout
JS_SetInterruptHandler(runtime, timeout_check, &timeout_data);
```

**If using mmdc (CLI fallback):**
1. Run in a restricted environment (no network, limited filesystem)
2. Use temporary directories with restricted permissions
3. Clean up all temp files after rendering
4. Validate SVG output before displaying

**Acceptance criteria:**
- JavaScript engine is sandboxed
- Memory and CPU limits are enforced
- Malicious Mermaid input cannot escape the sandbox
- Timeout prevents infinite loops

### Task 27.4: Restrict file access

Ensure the application only accesses local files:

**Restrictions:**
1. **No remote content loading:**
   - Images: only `file://` and relative paths, no `http://` or `https://`
   - Links: external links open in system browser, not in the app
   - CSS: no `@import` from remote URLs
   - Fonts: only bundled fonts, no remote font loading
2. **No arbitrary code execution:**
   - Do not execute scripts found in markdown files
   - HTML passthrough is sanitized (no `<script>` tags)
   - SVG content is sanitized (no inline scripts)
3. **Path traversal prevention:**
   - Validate all file paths: resolve to absolute path, check it is within expected directories
   - Prevent `../../../etc/passwd` style attacks in image paths
   - Use `std::filesystem::canonical()` to resolve symlinks

**SVG sanitization for Mermaid output:**
```cpp
auto sanitize_svg(std::string_view svg) -> std::string
{
    // Remove any <script> elements
    // Remove on* event attributes
    // Remove <foreignObject> elements (can contain HTML)
    // Validate all href/xlink:href attributes
}
```

**Acceptance criteria:**
- No remote resources are loaded
- Path traversal attacks are blocked
- SVG output from Mermaid is sanitized
- Malicious markdown files cannot execute code

### Task 27.5: Validate all external input

Validate data at every trust boundary:

**Theme file validation (from Phase 22, verify and harden):**
1. JSON structure validation
2. Color value validation (no arbitrary strings)
3. String length limits: name < 100 chars, id < 64 chars
4. No embedded scripts or control characters in strings

**File content validation:**
1. Encoding validation (reject binary files with clear error)
2. File size limits (configurable, default 50MB)
3. Null byte detection (reject files containing null bytes)

**Config file validation:**
1. JSON structure validation
2. Value range checking for all numeric settings
3. Reject unknown keys (log warning, do not crash)

**Acceptance criteria:**
- Malformed input produces clear error messages (not crashes)
- Fuzzing with random data does not cause undefined behavior
- All validation is documented

### Task 27.6: Implement Content Security Policy (for wxWebView)

If using wxWebView for the preview pane, implement CSP:

**CSP rules:**
```
default-src 'none';
style-src 'unsafe-inline';
img-src data: file:;
font-src data:;
script-src 'none';
```

This blocks:
- All remote resource loading
- All script execution
- All external styles
- All form submissions

**Implementation:**
Inject CSP as a `<meta>` tag in the generated HTML:
```html
<meta http-equiv="Content-Security-Policy"
      content="default-src 'none'; style-src 'unsafe-inline'; img-src data: file:; font-src data:;">
```

**Acceptance criteria:**
- CSP prevents remote resource loading
- CSP prevents script execution in the preview
- CSP does not break normal rendering

### Task 27.7: Create security test suite

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_security.cpp`:

Test cases with malicious input:
1. **XSS via markdown:** `<script>alert('xss')</script>` -- must be stripped
2. **XSS via img tag:** `<img src=x onerror=alert(1)>` -- onerror must be stripped
3. **XSS via link:** `[click](javascript:alert(1))` -- javascript: URI must be blocked
4. **Path traversal in image:** `![](../../../etc/passwd)` -- must be blocked
5. **SVG with embedded script:** -- script elements must be stripped
6. **Malformed theme JSON with script injection:** -- must be rejected
7. **Very long input (DoS):** 10MB markdown file -- must handle gracefully
8. **Binary content in markdown:** null bytes -- must be rejected or handled
9. **Recursive markdown structures (DoS):** deeply nested lists (1000 levels) -- must handle
10. **Mermaid with malicious JavaScript:** (if using JS engine) -- must be sandboxed
11. **Event handler attributes in HTML:** `<div onmouseover="...">` -- must be stripped
12. **CSS injection:** `<div style="background:url(javascript:...)">` -- must be stripped
13. **data: URI with script:** `<a href="data:text/html,<script>...">` -- must be blocked
14. **Unicode-based attacks:** right-to-left override characters -- must be handled

**Acceptance criteria:**
- All 14+ test cases pass
- No XSS vector succeeds
- No path traversal succeeds
- No code execution possible via user content
- Application does not crash on any malicious input

### Task 27.8: Write security audit document

Create `/Users/ryanrentfro/code/markamp/docs/v1_docs/security_audit.md`:

Document:
1. Threat model (attack surfaces, trust boundaries)
2. Security controls implemented
3. Known limitations
4. Test results
5. Recommendations for future hardening

**Acceptance criteria:**
- Comprehensive security documentation
- All controls are described with rationale
- Known limitations are honestly documented

## Files Created/Modified

| File | Action |
|---|---|
| `src/core/HtmlSanitizer.h` | Created |
| `src/core/HtmlSanitizer.cpp` | Created |
| `src/rendering/HtmlRenderer.cpp` | Modified (integrate sanitizer) |
| `src/ui/PreviewPanel.cpp` | Modified (sanitization, CSP) |
| `src/core/MermaidRenderer.cpp` | Modified (sandboxing) |
| `src/core/FileSystem.cpp` | Modified (path validation) |
| `src/core/ThemeValidator.cpp` | Modified (string length limits) |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_security.cpp` | Created |
| `docs/v1_docs/security_audit.md` | Created |

## Dependencies

- All rendering and file access components
- Phase 17 Mermaid integration (for sandboxing)
- Phase 14 rendering pipeline (for sanitization)
- Phase 11 file system (for path validation)

## Estimated Complexity

**Medium** -- The HTML sanitizer is the most complex component, requiring careful whitelisting. Mermaid sandboxing depends on the chosen rendering approach. File access restrictions are straightforward but must be thorough.
