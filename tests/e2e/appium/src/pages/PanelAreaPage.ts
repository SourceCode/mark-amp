/**
 * PanelAreaPage — Page object for the bottom panel area.
 *
 * The panel area contains Output, Problems, Debug Console, and Terminal tabs.
 * Toggled via Cmd+J.
 */

import { sendShortcut, Shortcuts } from '../support/keyboard';

class PanelAreaPage {
    /**
     * Toggle the panel visibility via Cmd+J.
     */
    async togglePanel(): Promise<void> {
        await sendShortcut(Shortcuts.TOGGLE_PANEL);
        await browser.pause(400);
    }

    /**
     * Check if the panel area is visible by looking for
     * panel-related elements in the accessibility tree.
     */
    async isPanelVisible(): Promise<boolean> {
        const source = await browser.getPageSource();
        return (
            source.includes('Output') ||
            source.includes('Problems') ||
            source.includes('Terminal')
        );
    }

    /**
     * Get the page source for panel content analysis.
     */
    async getPanelSource(): Promise<string> {
        return browser.getPageSource();
    }
}

export default new PanelAreaPage();
