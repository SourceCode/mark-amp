# MarkAmp V27 Full Icon And Visual Overhaul Assessment

## Purpose

`v27` is the full icon-and-visual-overhaul pass for MarkAmp. It is not a backend pass, not a functionality-completion pass, and not a general architecture pass. It is the next dedicated execution package for changing and updating every icon and every visible visual treatment in the application so the product feels fully redesigned, unmistakably coherent, and premium throughout.

## Inputs Reviewed

- Prior planning packages in:
  - `/Users/ryanrentfro/code/markamp/docs/v18_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v19_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v20_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v21_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v22_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v23_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v24_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v25_docs`
  - `/Users/ryanrentfro/code/markamp/docs/v26_docs`
- Current icon and visual implementation in:
  - `/Users/ryanrentfro/code/markamp/resources/icons`
  - `/Users/ryanrentfro/code/markamp/src/ui`
  - `/Users/ryanrentfro/code/markamp/src/core`
  - `/Users/ryanrentfro/code/markamp/src/canvas`

## Executive Summary

MarkAmp now has more icon and visual infrastructure than the earlier UI passes assumed. The repository contains a serious icon migration and rendering stack, multiple token and layout systems, custom shell chrome, and several partially modernized surfaces. The problem is no longer absence of systems. The problem is mixed visual regimes.

The current product still operates as a blend of:

- manifest-driven SVG icon resolution
- custom inline SVG icon registration
- MUI migration scaffolding
- semantic icon mapping and validation seams
- legacy icon-character APIs
- emoji and unicode fallbacks
- file-type glyph fallbacks
- local per-surface paint logic
- partially tokenized shell chrome
- partially redesigned but still locally tuned screens

`v26` was a premium-refinement wave. `v27` must go further. It should not merely converge or polish the current state. It should redefine the product’s full visible identity:

- replace the mixed icon stack with a canonical MUI-led icon system plus explicit custom adapters only where necessary
- redesign every visible surface around one stronger visual language
- eliminate remaining legacy visual and icon residue
- use the existing migration infrastructure as an implementation lever rather than as proof that the work is complete

## What Changed Since Earlier UI Planning

### Earlier Assumption

Earlier UI passes mainly framed icon work as normalization and completion.

### Current Reality

The repository already contains dedicated icon migration seams:

- `/Users/ryanrentfro/code/markamp/src/core/IconInventory.h`
- `/Users/ryanrentfro/code/markamp/src/core/MuiIconMigration.h`
- `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.h`
- `/Users/ryanrentfro/code/markamp/src/core/IconSemanticMapper.h`
- `/Users/ryanrentfro/code/markamp/src/core/IconMetricsPolicy.h`
- `/Users/ryanrentfro/code/markamp/src/core/IconValidationGate.h`
- `/Users/ryanrentfro/code/markamp/src/core/MuiIconMigration.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/IconSemanticMapper.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/IconValidationGate.cpp`

The app also contains a manifest-driven icon system:

- `/Users/ryanrentfro/code/markamp/src/ui/IconManifest.h`
- `/Users/ryanrentfro/code/markamp/src/ui/IconManifest.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/IconPipeline.h`
- `/Users/ryanrentfro/code/markamp/src/ui/IconPipeline.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/IconManager.h`
- `/Users/ryanrentfro/code/markamp/src/ui/IconManager.cpp`
- `/Users/ryanrentfro/code/markamp/resources/icons/icon_manifest.json`

At the same time, visible legacy residue still exists in active UI paths:

- `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.h`
- `/Users/ryanrentfro/code/markamp/src/ui/FileIconResolver.h`
- `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp`

That means `v27` should not be another broad “improve iconography” pass. It should explicitly finish the replacement of the mixed icon regimes and then redesign every affected surface around the new visual language.

## Repository Evidence

### Icon-System Breadth

The codebase currently contains at least `53` icon-related files and assets under `src` and `resources`.

Relevant files include:

- `/Users/ryanrentfro/code/markamp/resources/icons/icon_manifest.json`
- `/Users/ryanrentfro/code/markamp/resources/icons/lucide/folder.svg`
- `/Users/ryanrentfro/code/markamp/resources/icons/lucide/folder-open.svg`
- `/Users/ryanrentfro/code/markamp/resources/icons/lucide/file.svg`
- `/Users/ryanrentfro/code/markamp/resources/icons/lucide/file-text.svg`
- `/Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.h`
- `/Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/FileTypeIconRegistry.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/FileTypeIconResolver.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/FileIconResolver.h`

### Explicit Icon Migration Infrastructure Exists

The repo already anticipates a stronger migration direction:

- `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.h`
  - calls itself the unified MUI-aware icon rendering pipeline
- `/Users/ryanrentfro/code/markamp/src/core/MuiIconMigration.h`
  - tracks migration progress by surface category
- `/Users/ryanrentfro/code/markamp/src/core/IconInventory.h`
  - stores legacy-to-MUI mappings
- `/Users/ryanrentfro/code/markamp/src/core/IconValidationGate.h`
  - bans legacy icon IDs and tracks violations

This is important: `v27` should use these files as execution anchors instead of inventing new migration concepts.

### Legacy Icon Residue Is Still Material

The repo scan still found `107` icon-residue style matches across UI/core/canvas code.

Concrete examples:

- `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.h`
  - still defines emoji category defaults like `📄`, `✏️`, `👁`, `🧭`, `💻`, `🧩`, `📝`, `🔍`, `🐛`, `⚙️`
- `/Users/ryanrentfro/code/markamp/src/ui/FileIconResolver.h`
  - still maps file types to glyph strings such as `{ }`, `M↓`, `⚙`, `🖼`, `📄`, `▶`, `🎨`, `📊`, `🔨`, `⎇`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - still registers sidebars and panel shells using emoji/unicode icon strings and `GetIconChar`
  - still creates `wxStaticText` icon labels directly for panel headers
- `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
  - still uses `⇲`, `✋`, `📝`, `✏`
- `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
  - still prefixes categories with `⚙` and `🧩`
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
  - still prefixes AI state text with `🤖`
- `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp`
  - still uses `⚙ Configuring...`
- `/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.cpp`
  - still uses `⚙`

### Visual Debt Is Still Broad

The UI tree still contains `886` matches for visible styling debt markers such as local font/color setters, placeholders, stubs, “for now”, and temporary visual logic.

The highest-risk files remain concentrated in:

- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

## Key Findings

### 1. The Icon Problem Is No Longer “Lack Of Assets”

The repository has:

- a manifest
- a registry
- a pipeline
- a MUI migration layer
- icon metrics
- semantic mapping
- validation gates
- a gallery dialog
- per-surface icon definitions

The problem is coexistence of too many icon systems and semantics, not lack of infrastructure.

### 2. Visible Surfaces Still Carry Legacy Visual DNA

Even where token or custom shell work exists, many surfaces still read as iterative evolution rather than full redesign. The shell is stronger than the interior surface fleet.

### 3. Some “Modernization” Work Is Still Transitional Rather Than Final

Examples:

- `/Users/ryanrentfro/code/markamp/src/core/IconInventory.cpp`
  - contains only a small initial mapping set
- `/Users/ryanrentfro/code/markamp/src/core/MuiIconMigration.h`
  - tracks progress for a limited set of surface categories
- `/Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp`
  - registers a large custom inline SVG icon set, which may conflict with the stated MUI-first migration direction
- `/Users/ryanrentfro/code/markamp/resources/icons/icon_manifest.json`
  - is broad, but creates another icon vocabulary alongside MUI mapping seams rather than clearly resolving the system choice

### 4. V26 Was Necessary But Not Sufficient

`v26` organized premium UI refinement well, but it still treated iconography as one phase among many. `v27` should elevate icon-system replacement and visual redesign to the top of the stack and make them foundational to everything else.

## Reclassification Of Earlier UI Planning

### Retire Or Treat As Foundation

- `v22` broad UI discovery and “7200 improvements” framing
- first-pass shell and token foundation work
- first-pass icon normalization planning
- `v26` convergence framing where the next move is now explicit redesign rather than only refinement

### Merge Forward Into V27

- `v18`, `v19`, and `v20` MUI icon migration intent
- `v22` iconography and cleanup phases
- `v24` syntax/theme/icon closure framing
- `v25` syntax/theme/icon visible closure
- `v26` iconography optical alignment and semantic completion

### De-Scope From V27

- invisible service cleanup not required for icon or visual overhaul
- backend functionality completion
- release-readiness work unrelated to visible product quality

## Highest-Risk Remaining Icon Inconsistencies

1. Competing icon systems
   Manifest-driven SVGs, custom inline SVG registry icons, emoji/unicode strings, file-glyph fallbacks, and MUI migration seams all coexist.

2. Semantic drift
   Different surfaces appear to map similar actions to different icon vocabularies and naming schemes.

3. Optical inconsistency
   Some surfaces still render icon text via `wxStaticText`, while others use registered SVGs or manifest-driven bitmap generation.

4. State inconsistency
   Icon hover, selected, disabled, warning, and error behavior is not yet clearly one system across shell, panels, menus, dialogs, status, notebook, and canvas.

## Highest-Risk Remaining Visual-System Inconsistencies

1. Local styling still competes with tokenized styling on critical surfaces.
2. Panel and sidebar shells remain less mature than top-level shell chrome.
3. Settings, gallery, startup, and transient surfaces still expose too much utility-style or local styling logic.
4. Notebook and canvas remain visibly behind the shell in premium treatment.

## Definitions Of Visually Transformed

### Icon System

- all visible icons resolve through one canonical system
- emoji/unicode stand-ins are removed from the visible app path
- icon semantics are documented and stable across all features

### Application Shell

- shell regions, panel boundaries, splitters, and tabs read as one redesigned workspace
- no area feels like an older visual generation

### Editor

- syntax, chrome, diagnostics, minimap, and overlays feel redesigned rather than merely tuned

### Notebook

- notebook cells, toolbars, outputs, and execution states have a distinct but fully premium visual identity

### Canvas

- canvas chrome, palette, guides, handles, overlays, and empty states feel as intentionally designed as the editor shell

### Panels / Settings / Dialogs

- all utility and management surfaces feel like first-class product surfaces, not internal tools

## Recommended V27 Strategy

1. Treat icon replacement and visual-language redesign as two separate foundations.
2. Finish the canonical icon system before redesigning all dependent controls and surfaces.
3. Use the existing MUI migration, semantic mapper, metrics policy, and validation gate as the system spine.
4. Redesign shell, editor, notebook, canvas, panels, settings, dialogs, and structured surfaces after the icon and visual foundations are explicit.
5. End with theme parity, accessibility visuals, motion, micro-interactions, cleanup, and a rigorous visual QA matrix.

## Acceptance Standard For V27

The `v27` wave is successful when:

- every visible icon has been evaluated, remapped, replaced, or explicitly justified
- the application no longer presents mixed icon systems on the visible path
- every primary surface feels redesigned, not just polished
- shell, editor, notebook, canvas, panels, settings, menus, toolbars, dialogs, and feedback surfaces clearly belong to one premium visual identity
- theme parity, accessibility visuals, and motion feel intentional and complete
- the product looks fully transformed rather than incrementally improved
