/**
 * TabBarPage — Page object for tab bar interactions.
 *
 * Tab bar items are discovered as children of the editor area
 * with text labels matching filenames.
 */

class TabBarPage {
    /**
     * Find a tab element by its filename text.
     */
    async findTab(filename: string): Promise<WebdriverIO.Element | null> {
        try {
            const el = await browser.$(
                `-ios predicate string:elementType == 48 AND value CONTAINS "${filename}"`
            );
            const exists = await el.isExisting();
            return exists ? el : null;
        } catch {
            return null;
        }
    }

    /**
     * Click a tab by filename to switch to it.
     */
    async clickTab(filename: string): Promise<void> {
        const tab = await this.findTab(filename);
        if (tab) {
            await tab.click();
            await browser.pause(300);
        } else {
            console.warn(`Tab "${filename}" not found – workspace may not be open`);
        }
    }

    /**
     * Check if a tab with the given filename exists.
     */
    async isTabOpen(filename: string): Promise<boolean> {
        const tab = await this.findTab(filename);
        return tab !== null;
    }

    /**
     * Get all visible tab text labels.
     */
    async getAllTabLabels(): Promise<string[]> {
        const elements = await browser.$$(
            '-ios predicate string:elementType == 48'
        );
        const labels: string[] = [];
        for (const el of elements) {
            const value = await el.getAttribute('value');
            if (value && (value.endsWith('.md') || value.endsWith('.txt') || value.endsWith('.json'))) {
                labels.push(value);
            }
        }
        return labels;
    }
}

export default new TabBarPage();
