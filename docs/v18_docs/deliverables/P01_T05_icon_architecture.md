# P01-T05: MUI Icon Architecture and Legacy Isolation Plan

> **Phase 01 — Integration Inventory and Shared Contracts**
> **Status:** Complete
> **Scope:** Maps all icon systems, defines MUI target architecture, and plans legacy deprecation.
> **Rollback:** Documentation only — no code changes.

---

## 1 · Current Icon Systems

### 1.1 System Inventory

| # | System           | Source                         | Description                                    | Count |
|---|------------------|--------------------------------|------------------------------------------------|-------|
| 1 | **IconRegistry** | `IconLibrary.cpp`              | Inline SVG strings registered by name           | 30+   |
| 2 | **IconPipeline** | `IconPipeline.cpp`             | Manifest-driven SVG loader from disk            | Manifest-based |
| 3 | **IconManager**  | `IconManager.cpp`              | Singleton facade: pipeline → registry → fallback| 1     |
| 4 | **IconCache**    | Part of `IconManager`/`IconPipeline` | Keyed by name+size+color+scale           | 2 caches |
| 5 | **Lucide assets**| `resources/icons/lucide/`      | SVG files on disk                               | ~100+ |
| 6 | **FileTypeIconResolver** | `FileTypeIconResolver.cpp` | Extension → icon-name mapping              | ~20   |

### 1.2 IconLibrary.cpp Registration Categories

| Category     | Prefix           | Count | Example Icon IDs                           |
|-------------|------------------|-------|-------------------------------------------|
| Activity Bar | `activity-*`     | 14    | `activity-explorer`, `activity-canvas`, `activity-ai` |
| Toolbar      | `toolbar-*`      | 6     | `toolbar-save`, `toolbar-split-view`, `toolbar-focus` |
| File Types   | `filetype-*`     | 6     | `filetype-markdown`, `filetype-cpp`, `filetype-python` |
| Panel        | `panel-*`        | 4     | `panel-close`, `panel-maximize`, `panel-menu` |
| Status       | `status-*`       | 5     | `status-git`, `status-error`, `status-warning` |
| **Total**    |                  | **35**|                                           |

### 1.3 Resolution Chain

```
IconManager::get_icon_bitmap(name, size, color, scale)
  │
  ├── 1. Check IconCache → hit? return cached bitmap
  │
  ├── 2. Check IconPipeline (manifest-driven)
  │      └── IconManifest::get_entry(name) → SVG asset path → render
  │
  ├── 3. Check IconRegistry (inline SVGs)
  │      └── registry_.get_icon(name) → SvgDocument → IconRenderer::RenderIcon()
  │
  └── 4. Fallback: render "?" glyph placeholder (never transparent)
```

### 1.4 Consumer Sites

| Consumer              | API Used                                | Icon Names Used            |
|-----------------------|-----------------------------------------|----------------------------|
| `ActivityBar.cpp`      | `IconManager::get().draw_icon()`        | `activity-*` (14 icons)   |
| `Toolbar.cpp`          | `IconManager::get().draw_icon()`        | `toolbar-*` (6 icons)     |
| `StatusBarPanel.cpp`   | `FileTypeIconResolver::GetFileIcon()`   | `filetype-*` via resolver |
| `FileTreePanel.cpp`    | `IconPipeline::get_file_icon()`         | Manifest-driven           |
| `CanvasRenderer.cpp`   | `IconManager::get().draw_icon()`        | Various object icons      |
| `SidebarHeader.cpp`    | `IconManager::get().draw_icon()`        | `panel-*` (3 icons)       |
| `TabBar`               | `IconPipeline::get_file_icon()`         | Manifest-driven           |

---

## 2 · Target Architecture (MUI)

### 2.1 Canonical MUI Icon ID Scheme

All icon IDs follow a namespace-scoped convention:

```
{domain}.{context}.{name}

Examples:
  ui.activity.explorer
  ui.activity.search
  ui.toolbar.save
  ui.toolbar.focus
  ui.panel.close
  ui.panel.maximize
  ui.status.git
  ui.status.error
  file.type.markdown
  file.type.cpp
  file.folder.open
  file.folder.closed
```

### 2.2 Single Rendering Path

All icon rendering goes through `IconManager::get_icon_bitmap()`:

```
Consumer → IconManager::get_icon_bitmap(mui_id, size, color, scale)
             │
             ├── IconPipeline (primary: disk-based SVG assets)
             │     └── Manifest maps mui_id → SVG asset path
             │
             └── Fallback: "?" glyph (no more inline SVG registry)
```

### 2.3 Asset Organization

```
resources/icons/
├── mui/                     # New MUI icon assets
│   ├── ui/
│   │   ├── activity/        # activity bar icons
│   │   ├── toolbar/         # toolbar icons
│   │   ├── panel/           # panel chrome icons
│   │   └── status/          # status bar icons
│   └── file/
│       ├── type/            # file type icons
│       └── folder/          # folder icons
├── lucide/                  # Legacy (deprecated, keep for migration)
└── icon-manifest.json       # Updated manifest mapping MUI IDs → asset paths
```

---

## 3 · Migration Plan

### 3.1 Phases

| Phase | Description                                           | Deliverable               |
|-------|-------------------------------------------------------|---------------------------|
| **M1** | Create MUI asset directory and copy/rename SVGs      | `resources/icons/mui/`    |
| **M2** | Update `icon-manifest.json` with MUI ID → asset path | Manifest file             |
| **M3** | Create ID compatibility adapter (old → new)          | `IconIdAdapter.h/.cpp`    |
| **M4** | Update all consumer call sites to use MUI IDs        | Code changes              |
| **M5** | Remove `IconLibrary.cpp` inline SVG registrations    | Delete `RegisterCoreIcons()` |
| **M6** | Remove unused Lucide assets                          | Delete unused SVG files   |

### 3.2 Compatibility Adapter

During migration, `IconIdAdapter` translates legacy IDs to MUI IDs:

| Legacy ID               | MUI ID                    |
|-------------------------|---------------------------|
| `activity-explorer`      | `ui.activity.explorer`    |
| `activity-search`        | `ui.activity.search`      |
| `activity-canvas`        | `ui.activity.canvas`      |
| `activity-ai`            | `ui.activity.ai`          |
| `activity-settings`      | `ui.activity.settings`    |
| `toolbar-save`           | `ui.toolbar.save`         |
| `toolbar-split-view`     | `ui.toolbar.split`        |
| `toolbar-preview-view`   | `ui.toolbar.preview`      |
| `toolbar-source-view`    | `ui.toolbar.source`       |
| `toolbar-focus`          | `ui.toolbar.focus`        |
| `toolbar-themes`         | `ui.toolbar.themes`       |
| `panel-close`            | `ui.panel.close`          |
| `panel-maximize`         | `ui.panel.maximize`       |
| `panel-minimize`         | `ui.panel.minimize`       |
| `panel-menu`             | `ui.panel.menu`           |
| `status-git`             | `ui.status.git`           |
| `status-error`           | `ui.status.error`         |
| `status-warning`         | `ui.status.warning`       |
| `status-info`            | `ui.status.info`          |
| `status-sync`            | `ui.status.sync`          |
| `filetype-default`       | `file.type.default`       |
| `filetype-folder`        | `file.type.folder`        |
| `filetype-markdown`      | `file.type.markdown`      |
| `filetype-cpp`           | `file.type.cpp`           |
| `filetype-header`        | `file.type.header`        |
| `filetype-python`        | `file.type.python`        |

### 3.3 Deprecation Warnings

During the adapter phase, every legacy ID lookup logs a deprecation warning:

```cpp
MARKAMP_LOG_WARN("Deprecated icon ID '{}' → use '{}' instead", legacy_id, mui_id);
```

### 3.4 Systems to be Deprecated

| System                     | Action                        | Timeline       |
|----------------------------|-------------------------------|----------------|
| `IconLibrary.cpp`          | Remove `RegisterCoreIcons()`  | After M4       |
| `IconRegistry` (inline SVG)| Keep as pipeline-internal only| After M4       |
| Lucide SVG folder          | Remove unused files           | After M6       |
| Emoji icons in StatusBar   | Replace with `ui.status.*`    | During M4      |

---

## 4 · Icon Rendering Specifications

### 4.1 Standard Sizes

| Context       | Size (px) | Scale | Notes                        |
|---------------|-----------|-------|------------------------------|
| Activity Bar  | 24×24     | 1.0–2.0 | Retina-aware via `SetScaleFactor` |
| Toolbar       | 20×20     | 1.0–2.0 | Slightly smaller than activity bar |
| File Tree     | 16×16     | 1.0–2.0 | Compact inline icons             |
| Tab Bar       | 16×16     | 1.0–2.0 | Same as file tree                |
| Status Bar    | 14×14     | 1.0–2.0 | Smallest standard size           |

### 4.2 Color Rules

| State      | Color Source                              |
|-----------|------------------------------------------|
| Active     | `ThemeColorToken::EditorFg` (full opacity)|
| Inactive   | `EditorFg` blended 30% with `BgPanel`   |
| Hover      | `EditorFg` (full opacity)                |
| Disabled   | `TextMuted`                              |

### 4.3 Cache Warm-Up

`IconPipeline::warm_cache()` pre-loads 15 common file type icons at startup. MUI migration should update this list to use MUI IDs.
