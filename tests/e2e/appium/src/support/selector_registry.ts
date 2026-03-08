/**
 * @file selector_registry.ts
 *
 * Central registry of all ma.* accessibility selectors used across
 * the MarkAmp application. Every selector used in E2E tests must
 * be registered here with its surface prefix and description.
 *
 * Tasks 1–18 (Phase 02): Audit, registration, uniqueness checking,
 * and lint rule for dynamic-label prevention.
 */

// ── Selector Definition ──

export interface SelectorEntry {
    /** The full ma.* selector string */
    selector: string;
    /** Human-readable description of what this selector identifies */
    description: string;
    /** The surface prefix this selector belongs to */
    surface: string;
    /** Whether this selector is required for basic app functionality */
    required: boolean;
}

// ── Registered Surface Prefixes ──

export const SURFACE_PREFIXES = [
    'ma.shell',
    'ma.activitybar',
    'ma.editor',
    'ma.filetree',
    'ma.settings',
    'ma.commandpalette',
    'ma.statusbar',
    'ma.panelarea',
    'ma.tabbar',
    'ma.breadcrumb',
    'ma.toolbar',
    'ma.search',
    'ma.findreplace',
    'ma.preview',
    'ma.outline',
    'ma.sourcecontrol',
    'ma.extensions',
    'ma.themegallery',
    'ma.terminal',
    'ma.debug',
    'ma.canvas',
    'ma.graph',
    'ma.notebook',
    'ma.av',
    'ma.fsrs',
    'ma.ai',
    'ma.sync',
    'ma.vault',
    'ma.tasks',
    'ma.calendar',
    'ma.gantt',
    'ma.presentation',
    'ma.notification',
    'ma.welcome',
] as const;

export type SurfacePrefix = typeof SURFACE_PREFIXES[number];

// ── Selector Registry ──

/**
 * Complete registry of all ma.* selectors.
 * Tasks 1–16: One entry per audited control across all surfaces.
 */
export const SELECTOR_REGISTRY: SelectorEntry[] = [
    // ── Task 1–2: App Shell Controls ──
    { selector: 'ma.shell.main_frame', description: 'Main application window frame', surface: 'ma.shell', required: true },
    { selector: 'ma.shell.menubar', description: 'Application menu bar', surface: 'ma.shell', required: false },
    { selector: 'ma.shell.content_area', description: 'Main content area below toolbar', surface: 'ma.shell', required: false },

    // ── Task 3: Editor and Tab Controls ──
    { selector: 'ma.editor.panel', description: 'Editor panel container', surface: 'ma.editor', required: true },
    { selector: 'ma.editor.textarea', description: 'Editor text input area', surface: 'ma.editor', required: false },
    { selector: 'ma.editor.gutter', description: 'Editor line number gutter', surface: 'ma.editor', required: false },
    { selector: 'ma.editor.minimap', description: 'Editor minimap panel', surface: 'ma.editor', required: false },
    { selector: 'ma.tabbar.container', description: 'Tab bar container', surface: 'ma.tabbar', required: false },
    { selector: 'ma.tabbar.active_tab', description: 'Currently active tab', surface: 'ma.tabbar', required: false },
    { selector: 'ma.tabbar.close_btn', description: 'Tab close button', surface: 'ma.tabbar', required: false },

    // ── Task 4: File Tree and Explorer Controls ──
    { selector: 'ma.filetree.ctrl', description: 'File tree control', surface: 'ma.filetree', required: true },
    { selector: 'ma.filetree.root', description: 'File tree root node', surface: 'ma.filetree', required: false },
    { selector: 'ma.filetree.item', description: 'Individual file tree item', surface: 'ma.filetree', required: false },
    { selector: 'ma.filetree.folder', description: 'Folder node in file tree', surface: 'ma.filetree', required: false },
    { selector: 'ma.filetree.context_menu', description: 'File tree context menu', surface: 'ma.filetree', required: false },

    // ── Task 5: Activity Bar Controls ──
    { selector: 'ma.activitybar', description: 'Activity bar container', surface: 'ma.activitybar', required: true },
    { selector: 'ma.activitybar.explorer', description: 'Explorer section button', surface: 'ma.activitybar', required: false },
    { selector: 'ma.activitybar.search', description: 'Search section button', surface: 'ma.activitybar', required: false },
    { selector: 'ma.activitybar.sourcecontrol', description: 'Source control section button', surface: 'ma.activitybar', required: false },
    { selector: 'ma.activitybar.extensions', description: 'Extensions section button', surface: 'ma.activitybar', required: false },
    { selector: 'ma.activitybar.settings', description: 'Settings section button', surface: 'ma.activitybar', required: false },

    // ── Task 6: Settings Dialog Controls ──
    { selector: 'ma.settings.panel', description: 'Settings panel container', surface: 'ma.settings', required: true },
    { selector: 'ma.settings.search', description: 'Settings search input', surface: 'ma.settings', required: false },
    { selector: 'ma.settings.categories', description: 'Settings category list', surface: 'ma.settings', required: false },
    { selector: 'ma.settings.content', description: 'Settings content area', surface: 'ma.settings', required: false },

    // ── Task 7: Command Palette Controls ──
    { selector: 'ma.commandpalette', description: 'Command palette container', surface: 'ma.commandpalette', required: true },
    { selector: 'ma.commandpalette.input', description: 'Command palette search input', surface: 'ma.commandpalette', required: false },
    { selector: 'ma.commandpalette.results', description: 'Command palette results list', surface: 'ma.commandpalette', required: false },
    { selector: 'ma.commandpalette.item', description: 'Command palette result item', surface: 'ma.commandpalette', required: false },

    // ── Task 8: Panel Area Tabs ──
    { selector: 'ma.panelarea', description: 'Bottom panel area container', surface: 'ma.panelarea', required: false },
    { selector: 'ma.panelarea.output', description: 'Output tab', surface: 'ma.panelarea', required: false },
    { selector: 'ma.panelarea.problems', description: 'Problems tab', surface: 'ma.panelarea', required: false },
    { selector: 'ma.panelarea.debug_console', description: 'Debug console tab', surface: 'ma.panelarea', required: false },
    { selector: 'ma.panelarea.terminal', description: 'Terminal tab', surface: 'ma.panelarea', required: false },

    // ── Task 9: Terminal and Debug Panel Controls ──
    { selector: 'ma.terminal', description: 'Terminal panel container', surface: 'ma.terminal', required: false },
    { selector: 'ma.terminal.input', description: 'Terminal input area', surface: 'ma.terminal', required: false },
    { selector: 'ma.terminal.output', description: 'Terminal output area', surface: 'ma.terminal', required: false },
    { selector: 'ma.debug', description: 'Debug panel container', surface: 'ma.debug', required: false },
    { selector: 'ma.debug.toolbar', description: 'Debug toolbar', surface: 'ma.debug', required: false },
    { selector: 'ma.debug.console', description: 'Debug console area', surface: 'ma.debug', required: false },

    // ── Task 10: Extension Browser Controls ──
    { selector: 'ma.extensions', description: 'Extension browser container', surface: 'ma.extensions', required: false },
    { selector: 'ma.extensions.search', description: 'Extension search input', surface: 'ma.extensions', required: false },
    { selector: 'ma.extensions.list', description: 'Extension list', surface: 'ma.extensions', required: false },
    { selector: 'ma.extensions.detail', description: 'Extension detail panel', surface: 'ma.extensions', required: false },
    { selector: 'ma.extensions.install_btn', description: 'Extension install button', surface: 'ma.extensions', required: false },

    // ── Task 11: Theme Gallery Controls ──
    { selector: 'ma.themegallery', description: 'Theme gallery container', surface: 'ma.themegallery', required: false },
    { selector: 'ma.themegallery.list', description: 'Theme list', surface: 'ma.themegallery', required: false },
    { selector: 'ma.themegallery.preview', description: 'Theme preview area', surface: 'ma.themegallery', required: false },
    { selector: 'ma.themegallery.apply_btn', description: 'Theme apply button', surface: 'ma.themegallery', required: false },

    // ── Task 12: Search/Find-Replace Controls ──
    { selector: 'ma.search', description: 'Global search panel container', surface: 'ma.search', required: false },
    { selector: 'ma.search.input', description: 'Global search input', surface: 'ma.search', required: false },
    { selector: 'ma.search.results', description: 'Search results list', surface: 'ma.search', required: false },
    { selector: 'ma.search.replace_input', description: 'Search replace input', surface: 'ma.search', required: false },
    { selector: 'ma.search.regex_toggle', description: 'Regex toggle button', surface: 'ma.search', required: false },
    { selector: 'ma.search.case_toggle', description: 'Case-sensitive toggle button', surface: 'ma.search', required: false },
    { selector: 'ma.search.word_toggle', description: 'Whole-word toggle button', surface: 'ma.search', required: false },
    { selector: 'ma.findreplace', description: 'In-file find/replace bar', surface: 'ma.findreplace', required: false },
    { selector: 'ma.findreplace.find_input', description: 'Find input field', surface: 'ma.findreplace', required: false },
    { selector: 'ma.findreplace.replace_input', description: 'Replace input field', surface: 'ma.findreplace', required: false },
    { selector: 'ma.findreplace.next_btn', description: 'Find next button', surface: 'ma.findreplace', required: false },
    { selector: 'ma.findreplace.prev_btn', description: 'Find previous button', surface: 'ma.findreplace', required: false },
    { selector: 'ma.findreplace.replace_btn', description: 'Replace button', surface: 'ma.findreplace', required: false },
    { selector: 'ma.findreplace.replace_all_btn', description: 'Replace all button', surface: 'ma.findreplace', required: false },

    // ── Task 13: Graph View Controls ──
    { selector: 'ma.graph', description: 'Knowledge graph container', surface: 'ma.graph', required: false },
    { selector: 'ma.graph.canvas', description: 'Graph canvas area', surface: 'ma.graph', required: false },
    { selector: 'ma.graph.minimap', description: 'Graph minimap', surface: 'ma.graph', required: false },
    { selector: 'ma.graph.filter', description: 'Graph filter controls', surface: 'ma.graph', required: false },
    { selector: 'ma.graph.backlinks', description: 'Backlinks panel', surface: 'ma.graph', required: false },

    // ── Task 14: Canvas Workspace Controls ──
    { selector: 'ma.canvas', description: 'Canvas workspace container', surface: 'ma.canvas', required: false },
    { selector: 'ma.canvas.board', description: 'Canvas board area', surface: 'ma.canvas', required: false },
    { selector: 'ma.canvas.toolbar', description: 'Canvas toolbar', surface: 'ma.canvas', required: false },
    { selector: 'ma.canvas.zoom', description: 'Canvas zoom controls', surface: 'ma.canvas', required: false },
    { selector: 'ma.canvas.grid_toggle', description: 'Canvas grid toggle', surface: 'ma.canvas', required: false },
    { selector: 'ma.canvas.snap_toggle', description: 'Canvas snap-to-grid toggle', surface: 'ma.canvas', required: false },
    { selector: 'ma.canvas.layers', description: 'Canvas layers panel', surface: 'ma.canvas', required: false },
    { selector: 'ma.canvas.properties', description: 'Canvas properties panel', surface: 'ma.canvas', required: false },

    // ── Task 15: Notebook Controls ──
    { selector: 'ma.notebook', description: 'Notebook container', surface: 'ma.notebook', required: false },
    { selector: 'ma.notebook.toolbar', description: 'Notebook toolbar', surface: 'ma.notebook', required: false },
    { selector: 'ma.notebook.cell_list', description: 'Notebook cell list', surface: 'ma.notebook', required: false },
    { selector: 'ma.notebook.add_cell_btn', description: 'Add cell button', surface: 'ma.notebook', required: false },

    // ── Task 16: AV (Database) Controls ──
    { selector: 'ma.av', description: 'AV database container', surface: 'ma.av', required: false },
    { selector: 'ma.av.table', description: 'AV table view', surface: 'ma.av', required: false },
    { selector: 'ma.av.gallery', description: 'AV gallery view', surface: 'ma.av', required: false },
    { selector: 'ma.av.kanban', description: 'AV kanban view', surface: 'ma.av', required: false },
    { selector: 'ma.av.toolbar', description: 'AV toolbar', surface: 'ma.av', required: false },
    { selector: 'ma.av.add_column_btn', description: 'Add column button', surface: 'ma.av', required: false },
    { selector: 'ma.av.filter_btn', description: 'Filter button', surface: 'ma.av', required: false },
    { selector: 'ma.av.sort_btn', description: 'Sort button', surface: 'ma.av', required: false },

    // ── Additional Controls ──
    { selector: 'ma.statusbar', description: 'Status bar container', surface: 'ma.statusbar', required: true },
    { selector: 'ma.statusbar.line_col', description: 'Line/column indicator', surface: 'ma.statusbar', required: false },
    { selector: 'ma.statusbar.encoding', description: 'File encoding indicator', surface: 'ma.statusbar', required: false },
    { selector: 'ma.statusbar.branch', description: 'Git branch indicator', surface: 'ma.statusbar', required: false },
    { selector: 'ma.breadcrumb', description: 'Breadcrumb bar container', surface: 'ma.breadcrumb', required: false },
    { selector: 'ma.toolbar', description: 'Main toolbar container', surface: 'ma.toolbar', required: false },
    { selector: 'ma.outline', description: 'Outline panel container', surface: 'ma.outline', required: false },
    { selector: 'ma.preview', description: 'Markdown preview container', surface: 'ma.preview', required: false },
    { selector: 'ma.sourcecontrol', description: 'Source control panel container', surface: 'ma.sourcecontrol', required: false },
    { selector: 'ma.sourcecontrol.changes', description: 'Source control changes list', surface: 'ma.sourcecontrol', required: false },
    { selector: 'ma.sourcecontrol.staged', description: 'Source control staged list', surface: 'ma.sourcecontrol', required: false },
    { selector: 'ma.sourcecontrol.commit_input', description: 'Commit message input', surface: 'ma.sourcecontrol', required: false },
    { selector: 'ma.sourcecontrol.commit_btn', description: 'Commit button', surface: 'ma.sourcecontrol', required: false },
    { selector: 'ma.notification', description: 'Notification center', surface: 'ma.notification', required: false },
    { selector: 'ma.welcome', description: 'Welcome screen container', surface: 'ma.welcome', required: false },
    { selector: 'ma.welcome.recent_files', description: 'Welcome recent files list', surface: 'ma.welcome', required: false },
    { selector: 'ma.welcome.open_folder_btn', description: 'Welcome open folder button', surface: 'ma.welcome', required: false },

    // ── FSRS / Flashcard Controls ──
    { selector: 'ma.fsrs', description: 'FSRS flashcard container', surface: 'ma.fsrs', required: false },
    { selector: 'ma.fsrs.deck_list', description: 'Flashcard deck list', surface: 'ma.fsrs', required: false },
    { selector: 'ma.fsrs.card', description: 'Current flashcard', surface: 'ma.fsrs', required: false },
    { selector: 'ma.fsrs.rating_btns', description: 'FSRS rating buttons', surface: 'ma.fsrs', required: false },

    // ── AI Assistant Controls ──
    { selector: 'ma.ai', description: 'AI assistant panel container', surface: 'ma.ai', required: false },
    { selector: 'ma.ai.input', description: 'AI chat input', surface: 'ma.ai', required: false },
    { selector: 'ma.ai.output', description: 'AI response output', surface: 'ma.ai', required: false },
    { selector: 'ma.ai.provider_select', description: 'AI provider selector', surface: 'ma.ai', required: false },
    { selector: 'ma.ai.actions', description: 'AI quick actions panel', surface: 'ma.ai', required: false },

    // ── Cloud Sync Controls ──
    { selector: 'ma.sync', description: 'Cloud sync settings panel', surface: 'ma.sync', required: false },
    { selector: 'ma.sync.status', description: 'Sync status indicator', surface: 'ma.sync', required: false },
    { selector: 'ma.sync.manual_btn', description: 'Manual sync button', surface: 'ma.sync', required: false },
    { selector: 'ma.sync.history', description: 'Sync history log', surface: 'ma.sync', required: false },

    // ── Security / Vault Controls ──
    { selector: 'ma.vault', description: 'Vault settings panel', surface: 'ma.vault', required: false },
    { selector: 'ma.vault.lock_btn', description: 'Vault lock button', surface: 'ma.vault', required: false },
    { selector: 'ma.vault.unlock_input', description: 'Vault password input', surface: 'ma.vault', required: false },

    // ── Task / Calendar / Gantt Controls ──
    { selector: 'ma.tasks', description: 'Task panel container', surface: 'ma.tasks', required: false },
    { selector: 'ma.tasks.list', description: 'Task list', surface: 'ma.tasks', required: false },
    { selector: 'ma.tasks.add_btn', description: 'Add task button', surface: 'ma.tasks', required: false },
    { selector: 'ma.calendar', description: 'Calendar view container', surface: 'ma.calendar', required: false },
    { selector: 'ma.gantt', description: 'Gantt timeline container', surface: 'ma.gantt', required: false },

    // ── Presentation Controls ──
    { selector: 'ma.presentation', description: 'Presentation view container', surface: 'ma.presentation', required: false },
    { selector: 'ma.presentation.slides', description: 'Slide list', surface: 'ma.presentation', required: false },
    { selector: 'ma.presentation.controls', description: 'Slide navigation controls', surface: 'ma.presentation', required: false },
];

// ── Task 17: Uniqueness Checker ──

/**
 * Check that all registered selectors are unique.
 * Returns an array of duplicate selector strings (empty if all unique).
 */
export function findDuplicateSelectors(): string[] {
    const seen = new Set<string>();
    const duplicates: string[] = [];
    for (const entry of SELECTOR_REGISTRY) {
        if (seen.has(entry.selector)) {
            duplicates.push(entry.selector);
        }
        seen.add(entry.selector);
    }
    return duplicates;
}

// ── Task 18: Dynamic-Label Lint Check ──

/**
 * Validate that no selector contains dynamic content patterns.
 * Returns an array of selectors that look dynamic (contain digits,
 * UUID patterns, timestamps, or path separators).
 */
export function findDynamicSelectors(): string[] {
    const dynamicPatterns = [
        /\d{4}-\d{2}-\d{2}/,  // Date patterns
        /[0-9a-f]{8}-/,        // UUID-like patterns
        /\//,                   // Path separators
        /\d{10,}/,             // Timestamps
    ];

    return SELECTOR_REGISTRY
        .filter(entry =>
            dynamicPatterns.some(pattern => pattern.test(entry.selector))
        )
        .map(entry => entry.selector);
}

/**
 * Validate that all selectors use a registered surface prefix.
 * Returns selectors that do not match any registered prefix.
 */
export function findUnregisteredPrefixes(): string[] {
    return SELECTOR_REGISTRY
        .filter(entry =>
            !SURFACE_PREFIXES.some(prefix => entry.selector.startsWith(prefix))
        )
        .map(entry => entry.selector);
}

/**
 * Get all required selectors.
 */
export function getRequiredSelectors(): string[] {
    return SELECTOR_REGISTRY
        .filter(entry => entry.required)
        .map(entry => entry.selector);
}

/**
 * Get all selectors for a given surface.
 */
export function getSelectorsBySurface(surface: SurfacePrefix): string[] {
    return SELECTOR_REGISTRY
        .filter(entry => entry.surface === surface)
        .map(entry => entry.selector);
}
