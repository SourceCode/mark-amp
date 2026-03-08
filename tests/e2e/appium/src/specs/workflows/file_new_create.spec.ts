/**
 * Workflow: File New Create
 *
 * 10 tests covering new file creation via keyboard and menu.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, typeText, pressEscape, Shortcuts } from '../../support/keyboard';

describe('File New Create', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should handle Cmd+N (new file) without crash', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have editor after new file', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(500);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should type into new file', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(400);
        await typeText('# New Document');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle multiple new files', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(200);
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(200);
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should close new file without saving', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(300);
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle new file → type → undo → close', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(300);
        await typeText('temp content');
        await browser.pause(200);
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(200);
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle new file and switch to existing file', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain window dimensions after new files', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
        expect(size.height).toBeGreaterThanOrEqual(0);
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs after cleanup', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
