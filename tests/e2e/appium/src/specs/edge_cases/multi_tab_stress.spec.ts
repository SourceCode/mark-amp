/**
 * Edge Case: Multi-Tab Stress
 *
 * 8 tests covering opening many files and tab overflow handling.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Multi-Tab Stress', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should open multiple files without crash', async () => {
        const files = ['README', 'NOTES', 'large_file', 'empty'];
        for (const f of files) {
            await FileTreePage.clickFile(f);
            await browser.pause(400);
        }
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle rapid file switches', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(100);
        await FileTreePage.clickFile('NOTES');
        await browser.pause(100);
        await FileTreePage.clickFile('README');
        await browser.pause(100);
        await FileTreePage.clickFile('NOTES');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle close tab after opening many', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(200);
        await FileTreePage.clickFile('NOTES');
        await browser.pause(200);
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle closing all tabs', async () => {
        for (let i = 0; i < 5; i++) {
            await sendShortcut(Shortcuts.CLOSE_TAB);
            await browser.pause(200);
        }
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell main_frame with many tabs', async () => {
        await FileTreePage.clickFile('README');
        await FileTreePage.clickFile('NOTES');
        await browser.pause(300);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should NOT have error overlays after tab stress', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs after tab stress', async () => {
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT crash after complete tab lifecycle', async () => {
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
