/**
 * Edge Case: Performance Memory Stability
 *
 * 10 tests covering stability under sustained operations.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, typeText, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Performance Memory Stability', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should survive 50 file switches', async () => {
        for (let i = 0; i < 25; i++) {
            await FileTreePage.clickFile('README');
            await browser.pause(50);
            await FileTreePage.clickFile('NOTES');
            await browser.pause(50);
        }
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should survive 30 command palette open/close', async () => {
        for (let i = 0; i < 30; i++) {
            await sendShortcut(Shortcuts.COMMAND_PALETTE);
            await browser.pause(50);
            await pressEscape();
            await browser.pause(50);
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should survive 20 sidebar toggles', async () => {
        for (let i = 0; i < 20; i++) {
            await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
            await browser.pause(50);
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should survive 20 panel toggles', async () => {
        for (let i = 0; i < 20; i++) {
            await sendShortcut(Shortcuts.TOGGLE_PANEL);
            await browser.pause(50);
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should survive 100 character type and undo', async () => {
        await FileTreePage.clickFile('NOTES');
        await browser.pause(300);
        await typeText('a'.repeat(100));
        await browser.pause(300);
        for (let i = 0; i < 20; i++) {
            await sendShortcut(Shortcuts.UNDO);
        }
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should survive sustained shortcut barrage', async () => {
        const shortcuts = [
            Shortcuts.EXPLORER, Shortcuts.SEARCH, Shortcuts.EXPLORER,
            Shortcuts.TOGGLE_SIDEBAR, Shortcuts.TOGGLE_SIDEBAR,
            Shortcuts.TOGGLE_PANEL, Shortcuts.TOGGLE_PANEL,
        ];
        for (let round = 0; round < 3; round++) {
            for (const s of shortcuts) {
                await sendShortcut(s);
                await browser.pause(30);
            }
        }
        await pressEscape();
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell after sustained operations', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should have correct window title after stress', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
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
