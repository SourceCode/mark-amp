# Phase 22: Theme Import and Export

## Objective

Implement the complete theme import and export workflow, including file dialog integration, JSON validation, duplicate handling, user-created theme support, and persistence of imported themes across sessions.

## Prerequisites

- Phase 07 (Theme Data Model and JSON Serialization)
- Phase 21 (Theme Gallery UI)

## Deliverables

1. Theme import from JSON files with validation
2. Theme export to JSON files
3. Imported theme persistence across sessions
4. Theme validation with user-friendly error messages
5. Duplicate theme handling
6. Batch import/export support

## Tasks

### Task 22.1: Implement theme import workflow

Complete the import flow triggered from the Theme Gallery:

**Import workflow:**
1. User clicks "Import Theme" button in the gallery
2. File open dialog appears with filter: `Theme files (*.json, *.theme.json)|*.json;*.theme.json`
3. User selects a file
4. File is read and parsed as JSON
5. JSON is validated against the theme schema
6. If valid: add to ThemeRegistry, save to user themes directory, refresh gallery
7. If invalid: show error message in gallery toolbar

**Validation checks:**
1. Valid JSON (parse check)
2. Has `name` field (non-empty string)
3. Has `colors` object
4. Colors object has all 10 required keys:
   - `--bg-app`, `--bg-panel`, `--bg-header`, `--bg-input`
   - `--text-main`, `--text-muted`
   - `--accent-primary`, `--accent-secondary`
   - `--border-light`, `--border-dark`
5. All color values are valid (hex or rgba format)
6. Optional: `id` field (if missing, auto-generate from name)

**Error messages:**
| Error | Message |
|---|---|
| Invalid JSON | "Error parsing theme file. Ensure it is valid JSON." |
| Missing name | "Invalid theme format: Missing 'name' field." |
| Missing colors | "Invalid theme format: Missing 'colors' object." |
| Missing color key | "Invalid theme format: Missing color '{key}'." |
| Invalid color value | "Invalid theme format: '{key}' has invalid color value." |

**Acceptance criteria:**
- File dialog opens with correct filter
- Valid themes are imported and immediately available
- Invalid themes show descriptive error messages
- Imported themes appear in the gallery grid

### Task 22.2: Implement theme export workflow

Complete the export flow triggered from theme preview card:

**Export workflow:**
1. User clicks export button on a theme card
2. Save file dialog appears with default filename: `{theme_name_snake_case}.theme.json`
3. User selects save location
4. Theme is serialized to JSON with pretty-printing (2-space indent)
5. JSON is written to the selected file
6. Success feedback (optional: brief notification)

**JSON output format (matching reference ThemeGallery.tsx lines 82-88):**
```json
{
  "id": "midnight-neon",
  "name": "Midnight Neon",
  "colors": {
    "--bg-app": "#050510",
    "--bg-panel": "#0a0a1f",
    ...
  }
}
```

The format must be identical to the React version so themes can be exchanged between implementations.

**Acceptance criteria:**
- Save dialog appears with correct default filename
- Exported JSON is pretty-printed and valid
- Exported theme can be re-imported without modification
- Exported theme can be imported by the React version

### Task 22.3: Implement imported theme persistence

Imported themes must survive application restarts:

**Persistence strategy:**
1. When a theme is imported, copy the JSON file to the user themes directory:
   - macOS: `~/Library/Application Support/MarkAmp/themes/`
   - Windows: `%APPDATA%/MarkAmp/themes/`
   - Linux: `~/.config/markamp/themes/`
2. On startup, ThemeRegistry scans this directory and loads all `.json` / `.theme.json` files
3. File naming: `{theme_id}.theme.json` (sanitized for filesystem)

**File naming sanitization:**
- Replace spaces with underscores
- Remove special characters (keep alphanumeric, underscore, hyphen)
- Lowercase

**Acceptance criteria:**
- Imported themes persist across application restarts
- User themes directory is created if it does not exist
- Multiple imported themes are all loaded
- Corrupted theme files in the directory are skipped with a warning

### Task 22.4: Implement duplicate theme handling

When importing a theme with an ID that already exists:

**For built-in themes:**
- If the imported theme has the same ID as a built-in theme:
  - Generate a new unique ID: `{original_id}-custom-{timestamp}`
  - Append " (Custom)" to the name
  - Import as a new theme

**For previously imported themes:**
- If the imported theme has the same ID as an existing custom theme:
  - Ask the user: "A theme with this name already exists. Replace it?"
  - [Replace] [Keep Both] [Cancel]
  - Replace: overwrite the existing theme file
  - Keep Both: generate new ID (same as built-in handling)
  - Cancel: abort import

**Acceptance criteria:**
- Importing a theme with a built-in ID creates a copy, not overwrite
- User is prompted for duplicate custom themes
- Both "Replace" and "Keep Both" work correctly

### Task 22.5: Implement theme deletion

Allow users to delete imported (custom) themes:

**Rules:**
- Built-in themes cannot be deleted
- Custom themes can be deleted from the gallery
- Deleting the active theme switches to the default theme first

**UI:**
- Right-click on a custom theme card shows a context menu: "Delete Theme"
- Or: add a small delete button (trash icon) that appears on hover, only for custom themes
- Confirmation dialog: "Delete theme '{name}'? This cannot be undone."
- On confirm: delete the JSON file from user themes directory, remove from registry

**Acceptance criteria:**
- Custom themes can be deleted
- Built-in themes show no delete option
- Deleting the active theme switches to default
- Confirmation prevents accidental deletion

### Task 22.6: Implement theme validation service

Create `/Users/ryanrentfro/code/markamp/src/core/ThemeValidator.h` and `ThemeValidator.cpp`:

```cpp
namespace markamp::core {

struct ValidationResult {
    bool is_valid{false};
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

class ThemeValidator
{
public:
    auto validate(const nlohmann::json& json) -> ValidationResult;
    auto validate(const Theme& theme) -> ValidationResult;

private:
    void check_required_fields(const nlohmann::json& json, ValidationResult& result);
    void check_color_values(const nlohmann::json& colors, ValidationResult& result);
    void check_contrast_ratios(const Theme& theme, ValidationResult& result);
    void check_color_sanity(const Theme& theme, ValidationResult& result);
};

} // namespace markamp::core
```

**Validation levels:**
1. **Structural** (errors): required fields, correct types
2. **Color validity** (errors): parseable color values
3. **Contrast** (warnings): low contrast between text and background (WCAG AA ratio < 4.5:1)
4. **Sanity** (warnings): unusual choices (e.g., very similar bg_app and bg_panel)

**Acceptance criteria:**
- Structural validation catches all missing/invalid fields
- Color parsing validates all 10 color values
- Contrast warnings are generated for low-contrast themes
- Validation result contains specific, actionable error messages

### Task 22.7: Implement batch export (optional enhancement)

Allow exporting all themes at once:

- "Export All" button in the gallery toolbar
- Creates a ZIP file (or a directory) containing all theme JSON files
- Uses a folder selection dialog

**Acceptance criteria:**
- All themes are exported as individual JSON files in the chosen directory
- Filenames are unique and filesystem-safe

### Task 22.8: Write import/export tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_theme_import_export.cpp`:

Test cases:
1. Import valid theme JSON
2. Import JSON missing `name` field
3. Import JSON missing `colors` object
4. Import JSON with invalid color value
5. Import JSON with extra fields (should succeed, ignore extras)
6. Import duplicate built-in theme ID (generates new ID)
7. Export theme produces valid JSON
8. Export + re-import produces identical theme
9. Theme persistence to user directory
10. Theme loading from user directory on startup
11. Theme deletion removes file
12. Contrast validation warnings
13. Cross-compatibility: export from C++, import with format matching React reference

**Acceptance criteria:**
- All tests pass
- Round-trip import/export produces identical themes
- At least 18 test assertions

## Files Created/Modified

| File | Action |
|---|---|
| `src/core/ThemeValidator.h` | Created |
| `src/core/ThemeValidator.cpp` | Created |
| `src/core/ThemeRegistry.h` | Modified (delete, duplicate handling) |
| `src/core/ThemeRegistry.cpp` | Modified (persistence, loading) |
| `src/ui/ThemeGallery.h` | Modified (import/export integration) |
| `src/ui/ThemeGallery.cpp` | Modified (import/export integration) |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_theme_import_export.cpp` | Created |

## Dependencies

- Phase 07 Theme, ThemeRegistry, JSON serialization
- Phase 21 ThemeGallery
- nlohmann/json
- std::filesystem

## Estimated Complexity

**Medium** -- The core import/export is straightforward JSON I/O. Validation, duplicate handling, and persistence across sessions add moderate complexity. The main challenge is providing a polished user experience with helpful error messages.
