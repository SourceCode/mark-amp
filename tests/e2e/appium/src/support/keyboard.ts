/**
 * keyboard.ts — Type-safe macOS keyboard shortcut helpers for Appium mac2.
 *
 * Maps human-readable shortcut strings like "Cmd+Shift+P" to the
 * `macos: keys` execute command format.
 *
 * IMPORTANT: The mac2 driver requires single-character keys or XCUITest
 * Unicode virtual key codes. Multi-character key names like 'Escape'
 * must be mapped to their Unicode Private Use Area equivalents.
 */

/** macOS modifier flags for the mac2 driver `macos: keys` command */
export const ModifierFlags = {
    Cmd:     1 << 20,   // NSEventModifierFlagCommand
    Shift:   1 << 17,   // NSEventModifierFlagShift
    Option:  1 << 19,   // NSEventModifierFlagOption
    Ctrl:    1 << 18,   // NSEventModifierFlagControl
} as const;

/**
 * XCUITest virtual key code mapping.
 * Keys from W3C WebDriver spec + Apple-specific codes.
 */
const VIRTUAL_KEYS: Record<string, string> = {
    'escape':     '\uE00C',
    'return':     '\uE006',
    'enter':      '\uE007',
    'tab':        '\uE004',
    'backspace':  '\uE003',
    'delete':     '\uE017',
    'space':      '\uE00D',
    'uparrow':    '\uE013',
    'downarrow':  '\uE015',
    'leftarrow':  '\uE012',
    'rightarrow': '\uE014',
    'up':         '\uE013',
    'down':       '\uE015',
    'left':       '\uE012',
    'right':      '\uE014',
    'home':       '\uE011',
    'end':        '\uE010',
    'pageup':     '\uE00E',
    'pagedown':   '\uE00F',
    'f1':         '\uE031',
    'f2':         '\uE032',
    'f3':         '\uE033',
    'f4':         '\uE034',
    'f5':         '\uE035',
    'f6':         '\uE036',
    'f7':         '\uE037',
    'f8':         '\uE038',
    'f9':         '\uE039',
    'f10':        '\uE03A',
    'f11':        '\uE03B',
    'f12':        '\uE040',
    'f13':        '\uE041',
    'f14':        '\uE042',
    'f15':        '\uE043',
    'f16':        '\uE044',
    'f17':        '\uE045',
    'f18':        '\uE046',
    'f19':        '\uE047',
};

/** A single key press with optional modifier flags */
interface KeyPress {
    key: string;
    modifierFlags?: number;
}

/**
 * Map a key name to its XCUITest virtual key code if needed.
 * Single-character keys pass through unchanged.
 */
function resolveKey(keyName: string): string {
    const lower = keyName.toLowerCase();
    if (VIRTUAL_KEYS[lower]) {
        return VIRTUAL_KEYS[lower];
    }
    // For , (comma) and other single chars, return as-is
    if (keyName.length === 1) {
        return keyName.toLowerCase();
    }
    // Unknown multi-char name — try single lowercase char
    return keyName.toLowerCase();
}

/**
 * Parse a human-readable shortcut string into modifier flags.
 * Example: "Cmd+Shift+P" → { key: 'p', modifierFlags: Cmd | Shift }
 */
function parseShortcut(shortcut: string): KeyPress {
    const parts = shortcut.split('+').map(p => p.trim());
    const keyName = parts[parts.length - 1];
    const key = resolveKey(keyName);
    let flags = 0;

    for (let i = 0; i < parts.length - 1; i++) {
        const mod = parts[i].toLowerCase();
        if (mod === 'cmd' || mod === 'command') flags |= ModifierFlags.Cmd;
        else if (mod === 'shift') flags |= ModifierFlags.Shift;
        else if (mod === 'option' || mod === 'alt') flags |= ModifierFlags.Option;
        else if (mod === 'ctrl' || mod === 'control') flags |= ModifierFlags.Ctrl;
    }

    return flags > 0 ? { key, modifierFlags: flags } : { key };
}

/**
 * Send a keyboard shortcut via the mac2 `macos: keys` command.
 *
 * @param shortcut - e.g. "Cmd+Shift+P", "Cmd+S", "Escape"
 */
export async function sendShortcut(shortcut: string): Promise<void> {
    const keyPress = parseShortcut(shortcut);
    await browser.execute('macos: keys', {
        keys: [keyPress],
    });
}

/**
 * Type text character by character.
 */
export async function typeText(text: string): Promise<void> {
    for (const char of text) {
        await browser.execute('macos: keys', {
            keys: [{ key: char }],
        });
    }
}

/**
 * Press Escape key to dismiss dialogs/palettes.
 */
export async function pressEscape(): Promise<void> {
    await browser.execute('macos: keys', {
        keys: [{ key: '\uE00C' }],
    });
}

/**
 * Press Enter key.
 */
export async function pressEnter(): Promise<void> {
    await browser.execute('macos: keys', {
        keys: [{ key: '\uE006' }],
    });
}

/**
 * Press Tab key for focus navigation.
 */
export async function pressTab(): Promise<void> {
    await browser.execute('macos: keys', {
        keys: [{ key: '\uE004' }],
    });
}

/**
 * Press arrow keys.
 */
export async function pressArrowDown(): Promise<void> {
    await browser.execute('macos: keys', {
        keys: [{ key: '\uE015' }],
    });
}

export async function pressArrowUp(): Promise<void> {
    await browser.execute('macos: keys', {
        keys: [{ key: '\uE013' }],
    });
}

export async function pressArrowLeft(): Promise<void> {
    await browser.execute('macos: keys', {
        keys: [{ key: '\uE012' }],
    });
}

export async function pressArrowRight(): Promise<void> {
    await browser.execute('macos: keys', {
        keys: [{ key: '\uE014' }],
    });
}

/** Common MarkAmp keyboard shortcuts */
export const Shortcuts = {
    COMMAND_PALETTE: 'Cmd+Shift+P',
    SETTINGS:       'Cmd+,',
    NEW_FILE:       'Cmd+N',
    OPEN_FILE:      'Cmd+O',
    SAVE:           'Cmd+S',
    CLOSE_TAB:      'Cmd+W',
    CLOSE_ALL_TABS: 'Cmd+Shift+W',
    TOGGLE_SIDEBAR: 'Cmd+B',
    TOGGLE_PANEL:   'Cmd+J',
    UNDO:           'Cmd+Z',
    REDO:           'Cmd+Shift+Z',
    EXPLORER:       'Cmd+Shift+E',
    SEARCH:         'Cmd+Shift+F',
    FIND:           'Cmd+F',
    FIND_REPLACE:   'Cmd+H',
    QUICK_OPEN:     'Cmd+P',
    ZEN_MODE:       'Cmd+Shift+Return',
    GO_TO_LINE:     'Ctrl+G',
    SPLIT_EDITOR:   'Cmd+\\',
    SELECT_ALL:     'Cmd+A',
} as const;
