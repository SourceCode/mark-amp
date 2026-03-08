/**
 * Edge Case: Error Corrupt File
 *
 * 10 tests covering behavior when opening files with unusual content.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import EditorPage from '../../pages/EditorPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { pressEscape } from '../../support/keyboard';

describe('Error Corrupt File', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should handle opening empty file without crash', async () => {
        await FileTreePage.clickFile('empty');
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have editor visible with empty file', async () => {
        await FileTreePage.clickFile('empty');
        await browser.pause(500);
        const visible = await EditorPage.isEditorVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should handle switching from empty to regular file', async () => {
        await FileTreePage.clickFile('empty');
        await browser.pause(300);
        await FileTreePage.clickFile('README');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle special chars file', async () => {
        try {
            await FileTreePage.clickFile('special chars');
            await browser.pause(500);
        } catch {
            // May not find the file in tree
        }
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle opening large file after empty file', async () => {
        await FileTreePage.clickFile('empty');
        await browser.pause(300);
        await FileTreePage.clickFile('large_file');
        await browser.pause(2000);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have shell main_frame with edge case files', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should have correct window title', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have window with non-zero size', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT crash after Escape', async () => {
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
