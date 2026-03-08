/**
 * CommandPalettePage — Page object for the Command Palette.
 *
 * Selector: ~ma.commandpalette
 * Requires workspace open for palette element accessibility.
 */

import { sendShortcut, pressEscape, typeText, Shortcuts } from '../support/keyboard';

class CommandPalettePage {
    get container() {
        return $('~ma.commandpalette');
    }

    /**
     * Open the command palette via Cmd+Shift+P.
     */
    async open(): Promise<void> {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(500);
    }

    /**
     * Dismiss the command palette via Escape.
     */
    async dismiss(): Promise<void> {
        await pressEscape();
        await browser.pause(300);
    }

    /**
     * Check if the palette container element is present.
     */
    async isVisible(): Promise<boolean> {
        try {
            return await this.container.isExisting();
        } catch {
            return false;
        }
    }

    /**
     * Type a query into the palette input.
     */
    async typeQuery(text: string): Promise<void> {
        await typeText(text);
        await browser.pause(300);
    }

    /**
     * Open palette, type query, and pause to allow filtering.
     */
    async openAndSearch(query: string): Promise<void> {
        await this.open();
        await this.typeQuery(query);
    }

    /**
     * Select the first result by pressing Enter.
     */
    async selectFirstResult(): Promise<void> {
        const { pressEnter } = await import('../support/keyboard');
        await pressEnter();
        await browser.pause(300);
    }

    /**
     * Full workflow: open palette, search, select first result.
     */
    async executeCommand(query: string): Promise<void> {
        await this.openAndSearch(query);
        await this.selectFirstResult();
    }
}

export default new CommandPalettePage();
