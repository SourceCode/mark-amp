/**
 * Workflow: Shortcuts — File Operations
 *
 * 10 tests covering exhaustive file operation shortcuts.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Shortcuts File Operations', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should handle Cmd+N (New File)', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(400);
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+O (Open File)', async () => {
        await sendShortcut(Shortcuts.OPEN_FILE);
        await browser.pause(500);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+S (Save)', async () => {
        await sendShortcut(Shortcuts.SAVE);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+W (Close Tab)', async () => {
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+Shift+W (Close All)', async () => {
        await sendShortcut(Shortcuts.CLOSE_ALL_TABS);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+P (Quick Open)', async () => {
        await sendShortcut(Shortcuts.QUICK_OPEN);
        await browser.pause(400);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle all file shortcuts in sequence', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(200);
        await sendShortcut(Shortcuts.SAVE);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(200);
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell during file shortcut barrage', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
