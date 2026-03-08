/**
 * FindReplacePage — Page object for the in-editor Find and Replace bar.
 *
 * Opened via Cmd+F (find) or Cmd+H / Cmd+Shift+H (find & replace).
 */

import { sendShortcut, typeText, pressEscape, pressEnter } from '../support/keyboard';

/** Find shortcut (Cmd+F) */
const FIND_SHORTCUT = 'Cmd+F';
/** Replace shortcut (Cmd+H) */
const REPLACE_SHORTCUT = 'Cmd+H';

class FindReplacePage {
    /**
     * Open the find bar via Cmd+F.
     */
    async openFind(): Promise<void> {
        await sendShortcut(FIND_SHORTCUT);
        await browser.pause(400);
    }

    /**
     * Open find & replace via Cmd+H.
     */
    async openReplace(): Promise<void> {
        await sendShortcut(REPLACE_SHORTCUT);
        await browser.pause(400);
    }

    /**
     * Close the find bar.
     */
    async close(): Promise<void> {
        await pressEscape();
        await browser.pause(300);
    }

    /**
     * Type text into the currently focused find input.
     */
    async typeSearchText(text: string): Promise<void> {
        await typeText(text);
        await browser.pause(300);
    }

    /**
     * Find next match by pressing Enter in the find field.
     */
    async findNext(): Promise<void> {
        await pressEnter();
        await browser.pause(200);
    }

    /**
     * Full workflow: open find bar, type query.
     */
    async findText(text: string): Promise<void> {
        await this.openFind();
        await this.typeSearchText(text);
    }
}

export default new FindReplacePage();
