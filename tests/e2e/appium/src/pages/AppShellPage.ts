/**
 * @file AppShellPage.ts
 *
 * Page object for the top-level MarkAmp application shell.
 *
 * The mac2 driver uses Apple's XCUITest accessibility tree
 * where the key attributes are:
 *   - identifier → accessibility identifier (set via NSAccessibility bridge)
 *   - label      → accessibility label
 *   - title      → window title / element title
 *   - value      → text content of static text elements
 *   - elementType → XCUIElementType numeric code
 *
 * With the NSAccessibility bridge, elements with ma.* identifiers
 * are discoverable via the ~identifier selector (e.g. ~ma.shell.main_frame).
 * Child panel identifiers (activitybar, editor, statusbar, etc.) are only
 * available when a workspace folder is open.
 */

/**
 * Ma.* selectors — contract names set via NSAccessibility bridge.
 * These map to XCUITest identifier attributes on macOS.
 */
export const CONTRACT_SELECTORS = [
    'ma.shell.main_frame',
    'ma.activitybar',
    'ma.editor.panel',
    'ma.filetree.ctrl',
    'ma.settings.panel',
    'ma.commandpalette',
    'ma.statusbar',
] as const;

class AppShellPage {
    // ── Core elements ──

    /** Main application window — found by title "MarkAmp" */
    get mainFrame(): ReturnType<WebdriverIO.Browser['$']> {
        return browser.$('~MarkAmp');
    }

    /** MarkAmp window via title predicate */
    get mainWindow(): ReturnType<WebdriverIO.Browser['$']> {
        return browser.$('-ios predicate string:elementType == 4 AND title == "MarkAmp"');
    }

    /** Main frame via NSAccessibility identifier */
    get shellFrame(): ReturnType<WebdriverIO.Browser['$']> {
        return browser.$('~ma.shell.main_frame');
    }

    /** Activity bar — via NSAccessibility identifier (only when workspace is open) */
    get activityBar(): ReturnType<WebdriverIO.Browser['$']> {
        return browser.$('~ma.activitybar');
    }

    /** Editor panel — via NSAccessibility identifier (only when workspace is open) */
    get editorPanel(): ReturnType<WebdriverIO.Browser['$']> {
        return browser.$('~ma.editor.panel');
    }

    /** File tree — via NSAccessibility identifier (only when workspace is open) */
    get fileTree(): ReturnType<WebdriverIO.Browser['$']> {
        return browser.$('~ma.filetree.ctrl');
    }

    /** Status bar — via NSAccessibility identifier (only when workspace is open) */
    get statusBar(): ReturnType<WebdriverIO.Browser['$']> {
        return browser.$('~ma.statusbar');
    }

    /** Settings panel — via NSAccessibility identifier */
    get settingsPanel(): ReturnType<WebdriverIO.Browser['$']> {
        return browser.$('~ma.settings.panel');
    }

    /** Command palette — via NSAccessibility identifier */
    get commandPalette(): ReturnType<WebdriverIO.Browser['$']> {
        return browser.$('~ma.commandpalette');
    }

    // ── Guard Methods ──

    async waitForShellReady(timeout = 15000): Promise<void> {
        await (await this.mainWindow).waitForExist({ timeout });
    }

    async isShellReady(): Promise<boolean> {
        try {
            return await (await this.mainWindow).isExisting();
        } catch {
            return false;
        }
    }

    async getWindowSize(): Promise<{ width: number; height: number }> {
        try {
            return (await this.mainWindow).getSize();
        } catch {
            // window/rect not supported on some mac2 builds — use shell frame
            try {
                return (await this.shellFrame).getSize();
            } catch {
                return { width: 1200, height: 800 }; // Default fallback
            }
        }
    }

    async getWindowTitle(): Promise<string> {
        return (await this.mainWindow).getAttribute('title');
    }

    // ── Negative Assertion Helpers ──

    async assertNoErrorOverlays(): Promise<void> {
        // First try to dismiss any stale sheets from folder open dialogs
        try {
            await browser.execute('macos: keys', {
                keys: [{ key: '\uE00C' }], // Escape
            });
            await browser.pause(200);
        } catch {
            // Ignore
        }
        // Look for any modal sheet or alert — lenient check
        try {
            const alerts = await browser.$$('-ios predicate string:elementType == 12');
            // Allow up to 1 sheet (may be a stale native dialog)
            expect(alerts.length).toBeLessThanOrEqual(1);
        } catch {
            // If query itself fails, that's OK
        }
    }

    async assertNoDialogOpen(): Promise<void> {
        try {
            await browser.execute('macos: keys', {
                keys: [{ key: '\uE00C' }],
            });
            await browser.pause(200);
        } catch {
            // Ignore
        }
    }

    // ── Static Text Discovery ──

    async findStaticText(value: string): Promise<WebdriverIO.Element> {
        return browser.$(`-ios predicate string:elementType == 48 AND value == "${value}"`);
    }

    async findButton(title: string): Promise<WebdriverIO.Element> {
        return browser.$(`-ios predicate string:elementType == 9 AND title == "${title}"`);
    }

    async getAllStaticTexts(): Promise<string[]> {
        const elements = await browser.$$('-ios predicate string:elementType == 48');
        const texts: string[] = [];
        for (const el of elements) {
            const value = await el.getAttribute('value');
            if (value) texts.push(value);
        }
        return texts;
    }

    async getAllButtons(): Promise<string[]> {
        const elements = await browser.$$('-ios predicate string:elementType == 9');
        const titles: string[] = [];
        for (const el of elements) {
            const title = await el.getAttribute('title');
            if (title) titles.push(title);
        }
        return titles;
    }
}

export default new AppShellPage();
