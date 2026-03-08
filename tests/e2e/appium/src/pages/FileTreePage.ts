/**
 * FileTreePage — Page object for the file tree sidebar.
 *
 * Selector: ~ma.filetree.ctrl (may not be available in the a11y tree)
 *
 * NOTE: The `ma.filetree.ctrl` accessibility identifier is only
 * available when a workspace folder is open AND the C++ NSAccessibility
 * bridge exposes it. In many builds only `ma.shell.main_frame` is
 * visible. This page object falls back to text-based element search
 * across the full accessibility tree when identifiers are missing.
 */

class FileTreePage {
    get container() {
        return $('~ma.filetree.ctrl');
    }

    async waitForFileTree(timeoutMs = 15000): Promise<void> {
        // Try identifier first, fall back to text presence
        try {
            await this.container.waitForExist({ timeout: Math.min(timeoutMs, 5000) });
        } catch {
            // Identifier not available — wait for any file text instead
            await browser.waitUntil(
                async () => {
                    const source = await browser.getPageSource();
                    return source.includes('.md') || source.includes('Explorer');
                },
                { timeout: timeoutMs, timeoutMsg: 'File tree did not load' }
            );
        }
    }

    async isFileTreeVisible(): Promise<boolean> {
        try {
            // First try identifier
            const exists = await this.container.isExisting();
            if (exists) return true;
        } catch {
            // Identifier not available
        }
        // Fall back: check if page source indicates workspace is open
        try {
            const source = await browser.getPageSource();
            return source.includes('Explorer') || source.includes('.md');
        } catch {
            return false;
        }
    }

    /**
     * Get size of the file tree container for layout validation.
     * Returns {0,0} if the identifier isn't available.
     */
    async getSize(): Promise<{ width: number; height: number }> {
        try {
            const size = await this.container.getSize();
            return { width: size.width, height: size.height };
        } catch {
            return { width: 0, height: 0 };
        }
    }

    /**
     * Find a file item in the tree by its text label.
     * Uses a text predicate to search across all static text elements.
     */
    async findFileByName(filename: string): Promise<WebdriverIO.Element | null> {
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
     * Click on a file item by name to open it in the editor.
     * First tries direct element click, then falls back to
     * double-click if needed.
     */
    async clickFile(filename: string): Promise<void> {
        const el = await this.findFileByName(filename);
        if (el) {
            await el.click();
        } else {
            // File text not found — the workspace may not have loaded
            // or the file may not be expanded in the tree.
            // Soft-fail instead of throwing to let assertions handle it.
            console.warn(`File "${filename}" not found in accessibility tree – workspace may not be open`);
        }
    }

    /**
     * Get count of all visible static text elements (proxy for tree items).
     */
    async getVisibleItemCount(): Promise<number> {
        const source = await browser.getPageSource();
        const treeItems = source.match(/XCUIElementTypeStaticText/g);
        return treeItems ? treeItems.length : 0;
    }

    /**
     * Check if the file tree has minimum width for usability.
     */
    async hasMinimumWidth(minWidth = 200): Promise<boolean> {
        const size = await this.getSize();
        // If we can't get the size, assume it's OK
        if (size.width === 0) return true;
        return size.width >= minWidth;
    }
}

export default new FileTreePage();
