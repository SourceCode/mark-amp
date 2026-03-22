# Phase 14: Plugin, Extension, Contribution, And Gating Completion

## Phase Intent

Finish only the extension and contribution work required for release-path trust.

## Release-Ready Exit Criteria

- release-path extension contribution points behave honestly and safely,
- plugin and extension surfaces do not expose unfinished or misleading capabilities,
- extension-related failures are gated, observable, and recoverable.

## Task Count

3

## Task P14-T01

- Phase ID: P14
- Task ID: P14-T01
- Task Title: Audit release-path extension contribution points against real runtime support
- Priority: P1
- Category: Release Readiness
- Objective: ensure extension contribution points visible on the release path are backed by real runtime behavior.
- Why This Matters Now: extension ambition is broad, but the release candidate only needs truthful and dependable supported scope.
- Release Gap Statement: contribution breadth can exceed real runtime support and confuse release readiness.
- User / Product Impact: misleading extension capability harms trust and creates support burden.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ExtensionManagement.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp`
- Prior Plan References: `v23 Phase 11`; `v24 Phase 14`
- Scope: commands, themes, snippets, menus, settings, visible extension surfaces.
- Out of Scope: future contribution types not required on the release path.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ExtensionManifest.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp`
- Related Features / Systems / Components: extensions; plugins; contribution validator; marketplace
- Current Behavior: infrastructure is broad, but supported release-path scope still needs tighter truth-in-advertising.
- Intended Release-Ready Behavior: only actually supported contribution points remain enabled on the release path.
- Missing Pieces: supported-scope matrix; gate rules; validation tests.
- Technical Approach: classify contribution points as release-supported, gated, or future, then enforce that classification in UI and validation.
- Implementation Steps:
1. Define the release-supported contribution set.
2. Gate unsupported contribution points in validation and surface registration.
3. Add tests for unsupported contribution rejection or gating.
- Validation Steps:
1. Install or simulate contributions in each release-supported category.
2. Verify unsupported ones are clearly rejected or hidden.
- Acceptance Criteria: release-path extension behavior matches advertised support exactly.
- Dependencies: P01-T02
- Parallelization Notes: extension runtime hardening can proceed in parallel.
- Risks / Failure Modes: silently accepted but ineffective contributions.
- Observability / Diagnostics Notes: log contribution validation failures with extension ID and category.
- Rollback / Safety Notes: prefer explicit unsupported diagnostics over partial acceptance.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp`

## Task P14-T02

- Phase ID: P14
- Task ID: P14-T02
- Task Title: Harden extension lifecycle failure handling and recovery on the release path
- Priority: P1
- Category: Diagnostics / Recovery
- Objective: ensure misbehaving plugins or extensions do not silently destabilize the shell.
- Why This Matters Now: release confidence requires visible and recoverable failure handling at extension boundaries.
- Release Gap Statement: extension breadth without clear recovery semantics creates systemic instability risk.
- User / Product Impact: extension crashes or bad contributions should not corrupt the workbench.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ExtensionHostRecovery.h`; `/Users/ryanrentfro/code/markamp/src/core/PluginQuarantine.h`; `/Users/ryanrentfro/code/markamp/src/core/PluginSafeCall.h`
- Prior Plan References: `v23 Phase 11`; `v24 Phase 14`; `v7` resilience planning
- Scope: activation failures, safe-call wrapping, quarantine, user-visible recovery messaging.
- Out of Scope: marketplace policy expansion.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ExtensionTelemetry.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ExtensionStorage.cpp`
- Related Features / Systems / Components: recovery; quarantine; safe calls; telemetry
- Current Behavior: protective infrastructure exists, but release-path signoff still needs explicit adoption and validation.
- Intended Release-Ready Behavior: extension failures are isolated, observable, and recoverable without destabilizing the shell.
- Missing Pieces: release-path smoke coverage; user-facing error contracts; recovery adoption proof.
- Technical Approach: add release-path activation/recovery scenarios and wire failure reporting into the signoff process.
- Implementation Steps:
1. Define extension failure scenarios that matter on the release path.
2. Verify safe-call, quarantine, and recovery paths handle them.
3. Add smoke or integration tests for those scenarios.
- Validation Steps:
1. Simulate activation and runtime failures.
2. Confirm shell stability and clear user-visible diagnostics.
- Acceptance Criteria: extension failures are contained and do not silently break release-path shell behavior.
- Dependencies: P14-T01; P19-T01
- Parallelization Notes: largely isolated from core workflow work.
- Risks / Failure Modes: swallowed failures; incomplete quarantine coverage.
- Observability / Diagnostics Notes: capture extension failure reports with extension identity and recovery action.
- Rollback / Safety Notes: fail closed by disabling misbehaving extensions.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/ExtensionHostRecovery.h`

## Task P14-T03

- Phase ID: P14
- Task ID: P14-T03
- Task Title: Gate marketplace and theme-extension UI to truthful release scope
- Priority: P2
- Category: Release Readiness
- Objective: ensure marketplace-facing surfaces only expose supported and dependable release behavior.
- Why This Matters Now: marketplace UX can otherwise imply a depth of support not yet reached.
- Release Gap Statement: theme marketplace and extension browsing risk over-promising during release hardening.
- User / Product Impact: misleading browse/install experiences can undermine trust even if core IDE workflows are solid.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ExtensionsBrowserPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeMarketplace.h`; `/Users/ryanrentfro/code/markamp/tests/unit/test_theme_marketplace.cpp`
- Prior Plan References: `v23 Phase 11`; `v24 Phase 14`
- Scope: browse/install UI, theme marketplace promises, supported messaging.
- Out of Scope: new marketplace features.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/GalleryService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/VsixService.cpp`
- Related Features / Systems / Components: marketplace; themes; extension install UI
- Current Behavior: marketplace-related surfaces may still exceed release-supported confidence.
- Intended Release-Ready Behavior: marketplace UI is either truthful and stable or clearly gated down.
- Missing Pieces: scope declaration; UI messaging; validation of install/error paths.
- Technical Approach: tie marketplace visibility and messaging to the supported-scope matrix from P14-T01.
- Implementation Steps:
1. Audit marketplace-facing UI for unsupported claims.
2. Gate or relabel unsupported paths.
3. Add install/error smoke coverage for supported flows.
- Validation Steps:
1. Exercise supported browse/install paths.
2. Confirm unsupported paths are clearly hidden or labeled.
- Acceptance Criteria: marketplace UI does not imply unsupported release behavior.
- Dependencies: P14-T01
- Parallelization Notes: can be deferred if marketplace is not core release-critical, but scope must still be explicit.
- Risks / Failure Modes: user confusion due to partially visible unsupported flows.
- Release Notes / Cleanup Notes: note any intentionally gated marketplace scope.
- Observability / Diagnostics Notes: log install failures and unsupported-path access attempts.
- Rollback / Safety Notes: hide unsupported marketplace actions rather than soft-failing them.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/ExtensionsBrowserPanel.cpp`
