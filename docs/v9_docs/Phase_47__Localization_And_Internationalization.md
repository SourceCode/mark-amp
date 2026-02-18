# Phase 47: Localization and Internationalization

## Overview
MarkAmp has no internationalization infrastructure. All strings are hardcoded in English. This phase adds localization support: string externalization, locale detection, RTL support, date/number formatting, and translation infrastructure.

## Prerequisites
- Phase 36 (Command system for localized commands)
- Phase 05 (Settings for language selection)
- Phase 31 (Accessibility for screen reader localization)

## Tasks

### Task 1: Create Localization Framework
**Files:** `src/core/Localization.h`, `src/core/Localization.cpp`
**Description:** Create localization framework: string table loading, locale detection, string lookup with fallback, pluralization, and interpolation.
**Acceptance Criteria:**
- String table format: JSON (`{"key": "translated string"}`)
- Locale detection: system locale on startup
- Lookup: `L10N("key")` returns localized string
- Fallback: missing translation falls back to English
- Interpolation: `L10N("greeting", {{"name", "User"}})` -> "Hello, User"
- Pluralization: `L10N_PLURAL("items", count)` -> "1 item" / "5 items"

### Task 2: Externalize Core UI Strings
**Files:** All `src/ui/*.cpp` files
**Description:** Extract all hardcoded English strings from UI components to the string table. All user-visible text must use L10N macro.
**Acceptance Criteria:**
- Menu items: all localized
- Button labels: all localized
- Dialog titles and messages: all localized
- Status bar text: all localized
- Tooltip text: all localized
- Error messages: all localized

### Task 3: Externalize Command Palette Strings
**Files:** `src/ui/CommandPalette.cpp`, `src/core/Command.h`
**Description:** All command titles, descriptions, and category names externalized.
**Acceptance Criteria:**
- Command titles: localized
- Command descriptions: localized
- Category names: localized
- Shortcut descriptions: localized
- Quick pick items: localized
- Input box prompts: localized

### Task 4: Externalize Settings Strings
**Files:** `src/core/SettingsCatalog.cpp`, `src/ui/SettingsPanel.cpp`
**Description:** All setting names, descriptions, and option labels externalized.
**Acceptance Criteria:**
- Setting names: localized
- Setting descriptions: localized
- Enum option labels: localized
- Section headers: localized
- Setting category names: localized
- Validation error messages: localized

### Task 5: Externalize Notification Strings
**Files:** `src/core/NotificationService.cpp`, `src/ui/NotificationManager.cpp`
**Description:** All notification titles, messages, and action labels externalized.
**Acceptance Criteria:**
- Notification titles: localized
- Notification messages: localized with interpolation
- Action button labels: localized
- Toast messages: localized
- Progress descriptions: localized

### Task 6: Wire Locale Selection
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/Config.h`, `src/core/Localization.cpp`
**Description:** Language selection in settings. Auto-detect from system or manual override.
**Acceptance Criteria:**
- Settings: "Language" dropdown
- Options: Auto (system), English, + available translations
- Language change: restart prompt or live reload
- Language stored in config
- System locale detection on all platforms
- Override: `--lang=xx` command line flag

### Task 7: Wire Date and Number Formatting
**Files:** `src/core/Localization.cpp`
**Description:** Locale-aware formatting: dates, numbers, file sizes, durations.
**Acceptance Criteria:**
- Date format: locale-appropriate (MM/DD/YYYY vs DD.MM.YYYY)
- Number format: locale-appropriate (1,234.56 vs 1.234,56)
- File size: localized units (KB, MB, GB)
- Duration: localized ("5 minutes ago" vs "vor 5 Minuten")
- Relative time: "2 hours ago", "yesterday", "last week"
- All formatted outputs use locale

### Task 8: Wire RTL (Right-to-Left) Layout Support
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/EditorPanel.cpp`
**Description:** RTL support for Arabic, Hebrew, and other RTL languages: layout mirroring, text direction, cursor behavior.
**Acceptance Criteria:**
- RTL detection from locale
- Layout mirrored: sidebar on right, panels flipped
- Text direction: RTL in editor and preview
- Cursor: moves right-to-left in RTL text
- Mixed content: RTL paragraphs in LTR document
- Bidirectional text rendering

### Task 9: Wire CJK Text Support
**Files:** `src/ui/EditorPanel.cpp`, `src/core/Localization.cpp`
**Description:** CJK (Chinese, Japanese, Korean) support: IME integration, word wrap at character boundaries, CJK font selection.
**Acceptance Criteria:**
- IME integration: compose characters inline
- Word wrap: break at character boundaries (no mid-character break)
- Font fallback: CJK fonts for CJK characters
- Character width: CJK characters count as 2 columns
- Search: CJK character matching
- Sorting: locale-appropriate collation

### Task 10: Wire Translation File Management
**Files:** `src/core/Localization.cpp`
**Description:** Translation files in `locales/` directory: `en.json`, `de.json`, `fr.json`, etc. Support for partial translations with fallback.
**Acceptance Criteria:**
- Translation files: JSON format in `locales/` directory
- Fallback chain: locale-specific -> language -> English
- Partial translations: missing keys fall back to English
- Translation coverage report: percentage per language
- Hot reload: translation changes without restart
- Namespace support: `ui.menu.file.new` for organized keys

### Task 11: Wire Extension Localization Support
**Files:** `src/core/PluginManager.cpp`, `src/core/Localization.cpp`
**Description:** Extensions can provide translations. Extension manifest includes `l10n` contribution with translation files.
**Acceptance Criteria:**
- Extension manifest: `l10n` directory with translation files
- Extension strings loaded on activation
- Extension translations scoped by extension ID
- Fallback to extension's default language
- Extension UI uses L10N with extension scope

### Task 12: Wire Keyboard Layout Awareness
**Files:** `src/core/ShortcutManager.cpp`, `src/platform/PlatformAbstraction.h`
**Description:** Keyboard shortcuts aware of keyboard layout: shortcuts based on physical key position, not character output.
**Acceptance Criteria:**
- Shortcuts by physical key position (QWERTY-based)
- Display shortcut with local character (e.g., "Cmd+Z" on QWERTZ shows "Cmd+Y")
- Shortcut overlay shows correct keys for layout
- Layout detection from OS
- Common shortcuts work on all layouts
- Custom shortcuts: record by keycode

### Task 13: Wire Locale-Aware Sorting
**Files:** `src/core/Localization.cpp`, `src/ui/FileTreeCtrl.cpp`
**Description:** File tree and search results sort using locale-appropriate collation: accented characters sorted correctly, case sensitivity per locale.
**Acceptance Criteria:**
- Collation: locale-appropriate character ordering
- Accented characters: sorted with base character (a, a, a grouped)
- Case: configurable (case-sensitive or insensitive)
- Numeric: "file2" before "file10" (natural sort)
- CJK: stroke order or phonetic sorting
- Sort locale from application locale

### Task 14: Wire Locale-Aware Search
**Files:** `src/core/SearchEngine.cpp`, `src/core/Localization.cpp`
**Description:** Search respects locale: accent-insensitive search option, case folding per locale, CJK tokenization.
**Acceptance Criteria:**
- Accent-insensitive: "cafe" matches "cafe"
- Case folding: locale-appropriate (Turkish dotted I)
- CJK tokenization: character-based search
- Unicode normalization: NFC for consistent matching
- Collation-aware ranking
- Toggle: exact vs locale-aware search

### Task 15: Wire Spell Check Integration
**Files:** `src/ui/EditorPanel.cpp`, `src/platform/PlatformAbstraction.h`
**Description:** Integrate platform spell checker: macOS NSSpellChecker, Windows ISpellChecker, Linux hunspell.
**Acceptance Criteria:**
- Spell check underlines misspelled words
- Right-click: suggestions, add to dictionary, ignore
- Language detection per paragraph
- Custom dictionary per workspace
- Toggle spell check on/off
- Multiple language support in same document

### Task 16: Wire Locale Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register locale commands: "Language: Change Language", "Language: Spell Check Toggle", "Language: Add to Dictionary".
**Acceptance Criteria:**
- All commands registered
- "Change Language" shows language picker
- "Spell Check Toggle" enables/disables
- Commands localized in selected language

### Task 17: Wire Translation Contribution Guide
**Files:** `docs/CONTRIBUTING_TRANSLATIONS.md`
**Description:** Document translation process: how to create new translation files, translation key conventions, testing translations.
**Acceptance Criteria:**
- Translation file format documented
- Key naming conventions
- Pluralization rules per language
- Testing: how to verify translations
- Contribution process: PR with new translation
- Translation coverage tool usage

### Task 18: Wire Locale-Aware Preview Rendering
**Files:** `src/ui/PreviewPanel.cpp`, `src/rendering/HtmlRenderer.cpp`
**Description:** Preview respects document language: `lang` attribute in HTML, text direction, font selection.
**Acceptance Criteria:**
- `lang` attribute set on rendered HTML
- Text direction from document language
- Font selection appropriate for language
- Quotes: locale-appropriate quote marks
- Hyphenation: locale-aware (if supported)
- Date rendering in preview: localized

### Task 19: Wire First Launch Language Selection
**Files:** `src/ui/StartupPanel.cpp`, `src/core/Localization.cpp`
**Description:** On first launch, prompt for language selection before other onboarding steps.
**Acceptance Criteria:**
- First launch: language selection dialog
- Show available languages with native names
- Selection applied immediately
- Onboarding continues in selected language
- System language pre-selected
- Skip: use system language

### Task 20: Add Localization Tests
**Files:** `tests/unit/test_localization.cpp`
**Description:** Test localization: string lookup, fallback, interpolation, pluralization, date formatting, RTL detection.
**Acceptance Criteria:**
- String lookup: correct translation returned
- Fallback: missing key returns English
- Interpolation: variables substituted correctly
- Pluralization: correct form for count
- Date formatting: locale-appropriate output
- RTL detection: correct for known RTL locales

## Testing Requirements
- String lookup and fallback chain
- Interpolation and pluralization
- Date and number formatting per locale
- RTL layout mirroring

## Phase Completion Criteria
- All UI strings externalized
- Language selection in settings
- Locale-aware date, number, and size formatting
- RTL layout support
- CJK text and IME support
- Spell check integration
- Translation file infrastructure
- All tests pass
