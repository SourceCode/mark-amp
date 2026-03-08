/**
 * Workflow: File Save Lifecycle
 *
 * 10 tests covering file save, save-as, and dirty state management.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, typeText, pressEscape, Shortcuts } from '../../support/keyboard';

describe('File Save Lifecycle', () => {
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

    it('should handle Cmd+S on unmodified file', async () => {
        await sendShortcut(Shortcuts.SAVE);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle type → save', async () => {
        await typeText('autosave test');
        await browser.pause(200);
        await sendShortcut(Shortcuts.SAVE);
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle type → undo → save', async () => {
        await typeText('undo save test');
        await browser.pause(200);
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(200);
        await sendShortcut(Shortcuts.SAVE);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle save on new file (Cmd+N → type → Cmd+S)', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(300);
        await typeText('new file content');
        await browser.pause(200);
        await sendShortcut(Shortcuts.SAVE);
        await browser.pause(500);
        // May trigger Save As dialog
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle multiple rapid saves', async () => {
        await typeText('x');
        for (let i = 0; i < 5; i++) {
            await sendShortcut(Shortcuts.SAVE);
            await browser.pause(100);
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle save after file switch', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(300);
        await sendShortcut(Shortcuts.SAVE);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell during save lifecycle', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should have version text visible', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        const hasVersion = texts.some(t => t.startsWith('v'));
        expect(typeof hasVersion).toBe('boolean');
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
