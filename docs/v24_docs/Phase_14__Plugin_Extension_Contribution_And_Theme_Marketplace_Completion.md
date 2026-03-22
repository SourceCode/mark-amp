# Phase 14 - Plugin, Extension, Contribution, And Theme Marketplace Completion

## Phase Goal

Finish the contribution infrastructure that currently looks broad but still contains stub or partial production seams, especially around themes, extensions, and contributed actions/settings.

## Measurable Outcome

- Contributed commands, menus, settings, and themes integrate through authoritative shell systems.
- Theme marketplace and extension readiness no longer rely on placeholder local behavior.
- Plugin/extension pathways are aligned with release gating.

### Task P14-T01

- Phase ID: `P14`
- Task ID: `P14-T01`
- Task Title: Align extension contribution points with canonical commands, menus, and settings
- Priority: `P1`
- Category: `Migration Completion`
- Objective: Ensure contributed features cannot bypass the shell systems that core features are being migrated onto.
- Why This Matters Now: Finishing the core shell while leaving contribution pathways weaker would recreate the same drift.
- Execution Gap Statement: Extension contribution infrastructure is broad, but command/menu/settings completion work still needs to absorb it.
- User / Product Impact: Extensions could expose dead or inconsistent UI even after core shell cleanup.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ExtensionManifest.h`, `/Users/ryanrentfro/code/markamp/src/core/ContributionValidator.h`
- Prior Plan References: `v21` control/settings completion, `v23` Phase 11 and 18
- Scope: Contributed commands, menus, keybindings, settings, themes
- Out of Scope: Marketplace business features
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/PluginManager.h`, `/Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h`
- Related Features / Systems / Components: Extensions, contributions, shell action model
- Current Behavior: Contribution infrastructure exists but does not yet fully inherit the tightened shell rules.
- Intended Completed Behavior: Contributed surfaces go through the same manifest, context, and settings governance as built-in features.
- Missing Pieces: Governance integration and readiness policy for contributed UI
- Technical Approach: Require extension contributions to bind through canonical registries and readiness validators.
- Implementation Steps: Update validators; map contributions to shell action/settings registries; reject incomplete or invalid contributions.
- Validation Steps: Load representative extensions and verify contributed actions/settings/menu items behave like built-ins.
- Acceptance Criteria: No extension contribution bypasses the finalized shell governance model.
- Dependencies: `P04-T01`, `P13-T01`
- Parallelization Notes: Can progress while marketplace work is hardened.
- Risks / Failure Modes: Overly strict validation can break existing extensions without migration support.
- Cleanup / Migration Notes: Deprecate legacy contribution adapters once canonical paths are available.
- Observability / Diagnostics Notes: Emit contribution validation and registration diagnostics.
- Rollback / Safety Notes: Support compatibility shims with explicit deprecation windows.
- References / Context: Plugin and extension infrastructure
- Example scenarios where useful: A contributed command shows up in the palette and menu with the same enablement semantics as a built-in command.

### Task P14-T02

- Phase ID: `P14`
- Task ID: `P14-T02`
- Task Title: Finish theme marketplace behavior beyond local installed-ID placeholders
- Priority: `P1`
- Category: `Migration Completion`
- Objective: Replace placeholder marketplace behavior with real theme install/update flows or gate it cleanly.
- Why This Matters Now: Theme surfaces are user-visible and already imply distribution features.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/ThemeMarketplace.cpp` still contains placeholder local fallback behavior.
- User / Product Impact: Theme discovery and install surfaces can feel misleading.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ThemeMarketplace.cpp`
- Prior Plan References: `v22` theme parity, `v23` Phase 11
- Scope: Theme listing, install/update state, local-vs-remote behavior, error handling
- Out of Scope: Full commercial marketplace scope
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`
- Related Features / Systems / Components: Theme marketplace, theme gallery, theme registry
- Current Behavior: Theme marketplace behavior is incomplete and partially local.
- Intended Completed Behavior: Theme marketplace surfaces either perform real install/update behavior or are explicitly gated.
- Missing Pieces: Remote state, install flow, update flow, readiness policy
- Technical Approach: Separate local theme gallery behavior from remote marketplace behavior and complete or gate the latter.
- Implementation Steps: Audit remote/local seams; implement real marketplace state or hide remote affordances; sync install status with theme registry.
- Validation Steps: Browse themes, install/update/remove, restart, and verify theme availability and metadata.
- Acceptance Criteria: Theme marketplace no longer ships placeholder remote behavior.
- Dependencies: `P07-T02`, `P15-T01`
- Parallelization Notes: Can proceed with cloud/transport work if marketplace depends on network services.
- Risks / Failure Modes: Partial remote behavior can create broken install state or confusing UI.
- Cleanup / Migration Notes: Remove local placeholder marketplace shortcuts when real remote state exists.
- Observability / Diagnostics Notes: Emit marketplace fetch/install/update diagnostics.
- Rollback / Safety Notes: Keep local theme gallery usable independently if remote marketplace is unavailable.
- References / Context: Theme marketplace and gallery
- Example scenarios where useful: Installed themes remain installed and accurately marked after restart instead of only appearing in local placeholder lists.

### Task P14-T03

- Phase ID: `P14`
- Task ID: `P14-T03`
- Task Title: Complete plugin and extension runtime health, crash recovery, and quarantine integration
- Priority: `P2`
- Category: `Diagnostics / Recovery`
- Objective: Make extension runtime failures observable and survivable in the tightened completion model.
- Why This Matters Now: As more shell systems become authoritative, extension faults become more dangerous.
- Execution Gap Statement: Extension infrastructure is broad, but health and recovery paths must now be wired into release readiness.
- User / Product Impact: A broken extension can destabilize an otherwise finished shell.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ExtensionHostRecovery.h`, `/Users/ryanrentfro/code/markamp/src/core/PluginQuarantine.h`
- Prior Plan References: `v23` Phase 11 and 19
- Scope: Activation failure handling, restart, quarantine, contribution disablement
- Out of Scope: New extension-host architecture
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/PluginSafeCall.h`, `/Users/ryanrentfro/code/markamp/src/core/ExtensionTelemetry.h`
- Related Features / Systems / Components: Extension host, recovery, quarantine
- Current Behavior: Recovery infrastructure exists, but its integration with the newly stricter shell completion model needs proof.
- Intended Completed Behavior: Faulty extensions degrade safely without corrupting core shell readiness.
- Missing Pieces: Cross-linking with action/panel/settings readiness gates
- Technical Approach: Integrate extension health state with contribution registration and shell gating.
- Implementation Steps: Surface host health; disable or quarantine broken contributions; update notifications and diagnostics.
- Validation Steps: Simulate extension crashes and confirm safe degradation and recovery.
- Acceptance Criteria: Extension failures cannot leave dead contributed UI in the shell.
- Dependencies: `P14-T01`, `P19-T01`
- Parallelization Notes: Can progress with validation and release-gate work.
- Risks / Failure Modes: Over-aggressive quarantine can disable healthy extensions after transient faults.
- Cleanup / Migration Notes: Retire any silent-failure extension pathways.
- Observability / Diagnostics Notes: Emit activation/crash/quarantine and recovery events.
- Rollback / Safety Notes: Allow explicit user re-enable after quarantine with diagnostics visible.
- References / Context: Extension host recovery and quarantine services
- Example scenarios where useful: A crashing extension loses its contributed commands and panels cleanly instead of leaving dead UI behind.

### Task P14-T04

- Phase ID: `P14`
- Task ID: `P14-T04`
- Task Title: Verify contributed UI respects the final visual and action systems
- Priority: `P2`
- Category: `UI Quality / Styling`
- Objective: Prevent extensions from reintroducing shell inconsistency once core UI and action systems are tightened.
- Why This Matters Now: Contribution points can bypass the visual and command coherence the core product is trying to achieve.
- Execution Gap Statement: Contributed menus, settings, themes, and status items need explicit compatibility rules.
- User / Product Impact: Extension-heavy installs could still feel fragmented.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/StatusBarItemService.h`, `/Users/ryanrentfro/code/markamp/src/core/WebviewService.h`
- Prior Plan References: `v22` UI-system completion, `v23` Phase 11
- Scope: Contributed icon semantics, labels, status items, menus, settings row rendering
- Out of Scope: Third-party design recommendations outside contribution rules
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.cpp`
- Related Features / Systems / Components: Contributions, status items, menus, visual system
- Current Behavior: Contribution infrastructure does not yet fully enforce the final shell UI conventions.
- Intended Completed Behavior: Contributed UI fits the same action, icon, and state systems as core UI.
- Missing Pieces: Contribution-style validation and compatibility guidance
- Technical Approach: Add contribution validation rules for action metadata, iconography, and state presentation.
- Implementation Steps: Define rules; validate contributed UI metadata; surface warnings or rejections for incompatible contributions.
- Validation Steps: Load sample contributions with bad icon or label metadata and confirm rejection or warning.
- Acceptance Criteria: Contributed UI cannot silently undermine finalized shell coherence.
- Dependencies: `P07-T03`, `P14-T01`
- Parallelization Notes: Can progress after core visual-system enforcement exists.
- Risks / Failure Modes: Too little flexibility can make contributions needlessly brittle.
- Cleanup / Migration Notes: Deprecate permissive legacy contribution metadata where needed.
- Observability / Diagnostics Notes: Emit style-compatibility validation output for contributions.
- Rollback / Safety Notes: Start with warnings for non-critical violations, then tighten where needed.
- References / Context: Icon policy and contribution services
- Example scenarios where useful: A contributed status-bar item with missing label semantics is flagged before shipping.

### Task P14-T05

- Phase ID: `P14`
- Task ID: `P14-T05`
- Task Title: Add extension and theme contribution readiness tests
- Priority: `P2`
- Category: `Testing / Regression Protection`
- Objective: Protect contribution paths as core systems become more authoritative.
- Why This Matters Now: Contribution integration is a classic regression vector.
- Execution Gap Statement: Existing tests do not yet fully gate the new stricter contribution and readiness rules.
- User / Product Impact: Regressions here can create broken shell states in extension-enabled environments.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ContributionValidator.h`
- Prior Plan References: `v23` Phase 11 and 19
- Scope: Contribution validation, registration, gating, recovery
- Out of Scope: Marketplace search-result ranking tests
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_v23_service_stub_completion.cpp`
- Related Features / Systems / Components: Extensions, themes, contributions
- Current Behavior: Contribution readiness is under-tested relative to the shell changes in `v24`.
- Intended Completed Behavior: Contributed commands, menus, settings, and themes are regression-protected.
- Missing Pieces: Fixture extensions and readiness test matrix
- Technical Approach: Build fixture manifests and extension bundles that exercise valid and invalid contribution paths.
- Implementation Steps: Add fixtures; test contribution registration; test style/action/settings validation; wire into CI.
- Validation Steps: Seed intentionally invalid contributions and verify hard or soft failures per policy.
- Acceptance Criteria: Contribution readiness is gated with deterministic tests.
- Dependencies: `P14-T01` through `P14-T04`
- Parallelization Notes: Can scaffold while implementation hardens.
- Risks / Failure Modes: Fixture maintenance overhead if contribution schema evolves quickly.
- Cleanup / Migration Notes: Retire tests that only assert permissive placeholder behavior.
- Observability / Diagnostics Notes: Capture validator output in CI artifacts.
- Rollback / Safety Notes: Keep fixtures minimal and versioned with schema changes.
- References / Context: Contribution validator and extension infrastructure
- Example scenarios where useful: A fixture extension contributes a broken command handler reference and fails validation before runtime.
