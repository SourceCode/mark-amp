/**
 * SearchPage — Page object for the Search panel.
 *
 * The search panel is activated via Cmd+Shift+F and appears
 * in the sidebar area when the Search activity bar item is selected.
 */

import { sendShortcut, pressEscape, typeText, Shortcuts } from '../support/keyboard';

class SearchPage {
    /**
     * Open the search panel via keyboard shortcut.
     */
    async open(): Promise<void> {
        await sendShortcut(Shortcuts.SEARCH);
        await browser.pause(500);
    }

    /**
     * Close the search panel by pressing Escape.
     */
    async close(): Promise<void> {
        await pressEscape();
        await browser.pause(300);
    }

    /**
     * Type a search query. Assumes search panel is already open and focused.
     */
    async typeQuery(text: string): Promise<void> {
        await typeText(text);
        await browser.pause(500); // Allow search to process
    }

    /**
     * Open search and type query in one step.
     */
    async searchFor(text: string): Promise<void> {
        await this.open();
        await this.typeQuery(text);
    }

    /**
     * Check if the search panel has results (any static text elements
     * that look like file paths or match counts).
     */
    async hasResults(): Promise<boolean> {
        const source = await browser.getPageSource();
        // Look for match count patterns like "N results" in the accessibility tree
        return source.includes('result') || source.includes('match');
    }
}

export default new SearchPage();
