# Phase 16: Platform Accessibility And Native Service Completion

## Outcome

Finish the platform-specific and accessibility-adjacent seams where current behavior is intentionally no-op, stubbed, or deferred across non-primary platforms and OS integration layers.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P16-T01

- Phase ID: P16
- Task ID: P16-T01
- Task Title: Replace platform-specific no-op and stub branches with honest capability support
- Priority: P1
- Category: Platform / Accessibility Completion
- Atomic Completion Tasks Covered: 60
- Objective: Ensure platform abstractions are either truly implemented or explicitly unsupported, not silently no-op in product-critical paths.
- Why This Matters Now: Platform no-ops can hide incomplete behavior under the guise of cross-platform abstraction.
- Completion Gap Statement: Several platform classes still expose stub or no-op behavior for non-primary operating systems.
- User / Product Impact: Cross-platform quality remains uneven and can silently degrade outside macOS.
- Repository Evidence: [LinuxPlatform.h](/Users/ryanrentfro/code/markamp/src/platform/LinuxPlatform.h), [WinPlatform.h](/Users/ryanrentfro/code/markamp/src/platform/WinPlatform.h), [LinuxPlatform.cpp](/Users/ryanrentfro/code/markamp/src/platform/LinuxPlatform.cpp)
- Scope: Platform services, vibrancy/effects capability, window helpers, update hooks, and explicit capability reporting.
- Out of Scope: Pure UI polish differences.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/platform/LinuxPlatform.h`; `/Users/ryanrentfro/code/markamp/src/platform/WinPlatform.h`; `/Users/ryanrentfro/code/markamp/src/platform/LinuxPlatform.cpp`; `/Users/ryanrentfro/code/markamp/src/platform/MacPlatform.h`
- Related Features / Systems / Components: Platform abstraction, shell integration, OS services.
- Current Behavior: Some cross-platform branches intentionally do nothing or still advertise stub status.
- Intended Completed Behavior: Platform capabilities are truly implemented where supported and explicitly surfaced where unsupported.
- Missing Pieces: Capability matrix, real implementations, and unsupported-state handling.
- Technical Approach: Audit platform API surfaces and convert silent no-ops into real implementations or explicit capability responses.
- Implementation Steps: Inventory no-op branches; implement missing capabilities where in scope; surface unsupported states; update callers to respect capability results.
- Validation Steps: Exercise platform-specific features on macOS, Linux, and Windows where available.
- Acceptance Criteria: Product-critical platform features do not silently disappear behind no-op branches.
- Dependencies: Phase 01.
- Risks / Failure Modes: Platform capability drift can produce inconsistent user expectations if reporting remains unclear.
- Cleanup / Migration Notes where relevant: Remove comments calling primary implementations “stubs” once parity improves.
- Observability / Diagnostics Notes where relevant: Emit platform-capability reports at startup.
- Rollback / Safety Notes: Preserve feature gating on unsupported platforms rather than forcing weak fallbacks.
- References / Context: Completion includes honest cross-platform behavior.
- Example scenarios where useful: A platform-specific window effect either works or is reported as unsupported, not silently ignored.

### P16-T02

- Phase ID: P16
- Task ID: P16-T02
- Task Title: Finish accessibility identifier and screen-reader bridge support across platforms
- Priority: P1
- Category: Platform / Accessibility Completion
- Atomic Completion Tasks Covered: 60
- Objective: Remove platform gaps that keep accessibility support uneven across operating systems.
- Why This Matters Now: Accessibility integration is incomplete if only one platform has meaningful support.
- Completion Gap Statement: Accessibility identifiers and screen-reader bridges still contain no-op or stub branches on non-primary platforms.
- User / Product Impact: Assistive-technology users receive inconsistent support by platform.
- Repository Evidence: [AccessibilityIdentifier.h](/Users/ryanrentfro/code/markamp/src/platform/AccessibilityIdentifier.h), [StubAccessibilityIdentifier.cpp](/Users/ryanrentfro/code/markamp/src/platform/StubAccessibilityIdentifier.cpp), [LinuxScreenReaderBridge.cpp](/Users/ryanrentfro/code/markamp/src/platform/LinuxScreenReaderBridge.cpp)
- Scope: Accessibility IDs, screen-reader announcements, platform bridges, and automation hooks.
- Out of Scope: Visual focus-ring styling already handled in `v22`.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/platform/AccessibilityIdentifier.h`; `/Users/ryanrentfro/code/markamp/src/platform/StubAccessibilityIdentifier.cpp`; `/Users/ryanrentfro/code/markamp/src/platform/LinuxScreenReaderBridge.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ScreenReaderAnnouncer.cpp`
- Related Features / Systems / Components: Accessibility, automation, screen-reader support, platform bridges.
- Current Behavior: Accessibility support is stronger on macOS and thinner elsewhere.
- Intended Completed Behavior: Accessibility identifiers and announcements have real platform backends wherever the product claims support.
- Missing Pieces: Windows/Linux bridge implementations, identifier propagation, and test coverage.
- Technical Approach: Finish the cross-platform bridge layer beneath the current accessibility APIs.
- Implementation Steps: Implement identifier bridges; finish Linux/Windows announcement plumbing; align UI controls with platform hooks; add automation tests.
- Validation Steps: Verify identifiers and announcements with platform-specific automation or accessibility tooling.
- Acceptance Criteria: Accessibility APIs are not silently no-op on supported platforms.
- Dependencies: P16-T01.
- Risks / Failure Modes: Platform APIs vary significantly and may require different capability subsets.
- Cleanup / Migration Notes where relevant: Remove stub bridge files once real backends exist.
- Observability / Diagnostics Notes where relevant: Track announcement delivery and identifier registration failures.
- Rollback / Safety Notes: Keep unsupported capabilities explicit and queryable.
- References / Context: Completion requires real accessibility support, not just API placeholders.
- Example scenarios where useful: A control gets a stable accessibility identifier on Linux and Windows, not only on macOS.

### P16-T03

- Phase ID: P16
- Task ID: P16-T03
- Task Title: Finish platform-service stubs for updates file watches and shell integration helpers
- Priority: P2
- Category: Platform / Accessibility Completion
- Atomic Completion Tasks Covered: 60
- Objective: Complete the OS-facing helpers that higher-level systems already depend on.
- Why This Matters Now: Many completion gaps in other phases depend on platform-service depth.
- Completion Gap Statement: Platform services still return stub or placeholder results in some OS integration paths.
- User / Product Impact: Update, watch, and shell-helper behavior remains inconsistent.
- Repository Evidence: [PlatformServices.cpp](/Users/ryanrentfro/code/markamp/src/core/PlatformServices.cpp), [PlatformServices.h](/Users/ryanrentfro/code/markamp/src/core/PlatformServices.h), [MarkAmpApp.cpp](/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp)
- Scope: File-watch polling, update results, shell integration helpers, platform queues, and startup/shutdown interactions.
- Out of Scope: Remote network transports themselves.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/PlatformServices.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PlatformServices.h`; `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`
- Related Features / Systems / Components: Startup, updates, watching, platform events, restore.
- Current Behavior: Some platform helpers still behave as testing stubs or thin queues.
- Intended Completed Behavior: Higher-level systems can rely on platform services for real OS-backed behavior.
- Missing Pieces: Real watchers, update backends, lifecycle integration, and error handling.
- Technical Approach: Finish platform services as the actual OS abstraction layer rather than a placeholder utility layer.
- Implementation Steps: Replace stub results; implement watchers and update info where intended; align startup and restore callers; add per-platform capability tests.
- Validation Steps: Exercise file-watch events, update checks, and startup hooks on supported platforms.
- Acceptance Criteria: Platform services no longer return stub results in production-supported paths.
- Dependencies: P16-T01.
- Risks / Failure Modes: Startup timing and event-order issues can appear once real platform services are enabled.
- Cleanup / Migration Notes where relevant: Remove temporary polling/fallback assumptions after real watchers are stable.
- Observability / Diagnostics Notes where relevant: Publish platform-service health and fallback usage at startup.
- Rollback / Safety Notes: Use platform capability flags if some helpers must remain unavailable on certain OSes.
- References / Context: Several earlier phases depend on platform services becoming real.
- Example scenarios where useful: File-watch events reach the shell through a real OS path instead of a stub result.

### P16-T04

- Phase ID: P16
- Task ID: P16-T04
- Task Title: Finish prompt and input host behavior for non-mock runtime operation
- Priority: P2
- Category: UI Workflow Completion
- Atomic Completion Tasks Covered: 60
- Objective: Ensure prompt surfaces are production-backed and not overly dependent on mock-mode scaffolding.
- Why This Matters Now: Prompts and quick-input surfaces are central to many workflows.
- Completion Gap Statement: Prompt hosts still contain explicit mock-mode focus and thin runtime defaults.
- User / Product Impact: Prompt-driven workflows can behave inconsistently across test and real runtime modes.
- Repository Evidence: [PromptHost.cpp](/Users/ryanrentfro/code/markamp/src/ui/PromptHost.cpp), [InputBoxService.cpp](/Users/ryanrentfro/code/markamp/src/core/InputBoxService.cpp), [QuickPickService.cpp](/Users/ryanrentfro/code/markamp/src/core/QuickPickService.cpp)
- Scope: Prompt host runtime mode, input box, quick pick, multi-step prompts, and cancellation behavior.
- Out of Scope: Visual refinement of prompt surfaces.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/PromptHost.cpp`; `/Users/ryanrentfro/code/markamp/src/core/InputBoxService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/QuickPickService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Features / Systems / Components: Prompts, quick pick, input box, shell flows.
- Current Behavior: Prompt infrastructure is functional but still heavily shaped by mock-mode concerns and thin defaults.
- Intended Completed Behavior: Prompt flows behave consistently in production with explicit runtime guarantees and cancellation semantics.
- Missing Pieces: Full runtime ownership, multi-step behavior, and integration with settings/commands/workspace actions.
- Technical Approach: Keep test doubles below the prompt contract while strengthening runtime prompt orchestration.
- Implementation Steps: Audit prompt callers; harden runtime prompt orchestration; clarify cancellation and validation; reduce production dependence on mock-only assumptions.
- Validation Steps: Trigger prompts from file, settings, panel, and extension flows in runtime and headless-test modes.
- Acceptance Criteria: Prompt-driven features no longer depend on mock-path semantics in production.
- Dependencies: Phase 02, Phase 10.
- Risks / Failure Modes: Prompt orchestration changes can affect many workflows simultaneously.
- Cleanup / Migration Notes where relevant: Keep mock responders test-only and clearly separated from runtime flows.
- Observability / Diagnostics Notes where relevant: Track prompt open/close/submit/cancel outcomes.
- Rollback / Safety Notes: Preserve deterministic mock hooks only for tests.
- References / Context: Prompt infrastructure is small but highly cross-cutting.
- Example scenarios where useful: A quick-pick launched from a command behaves the same in runtime every time, not only in mock-driven tests.

### P16-T05

- Phase ID: P16
- Task ID: P16-T05
- Task Title: Add platform and accessibility completion gates by OS capability family
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Prevent cross-platform and accessibility completion work from regressing back into silent no-ops.
- Why This Matters Now: These gaps are easy to miss when development is macOS-first.
- Completion Gap Statement: Current tests do not yet prove enough about real platform and accessibility capability support beyond compilation.
- User / Product Impact: Platform-specific users can continue to receive degraded behavior unnoticed.
- Repository Evidence: [test_control_accessibility_semantics.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_control_accessibility_semantics.cpp), [test_app_init.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_app_init.cpp), [test_phase40_automation_hooks.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_phase40_automation_hooks.cpp)
- Scope: Platform capability reports, accessibility IDs, screen-reader announcements, prompt/runtime integration, file-watch/update hooks.
- Out of Scope: Visual accessibility styling.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_control_accessibility_semantics.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_app_init.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_phase40_automation_hooks.cpp`; `/Users/ryanrentfro/code/markamp/tests/integration`
- Related Features / Systems / Components: Platform abstraction, accessibility, startup, automation.
- Current Behavior: Some coverage checks compilation or semantics without proving complete runtime behavior.
- Intended Completed Behavior: Platform and accessibility completion is backed by capability-aware regression gates.
- Missing Pieces: Per-OS coverage strategy, capability fixtures, and runtime assertions.
- Technical Approach: Add capability-family tests that assert either real support or explicit unsupported status for each OS.
- Implementation Steps: Build OS capability matrix fixtures; add screen-reader and identifier tests; verify platform-service behavior; publish per-OS gate outputs.
- Validation Steps: Reintroduce a no-op accessibility path and confirm the relevant gate fails.
- Acceptance Criteria: Cross-platform and accessibility regressions cannot hide behind silent no-ops.
- Dependencies: P16-T01 through P16-T04.
- Risks / Failure Modes: Some platform tests may require different infrastructure or emulation per CI environment.
- Cleanup / Migration Notes where relevant: Remove placeholder app-init tests once real platform startup coverage exists.
- Observability / Diagnostics Notes where relevant: Publish platform-capability and accessibility test matrices.
- Rollback / Safety Notes: Mark unsupported capabilities explicitly rather than allowing ambiguous passing behavior.
- References / Context: This phase keeps completion honest beyond the primary development platform.
- Example scenarios where useful: A Linux accessibility bridge regression fails the Linux capability gate instead of being invisible from macOS.
