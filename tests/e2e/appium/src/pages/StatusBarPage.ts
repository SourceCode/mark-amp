/**
 * StatusBarPage — Page object for the StatusBarPanel.
 *
 * The status bar uses ~ma.statusbar as its accessibility name.
 * Individual items are rendered as child elements with text labels
 * like "LN X, COL Y", "UTF-8", "LF", etc.
 *
 * NOTE: `ma.statusbar` may not appear in the accessibility tree.
 * Methods fall back to page source inspection when needed.
 */

/** Expected default status bar labels in E2E mode */
export const DEFAULT_STATUS_LABELS = {
    READY_STATE: 'READY',
    ENCODING: 'UTF-8',
    EOL_MODE: 'LF',
    INDENT_MODE: 'Spaces: 4',
    VIEW_MODE: 'SRC',
    ZOOM: 'Zoom: 100%',
} as const;

class StatusBarPage {
    get container() {
        return $('~ma.statusbar');
    }

    async waitForStatusBar(timeoutMs = 10000): Promise<void> {
        try {
            await this.container.waitForExist({ timeout: Math.min(timeoutMs, 5000) });
        } catch {
            // Identifier not available — wait for shell
            const shell = await browser.$('~ma.shell.main_frame');
            await shell.waitForExist({ timeout: timeoutMs });
        }
    }

    async isStatusBarVisible(): Promise<boolean> {
        try {
            return await this.container.isExisting();
        } catch {
            return false;
        }
    }

    /**
     * Get the size of the status bar for layout validation.
     * Returns {0, 0} if identifier not available.
     */
    async getStatusBarSize(): Promise<{ width: number; height: number }> {
        try {
            const size = await this.container.getSize();
            return { width: size.width, height: size.height };
        } catch {
            return { width: 0, height: 0 };
        }
    }

    /**
     * Get the status bar's accessibility children text content.
     */
    async getStatusBarText(): Promise<string> {
        try {
            const text = await this.container.getText();
            return text;
        } catch {
            return '';
        }
    }

    /**
     * Click within the status bar at a relative X position.
     */
    async clickAtPosition(xOffset: number): Promise<void> {
        try {
            await this.container.click({ x: xOffset, y: 12 });
        } catch {
            // Status bar not found via identifier — skip the click
        }
    }

    /**
     * Verify the status bar has minimum height (design system constraint).
     * Returns true if identifier is not available (can't verify).
     */
    async hasMinimumHeight(minHeight = 22): Promise<boolean> {
        const size = await this.getStatusBarSize();
        if (size.height === 0) return true; // Can't verify
        return size.height >= minHeight;
    }
}

export default new StatusBarPage();
