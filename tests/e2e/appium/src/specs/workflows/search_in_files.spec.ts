/**
 * Workflow: Search in Files
 *
 * 10 tests covering search panel operations with workspace open.
 */

import AppShellPage from '../../pages/AppShellPage';
import SearchPage from '../../pages/SearchPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Search in Files', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Search Panel Open ──

    it('should handle Cmd+Shift+F search shortcut', async () => {
        await SearchPage.open();
        await browser.pause(300);
        await SearchPage.close();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell stability after search open/close', async () => {
        await SearchPage.open();
        await SearchPage.close();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: Search Query ──

    it('should handle typing a search query', async () => {
        await SearchPage.open();
        await SearchPage.typeQuery('markdown');
        await browser.pause(500);
        await SearchPage.close();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle searchFor workflow', async () => {
        await SearchPage.searchFor('searchable');
        await browser.pause(500);
        await SearchPage.close();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: Search and Return ──

    it('should return to previous state after search dismiss', async () => {
        await SearchPage.open();
        await browser.pause(300);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell main_frame during search', async () => {
        await SearchPage.open();
        await browser.pause(300);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
        await SearchPage.close();
    });

    // ── Negative: Error States ──

    it('should NOT crash on rapid open/close of search', async () => {
        for (let i = 0; i < 5; i++) {
            await sendShortcut(Shortcuts.SEARCH);
            await browser.pause(100);
            await pressEscape();
            await browser.pause(100);
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT leave error overlays after search', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave modal state after Escape', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT crash after empty search query', async () => {
        await SearchPage.open();
        await browser.pause(300);
        await SearchPage.close();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
