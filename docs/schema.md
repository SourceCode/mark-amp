# Schema & Data Models

> Persistence formats, serialization, and data structures in MarkAmp v2.3.16.

---

## Overview

MarkAmp uses multiple persistence mechanisms:

| Store             | Format                      | Location            | Purpose                           |
| ----------------- | --------------------------- | ------------------- | --------------------------------- |
| Configuration     | YAML                        | Platform config dir | Application settings              |
| Theme definitions | Markdown + YAML frontmatter | `themes/`           | Theme colors and tokens           |
| Theme JSON        | JSON                        | `resources/themes/` | Theme import/export               |
| Board data        | Binary (v2 format)          | User workspace      | Canvas boards                     |
| Extension storage | JSON key-value              | Per-extension dir   | Extension state                   |
| SQLite database   | SQLite3 (FTS5)              | Platform data dir   | Content indexing, search, AV data |
| Flashcard store   | SQLite3                     | Platform data dir   | FSRS flashcard data               |
| Workspace state   | JSON                        | Workspace root      | Session restore                   |

---

## Configuration Schema (`config.yaml`)

```yaml
theme: "midnight-neon"
view_mode: "split"
sidebar_visible: true
sidebar_width: 250
font_size: 14
font_family: "JetBrains Mono"
word_wrap: true
line_numbers: true
auto_save: false
tab_size: 4
insert_spaces: true
minimap_enabled: false
smooth_scrolling: true
sticky_scroll: true
bracket_matching: true
```

---

## Theme Schema (JSON)

```json
{
  "id": "theme-identifier",
  "name": "Human-Readable Name",
  "author": "Author Name",
  "description": "Theme description",
  "colors": {
    "--bg-app": "#1a1a2e",
    "--bg-panel": "#1f1f3a",
    "--bg-header": "#16162b",
    "--bg-input": "#12122a",
    "--text-main": "#e0e0e0",
    "--text-muted": "#808090",
    "--accent-primary": "#00d4ff",
    "--accent-secondary": "#ff6b9d",
    "--border-light": "#3a3a5a",
    "--border-dark": "#0a0a1a"
  }
}
```

### Required Color Fields

| Key                  | Purpose                         |
| -------------------- | ------------------------------- |
| `--bg-app`           | Main application background     |
| `--bg-panel`         | Panel/sidebar backgrounds       |
| `--bg-header`        | Title bar and toolbar           |
| `--bg-input`         | Input fields and editor         |
| `--text-main`        | Primary text                    |
| `--text-muted`       | Secondary/dimmed text           |
| `--accent-primary`   | Primary accent (links, buttons) |
| `--accent-secondary` | Secondary accent (highlights)   |
| `--border-light`     | Light borders                   |
| `--border-dark`      | Dark borders                    |

### Validation Rules

| Rule                      | Level   | Description                               |
| ------------------------- | ------- | ----------------------------------------- |
| All 10 color keys present | Error   | Missing keys reject the theme             |
| Valid hex color values    | Error   | Must be `#RGB`, `#RRGGBB`, or `#RRGGBBAA` |
| `id` ≤ 64 characters      | Error   | Theme ID length limit                     |
| `name` ≤ 100 characters   | Error   | Theme name length limit                   |
| No null bytes             | Error   | Security check                            |
| No control characters     | Error   | Security check                            |
| Text/bg contrast ≥ 4.5:1  | Warning | WCAG AA accessibility                     |

---

## Extension Manifest (`package.json`)

```json
{
  "name": "extension-id",
  "displayName": "Extension Name",
  "version": "1.0.0",
  "description": "Extension description",
  "publisher": "publisher-name",
  "engines": { "markamp": "^2.0.0" },
  "categories": ["Themes", "Languages"],
  "activationEvents": ["onLanguage:markdown"],
  "contributes": {
    "commands": [{ "command": "ext.doThing", "title": "Do Thing" }],
    "keybindings": [{ "command": "ext.doThing", "key": "ctrl+shift+t" }],
    "themes": [
      {
        "label": "My Theme",
        "uiTheme": "vs-dark",
        "path": "./themes/my-theme.json"
      }
    ]
  }
}
```

---

## Board Serialization (Canvas)

Board data uses a binary format (version 2):

| Field           | Type              | Description                       |
| --------------- | ----------------- | --------------------------------- |
| Magic           | `uint32`          | `0x4D415042` ("MAPB")             |
| Version         | `uint32`          | Format version (currently 2)      |
| Object count    | `uint32`          | Number of canvas objects          |
| Objects         | `CanvasObject[]`  | Serialized objects with type tags |
| Connector count | `uint32`          | Number of connections             |
| Connectors      | `ConnectorData[]` | Connection data                   |

Each `CanvasObject` contains:

- Type tag (sticky note, text box, image, shape, etc.)
- UUID identifier
- Transform2D (position, rotation, scale)
- Object-specific data (text content, color, dimensions)

---

## Attribute View Schema (SQLite)

| Table          | Purpose                                                |
| -------------- | ------------------------------------------------------ |
| `av_databases` | Database instances                                     |
| `av_columns`   | Column definitions (type, name, config)                |
| `av_rows`      | Row data                                               |
| `av_cells`     | Cell values (keyed by row + column)                    |
| `av_views`     | View configurations (table, gallery, kanban, timeline) |
| `av_filters`   | Saved filter definitions                               |
| `av_sorts`     | Saved sort definitions                                 |

### Column Types

| Type        | Storage             | Description                   |
| ----------- | ------------------- | ----------------------------- |
| Text        | `TEXT`              | Plain text                    |
| Number      | `REAL`              | Numeric value                 |
| Select      | `TEXT` (JSON)       | Single-select from options    |
| MultiSelect | `TEXT` (JSON)       | Multi-select from options     |
| Date        | `TEXT` (ISO 8601)   | Date/datetime                 |
| Checkbox    | `INTEGER`           | Boolean                       |
| URL         | `TEXT`              | URL                           |
| Email       | `TEXT`              | Email address                 |
| Phone       | `TEXT`              | Phone number                  |
| Formula     | `TEXT` (expression) | Computed value                |
| Relation    | `TEXT` (UUID ref)   | Cross-database reference      |
| Rollup      | `TEXT` (config)     | Aggregation of related values |

---

## Flashcard Store (SQLite)

| Table            | Purpose                        |
| ---------------- | ------------------------------ |
| `flashcards`     | Card data (front, back, cloze) |
| `review_history` | Review timestamps and ratings  |
| `scheduling`     | FSRS scheduling parameters     |
| `decks`          | Deck organization              |
| `tags`           | Card tagging                   |

### FSRS Fields

| Field      | Type    | Description                                 |
| ---------- | ------- | ------------------------------------------- |
| stability  | REAL    | Memory stability (days)                     |
| difficulty | REAL    | Card difficulty (0.0–1.0)                   |
| due_date   | TEXT    | Next review date (ISO 8601)                 |
| interval   | INTEGER | Current interval (days)                     |
| reps       | INTEGER | Total review count                          |
| lapses     | INTEGER | Lapse count (forgetting)                    |
| state      | INTEGER | Card state (new/learning/review/relearning) |

---

## Content Index (SQLite FTS5)

```sql
CREATE VIRTUAL TABLE content_index USING fts5(
    title,
    body,
    path,
    tags,
    tokenize = 'porter unicode61'
);
```

Used by `SearchEngine`, `ContentIndexer`, and `SearchService` for full-text document search.
