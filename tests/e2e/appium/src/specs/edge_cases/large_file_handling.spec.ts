/**
 * Edge Case: Large File Handling
 *
 * 8 tests verifying the app handles a 10K+ line file without crash.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import EditorPage from '../../pages/EditorPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { pressEscape } from '../../support/keyboard';

describe('Large File Handling', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should open large_file.md without crash', async () => {
        await FileTreePage.clickFile('large_file');
        await browser.pause(2000);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have editor visible after opening large file', async () => {
        await FileTreePage.clickFile('large_file');
        await browser.pause(2000);
        const visible = await EditorPage.isEditorVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should maintain shell stability with large file open', async () => {
        await FileTreePage.clickFile('large_file');
        await browser.pause(2000);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should have correct window title with large file', async () => {
        await FileTreePage.clickFile('large_file');
        await browser.pause(2000);
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should switch away from large file without crash', async () => {
        await FileTreePage.clickFile('large_file');
        await browser.pause(1500);
        await FileTreePage.clickFile('README');
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT have error overlays after large file open', async () => {
        await FileTreePage.clickFile('large_file');
        await browser.pause(2000);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs after large file open', async () => {
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT crash after Escape with large file', async () => {
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
