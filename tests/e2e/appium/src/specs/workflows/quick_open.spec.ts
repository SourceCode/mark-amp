/**
 * Workflow: Quick Open
 *
 * 10 tests covering Quick Open (Cmd+P) file switching.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, typeText, pressEscape, pressEnter, Shortcuts } from '../../support/keyboard';

describe('Quick Open', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should handle Cmd+P without crash', async () => {
        await sendShortcut(Shortcuts.QUICK_OPEN);
        await browser.pause(500);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle typing a filename query', async () => {
        await sendShortcut(Shortcuts.QUICK_OPEN);
        await browser.pause(400);
        await typeText('README');
        await browser.pause(300);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle selecting a file via Enter', async () => {
        await sendShortcut(Shortcuts.QUICK_OPEN);
        await browser.pause(400);
        await typeText('NOTES');
        await browser.pause(300);
        await pressEnter();
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should dismiss on Escape', async () => {
        await sendShortcut(Shortcuts.QUICK_OPEN);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell during Quick Open', async () => {
        await sendShortcut(Shortcuts.QUICK_OPEN);
        await browser.pause(300);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
        await pressEscape();
    });

    it('should handle reopening Quick Open', async () => {
        await sendShortcut(Shortcuts.QUICK_OPEN);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(200);
        await sendShortcut(Shortcuts.QUICK_OPEN);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle non-matching search query', async () => {
        await sendShortcut(Shortcuts.QUICK_OPEN);
        await browser.pause(300);
        await typeText('zzz_nonexistent_file');
        await browser.pause(300);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT crash on rapid open/close', async () => {
        for (let i = 0; i < 5; i++) {
            await sendShortcut(Shortcuts.QUICK_OPEN);
            await browser.pause(100);
            await pressEscape();
            await browser.pause(100);
        }
        await browser.pause(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave dialogs', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
