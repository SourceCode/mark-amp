# Phase 02: Main Menu Command Palette And Shortcut Parity

## Outcome

Make every menu item, palette entry, and accelerator trustworthy by binding all visible actions to canonical commands and eliminating unbound or duplicate paths.

## Improvement Count

126 atomic improvements across 6 execution tasks.

### P02-T01

- Phase ID: P02
- Task ID: P02-T01
- Task Title: Audit and bind every `MainFrame` menu item
- Priority: P0
- Category: Main Menu
- Atomic Improvements Covered: 21
- Objective: Ensure every appended menu item in `MainFrame` has a real execution path or is removed.
- Why This Matters Now: The main menu is one of the largest visible control surfaces and currently the most likely to contain dead items.
- Problem Statement: Search shows many appended IDs in `MainFrame.cpp` that do not appear again as handlers.
- User Impact: Users can click menu items that do nothing or diverge from other surfaces.
- Scope: File, Edit, View, Tools, Notebooks, Canvas, Git, Data, Sync, Window, Help menus.
- Out of Scope: Deep implementation of all downstream features.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Systems / Components: Menus, commands, event bus, shell.
- Current Behavior: `MainFrame.cpp` mixes direct lambdas, direct events, and likely unbound appended items.
- Intended Behavior: Every menu item is bound, validated, and mapped to one canonical command.
- Technical Approach: Build a menu-to-command binding table and remove any orphan menu IDs.
- Implementation Steps: Inventory menu IDs; compare against binds; add missing command routes; remove or hide unsupported menu items.
- Validation Steps: Trigger every menu item in automated smoke runs.
- Acceptance Criteria: No appended menu item remains unbound.
- Dependencies: Phase 01.
- Risks / Failure Modes: Hidden platform-specific menu behavior may obscure missing bindings.
- UX Notes: Unsupported features should disappear instead of looking implemented.
- Settings / Panel / Control Notes where relevant: Preferences menu and settings submenu behavior need deep-link correctness.
- Observability / Diagnostics Notes: Log menu ID to command ID resolution.
- Rollback / Safety Notes: Defer removal only if feature flagging is explicit and visible.
- References / Context: `kMenuTools*`, `kMenuNotebook*`, `kMenuCanvas*`, `kMenuGit*`, `kMenuData*`, and `kMenuSync*` are obvious high-risk sets in `MainFrame.cpp`.
- Example scenarios where useful: `Git > Status` either opens a real panel/workflow or is removed until implemented.

### P02-T02

- Phase ID: P02
- Task ID: P02-T02
- Task Title: Unify command palette registration with the central command registry
- Priority: P0
- Category: Command Routing
- Atomic Improvements Covered: 21
- Objective: Stop `MainFrame` from maintaining a second action universe inside the command palette.
- Why This Matters Now: Palette drift is already visible in notebook, canvas, and preferences actions.
- Problem Statement: `MainFrame.cpp` registers palette commands manually while `WorkbenchCommands.cpp` also defines commands.
- User Impact: Palette actions can behave differently from menu and shortcut actions.
- Scope: Palette population, command metadata sourcing, search categories, execution callbacks.
- Out of Scope: Palette UI redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp`
- Related Systems / Components: Command palette, command registry, event bus.
- Current Behavior: Command palette content is hand-assembled.
- Intended Behavior: Palette displays canonical commands and executes canonical command IDs.
- Technical Approach: Feed palette entries from the command registry plus context-aware filters.
- Implementation Steps: Remove manual duplicates; add registry adapter; preserve palette-specific ranking only.
- Validation Steps: Compare palette command set against registry export.
- Acceptance Criteria: Palette command definitions are no longer hand-maintained in `MainFrame`.
- Dependencies: P01-T02.
- Risks / Failure Modes: Registry metadata may be incomplete for palette presentation initially.
- UX Notes: Search relevance can remain palette-specific even if commands are shared.
- Settings / Panel / Control Notes where relevant: Settings deep-link entries need structured command arguments.
- Observability / Diagnostics Notes: Trace palette selection to command ID.
- Rollback / Safety Notes: Keep a temporary compatibility layer for legacy palette-only items if strictly needed.
- References / Context: Current palette notebook and canvas commands still use old event paths.
- Example scenarios where useful: “New Notebook” in the palette resolves through the same command used by the menu.

### P02-T03

- Phase ID: P02
- Task ID: P02-T03
- Task Title: Repair shortcut and accelerator parity with menus and commands
- Priority: P0
- Category: Enablement / Visibility Logic
- Atomic Improvements Covered: 21
- Objective: Ensure every advertised shortcut triggers the same action as its corresponding menu entry.
- Why This Matters Now: Shortcut drift is a common trust failure in large IDEs.
- Problem Statement: Shortcut hints are declared in menus, palette metadata, and platform keyboard maps separately.
- User Impact: Keyboard-driven users get inconsistent behavior.
- Scope: Menu accelerators, shortcut manager, platform keyboard mapping, command metadata parity.
- Out of Scope: Full keybinding editor redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PlatformKeyboard.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp`
- Related Systems / Components: Menus, shortcut manager, command registry, keybindings.
- Current Behavior: Shortcut metadata is duplicated and can conflict.
- Intended Behavior: One shortcut definition per command, with menu and palette reading from it.
- Technical Approach: Generate accelerators from canonical command metadata where possible.
- Implementation Steps: Audit advertised shortcuts; resolve conflicts; bind through shared command dispatch; update menu labels automatically.
- Validation Steps: Automated keypress tests and accelerator-to-command mapping checks.
- Acceptance Criteria: Menu labels, shortcut maps, and actual behavior stay in sync.
- Dependencies: P02-T01, P02-T02.
- Risks / Failure Modes: Platform-specific reserved shortcuts may need overrides.
- UX Notes: Preserve platform norms while keeping parity.
- Settings / Panel / Control Notes where relevant: Preferences and panel toggles should honor platform shortcut conventions.
- Observability / Diagnostics Notes: Log unmapped or conflicting shortcuts.
- Rollback / Safety Notes: Keep explicit override support per platform.
- References / Context: `PlatformKeyboard.cpp` and `MainFrame.cpp` both describe shortcuts today.
- Example scenarios where useful: `Ctrl+,` opens the same settings flow as the Window > Preferences menu item.

### P02-T04

- Phase ID: P02
- Task ID: P02-T04
- Task Title: Complete settings-related menu and deep-link flows
- Priority: P0
- Category: Main Menu
- Atomic Improvements Covered: 21
- Objective: Make Preferences, settings search, and deep-link actions actually open the correct settings surface.
- Why This Matters Now: Settings are a core control surface and already show obvious lifecycle gaps.
- Problem Statement: `SettingsOpenRequestEvent` is logged by `SettingsDeepLinkRouter.cpp` and only partially handled in `MainFrame.cpp`.
- User Impact: Preferences controls can look complete while failing to navigate or prefill correctly.
- Scope: Preferences menu, toolbar settings button, palette settings entries, settings search, open-to-setting, scope selection.
- Out of Scope: Full settings content completion.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
- Related Systems / Components: Settings router, settings dialog, activity bar, command palette.
- Current Behavior: Deep links mostly log or just switch activity-bar state.
- Intended Behavior: Any settings entry point opens the real settings host with query/setting/scope applied.
- Technical Approach: Wire deep-link requests into a concrete settings host interface instead of logging.
- Implementation Steps: Add host contract; route requests; connect dialog/panel open APIs; honor query and scope payloads.
- Validation Steps: Trigger settings open from menu, toolbar, and palette with query and setting targets.
- Acceptance Criteria: Settings deep links always land in the intended settings surface and target.
- Dependencies: Phase 08.
- Risks / Failure Modes: Multiple settings hosts may race or steal focus.
- UX Notes: Opening settings should be immediate and context-preserving.
- Settings / Panel / Control Notes where relevant: This is the bridge between control integrity and settings completeness.
- Observability / Diagnostics Notes: Trace requested setting ID/query and resolved host target.
- Rollback / Safety Notes: Keep one host authoritative; avoid simultaneous sidebar/dialog settings UIs in production.
- References / Context: `SettingsDialog` has `OpenToSetting` and `OpenWithQuery`, but the current router does not drive them.
- Example scenarios where useful: “Preferences: Open Font Settings” from the palette opens directly to `editor.fontSize`.

### P02-T05

- Phase ID: P02
- Task ID: P02-T05
- Task Title: Remove or hide production-visible menu items that still front stub subsystems
- Priority: P1
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 21
- Objective: Stop shipping menu affordances for subsystems that still only exist as placeholders.
- Why This Matters Now: Dead menu UI is one of the most misleading forms of incompleteness.
- Problem Statement: Some menu items target panel placeholders, unimplemented workflows, or partial event chains.
- User Impact: Users infer support that the product does not actually provide.
- Scope: Tools, Notebook, Canvas, Git, Data, Sync, History/PDF-related actions where implementation is still stubbed.
- Out of Scope: Permanent feature removal decisions.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`
- Related Systems / Components: Menus, panel registry, stub panels, partial workflows.
- Current Behavior: Visible actions may target incomplete surfaces.
- Intended Behavior: Production-visible menus only advertise working or intentionally gated functionality.
- Technical Approach: Gate incomplete actions behind feature flags or remove from production menus until ready.
- Implementation Steps: Classify actions; define supported status; remove or hide unsupported items; keep diagnostics for gated items.
- Validation Steps: Review production menu model after gating.
- Acceptance Criteria: No obviously stub-backed menu item remains exposed as if complete.
- Dependencies: P02-T01 and panel-assessment phases.
- Risks / Failure Modes: Over-hiding can hurt discoverability for near-ready features.
- UX Notes: Prefer absence over misleading affordance.
- Settings / Panel / Control Notes where relevant: Settings-related actions should stay visible only if the settings host is real.
- Observability / Diagnostics Notes: Track gated action attempts in debug builds only.
- Rollback / Safety Notes: Use feature flags for reversible gating.
- References / Context: `HistoryPanel.cpp` and `PDFViewerPanel.cpp` are explicit stubs today.
- Example scenarios where useful: A PDF annotation menu item is hidden until the PDF panel is no longer a stub.

### P02-T06

- Phase ID: P02
- Task ID: P02-T06
- Task Title: Add menu and accelerator smoke coverage
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 21
- Objective: Make menu and shortcut trustworthiness measurable.
- Why This Matters Now: The menu surface is too large for manual confidence.
- Problem Statement: Broken or unbound menu items can survive easily without automated coverage.
- User Impact: Primary navigation remains unreliable.
- Scope: Main menu traversal, submenu selection, accelerator invocation, palette parity checks.
- Out of Scope: Exhaustive deep feature validation for each downstream subsystem.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Systems / Components: Menus, shortcuts, command routing, diagnostics.
- Current Behavior: Menu trust depends too much on manual testing.
- Intended Behavior: CI can detect dead menu items and broken shortcut parity.
- Technical Approach: Build a control-smoke matrix around canonical action IDs.
- Implementation Steps: Enumerate menu items; add invoke-and-trace tests; verify state changes or expected no-op gates.
- Validation Steps: Run menu/accelerator smoke suite in CI.
- Acceptance Criteria: Unbound or wrong-target menu actions become release-blocking.
- Dependencies: P02-T01 through P02-T05.
- Risks / Failure Modes: Menu tests may be brittle if action IDs are unstable.
- UX Notes: Include keyboard-first workflows, not just pointer selection.
- Settings / Panel / Control Notes where relevant: Include Preferences and settings search menu flows.
- Observability / Diagnostics Notes: Attach control trace output to failures.
- Rollback / Safety Notes: Use deterministic test hooks rather than screen scraping when possible.
- References / Context: `MainFrame.cpp` is a major surface area for hidden no-op regressions.
- Example scenarios where useful: CI selects every top-level menu and verifies each enabled item resolves to a command.

