/**
 * OutlinePage — Page object for the document outline panel.
 *
 * Shows the heading structure of the current document.
 */

class OutlinePage {
    /**
     * Check if any outline-related content is visible.
     * The outline panel shows heading text from the active document.
     */
    async isOutlineVisible(): Promise<boolean> {
        const source = await browser.getPageSource();
        return source.includes('Outline') || source.includes('outline');
    }

    /**
     * Get the page source for outline content analysis.
     */
    async getOutlineSource(): Promise<string> {
        return browser.getPageSource();
    }
}

export default new OutlinePage();
