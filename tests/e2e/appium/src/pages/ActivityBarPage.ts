/**
 * ActivityBarPage — Page object for the 15 activity bar items.
 *
 * Each item has: label, icon_name, shortcut_hint, is_bottom.
 * Items are discovered as children of ~ma.activitybar.
 *
 * NOTE: `ma.activitybar` may not appear in the accessibility tree.
 * Methods are wrapped with try/catch for resilience.
 */

/** Known activity bar item metadata from CreateItems() */
export const ACTIVITY_BAR_ITEMS = {
    top: [
        { id: 'Explorer',       icon: 'activity-explorer',      shortcut: 'Cmd+Shift+E' },
        { id: 'Search',         icon: 'activity-search',        shortcut: 'Cmd+Shift+F' },
        { id: 'Notebooks',      icon: 'activity-notebooks',     shortcut: '' },
        { id: 'Canvas',         icon: 'activity-canvas',        shortcut: '' },
        { id: 'Knowledge Graph', icon: 'activity-graph',        shortcut: '' },
        { id: 'AI Assistant',   icon: 'activity-ai',            shortcut: '' },
        { id: 'Flashcards',     icon: 'activity-flashcards',    shortcut: '' },
        { id: 'Git',            icon: 'activity-git',           shortcut: '' },
        { id: 'Tasks',          icon: 'activity-tasks',         shortcut: '' },
        { id: 'Database',       icon: 'activity-database',      shortcut: '' },
        { id: 'Presentation',   icon: 'activity-presentation',  shortcut: '' },
        { id: 'Extensions',     icon: 'activity-extensions',    shortcut: '' },
    ],
    bottom: [
        { id: 'Settings',  icon: 'activity-settings',  shortcut: '' },
        { id: 'Themes',    icon: 'toolbar-themes',     shortcut: '' },
        { id: 'Accounts',  icon: 'activity-account',   shortcut: '' },
    ],
} as const;

export const ALL_ACTIVITY_BAR_LABELS = [
    ...ACTIVITY_BAR_ITEMS.top.map(i => i.id),
    ...ACTIVITY_BAR_ITEMS.bottom.map(i => i.id),
];

class ActivityBarPage {
    get container() {
        return $('~ma.activitybar');
    }

    async waitForActivityBar(timeoutMs = 10000): Promise<void> {
        try {
            await this.container.waitForExist({ timeout: Math.min(timeoutMs, 5000) });
        } catch {
            // Identifier not available — wait for shell readiness instead
            const shell = await browser.$('~ma.shell.main_frame');
            await shell.waitForExist({ timeout: timeoutMs });
        }
    }

    async isActivityBarVisible(): Promise<boolean> {
        try {
            return await this.container.isExisting();
        } catch {
            return false;
        }
    }

    /**
     * Get the size of the activity bar container for layout validation.
     * Returns {0, 0} if identifier not available.
     */
    async getActivityBarSize(): Promise<{ width: number; height: number }> {
        try {
            const size = await this.container.getSize();
            return { width: size.width, height: size.height };
        } catch {
            return { width: 0, height: 0 };
        }
    }

    /**
     * Click within the activity bar at a relative Y offset.
     * Items are stacked vertically with ~40px height each.
     * Falls back to using the window's left edge if identifier not found.
     */
    async clickItemByIndex(index: number): Promise<void> {
        const ITEM_HEIGHT = 40;
        const TOP_PADDING = 8;
        const yOffset = TOP_PADDING + (index * ITEM_HEIGHT) + (ITEM_HEIGHT / 2);

        try {
            await this.container.click({ x: 20, y: yOffset });
        } catch {
            // Activity bar not found — try clicking at the left edge of the window
            const shell = await browser.$('~ma.shell.main_frame');
            await shell.click({ x: 20, y: yOffset + 60 });
        }
    }
}

export default new ActivityBarPage();
