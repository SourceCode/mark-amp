# Phase 11: Plugin Extension Theme Marketplace And Contribution Completion

## Outcome

Finish the extension, marketplace, theme-sharing, and contribution systems where registries and manifests are broad but runtime fulfillment still remains partial, local, or stub-backed.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P11-T01

- Phase ID: P11
- Task ID: P11-T01
- Task Title: Finish extension contribution fulfillment behind registered commands menus and settings
- Priority: P1
- Category: Migration Completion
- Atomic Completion Tasks Covered: 60
- Objective: Ensure extension contributions drive real runtime behavior instead of only being parsed and registered.
- Why This Matters Now: Contribution breadth can overstate extension completeness if runtime fulfillers lag behind.
- Completion Gap Statement: Manifest parsing and registry infrastructure are ahead of some actual contribution execution paths.
- User / Product Impact: Extensions can appear installed and recognized without their contributions being fully real.
- Repository Evidence: [PluginManager.cpp](/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp), [ContributionValidator.cpp](/Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp), [LanguageProviderRegistry.h](/Users/ryanrentfro/code/markamp/src/core/LanguageProviderRegistry.h)
- Scope: Commands, menus, keybindings, settings, themes, snippets, and provider registrations.
- Out of Scope: Arbitrary third-party extension features outside declared contribution points.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ContributionValidator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/LanguageProviderRegistry.h`; `/Users/ryanrentfro/code/markamp/src/core/PluginContext.h`
- Related Features / Systems / Components: Plugins, extensions, commands, providers, contributions.
- Current Behavior: Contribution infrastructure is broad but some runtime consumers still lag or remain partial.
- Intended Completed Behavior: Registered contributions materialize into actual runtime capabilities where supported.
- Missing Pieces: Consumer parity, unsupported-state policy, and contribution-to-runtime validation.
- Technical Approach: Audit each contribution point for end-to-end fulfillment and gate unsupported ones explicitly.
- Implementation Steps: Map contribution points to consumers; finish missing consumers; add unsupported-state diagnostics; harden manifest-to-runtime checks.
- Validation Steps: Install built-in and sample extensions and verify contributions appear and execute correctly.
- Acceptance Criteria: No supported contribution point silently stops at registration.
- Dependencies: Phase 02, Phase 17.
- Risks / Failure Modes: Extension breadth can mask unsupported contribution combinations if validation stays shallow.
- Cleanup / Migration Notes where relevant: Remove dormant contribution-point code if it is not meant for production.
- Observability / Diagnostics Notes where relevant: Emit contribution-fulfillment diagnostics during activation.
- Rollback / Safety Notes: Keep unsupported contribution points clearly rejected rather than partially applied.
- References / Context: This phase converts extension infrastructure breadth into honest product capability.
- Example scenarios where useful: An extension-contributed command appears in menus only if it is actually executable.

### P11-T02

- Phase ID: P11
- Task ID: P11-T02
- Task Title: Replace placeholder marketplace and theme-sharing behavior with real metadata and transport flows
- Priority: P1
- Category: Stub Service Completion
- Atomic Completion Tasks Covered: 60
- Objective: Finish theme and extension marketplace features so they stop at real metadata and network behavior rather than local placeholders.
- Why This Matters Now: Marketplace surfaces can otherwise act as polished shells over incomplete backend logic.
- Completion Gap Statement: Theme marketplace behavior still contains local-installed fallback logic and “real implementation” placeholders.
- User / Product Impact: Theme and extension discovery/sharing are less complete than the UI implies.
- Repository Evidence: [ThemeMarketplace.cpp](/Users/ryanrentfro/code/markamp/src/core/ThemeMarketplace.cpp), [GalleryService.cpp](/Users/ryanrentfro/code/markamp/src/core/GalleryService.cpp), [ExtensionIconCache.cpp](/Users/ryanrentfro/code/markamp/src/core/ExtensionIconCache.cpp)
- Scope: Listings, install/uninstall, updates, ratings, metadata fetch, icon loading, and theme package import/export.
- Out of Scope: UI presentation refinements.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeMarketplace.cpp`; `/Users/ryanrentfro/code/markamp/src/core/GalleryService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ExtensionIconCache.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`
- Related Features / Systems / Components: Theme marketplace, extension gallery, sharing, downloads.
- Current Behavior: Some marketplace logic still falls back to shallow local lists or placeholder assets.
- Intended Completed Behavior: Marketplace and sharing features use real metadata, downloads, updates, and package flows.
- Missing Pieces: Transport integration, metadata models, update comparison, and asset loading.
- Technical Approach: Finish gallery/marketplace services and align theme and extension flows with real backend metadata.
- Implementation Steps: Implement listing retrieval; finish update checks; complete package install/export metadata; replace placeholder avatars where real icons exist.
- Validation Steps: Fetch listings, install packages, check updates, and reload assets under online and offline conditions.
- Acceptance Criteria: Marketplace workflows no longer depend on placeholder metadata or local-only fallback behavior for supported modes.
- Dependencies: Phase 12.
- Risks / Failure Modes: Network volatility can blur the line between unavailable service and incomplete implementation if errors are not classified well.
- Cleanup / Migration Notes where relevant: Remove “installed IDs only” fallback logic once real listings are available.
- Observability / Diagnostics Notes where relevant: Log listing fetch, asset-cache misses, and update-check outcomes.
- Rollback / Safety Notes: Keep offline fallback explicit and honest rather than pretending to be full marketplace data.
- References / Context: This closes the gap between marketplace UI and marketplace substance.
- Example scenarios where useful: Checking for theme updates compares real remote metadata instead of returning an empty placeholder list.

### P11-T03

- Phase ID: P11
- Task ID: P11-T03
- Task Title: Finish plugin activation, quarantine, telemetry, and deferred-activation lifecycle edges
- Priority: P2
- Category: Stub Service Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make plugin lifecycle edges as complete as the core activation path.
- Why This Matters Now: Activation breadth is high, but deferred and recovery semantics still need stronger completion guarantees.
- Completion Gap Statement: Deferred activation, quarantine, and related plugin lifecycle edges are structurally present but not fully pressure-tested.
- User / Product Impact: Plugin behavior can feel unpredictable during failures or delayed activation.
- Repository Evidence: [PluginManager.cpp](/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp), [PluginQuarantine.h](/Users/ryanrentfro/code/markamp/src/core/PluginQuarantine.h), [ExtensionTelemetry.cpp](/Users/ryanrentfro/code/markamp/src/core/ExtensionTelemetry.cpp)
- Scope: Deferred activation, crash recovery, quarantine, telemetry, pending activation cleanup, and extension host recovery.
- Out of Scope: New contribution point types.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PluginQuarantine.h`; `/Users/ryanrentfro/code/markamp/src/core/ExtensionTelemetry.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ExtensionHostRecovery.cpp`
- Related Features / Systems / Components: Plugin lifecycle, activation, recovery, telemetry.
- Current Behavior: Lifecycle scaffolding is strong, but some deferred and recovery edges remain under-realized.
- Intended Completed Behavior: Plugins activate, fail, quarantine, recover, and report state deterministically.
- Missing Pieces: Stress coverage, full failure semantics, and deferred-edge cleanup.
- Technical Approach: Close lifecycle edges using the existing plugin manager contracts and explicit recovery states.
- Implementation Steps: Audit pending activation cleanup; harden recovery/quarantine rules; complete telemetry and diagnostics for activation edges; add fixture-based stress tests.
- Validation Steps: Simulate activation failures, crashes, and deferred activation triggers.
- Acceptance Criteria: Plugin lifecycle edges no longer rely on weak or assumed recovery semantics.
- Dependencies: P11-T01.
- Risks / Failure Modes: Plugin recovery logic can become too aggressive and disrupt healthy plugins if thresholds are weak.
- Cleanup / Migration Notes where relevant: Remove lifecycle branches that only exist to support older partial activation models.
- Observability / Diagnostics Notes where relevant: Track deferred activation counts, quarantine reasons, and recovery loops.
- Rollback / Safety Notes: Favor disabling a failing plugin over repeated crash loops.
- References / Context: Extension systems are only complete when failure paths are also complete.
- Example scenarios where useful: A crashing extension is quarantined and reported consistently without destabilizing startup.

### P11-T04

- Phase ID: P11
- Task ID: P11-T04
- Task Title: Finish P1-P4 service surfaces that still return default or stub behavior
- Priority: P1
- Category: Stub Service Completion
- Atomic Completion Tasks Covered: 60
- Objective: Replace remaining service-level stubs that third-party or built-in features may consume.
- Why This Matters Now: Service stubs silently cap what the rest of the platform can actually do.
- Completion Gap Statement: Grammar, task runner, terminal, and adjacent extension-facing services still include stub-return behavior.
- User / Product Impact: Extensions and built-in features can only use a shallower platform than the API surface suggests.
- Repository Evidence: [TaskRunnerService.h](/Users/ryanrentfro/code/markamp/src/core/TaskRunnerService.h), [test_p1_p4_services.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_p1_p4_services.cpp), [GrammarEngine.h](/Users/ryanrentfro/code/markamp/src/core/GrammarEngine.h)
- Scope: Grammar, task runner, terminal-facing extension APIs, quick pick and input where runtime semantics still lag.
- Out of Scope: Entirely new extension service families.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/TaskRunnerService.h`; `/Users/ryanrentfro/code/markamp/tests/unit/test_p1_p4_services.cpp`; `/Users/ryanrentfro/code/markamp/src/core/GrammarEngine.h`; `/Users/ryanrentfro/code/markamp/src/core/TerminalService.h`
- Related Features / Systems / Components: Extension APIs, built-in plugins, language services, automation.
- Current Behavior: Some service layers still intentionally return defaults or stub values.
- Intended Completed Behavior: Extension-facing services deliver real runtime behavior or explicit unsupported states.
- Missing Pieces: Runtime integration, capability negotiation, and service validation.
- Technical Approach: Finish or explicitly gate each service surface claimed by the extension platform.
- Implementation Steps: Audit P1-P4 services; complete supported ones; mark unsupported ones clearly; update tests and docs for truthful capability reporting.
- Validation Steps: Exercise built-in and sample consumers against each completed service.
- Acceptance Criteria: No extension-facing service silently returns stubbed defaults in production-supported scenarios.
- Dependencies: Phase 17.
- Risks / Failure Modes: Overpromising service support can create platform-compatibility debt if capability reporting stays vague.
- Cleanup / Migration Notes where relevant: Remove stub tests once real service behavior is in place.
- Observability / Diagnostics Notes where relevant: Track service invocation, unsupported-feature reasons, and extension-facing errors.
- Rollback / Safety Notes: Prefer explicit unsupported responses over weak fake implementations.
- References / Context: This phase aligns the plugin platform with the service depth it advertises.
- Example scenarios where useful: A contributed task executes through a real task-runner service instead of returning an empty stub response.

### P11-T05

- Phase ID: P11
- Task ID: P11-T05
- Task Title: Add plugin and marketplace completion gates tied to real fulfillment status
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Prevent extension and marketplace completion debt from hiding behind registries and mocks.
- Why This Matters Now: Contribution breadth and UI polish can mask incomplete runtime fulfillment.
- Completion Gap Statement: Existing tests and diagnostics do not yet guarantee end-to-end extension and marketplace completion.
- User / Product Impact: Marketplace or extension regressions can undermine a large product area silently.
- Repository Evidence: [test_extension_management.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_extension_management.cpp), [test_extension_integration.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_extension_integration.cpp), [test_theme_marketplace.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_theme_marketplace.cpp)
- Scope: Contribution fulfillment, activation lifecycle, gallery fetch/install, theme package workflows, service capability reporting.
- Out of Scope: UI screenshot testing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_extension_management.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_extension_integration.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_theme_marketplace.cpp`; `/Users/ryanrentfro/code/markamp/tests/integration`
- Related Features / Systems / Components: Plugins, extensions, marketplace, themes, tests.
- Current Behavior: Tests still permit default-stub marketplace and service behavior in some areas.
- Intended Completed Behavior: Completion gates prove that exposed plugin and marketplace features really work.
- Missing Pieces: Real fixture coverage, service-capability assertions, and network-edge behavior tests.
- Technical Approach: Add fixture-based extension packages and marketplace doubles below the transport/service boundary.
- Implementation Steps: Replace empty-list stub expectations; add install/update scenarios; verify contribution fulfillment and service availability; publish gate summaries.
- Validation Steps: Reintroduce a stub marketplace listing path and confirm the suite fails.
- Acceptance Criteria: Plugin and marketplace completion is protected by explicit regression gates.
- Dependencies: P11-T01 through P11-T04.
- Risks / Failure Modes: Integration coverage can become flaky if external transport is not isolated carefully.
- Cleanup / Migration Notes where relevant: Remove tests that treat empty stub listings as acceptable final behavior.
- Observability / Diagnostics Notes where relevant: Archive activation reports, service diagnostics, and install traces.
- Rollback / Safety Notes: Keep deterministic transport doubles for CI.
- References / Context: This phase prevents the extension subsystem from staying broad-but-partial.
- Example scenarios where useful: Installing a VSIX fixture results in real contributed commands and settings, not just a parsed manifest.
