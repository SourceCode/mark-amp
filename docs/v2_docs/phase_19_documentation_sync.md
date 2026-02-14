# Phase 19: Documentation & AGENTS.md Sync

**Priority:** Low
**Estimated Scope:** ~8 files affected
**Dependencies:** All prior phases (should be done last or near-last)

## Objective

Synchronize all documentation files (AGENTS.md, README.md, HISTORY.md, docs/) with the actual state of the codebase after all prior phases are implemented, fixing documented-but-nonexistent features, undocumented-but-existing features, and stale references.

## Background/Context

Several documentation discrepancies exist between what is documented and what actually works:

### Discrepancy 1: SettingsBatchChangedEvent

**File:** `/Users/ryanrentfro/code/markamp/HISTORY.md`

The v1.8.11 changelog entry mentions `SettingsBatchChangedEvent` as a feature. However, this event type does NOT exist in `Events.h` or anywhere in the source code. It was either:
- Planned but never implemented
- Implemented and then removed
- A documentation error

**Action:** If Phase 01 adds this event, document it correctly. If it remains unimplemented, remove the reference from HISTORY.md or add a note.

### Discrepancy 2: AGENTS.md describes PluginContext inaccurately

**File:** `/Users/ryanrentfro/code/markamp/AGENTS.md`

Line 35 states:
```
PluginContext -- Runtime context providing EventBus*, Config*, register_command_handler, extension_path, workspace_state, global_state
```

This lists only 6 fields. After Phase 02 and 03, PluginContext will have 25+ populated fields including all P1-P4 service pointers. The documentation should be updated to reflect the full context.

### Discrepancy 3: AGENTS.md lists features that are data-only

The AGENTS.md "UI components" section (line 20) lists OutputPanel, ProblemsPanel, TreeViewHost, WalkthroughPanel, WebviewHostPanel alongside real UI panels like ExtensionsBrowserPanel. After Phase 06 converts them to real wxPanels, no update is needed. But if Phase 06 is not yet complete when docs are synced, these should be annotated as "data-only, not yet rendered."

### Discrepancy 4: Test count

**File:** `/Users/ryanrentfro/code/markamp/README.md` and `/Users/ryanrentfro/code/markamp/AGENTS.md`

Both claim "21 test targets, 100% pass rate." After Phase 13 adds new test targets, this count needs updating.

### Discrepancy 5: Extension services not documented in AGENTS.md

The P1-P4 extension services (SnippetEngine, WorkspaceService, TextEditorService, ProgressService, ExtensionEvents, EnvironmentService, GrammarEngine, TerminalService, TaskRunnerService, NotificationServiceAPI, StatusBarItemService, InputBoxService, QuickPickService) are not mentioned in AGENTS.md's "Key Systems" section, despite being significant new additions in v1.9.12.

### Discrepancy 6: Platform stubs documented as implementations

AGENTS.md line 22 says:
```
src/platform/ -- Platform abstractions: MacPlatform (Objective-C++ bridge)
```

This only mentions MacPlatform. WinPlatform and LinuxPlatform exist but are omitted, giving the false impression that only macOS is supported.

### Discrepancy 7: docs/ directory may contain stale architecture docs

The `docs/` directory likely contains architecture documents, API references, and guides that reference the pre-v1.9.12 architecture without the extension infrastructure.

## Detailed Tasks

### Task 1: Fix HISTORY.md SettingsBatchChangedEvent reference

**File:** `/Users/ryanrentfro/code/markamp/HISTORY.md`

Find the v1.8.11 entry that references `SettingsBatchChangedEvent`. Either:
- If the event was added in Phase 01: verify the name matches and leave as-is
- If the event was NOT added: change to a correct description or remove the reference

### Task 2: Update AGENTS.md PluginContext section

**File:** `/Users/ryanrentfro/code/markamp/AGENTS.md`

Update the PluginContext bullet (line 35) to reflect all populated fields:

```markdown
- `PluginContext` -- Runtime context providing `EventBus*`, `Config*`, `register_command_handler`, `extension_path`, `workspace_state`, `global_state`, `FeatureRegistry*`, and all extension service pointers (SnippetEngine, WorkspaceService, TextEditorService, ProgressService, EnvironmentService, NotificationService, StatusBarItemService, InputBoxService, QuickPickService, OutputChannelService, DiagnosticsService, TreeDataProviderRegistry, WebviewService, DecorationService, FileSystemProviderRegistry, LanguageProviderRegistry)
```

### Task 3: Add Extension Services to AGENTS.md Key Systems

**File:** `/Users/ryanrentfro/code/markamp/AGENTS.md`

Add a new "Extension Services" section under "Key Systems":

```markdown
### Extension Services (P1-P4)

- `SnippetEngine` -- Snippet expansion with tab stops, placeholders, and choice lists
- `WorkspaceService` -- Workspace file access and configuration
- `TextEditorService` -- Editor decoration and text manipulation API
- `ProgressService` -- Progress bar/notification lifecycle management
- `EnvironmentService` -- Clipboard, URI opening, machine ID, app metadata
- `NotificationService` -- Extension-facing notification API (Info/Warning/Error with actions)
- `StatusBarItemService` -- Status bar item creation and management
- `InputBoxService` -- Modal text input prompts
- `QuickPickService` -- Filterable selection lists (single and multi-select)
- `ExtensionEventBus` -- Extension-scoped event pub/sub
- `GrammarEngine` -- P3 stub for TextMate grammar support
- `TerminalService` -- P4 stub (N/A for Markdown editor)
- `TaskRunnerService` -- P4 stub (N/A for Markdown editor)
```

### Task 4: Update platform documentation

**File:** `/Users/ryanrentfro/code/markamp/AGENTS.md`

Update line 22:

```markdown
- `src/platform/` -- Platform abstractions: MacPlatform (Objective-C++), WinPlatform (Win32 API), LinuxPlatform (GTK/GDK)
```

### Task 5: Update test count in README.md and AGENTS.md

**Files:**
- `/Users/ryanrentfro/code/markamp/README.md`
- `/Users/ryanrentfro/code/markamp/AGENTS.md`

Update "21 test targets" to the actual count after Phase 13.

### Task 6: Add version history entry for v2 changes

**File:** `/Users/ryanrentfro/code/markamp/HISTORY.md`

After all phases are complete, add a v2.0.0 (or appropriate version) entry summarizing:
- Extension service wiring
- Panel UI rendering
- BuiltIn plugin behavioral activation
- Event system refactor
- Platform completion
- Math rendering
- Test coverage expansion
- etc.

### Task 7: Review and update docs/ directory

**File:** All files in `/Users/ryanrentfro/code/markamp/docs/`

Review each document for accuracy. Common issues to fix:
- Architecture diagrams that show only the original core (pre-extension)
- API references that omit PluginContext fields
- User guides that describe features as working when they were stubs
- Security audit that does not cover extension sandbox

### Task 8: Add CLAUDE.md for Claude Code compatibility

**File:** New file `/Users/ryanrentfro/code/markamp/CLAUDE.md`

The initial exploration found that `CLAUDE.md` does not exist (the read failed). If the project uses Claude Code, create a `CLAUDE.md` that provides project-specific instructions:

```markdown
# Claude Code Instructions for MarkAmp

## Build & Test
- Configure: `cmake --preset debug`
- Build: `cmake --build build/debug -j$(sysctl -n hw.ncpu)`
- Test: `cd build/debug && ctest --output-on-failure`

## Code Style
- C++23 with trailing return types
- 4-space indentation, Allman brace style
- Use `auto` with trailing return type for function declarations
- Namespace: `markamp::core`, `markamp::ui`, `markamp::rendering`, `markamp::platform`, `markamp::app`
- Use `MARKAMP_LOG_*` macros for logging (DEBUG, INFO, WARN, ERROR)
- Event types declared in Events.h using MARKAMP_DECLARE_EVENT macros
- Tests use Catch2 with descriptive section names

## Key Architecture
- EventBus is the backbone -- all inter-component communication goes through it
- Services are passed via constructor injection (NOT ServiceRegistry singleton)
- PluginContext aggregates all service pointers for extension access
- Config uses YAML frontmatter in Markdown files
- ThemeEngine provides cached wxBrush/wxPen/wxFont objects

## Important Conventions
- Never use `catch(...)` -- always use typed exception handlers
- Guard filesystem operations with std::error_code overloads
- Use `[[nodiscard]]` on all query methods
- Check pointer validity before dereferencing service pointers from PluginContext
```

## Acceptance Criteria

1. HISTORY.md does not reference non-existent event types
2. AGENTS.md accurately describes all PluginContext fields
3. AGENTS.md documents P1-P4 extension services
4. AGENTS.md lists all three platform implementations
5. Test counts are accurate
6. docs/ directory contents match the current architecture
7. CLAUDE.md exists with accurate build/test/style instructions

## Testing Requirements

- No code changes, so no build/test regression
- Review all documentation files for accuracy
- Verify AGENTS.md against actual source code
- Verify HISTORY.md changelog entries against actual commits
