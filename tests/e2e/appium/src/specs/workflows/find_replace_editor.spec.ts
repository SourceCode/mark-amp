/**
 * Workflow: Find and Replace in Editor
 *
 * 10 tests covering in-editor find/replace operations.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import FindReplacePage from '../../pages/FindReplacePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Find and Replace in Editor', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
        await FileTreePage.clickFile('NOTES');
        await browser.pause(500);
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Find Bar Open ──

    it('should open find bar via Cmd+F', async () => {
        await FindReplacePage.openFind();
        await browser.pause(300);
        await FindReplacePage.close();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should open find-replace via Cmd+H', async () => {
        await FindReplacePage.openReplace();
        await browser.pause(300);
        await FindReplacePage.close();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: Find Text ──

    it('should type search text without crash', async () => {
        await FindReplacePage.findText('Architecture');
        await browser.pause(300);
        await FindReplacePage.close();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle find next without crash', async () => {
        await FindReplacePage.openFind();
        await FindReplacePage.typeSearchText('the');
        await FindReplacePage.findNext();
        await FindReplacePage.close();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: Shell Stability ──

    it('should maintain shell during find operations', async () => {
        await FindReplacePage.openFind();
        await browser.pause(200);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
        await FindReplacePage.close();
    });

    it('should restore shell after find dismiss', async () => {
        await FindReplacePage.openFind();
        await FindReplacePage.typeSearchText('test');
        await FindReplacePage.close();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Negative: Error States ──

    it('should NOT crash on rapid find open/close', async () => {
        for (let i = 0; i < 5; i++) {
            await sendShortcut(Shortcuts.FIND);
            await browser.pause(100);
            await pressEscape();
            await browser.pause(100);
        }
        await browser.pause(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT crash when searching for non-existent text', async () => {
        await FindReplacePage.findText('zzz_nonexistent_xyz');
        await browser.pause(300);
        await FindReplacePage.close();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT leave error overlays after find operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave dialogs after Escape', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
