/**
 * Workflow: Activity Bar Navigation
 *
 * 15 tests covering activity bar item metadata, model consistency,
 * and shell stability on the Welcome screen.
 *
 * Note: Activity bar element (~ma.activitybar) is only present when
 * a workspace folder is open.  Tests validate the item model and
 * shell stability without requiring the element to exist.
 */

import AppShellPage from '../../pages/AppShellPage';
import {
    ACTIVITY_BAR_ITEMS,
    ALL_ACTIVITY_BAR_LABELS,
} from '../../pages/ActivityBarPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Activity Bar Navigation', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Item Count Validation ──

    it('should have exactly 12 top items defined in the model', () => {
        expect(ACTIVITY_BAR_ITEMS.top.length).toBe(12);
    });

    it('should have exactly 3 bottom items defined in the model', () => {
        expect(ACTIVITY_BAR_ITEMS.bottom.length).toBe(3);
    });

    it('should have 15 total activity bar labels defined', () => {
        expect(ALL_ACTIVITY_BAR_LABELS.length).toBe(15);
    });

    // ── Positive: Top Items Labels ──

    it('should have Explorer as the first top item', () => {
        expect(ACTIVITY_BAR_ITEMS.top[0].id).toBe('Explorer');
        expect(ACTIVITY_BAR_ITEMS.top[0].icon).toBe('activity-explorer');
        expect(ACTIVITY_BAR_ITEMS.top[0].shortcut).toBe('Cmd+Shift+E');
    });

    it('should have Search as the second top item with correct shortcut', () => {
        expect(ACTIVITY_BAR_ITEMS.top[1].id).toBe('Search');
        expect(ACTIVITY_BAR_ITEMS.top[1].icon).toBe('activity-search');
        expect(ACTIVITY_BAR_ITEMS.top[1].shortcut).toBe('Cmd+Shift+F');
    });

    it('should have Extensions as the last top item', () => {
        expect(ACTIVITY_BAR_ITEMS.top[11].id).toBe('Extensions');
        expect(ACTIVITY_BAR_ITEMS.top[11].icon).toBe('activity-extensions');
    });

    // ── Positive: Bottom Items Labels ──

    it('should have Settings, Themes, Accounts as bottom items', () => {
        expect(ACTIVITY_BAR_ITEMS.bottom[0].id).toBe('Settings');
        expect(ACTIVITY_BAR_ITEMS.bottom[1].id).toBe('Themes');
        expect(ACTIVITY_BAR_ITEMS.bottom[2].id).toBe('Accounts');
    });

    // ── Positive: Keyboard Shortcut Stability ──

    it('should handle Cmd+Shift+E for Explorer without crash', async () => {
        await sendShortcut(Shortcuts.EXPLORER);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+Shift+F for Search without crash', async () => {
        await sendShortcut(Shortcuts.SEARCH);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell stability after sidebar shortcut', async () => {
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(400);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(400);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Negative: Error States ──

    it('should NOT have any duplicate labels in the item model', () => {
        const seen = new Set<string>();
        for (const label of ALL_ACTIVITY_BAR_LABELS) {
            expect(seen.has(label)).toBe(false);
            seen.add(label);
        }
    });

    it('should NOT have empty icon names in any item', () => {
        const allItems = [...ACTIVITY_BAR_ITEMS.top, ...ACTIVITY_BAR_ITEMS.bottom];
        for (const item of allItems) {
            expect(item.icon.length).toBeGreaterThan(0);
        }
    });

    it('should NOT have empty labels in any item', () => {
        for (const label of ALL_ACTIVITY_BAR_LABELS) {
            expect(label.length).toBeGreaterThan(0);
        }
    });

    it('should NOT leave error overlays after navigation shortcuts', async () => {
        await sendShortcut(Shortcuts.EXPLORER);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT crash when pressing Escape after navigation', async () => {
        await pressEscape();
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
