# Phase 17: Advanced Domain Release Triage And Gating

## Phase Intent

Explicitly decide which advanced-domain systems must be real for release, which must be gated, and which can be deferred.

## Release-Ready Exit Criteria

- no advanced-domain surface visible on the release path is knowingly placeholder-backed,
- non-critical domains are gated or de-scoped explicitly,
- critical advanced-domain slices have minimal release-ready definitions of done.

## Task Count

3

## Task P17-T01

- Phase ID: P17
- Task ID: P17-T01
- Task Title: Triage advanced-domain systems by release necessity instead of breadth
- Priority: P1
- Category: Release Readiness
- Objective: convert broad advanced-domain ambition into an explicit release support matrix.
- Why This Matters Now: `v25` must remove backlog noise and prevent non-critical depth from blocking release closure.
- Release Gap Statement: advanced domains such as AV, graph, tasks, presentation, and node editor are too broad to treat uniformly at this stage.
- User / Product Impact: clear scope protects release quality and avoids misleading UI.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/node_editor/AudioRuntime.h`; `/Users/ryanrentfro/code/markamp/src/node_editor/GraphicsRuntime.h`; `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.h`
- Prior Plan References: `v23 Phase 14`; `v23 Phase 15`; `v24 Phase 17`
- Scope: node editor, AV, graph, tasks, presentation, notebook-adjacent advanced features visible in the shell.
- Out of Scope: future domain expansion.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/GraphService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PresentationManager.cpp`
- Related Features / Systems / Components: advanced domains; release scope; gating
- Current Behavior: many advanced domains exist structurally but with varying runtime completeness.
- Intended Release-Ready Behavior: only explicitly supported advanced-domain slices remain visible and claimed for release.
- Missing Pieces: support matrix; visibility decisions; signoff criteria.
- Technical Approach: classify each advanced-domain surface as must-finish, gate, or defer based on direct release-path impact.
- Implementation Steps:
1. Inventory visible advanced-domain entry points.
2. Assign must-finish, gated, or deferred status.
3. Update action/panel readiness scope and docs.
- Validation Steps:
1. Confirm gated domains are hidden from release paths.
2. Confirm must-finish domains have concrete tasks and evidence expectations.
- Acceptance Criteria: advanced-domain scope is explicit and no longer implied by residual UI alone.
- Dependencies: P01-T03
- Parallelization Notes: planning and gating work can run while core features finish.
- Risks / Failure Modes: ambiguous scope leading to last-minute churn.
- Release Notes / Cleanup Notes: include a release-supported advanced-domain matrix.
- Observability / Diagnostics Notes: record gated domain IDs in signoff output.
- Rollback / Safety Notes: prefer gating to speculative completion.
- References / Context: `docs/v24_docs/Phase_17__Advanced_Domain_Workflows_AV_Graph_Tasks_Presentation_And_Node_Editor.md`

## Task P17-T02

- Phase ID: P17
- Task ID: P17-T02
- Task Title: Finish the minimum release-critical advanced-domain slices that remain visible
- Priority: P1
- Category: Release Readiness
- Objective: complete only the advanced-domain features that materially affect visible release workflows.
- Why This Matters Now: some advanced features may still be part of the product’s marketed or visible release path.
- Release Gap Statement: visible advanced-domain shells cannot remain thin if they stay in scope.
- User / Product Impact: incomplete visible advanced domains create product-wide trust drag.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.h`; `/Users/ryanrentfro/code/markamp/src/core/GraphService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskService.cpp`
- Prior Plan References: `v23 Phase 15`; `v24 Phase 17`
- Scope: only advanced domains classified as release-supported by P17-T01.
- Out of Scope: domains classified as gated or deferred.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/BacklinkService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CalendarEngine.cpp`
- Related Features / Systems / Components: AV; graph; tasks; presentation
- Current Behavior: some advanced features still have thin UI or partial workflow depth.
- Intended Release-Ready Behavior: supported advanced slices meet minimal trustworthy workflow and validation bars.
- Missing Pieces: per-domain minimum workflow definition; targeted closure tasks; validation.
- Technical Approach: limit scope to the supported matrix and define one or two must-pass workflows per supported domain.
- Implementation Steps:
1. Define the minimum supported workflow per retained domain.
2. Finish the missing blocking pieces only for those workflows.
3. Add minimal smoke and signoff evidence.
- Validation Steps:
1. Run the supported workflows end-to-end.
2. Verify hidden/gated domains no longer leak into the release path.
- Acceptance Criteria: retained advanced-domain slices are trustworthy enough to remain visible.
- Dependencies: P17-T01
- Parallelization Notes: split by domain owner.
- Risks / Failure Modes: retained scope still too broad; hidden dependencies on gated domains.
- Observability / Diagnostics Notes: capture domain-specific smoke results.
- Rollback / Safety Notes: reclassify weak slices as gated rather than stretching scope.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.h`

## Task P17-T03

- Phase ID: P17
- Task ID: P17-T03
- Task Title: Gate placeholder node-editor runtimes and other deferred advanced engines out of release scope
- Priority: P1
- Category: Placeholder / Stub Removal
- Objective: ensure placeholder advanced engines are not accidentally treated as release-ready.
- Why This Matters Now: several advanced runtimes still explicitly describe placeholder computation.
- Release Gap Statement: node-editor runtimes and similar advanced engines still advertise deferred real computation.
- User / Product Impact: visible placeholder advanced engines would seriously undermine release credibility.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/node_editor/AudioRuntime.h`; `/Users/ryanrentfro/code/markamp/src/node_editor/DataGenRuntime.h`; `/Users/ryanrentfro/code/markamp/src/node_editor/GraphicsRuntime.h`
- Prior Plan References: `v23 Phase 14`; `v24 Phase 17`
- Scope: node editor domain runtimes and any advanced engine explicitly marked placeholder or deferred.
- Out of Scope: hidden internal scaffolding not exposed on the release path.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/node_editor/CodeFlowRuntime.h`; `/Users/ryanrentfro/code/markamp/src/node_editor/DataTransformRuntime.h`
- Related Features / Systems / Components: node editor; advanced runtimes; release gating
- Current Behavior: several advanced runtimes explicitly state they return placeholder results.
- Intended Release-Ready Behavior: placeholder advanced engines are gated, hidden, or clearly unsupported.
- Missing Pieces: release-path visibility audit; gating rules; panel/action cleanup.
- Technical Approach: feed advanced-engine visibility through the same action/panel readiness gate used elsewhere.
- Implementation Steps:
1. Identify all visible entry points into placeholder advanced engines.
2. Gate or hide them from release scope.
3. Update release docs and signoff reports.
- Validation Steps:
1. Verify placeholder advanced engines are not reachable from visible release-path UI.
2. Confirm signoff reports list them as gated, not unfinished.
- Acceptance Criteria: explicitly placeholder advanced runtimes are not exposed as release-ready features.
- Dependencies: P17-T01; P01-T02
- Parallelization Notes: low-conflict gating work.
- Risks / Failure Modes: stray shortcuts or menu items remain visible.
- Release Notes / Cleanup Notes: document gated advanced-engine scope for release clarity.
- Observability / Diagnostics Notes: include gated advanced-engine inventory in final signoff.
- Rollback / Safety Notes: hiding is safer than retaining speculative runtime support.
- References / Context: `/Users/ryanrentfro/code/markamp/src/node_editor/AudioRuntime.h`
