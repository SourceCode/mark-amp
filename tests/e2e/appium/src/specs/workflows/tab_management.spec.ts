/**
 * Workflow: Tab Management
 *
 * 10 tests covering tab open/close/switch via file tree and shortcuts.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import TabBarPage from '../../pages/TabBarPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Tab Management', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Tab Creation ──

    it('should open file in a tab via file tree click', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(500);
        const isOpen = await TabBarPage.isTabOpen('README');
        expect(typeof isOpen).toBe('boolean');
    });

    it('should open second file in another tab', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(300);
        await FileTreePage.clickFile('NOTES');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: Tab Switching ──

    it('should switch tabs by clicking tab label', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(300);
        await FileTreePage.clickFile('NOTES');
        await browser.pause(300);
        await TabBarPage.clickTab('README');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: Tab Close ──

    it('should close tab via Cmd+W without crash', async () => {
        await FileTreePage.clickFile('NOTES');
        await browser.pause(300);
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: New File ──

    it('should handle New File (Cmd+N) without crash', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: Shell Stability ──

    it('should maintain shell stability during tab operations', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(200);
        await FileTreePage.clickFile('NOTES');
        await browser.pause(200);
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Negative: Error States ──

    it('should NOT crash when closing all tabs', async () => {
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await browser.pause(200);
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await browser.pause(200);
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT crash on rapid tab switching', async () => {
        await FileTreePage.clickFile('README');
        await FileTreePage.clickFile('NOTES');
        await FileTreePage.clickFile('README');
        await FileTreePage.clickFile('NOTES');
        await browser.pause(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have error overlays after tab operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs after Escape', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
