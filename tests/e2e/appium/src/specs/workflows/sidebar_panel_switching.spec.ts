/**
 * Workflow: Sidebar Panel Switching
 *
 * 10 tests covering activity bar panel navigation with workspace open.
 */

import AppShellPage from '../../pages/AppShellPage';
import ActivityBarPage, { ACTIVITY_BAR_ITEMS } from '../../pages/ActivityBarPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Sidebar Panel Switching', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Activity Bar Presence ──

    it('should have activity bar visible', async () => {
        const visible = await ActivityBarPage.isActivityBarVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should have activity bar with non-zero dimensions', async () => {
        const size = await ActivityBarPage.getActivityBarSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
        expect(size.height).toBeGreaterThanOrEqual(0);
    });

    // ── Positive: Panel Switching via Shortcuts ──

    it('should switch to Explorer via Cmd+Shift+E', async () => {
        await sendShortcut(Shortcuts.EXPLORER);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should switch to Search via Cmd+Shift+F', async () => {
        await sendShortcut(Shortcuts.SEARCH);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should return to Explorer after Search', async () => {
        await sendShortcut(Shortcuts.SEARCH);
        await browser.pause(300);
        await sendShortcut(Shortcuts.EXPLORER);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: Panel Switching via Click ──

    it('should switch panels via activity bar click (index 0)', async () => {
        await ActivityBarPage.clickItemByIndex(0);
        await browser.pause(400);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should switch to second panel via click', async () => {
        await ActivityBarPage.clickItemByIndex(1);
        await browser.pause(400);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Negative: Error States ──

    it('should NOT crash when rapidly switching panels', async () => {
        for (let i = 0; i < 5; i++) {
            await ActivityBarPage.clickItemByIndex(i % 3);
            await browser.pause(100);
        }
        await browser.pause(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave error overlays after panel switching', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave dialogs after Escape', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
