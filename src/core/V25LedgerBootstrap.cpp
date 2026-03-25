/// @file V25LedgerBootstrap.cpp
/// @brief V25 P01-T01: Populates ledger with all V25 tasks and criteria.
#include "core/V25LedgerBootstrap.h"

namespace markamp::core
{

void bootstrap_v25_tasks(ExecutionLedger& ledger)
{
    // Helper to register a task with common defaults.
    auto reg = [&](const std::string& task_id, const std::string& phase_id,
                   const std::string& title, Subsystem subsystem, int priority,
                   std::vector<std::string> deps = {}) {
        LedgerTask t;
        t.task_id     = task_id;
        t.phase_id    = phase_id;
        t.title       = title;
        t.subsystem   = subsystem;
        t.priority    = priority;
        t.dependencies = std::move(deps);
        [[maybe_unused]] auto ok = ledger.register_task(t);
    };

    // ── Phase 01: Release Spine ──
    reg("P01-T01", "P01", "Wire execution ledger for V25 subsystem map", Subsystem::kGovernance, 0);
    reg("P01-T02", "P01", "Enforce release gates for dead actions and placeholders", Subsystem::kGovernance, 0, {"P01-T01"});
    reg("P01-T03", "P01", "Define subsystem done criteria and evidence templates", Subsystem::kGovernance, 0, {"P01-T01"});

    // ── Phase 02: Artifact Lifecycle ──
    reg("P02-T01", "P02", "Remove direct untitled creation from UI surfaces", Subsystem::kArtifactLifecycle, 0);
    reg("P02-T02", "P02", "Finish unsaved buffer adoption and state transitions", Subsystem::kArtifactLifecycle, 0, {"P02-T01"});
    reg("P02-T03", "P02", "Make notebook/canvas creation first-class in lifecycle", Subsystem::kArtifactLifecycle, 1, {"P02-T01"});

    // ── Phase 03: Persistence ──
    reg("P03-T01", "P03", "Move save authority out of LayoutManager", Subsystem::kPersistence, 0);
    reg("P03-T02", "P03", "Harden autosave, recovery, external-change detection", Subsystem::kPersistence, 0, {"P03-T01"});
    reg("P03-T03", "P03", "Make session restore artifact-aware and verifiable", Subsystem::kPersistence, 1, {"P03-T01"});

    // ── Phase 04: Shell Actions ──
    reg("P04-T01", "P04", "Adopt ActionManifest as canonical registry", Subsystem::kShellCommands, 0);
    reg("P04-T02", "P04", "Normalize enablement against live shell context", Subsystem::kShellCommands, 0, {"P04-T01"});
    reg("P04-T03", "P04", "Retire direct widget bindings bypassing commands", Subsystem::kShellCommands, 1, {"P04-T01"});

    // ── Phase 05: Panels ──
    reg("P05-T01", "P05", "Replace placeholder panels with real or gating", Subsystem::kPanels, 0);
    reg("P05-T02", "P05", "Harden explorer/search/panel restore", Subsystem::kPanels, 0, {"P05-T01"});
    reg("P05-T03", "P05", "Establish one real settings host path", Subsystem::kSettings, 0);

    // ── Phase 06: Editor/Search ──
    reg("P06-T01", "P06", "Replace empty search with real indexed search", Subsystem::kSearch, 0);
    reg("P06-T02", "P06", "Align preview with real renderer and diagnostics", Subsystem::kEditor, 1, {"P06-T01"});
    reg("P06-T03", "P06", "Finish editor diagnostics navigation and overlays", Subsystem::kEditor, 1);

    // ── Phase 07: Visual System ──
    reg("P07-T01", "P07", "Remove placeholder visuals and icon inconsistencies", Subsystem::kVisualSystem, 1);
    reg("P07-T02", "P07", "Raise syntax highlighting to release dependability", Subsystem::kVisualSystem, 1);
    reg("P07-T03", "P07", "Normalize empty/loading/error states across surfaces", Subsystem::kVisualSystem, 1);

    // ── Phase 10: Workspace ──
    reg("P10-T01", "P10", "Harden workspace open/reopen continuity", Subsystem::kWorkspace, 0);
    reg("P10-T02", "P10", "Make explorer/breadcrumbs context-correct", Subsystem::kWorkspace, 1, {"P10-T01"});
    reg("P10-T03", "P10", "Add workspace continuity smoke validation", Subsystem::kWorkspace, 1, {"P10-T01"});

    // ── Phase 11: Source Control ──
    reg("P11-T01", "P11", "Replace fake Git status/hash behavior", Subsystem::kSourceControl, 0);
    reg("P11-T02", "P11", "Complete repository snapshot diff and history", Subsystem::kSourceControl, 1, {"P11-T01"});
    reg("P11-T03", "P11", "Gate or finish history/diff panels", Subsystem::kSourceControl, 1, {"P11-T01"});

    // ── Phase 12: Build/Terminal ──
    reg("P12-T01", "P12", "Close dead execution-surface controls", Subsystem::kBuildTerminal, 1);
    reg("P12-T02", "P12", "Align terminal/output/problems with real state", Subsystem::kBuildTerminal, 1, {"P12-T01"});
    reg("P12-T03", "P12", "Add build/run/output smoke coverage", Subsystem::kBuildTerminal, 1);

    // ── Phase 13: Settings ──
    reg("P13-T01", "P13", "Complete SettingsStateOwner adoption", Subsystem::kSettings, 0);
    reg("P13-T02", "P13", "Finish workspace-scope overrides and deep-link", Subsystem::kSettings, 1, {"P13-T01"});
    reg("P13-T03", "P13", "De-duplicate settings schema ownership", Subsystem::kSettings, 1, {"P13-T01"});

    // ── Phase 14: Plugins ──
    reg("P14-T01", "P14", "Audit contribution points against runtime", Subsystem::kPlugins, 1);
    reg("P14-T02", "P14", "Harden extension failure handling/recovery", Subsystem::kPlugins, 0, {"P14-T01"});
    reg("P14-T03", "P14", "Gate marketplace/theme UI to truthful scope", Subsystem::kPlugins, 1, {"P14-T01"});

    // ── Phase 15: Cloud/Security ──
    reg("P15-T01", "P15", "Replace placeholder XOR encryption", Subsystem::kCloudSync, 0);
    reg("P15-T02", "P15", "Complete or gate S3/WebDAV transport", Subsystem::kCloudSync, 1, {"P15-T01"});
    reg("P15-T03", "P15", "Add sync/security diagnostics/release-gate", Subsystem::kCloudSync, 1, {"P15-T01"});

    // ── Phase 16: Rendering ──
    reg("P16-T01", "P16", "Close placeholder rendering branches", Subsystem::kRendering, 0);
    reg("P16-T02", "P16", "Finish or gate PDF/media/print flows", Subsystem::kRendering, 1, {"P16-T01"});
    reg("P16-T03", "P16", "Add import/export fidelity regression checks", Subsystem::kRendering, 1);

    // ── Phase 17: Advanced Domains ──
    reg("P17-T01", "P17", "Triage advanced domains by release necessity", Subsystem::kAdvancedDomains, 1, {"P01-T03"});
    reg("P17-T02", "P17", "Finish release-critical advanced-domain slices", Subsystem::kAdvancedDomains, 1, {"P17-T01"});
    reg("P17-T03", "P17", "Gate placeholder node-editor runtimes", Subsystem::kAdvancedDomains, 1, {"P17-T01", "P01-T02"});

    // ── Phase 18: Migration/Stub Retirement ──
    reg("P18-T01", "P18", "Remove duplicate release-path ownership", Subsystem::kGovernance, 0, {"P02-T01", "P04-T03", "P05-T03"});
    reg("P18-T02", "P18", "Use inventories to finish/gate remaining placeholders", Subsystem::kGovernance, 1, {"P17-T01"});
    reg("P18-T03", "P18", "Finalize release-path labels and copy", Subsystem::kVisualSystem, 2, {"P07-T03", "P17-T01"});

    // ── Phase 19: Validation ──
    reg("P19-T01", "P19", "Turn smoke coverage into required release gate", Subsystem::kValidation, 0, {"P01-T03"});
    reg("P19-T02", "P19", "Add restart/recovery/invalid-state regression", Subsystem::kValidation, 0, {"P03-T02", "P03-T03", "P19-T01"});
    reg("P19-T03", "P19", "Consolidate observability into validation dashboard", Subsystem::kValidation, 1, {"P01-T01", "P01-T02", "P01-T03", "P19-T01"});

    // ── Phase 20: Release Signoff ──
    reg("P20-T01", "P20", "Run subsystem signoff from real evidence", Subsystem::kRelease, 0, {"P19-T01", "P19-T02", "P19-T03"});
    reg("P20-T02", "P20", "Align packaging/versioning/defaults with readiness", Subsystem::kRelease, 1, {"P17-T01", "P18-T03", "P20-T01"});
    reg("P20-T03", "P20", "Execute final blocker review and close V25", Subsystem::kRelease, 0, {"P20-T01", "P20-T02"});
}

void bootstrap_v25_criteria(SubsystemDoneCriteria& criteria)
{
    auto add = [&](Subsystem s, const std::string& id, const std::string& desc) {
        DoneCriterion c;
        c.criterion_id = id;
        c.description  = desc;
        c.is_required  = true;
        criteria.add_criterion(s, c);
    };

    add(Subsystem::kGovernance,        "gov-ledger",           "V25 ledger populated with all 60 tasks");
    add(Subsystem::kGovernance,        "gov-gates",            "Release gates block on dead actions/placeholders");
    add(Subsystem::kArtifactLifecycle, "artifact-creation",    "All creation routes through ArtifactCreationService");
    add(Subsystem::kArtifactLifecycle, "artifact-states",      "Unsaved buffer adoption is complete");
    add(Subsystem::kPersistence,       "persist-save",         "Save authority is out of LayoutManager");
    add(Subsystem::kPersistence,       "persist-autosave",     "Autosave and recovery produce metadata");
    add(Subsystem::kPersistence,       "persist-restore",      "Session restore is artifact-aware");
    add(Subsystem::kShellCommands,     "shell-manifest",       "ActionManifest is canonical for release actions");
    add(Subsystem::kShellCommands,     "shell-enablement",     "Enablement respects live context");
    add(Subsystem::kPanels,            "panels-ready",         "No placeholder panels on release path");
    add(Subsystem::kSettings,          "settings-host",        "Single authoritative settings host");
    add(Subsystem::kSearch,            "search-indexed",       "Search returns real indexed results");
    add(Subsystem::kEditor,            "editor-diagnostics",   "Diagnostics navigation is trustworthy");
    add(Subsystem::kVisualSystem,      "visual-no-placeholder","No placeholder visuals on release path");
    add(Subsystem::kWorkspace,         "workspace-reopen",     "Workspace reopen is dependable");
    add(Subsystem::kSourceControl,     "sc-real-git",          "No fake Git status/hashes");
    add(Subsystem::kBuildTerminal,     "build-no-dead",        "No dead execution surface controls");
    add(Subsystem::kPlugins,           "plugins-contrib",      "Extension contributions match runtime");
    add(Subsystem::kCloudSync,         "cloud-real-crypto",    "Real encryption replaces XOR placeholder");
    add(Subsystem::kRendering,         "render-no-placeholder","No placeholder rendering on release path");
    add(Subsystem::kAdvancedDomains,   "advanced-triaged",     "Advanced domains explicitly scoped");
    add(Subsystem::kValidation,        "validation-smoke",     "Smoke coverage is a release gate");
    add(Subsystem::kRelease,           "release-signoff",      "All subsystems have explicit verdicts");
}

} // namespace markamp::core
