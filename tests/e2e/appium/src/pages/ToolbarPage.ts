/**
 * ToolbarPage — Page object for the main toolbar.
 *
 * The toolbar contains view mode toggles, save, theme gallery, and settings buttons.
 * Buttons are discovered by title attribute in the accessibility tree.
 */

import AppShellPage from './AppShellPage';

class ToolbarPage {
    /**
     * Find a toolbar button by its title/label.
     */
    async findButton(title: string): Promise<WebdriverIO.Element> {
        return AppShellPage.findButton(title);
    }

    /**
     * Click a toolbar button by its title.
     */
    async clickButton(title: string): Promise<void> {
        const btn = await this.findButton(title);
        await btn.click();
        await browser.pause(300);
    }

    /**
     * Get all visible button titles in the application.
     */
    async getAllButtonTitles(): Promise<string[]> {
        return AppShellPage.getAllButtons();
    }

    /**
     * Verify a button with the given title exists.
     */
    async isButtonPresent(title: string): Promise<boolean> {
        try {
            const btn = await this.findButton(title);
            return await btn.isExisting();
        } catch {
            return false;
        }
    }
}

export default new ToolbarPage();
