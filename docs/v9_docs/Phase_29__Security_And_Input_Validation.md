# Phase 29: Security and Input Validation

## Overview
Result.h, SafePath, SafeRegex, StringGuards, ValidationUtils, MimeValidator, NumericGuards, SafeAllocation, and BoundedContainer exist as new (untracked) files. These resilience primitives exist but are not integrated into the codebase. This phase wires security and validation throughout the application.

## Prerequisites
- Phase 01 (EventBus hardening)
- Phase 02 (Config system)
- Phase 04 (Plugin system for sandbox enforcement)

## Tasks

### Task 1: Wire Result<T> Error Handling Across Core Services
**Files:** `src/core/Result.h`, `src/core/VaultService.cpp`, `src/core/GitService.cpp`
**Description:** Result.h defines Result<T> using std::expected. Wire it as the return type for all fallible operations in core services, replacing raw exceptions and bool returns.
**Acceptance Criteria:**
- VaultService methods return `Result<T>` for file operations
- GitService methods return `Result<T>` for git operations
- Error type carries context: message, source, error code
- Callers use `.and_then()` or `.or_else()` for chaining
- No silent failures: all errors surfaced
- Existing tests updated for Result<T> returns

### Task 2: Wire SafePath for All File System Operations
**Files:** `src/core/SafePath.cpp`, `src/core/SafePath.h`, `src/core/VaultService.cpp`, `src/core/AsyncFileLoader.cpp`
**Description:** SafePath validates and sanitizes file paths. Wire it at all entry points where user-provided paths are used.
**Acceptance Criteria:**
- Path traversal attacks blocked (`../` sequences rejected)
- Null bytes in paths rejected
- Paths confined to workspace root (no escape)
- Symlink resolution with boundary check
- All VaultService file operations use SafePath
- All AsyncFileLoader operations use SafePath

### Task 3: Wire SafeRegex for All Regex Operations
**Files:** `src/core/SafeRegex.cpp`, `src/core/SafeRegex.h`, `src/core/SearchEngine.cpp`, `src/core/FindReplaceService.cpp`
**Description:** SafeRegex wraps std::regex with timeout and complexity limits. Wire it in search, find/replace, and any user-provided regex input.
**Acceptance Criteria:**
- Regex compilation timeout: 100ms
- Regex execution timeout: 500ms
- Complexity limit: reject catastrophic backtracking patterns
- SearchEngine uses SafeRegex for regex search
- FindReplaceService uses SafeRegex
- Error message on invalid/timeout regex

### Task 4: Wire StringGuards for User Input Validation
**Files:** `src/core/StringGuards.cpp`, `src/core/StringGuards.h`
**Description:** StringGuards provides string validation: max length, allowed characters, encoding validation. Wire at all user input points.
**Acceptance Criteria:**
- File name validation: max 255 chars, no illegal characters
- Tag name validation: alphanumeric + hyphens, max 50 chars
- Search query validation: max 1000 chars
- Wiki-link target validation: no control characters
- Config value validation: type-appropriate constraints
- Consistent error messages for invalid input

### Task 5: Wire ValidationUtils for Structured Data
**Files:** `src/core/ValidationUtils.cpp`, `src/core/ValidationUtils.h`
**Description:** ValidationUtils provides validation for structured data: JSON, YAML, frontmatter. Wire at all points where structured data is parsed.
**Acceptance Criteria:**
- Frontmatter validation: valid YAML, expected types
- Config file validation: schema compliance
- Extension manifest validation: required fields
- Board serialization validation: valid JSON structure
- Sync state validation: expected format
- Validation errors include path to invalid field

### Task 6: Wire MimeValidator for File Type Verification
**Files:** `src/core/MimeValidator.cpp`, `src/core/MimeValidator.h`
**Description:** MimeValidator checks file content against expected MIME types. Wire at file import, image paste, and extension asset loading.
**Acceptance Criteria:**
- Image paste: verify content is actual image (not renamed executable)
- File import: verify MIME matches extension
- Extension assets: validate expected types
- PDF viewer: verify content is PDF
- Rejection: clear error message with detected vs expected type
- Magic byte checking for common formats

### Task 7: Wire NumericGuards for Numeric Input
**Files:** `src/core/NumericGuards.h`
**Description:** NumericGuards provides overflow protection, range clamping, and safe arithmetic. Wire at all numeric inputs and computations.
**Acceptance Criteria:**
- Zoom level clamped: 25% to 400%
- Canvas coordinates clamped to reasonable range
- Font size clamped: 6pt to 72pt
- Line number validated: >= 1
- Timer duration validated: 1 second to 24 hours
- Integer overflow protection in arithmetic

### Task 8: Wire BoundedContainer for Collection Limits
**Files:** `src/core/BoundedContainer.h`, `src/core/EventBus.cpp`
**Description:** BoundedContainer provides bounded vectors and maps. Wire in EventBus queued_events_ (currently unbounded vector) and other collections that could grow unbounded.
**Acceptance Criteria:**
- EventBus queued_events_ bounded (max 10,000)
- Navigation history bounded (max 50 per pane)
- Undo stack bounded (max 100 operations)
- Search history bounded (max 50 entries)
- Output channel bounded (max 10,000 lines)
- Overflow policy: drop oldest or reject newest (configurable)

### Task 9: Wire SafeAllocation for Memory Safety
**Files:** `src/core/SafeAllocation.h`
**Description:** SafeAllocation provides allocation guards: max allocation size, total memory budget, allocation failure handling. Wire in large allocation sites.
**Acceptance Criteria:**
- Large file load: allocation guard (max 100MB)
- Canvas board load: allocation guard (max 200MB)
- Image decode: allocation guard (max 50MB per image)
- Search index: allocation guard (max 100MB)
- Allocation failure: graceful degradation with error message
- Memory budget tracked globally

### Task 10: Wire Extension Sandbox Security
**Files:** `src/core/ExtensionSandbox.cpp`, `src/core/PluginManager.cpp`
**Description:** Enforce extension sandbox: file system access restricted to extension directory and workspace, network access requires permission, process spawning blocked.
**Acceptance Criteria:**
- File system: read only extension dir + workspace (with permission)
- Network: blocked unless `network` permission granted
- Process spawning: blocked
- IPC: only through approved channels
- Resource limits: 50MB memory, 10% CPU per extension
- Sandbox violation logged and extension notified

### Task 11: Wire Input Sanitization for Extension API
**Files:** `src/core/PluginContext.h`, `src/core/PluginManager.cpp`
**Description:** All inputs from extensions through PluginContext are validated: string lengths, path safety, event payload sizes, API call rates.
**Acceptance Criteria:**
- String parameters: max length enforced
- Path parameters: SafePath validation
- Event payloads: max size (1MB)
- API call rate: max 100 calls/second per extension
- Invalid input: error returned to extension
- Rate limit exceeded: throttle with warning

### Task 12: Wire Content Security for Preview
**Files:** `src/ui/PreviewPanel.cpp`, `src/rendering/HtmlRenderer.cpp`
**Description:** Preview panel renders HTML from Markdown. Ensure XSS protection: sanitize HTML, restrict script execution, validate URLs.
**Acceptance Criteria:**
- Script tags stripped from rendered HTML
- Event handlers stripped (onclick, onerror, etc.)
- JavaScript: protocol in URLs blocked
- Data: URLs for images only
- Iframe sandboxed with restricted permissions
- External resource loading configurable

### Task 13: Wire Credential Security
**Files:** `src/core/CloudSyncService.cpp`, `src/core/AIService.cpp`
**Description:** API keys, passwords, and tokens stored in OS keychain (not config files). Credential access logged.
**Acceptance Criteria:**
- macOS: Keychain Services for credential storage
- Windows: Credential Manager
- Linux: Secret Service API
- Credentials never written to config files
- Credential access logged (without values)
- Credential rotation support

### Task 14: Wire Audit Logging
**Files:** `src/core/StructuredLogger.h`, `src/core/OutputChannelService.cpp`
**Description:** Security-relevant events logged to audit channel: file operations, extension activities, sync operations, credential access.
**Acceptance Criteria:**
- Audit log in "Security" output channel
- Events: file create/delete, extension load/unload, sync start/complete
- Credential access events (without values)
- Sandbox violation events
- Validation failure events
- Audit log exportable

### Task 15: Wire Safe Mode Recovery
**Files:** `src/app/MarkAmpApp.cpp`
**Description:** Safe mode: launch with extensions disabled, default theme, minimal config. Triggered by crash on previous launch or manual flag.
**Acceptance Criteria:**
- Safe mode detects crash on previous launch
- Safe mode disables: extensions, sync, AI, custom theme
- Safe mode indicator in title bar
- "Exit Safe Mode" restarts normally
- Manual safe mode: `--safe-mode` flag
- Safe mode logs reason for activation

### Task 16: Wire Exception Boundaries
**Files:** `src/app/MarkAmpApp.cpp`, `src/core/EventBus.cpp`
**Description:** main() currently has zero exception handling. Wire exception boundaries: top-level catch in main(), per-service catch in EventBus handlers.
**Acceptance Criteria:**
- main() wrapped in try/catch with crash reporting
- EventBus handlers wrapped in exception boundary
- Extension callbacks wrapped in exception boundary
- Rendering pipeline wrapped in exception boundary
- Exception: log, notify user, attempt recovery
- Never `catch(...)` - always typed handlers per CLAUDE.md

### Task 17: Wire Thread Safety Enforcement
**Files:** `src/core/ThreadBoundary.h`, `src/core/EventBus.cpp`
**Description:** ThreadBoundary exists. Wire it to enforce main-thread-only access for UI operations, detect cross-thread violations.
**Acceptance Criteria:**
- UI operations assert main thread
- EventBus publish from any thread, handlers on main thread
- Cross-thread violation: debug assert + log
- Thread boundary checked at: panel creation, Scintilla access, layout
- Background threads: only via approved patterns (AsyncFileLoader, etc.)

### Task 18: Wire Security Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register security commands: "Security: Show Audit Log", "Security: Manage Credentials", "Security: Safe Mode", "Security: Extension Permissions".
**Acceptance Criteria:**
- All commands registered in command palette
- "Show Audit Log" opens audit output channel
- "Manage Credentials" opens credential manager UI
- "Safe Mode" restarts in safe mode
- "Extension Permissions" opens permission settings

### Task 19: Wire Security Settings
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/Config.h`
**Description:** Security settings: auto-save credentials, preview security level, extension sandbox strictness, audit log level.
**Acceptance Criteria:**
- Credential storage: keychain / memory-only options
- Preview security: strict (no external), moderate, permissive
- Extension sandbox: strict, moderate, disabled
- Audit log level: off, basic, verbose
- Security warnings for unsafe settings

### Task 20: Add Security Tests
**Files:** `tests/unit/test_security.cpp`, `tests/unit/test_safe_path.cpp`, `tests/unit/test_safe_regex.cpp`, `tests/unit/test_string_guards.cpp`, `tests/unit/test_validation_utils.cpp`, `tests/unit/test_mime_validator.cpp`, `tests/unit/test_numeric_guards.cpp`, `tests/unit/test_safe_allocation.cpp`
**Description:** Test all security primitives: path safety, regex safety, input validation, MIME checking, numeric guards, allocation guards.
**Acceptance Criteria:**
- SafePath: traversal attacks, null bytes, symlinks all caught
- SafeRegex: timeout and complexity limits enforced
- StringGuards: length and character set enforced
- MimeValidator: correct detection for all supported types
- NumericGuards: overflow protection verified
- BoundedContainer: overflow policy enforced

## Testing Requirements
- Path traversal attack prevention
- Regex denial-of-service prevention
- Input validation for all user-facing inputs
- MIME type verification accuracy
- Numeric overflow protection

## Phase Completion Criteria
- Result<T> used for all fallible operations
- SafePath at all file system entry points
- SafeRegex for all regex operations
- Extension sandbox enforced
- Credential storage in OS keychain
- Exception boundaries at all entry points
- All tests pass
